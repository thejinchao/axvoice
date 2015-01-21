#include "stdafx.h"
#include "AV_Voice2TextWrap.h"
#include "AV_Global.h"
#include "AV_VoiceManager.h"
#include "AV_VoiceItem.h"
#include "AV_MessageQueue.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//Voice2TextSession
//////////////////////////////////////////////////////////////////////////////////////////////
Voice2TextSession::Voice2TextSession(unsigned int _voiceID, const std::string& _localWavFile)
	: hThread(0)
	, voiceID(_voiceID)
	, strLocalWavFile(_localWavFile)
{
}

//--------------------------------------------------------------------------------------------
Voice2TextSession::~Voice2TextSession()
{
}

//--------------------------------------------------------------------------------------------
bool Voice2TextSession::run(void)
{
	//create record thread
	UINT nThreadID;
	hThread = (HANDLE)_beginthreadex(NULL, 0, _convertThread, this, CREATE_SUSPENDED, &nThreadID);
	if (hThread == 0) return false;

	::ResumeThread(hThread);
	return true;
}

//--------------------------------------------------------------------------------------------
UINT CALLBACK Voice2TextSession::_convertThread(void* pParam)
{
	Voice2TextSession* pThis = (Voice2TextSession*)pParam;

	std::string result, error;
	bool success = pThis->convertThread(result, error);

	char debug_text[1024];
	StringCbPrintfA(debug_text, 1024, "id=%d, success=%d, result=%s", 
		pThis->voiceID, success?1:0, error.c_str());
	OutputDebugStringA(debug_text);

	//update item status
	if(IFlyEngine::getInstance()->m_completeCallback)
	{
		(IFlyEngine::getInstance()->m_completeCallback)(pThis->voiceID, success, error.c_str());
	}

	//callback message
	MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_TOTXT_MSG, pThis->voiceID, result.c_str(), success, error.c_str());

	::_endthreadex(0);
	return 0;
}

//--------------------------------------------------------------------------------------------
bool Voice2TextSession::convertThread(std::string& result, std::string& error)
{
	Proc_MSPLogin MSPLogin = IFlyEngine::getInstance()->MSPLogin;
	Proc_MSPLogout MSPLogout = IFlyEngine::getInstance()->MSPLogout;
	Proc_QISRSessionBegin QISRSessionBegin = IFlyEngine::getInstance()->QISRSessionBegin;
	Proc_QISRSessionEnd QISRSessionEnd = IFlyEngine::getInstance()->QISRSessionEnd;
	Proc_QISRAudioWrite QISRAudioWrite = IFlyEngine::getInstance()->QISRAudioWrite;
	Proc_QISRGetResult QISRGetResult = IFlyEngine::getInstance()->QISRGetResult;

	result = "failed";

	//open wav file
	FILE* fpWavFile=0;
	fopen_s(&fpWavFile, strLocalWavFile.c_str(), "rb");
	if(fpWavFile==0) 
	{
		error = "open file error";
		return false;
	}

	fseek(fpWavFile, 0, SEEK_END);
	size_t pcmSize = ftell(fpWavFile);
	fseek(fpWavFile, 0, SEEK_SET);

	char* pcmData = new char[pcmSize];
	if(pcmSize!=fread(pcmData, 1, pcmSize, fpWavFile))
	{
		//error
		delete[] pcmData; fclose(fpWavFile); 
		error = "read file error";
		return false;
	}
	fclose(fpWavFile); fpWavFile=0;

	char szLoginParam[MAX_PATH]={0};
	::StringCbPrintfA(szLoginParam, MAX_PATH, "appid = %s, work_dir = %s", 
		IFlyEngine::getInstance()->m_appID.c_str(),
		g_cacheAudioPath.c_str());

	char szErrorMsg[MAX_PATH]={0};

	//user loagin
	int errCode = MSPLogin(NULL, NULL, szLoginParam);
	if(errCode != MSP_SUCCESS)
	{
		delete[] pcmData;
		StringCbPrintfA(szErrorMsg, MAX_PATH, "MSPLogin failed, code=", errCode);
		error = szErrorMsg;
		return false;
	}

	//begin a session
	const char* param = "sub=iat,auf=audio/L16;rate=16000,aue=raw,ent=sms16k,rst=plain,rse=utf8";//可参考可设置参数列表
	const char *sessionID = QISRSessionBegin(NULL, param, &errCode);
	if(errCode != MSP_SUCCESS)
	{
		delete[] pcmData; MSPLogout();
		StringCbPrintfA(szErrorMsg, MAX_PATH, "QISRSessionBegin failed, code=", errCode);
		error = szErrorMsg;
		return false;
	}

	size_t pcmPoint = 0;
	std::string strResult;
	int epStatus = MSP_EP_LOOKING_FOR_SPEECH;
	int recStatus = MSP_REC_STATUS_SUCCESS ;

	//begin upload
	while(true)
	{
		unsigned int len = 6400;
		if (pcmSize < 12800) {
			len = pcmSize;
		}

		int audStat = (pcmPoint==0) ? MSP_AUDIO_SAMPLE_FIRST : MSP_AUDIO_SAMPLE_CONTINUE;
		if (len<=0) break;

		errCode = QISRAudioWrite(sessionID, (const void *)(pcmData + pcmPoint), len, audStat, &epStatus, &recStatus);
		if(errCode != MSP_SUCCESS) break;

		pcmPoint += (size_t)len;
		pcmSize -= (size_t)len;
		if (recStatus == MSP_REC_STATUS_SUCCESS) {
			//we can get result from server
			const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);
			if (NULL != rslt) strResult += rslt;
		}
		if (epStatus == MSP_EP_AFTER_SPEECH) break;

		//like a human...
		Sleep(150);
	}

	delete[] pcmData; pcmData=0;

	//send last audio
	errCode = QISRAudioWrite(sessionID, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &epStatus, &recStatus);
	if(errCode != MSP_SUCCESS) {
		QISRSessionEnd(sessionID, NULL); MSPLogout();
		StringCbPrintfA(szErrorMsg, MAX_PATH, "QISRAudioWrite failed, code=", errCode);
		error = szErrorMsg;
		return false;
	};

	//continue get result
	while (recStatus != MSP_REC_STATUS_COMPLETE && MSP_SUCCESS==errCode) {
		const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &errCode);
		if(errCode != MSP_SUCCESS) {
			QISRSessionEnd(sessionID, NULL); MSPLogout();
			StringCbPrintfA(szErrorMsg, MAX_PATH, "QISRGetResult failed, code=", errCode);
			error = szErrorMsg;
			return false;
		};

		if (NULL != rslt) strResult += rslt;
		Sleep(150);  //like a human...
	}

	//close session
	QISRSessionEnd(sessionID, NULL); MSPLogout();

	result = "complete";
	error = strResult;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//IFlyEngine
