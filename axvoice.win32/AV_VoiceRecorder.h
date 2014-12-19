#pragma once

class VoiceRecorder
{
public:
	typedef void (CALLBACK *ON_COMPLETE_CALLBACK)(unsigned int, bool, const std::string&);  
	bool beginRecord(unsigned int voiceID, const std::string& localFile, ON_COMPLETE_CALLBACK cbComplete);
	void completeRecord(unsigned int voiceID);

private:
	ON_COMPLETE_CALLBACK m_completeCallback;

	WAVEFORMATEX	m_waveformat;
	HWAVEIN			m_hWaveIn;
	WAVEHDR			m_pWaveHdr;
	char *			m_bufRec;
	unsigned int	m_currentVoiceID;
	std::string		m_strLocalFile;

	bool			m_recordSuccess;
	std::string		m_errorCode;

	HANDLE			m_threadHandle;
	HANDLE			m_completeSignal;
	HANDLE			m_abortSignal;


private:
	static void CALLBACK _waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

	static UINT CALLBACK _recordThread(void* pParam);
	bool recordThread(std::string& error);

	bool _completeRecord(const char* szSaveFile, std::string& error);

public:
	VoiceRecorder();
	~VoiceRecorder();

	static VoiceRecorder* getInstance(void) { return s_Instance; }
	static VoiceRecorder* s_Instance;
};