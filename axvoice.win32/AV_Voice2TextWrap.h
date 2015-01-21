#pragma once

#include "../libifly/windows/inc/qisr.h"
#include "../libifly/windows/inc/msp_cmn.h"
#include "../libifly/windows/inc/msp_errors.h"

class Voice2TextSession
{
public:
	bool run(void);

private:
	unsigned int voiceID;
	std::string strLocalWavFile;
	HANDLE hThread;

private:
	static UINT CALLBACK _convertThread(void* pParam);
	bool convertThread(std::string& result, std::string& error);

public:
	Voice2TextSession(unsigned int voiceID, const std::string& localWavFile);
	~Voice2TextSession();
};

class IFlyEngine
{
public:
	typedef void (CALLBACK *ON_COMPLETE_CALLBACK)(unsigned int, bool, const std::string&);  

	/** load library */
	bool initEngine(const char* app_id);

	/** begin convert voice*/
	bool beginConvertVoice(unsigned int voice_id, ON_COMPLETE_CALLBACK callback);

private:
	std::string m_appID;

	Proc_MSPLogin MSPLogin;
	Proc_MSPLogout MSPLogout;
	Proc_QISRSessionBegin QISRSessionBegin;
	Proc_QISRSessionEnd QISRSessionEnd;
	Proc_QISRAudioWrite QISRAudioWrite;
	Proc_QISRGetResult QISRGetResult;

	typedef std::map< unsigned int, Voice2TextSession* > SessionMap;
	SessionMap m_sessionMap;

	ON_COMPLETE_CALLBACK m_completeCallback;

public:
	IFlyEngine();
	~IFlyEngine();

	static IFlyEngine* getInstance(void) { return s_Instance; }
	static IFlyEngine* s_Instance;

	friend class Voice2TextSession;
};
