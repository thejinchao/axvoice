#include "stdafx.h"
#include "AV_VoicePlayer.h"
#include "AV_OpenCoreWrap.h"
#include "AV_Interface.h"
#include "AV_MessageQueue.h"

#include "wavelib.h"

//--------------------------------------------------------------------------------------------
VoicePlayer* VoicePlayer::s_Instance = 0;

//--------------------------------------------------------------------------------------------
VoicePlayer::VoicePlayer()
	: m_hThread(0)
	, m_hAbortSignal(0)
	, m_voiceID(0)
{
	s_Instance = this;
	m_hAbortSignal = ::CreateEventA(0, TRUE, FALSE, 0);
}

//--------------------------------------------------------------------------------------------
VoicePlayer::~VoicePlayer()
{
	::CloseHandle(m_hAbortSignal); m_hAbortSignal=0;
}

//--------------------------------------------------------------------------------------------
void VoicePlayer::reset(void)
{
	::SetEvent(m_hAbortSignal);
}

//--------------------------------------------------------------------------------------------
bool VoicePlayer::startPlay(unsigned int voiceID, const char* szWavFile)
{
	//stop current thread first
	reset();

	//wait for thread close
	if(m_hThread!=NULL)
	{
		::WaitForSingleObject(m_hThread, 100);
		::CloseHandle(m_hThread); m_hThread=0;
	}

	//create record thread
	UINT nThreadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, _playThread, this, CREATE_SUSPENDED, &nThreadID);
	if (m_hThread == 0) return false;

	m_voiceID = voiceID;
	m_strWAVFile = szWavFile;

	ResetEvent(m_hAbortSignal);

	//start thread
	::ResumeThread(m_hThread);

	return true;
}

//--------------------------------------------------------------------------------------------
UINT CALLBACK VoicePlayer::_playThread(void* pParam)
{
	VoicePlayer::getInstance()->playThread();
	::_endthreadex(0);
	return 0;
}

//--------------------------------------------------------------------------------------------
void VoicePlayer::playThread(void)
{
	//check quit signal
	if(WAIT_OBJECT_0  == WaitForSingleObject(m_hAbortSignal, 0)) return;

	//begin play 
	HWAVELIB hWave = WaveLib_Init(m_strWAVFile.c_str(), FALSE);
	if(hWave==0) return; //TODO: ERROR
	
	//wait abort or sound done
	HANDLE hHandles[2];
	hHandles[0] = m_hAbortSignal;
	hHandles[1] = WaveLib_GetThreadHandle(hWave);
	DWORD ret = WaitForMultipleObjects(2, hHandles, FALSE, INFINITE);
	if(ret==WAIT_OBJECT_0)
	{
		//abort
	}else if(ret == WAIT_OBJECT_0+1)
	{
		//complete
	}

	//Close wave thread
	WaveLib_UnInit(hWave); hWave=0;

	//callback
	MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_PLAY_MSG, m_voiceID, "complete");
}