//////////////////////////////////////////////////////////////////////////////////////////////
IFlyEngine* IFlyEngine::s_Instance = 0;

//--------------------------------------------------------------------------------------------
IFlyEngine::IFlyEngine()
	: MSPLogin(0)
	, MSPLogout(0)
{
	s_Instance=this;
}

//--------------------------------------------------------------------------------------------
IFlyEngine::~IFlyEngine()
{
}

//--------------------------------------------------------------------------------------------
bool IFlyEngine::initEngine(const char* app_id)
{
	//param error
	if(app_id==0 || app_id[0]==0) return false;

	//init already?
	if(MSPLogin!=0) return true;

	//current path
	wchar_t wszDllName[MAX_PATH]={0};
	GetModuleFileName(g_hModuleHandle, wszDllName, MAX_PATH);
	PathRemoveFileSpec(wszDllName);
	PathAppend(wszDllName, L"msc.dll");

	//load dll
	HMODULE hMSC = ::LoadLibraryW(wszDllName);
	if(hMSC==0) return false;

	//get process
	MSPLogin = (Proc_MSPLogin)GetProcAddress(hMSC, "MSPLogin");
	MSPLogout = (Proc_MSPLogout)GetProcAddress(hMSC, "MSPLogout");
	QISRSessionBegin = (Proc_QISRSessionBegin)GetProcAddress(hMSC, "QISRSessionBegin");
	QISRSessionEnd = (Proc_QISRSessionEnd)GetProcAddress(hMSC, "QISRSessionEnd");
	QISRAudioWrite = (Proc_QISRAudioWrite)GetProcAddress(hMSC, "QISRAudioWrite");
	QISRGetResult = (Proc_QISRGetResult)GetProcAddress(hMSC, "QISRGetResult");

	if(MSPLogin==0 || MSPLogout==0 || QISRSessionBegin==0 || QISRSessionEnd==0 || QISRAudioWrite==0 || QISRGetResult==0)
	{
		::FreeLibrary(hMSC); hMSC=0;
		return false;
	}

	m_appID = app_id;
	return true;
}

//--------------------------------------------------------------------------------------------
bool IFlyEngine::beginConvertVoice(unsigned int voice_id, ON_COMPLETE_CALLBACK callback)
{
	m_completeCallback = callback;

	//init already?
	if(MSPLogin==0)
	{
		//callback message
		MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_TOTXT_MSG, 
			voice_id, "failed", false, "IFly engine not init");
		return false;
	}

	//find the voice item
	VoiceItem* item = VoiceManager::getInstance()->findItem(voice_id);
	if(item==0) return false; //TODO: ERROR
	if(item->getLocalStatus()!=VoiceItem::HAS_LOCAL_FILE) return false; //TODO: ERROR

	//check convert status
	if(item->getLocalStatus() != VoiceItem::HAS_LOCAL_FILE)
	{
		//TODO: ERROR
		return false;
	}

	if(item->getTextStatus() == VoiceItem::HAS_TEXT)
	{
		//return text
		MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_TOTXT_MSG, voice_id, 
			"complete", true, item->getText().c_str());
		return true;
	} 
	else if(item->getTextStatus() == VoiceItem::CONVERTING)
	{
		//converting... 
		//TODO: maybe give more information...
		return true;
	}

	//update statua
	item->beginConvertToText();

	//new convert sesson
	Voice2TextSession* session = new Voice2TextSession(voice_id, item->getLocalWavFile());
	m_sessionMap.insert(std::make_pair(voice_id, session));

	return session->run();
}
