#pragma once

class VoicePlayer
{
public:
	/// stop current play thread
	void reset(void);

	///create new play thread
	bool startPlay(unsigned int voiceID, const char* szWavFile);

private:
	unsigned int m_voiceID;
	std::string m_strWAVFile;
	HANDLE m_hThread;
	HANDLE m_hAbortSignal;

	static UINT CALLBACK _playThread(void* pParam);
	void playThread(void);

public:
	VoicePlayer();
	~VoicePlayer();

	static VoicePlayer* getInstance(void) { return s_Instance; }
	static VoicePlayer* s_Instance;
};
