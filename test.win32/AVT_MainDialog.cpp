#include "StdAfx.h"
#include "AVT_MainDialog.h"

//--------------------------------------------------------------------------------------------
CMainDialog::CMainDialog()
{
}

//--------------------------------------------------------------------------------------------
CMainDialog::~CMainDialog()
{
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//move to default position
	SetWindowPos(0, 100, 100, 0, 0, SWP_NOSIZE);

	//init axvoice
	char szTempPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, szTempPath);

	PathAppendA(szTempPath, "axvoice_cache_audio");
	::CreateDirectoryA(szTempPath, 0);

	AxVoice_Init(szTempPath, "http://www.dashengine.com/upload_voice", "54bef90a");

	m_axvoiceCallback = new Callback(this);

	SetTimer(101, 10);
	return (LRESULT)TRUE;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AxVoice_DispatchMessage(m_axvoiceCallback);
	return 0;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnRecordButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bool record = (BST_CHECKED==IsDlgButtonChecked(wID));
	if (record)
	{
		//AxTrace("begin record...");

		m_currentID = AxVoice_BeginRecord();
		::SetDlgItemInt(m_hWnd, IDC_EDIT_VOICE_ID, m_currentID, FALSE);

		::EnableWindow(GetDlgItem(IDC_BUTTON_CREATE), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_DOWNLOAD), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_PLAY), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_STOP), FALSE);
	}
	else
	{
		//AxTrace("complete record...");

		AxVoice_CompleteRecord(m_currentID);
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
void CMainDialog::onAxVoiceMessage(const AxVoiceMessage* message)
{
	switch(message->getType())
	{
	case AxVoiceMessage::MT_RECORD_MSG:
		{
			int counts = message->getParamCounts();
			std::string p0 = message->getParam(0);
			std::string p1 = message->getParam(1);
			std::string p2 = message->getParam(2);
			if( p0 == "complete")
			{
				//AxTrace("[%d]Record %s, result=%s",  message->getVoiceID(), p1.c_str(), p2.c_str());

				AxVoice_UploadVoice(m_currentID);

				::EnableWindow(GetDlgItem(IDC_BUTTON_CREATE), TRUE);
				::EnableWindow(GetDlgItem(IDC_BUTTON_DOWNLOAD), TRUE);
				::EnableWindow(GetDlgItem(IDC_BUTTON_PLAY), TRUE);
				::EnableWindow(GetDlgItem(IDC_BUTTON_STOP), TRUE);
			}
			
		}
		break;

	case AxVoiceMessage::MT_UPLOAD_MSG:
		{
			int counts = message->getParamCounts();
			std::string p0 = message->getParam(0);
			std::string p1 = message->getParam(1);
			std::string p2= message->getParam(2);
			if( p0 == "complete")
			{
				::SetDlgItemTextA(m_hWnd, IDC_EDIT_SERVER_URL, p2.c_str());
				//AxTrace("[%d]Upload %s, result=%s",  message->getVoiceID(), p1.c_str(), p2.c_str());
			}
		}
		break;

	case AxVoiceMessage::MT_DOWNLOAD_MSG:
		{
			int counts = message->getParamCounts();
			std::string p0 = message->getParam(0);
			std::string p1 = message->getParam(1);
			std::string p2= message->getParam(2);
			if( p0 == "complete")
			{
				//AxTrace("[%d]Download %s, result=%s",  message->getVoiceID(), p1.c_str(), p2.c_str());
			}
		}
		break;

	case AxVoiceMessage::MT_PLAY_MSG:
		{
			int counts = message->getParamCounts();
			std::string p0 = message->getParam(0);
			if( p0 == "complete")
			{
				//AxTrace("[%d]Play Voice complete", message->getVoiceID());
			}

		}
		break;

	case AxVoiceMessage::MT_TOTXT_MSG:
		{
			int counts = message->getParamCounts();
			std::string p0 = message->getParam(0);
			std::string p1 = message->getParam(1);
			std::string p2= message->getParam(2);
			if( p0 == "complete")
			{
				wchar_t wszTemp[1024]={0};
				_encode_utf8_to_ucs(p2.c_str(), wszTemp, 1024, p2.length());
				MessageBoxW(wszTemp, L"Text", MB_OK);
			}
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnCreateButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char szURL[1024]={0};
	::GetDlgItemTextA(m_hWnd, IDC_EDIT_SERVER_URL, szURL, 1024);

	m_currentID = AxVoice_CreatVoice(szURL);
	::SetDlgItemInt(m_hWnd, IDC_EDIT_VOICE_ID, m_currentID, FALSE);
	return 0;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnDownloadButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_currentID = ::GetDlgItemInt(m_hWnd, IDC_EDIT_VOICE_ID, 0, FALSE);
	AxVoice_DownloadVoice(m_currentID);
	return 0;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnPlayButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_currentID = ::GetDlgItemInt(m_hWnd, IDC_EDIT_VOICE_ID, 0, FALSE);
	AxVoice_PlayVoice(m_currentID);
	return 0;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnStopButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_currentID = ::GetDlgItemInt(m_hWnd, IDC_EDIT_VOICE_ID, 0, FALSE);
	AxVoice_StopVoice();
	return 0;
}

//--------------------------------------------------------------------------------------------
LRESULT CMainDialog::OnToTextButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_currentID = ::GetDlgItemInt(m_hWnd, IDC_EDIT_VOICE_ID, 0, FALSE);
	AxVoice_Voice2Text(m_currentID);

	return 0;
}

//------------------------------------------------------------------------------------------------------------------
unsigned int CMainDialog::_encode_utf8_to_ucs(const char* utf8_src, wchar_t* dest, unsigned int dest_len, unsigned int src_len)
{
	// count length for null terminated source...
	if (src_len == 0)
	{
		src_len = (unsigned int)strlen(utf8_src);
	}

	unsigned int destCapacity = dest_len;

	// while there is data in the source buffer, and space in the dest buffer
	for (unsigned int idx = 0; ((idx < src_len) && (destCapacity > 0));)
	{
		wchar_t	cp;
		unsigned char	cu = utf8_src[idx++];

		if (cu < 0x80)
		{
			cp = (wchar_t)(cu);
		}
		else if (cu < 0xE0)
		{
			cp = ((cu & 0x1F) << 6);
			cp |= (utf8_src[idx++] & 0x3F);
		}
		else if (cu < 0xF0)
		{
			cp = ((cu & 0x0F) << 12);
			cp |= ((utf8_src[idx++] & 0x3F) << 6);
			cp |= (utf8_src[idx++] & 0x3F);
		}
		else
		{
			cp = ((cu & 0x07) << 18);
			cp |= ((utf8_src[idx++] & 0x3F) << 12);
			cp |= ((utf8_src[idx++] & 0x3F) << 6);
			cp |= (utf8_src[idx++] & 0x3F);
		}

		*dest++ = cp;
		--destCapacity;
	}

	return dest_len - destCapacity;
}
