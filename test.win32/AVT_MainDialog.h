#pragma once
#include "resource.h"


class CMainDialog : public CDialogImpl< CMainDialog >
{
	/*************************************************************************
	Public Methods
	*************************************************************************/
public:

	/*************************************************************************
	Inherit Methods
	*************************************************************************/
public:
	enum { IDD = IDD_DIALOG_MAIN };

	BEGIN_MSG_MAP(CMainDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_CHECK_RECORD, OnRecordButton);
		COMMAND_ID_HANDLER(IDC_BUTTON_CREATE, OnCreateButton);
		COMMAND_ID_HANDLER(IDC_BUTTON_DOWNLOAD, OnDownloadButton);
		COMMAND_ID_HANDLER(IDC_BUTTON_PLAY, OnPlayButton);
		COMMAND_ID_HANDLER(IDC_BUTTON_STOP, OnStopButton);
		COMMAND_ID_HANDLER(IDC_BUTTON_TO_TEXT, OnToTextButton);
		
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRecordButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCreateButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDownloadButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPlayButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnStopButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnToTextButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	/*************************************************************************
	Implementation Methods
	*************************************************************************/
private:
	class Callback : public AxVoiceCallback
	{
	public:
		virtual void onMessage(const AxVoiceMessage* message) 	{ m_pMainDialog->onAxVoiceMessage(message); }

	public:
		Callback(CMainDialog* pMainDialog) : m_pMainDialog(pMainDialog) { }
		~Callback() {}
		CMainDialog* m_pMainDialog;
	};

	void onAxVoiceMessage(const AxVoiceMessage* message);

	/*************************************************************************
	Implementation Data
	*************************************************************************/
private:
	Callback* m_axvoiceCallback;
	unsigned int m_currentID;

	/*************************************************************************
	Construction and Destruction
	*************************************************************************/
public:
	CMainDialog();
	~CMainDialog();
};

