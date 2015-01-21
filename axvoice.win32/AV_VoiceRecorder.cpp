#include "stdafx.h"
#include "AV_VoiceRecorder.h"
#include "AV_Interface.h"
#include "AV_OpenCoreWrap.h"
#include "AV_Util.h"
#include "AV_MessageQueue.h"

//--------------------------------------------------------------------------------------------
VoiceRecorder* VoiceRecorder::s_Instance = 0;

//--------------------------------------------------------------------------------------------
VoiceRecorder::VoiceRecorder()
	: m_hWaveIn(0)
	, m_bufRec(0)
	, m_currentVoiceID(0)
	, m_threadHandle(0)
	, m_completeSignal(0)
	, m_abortSignal(0)
	, m_recordSuccess(false)
{
	s_Instance = this;

	m_waveformat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveformat.nChannels = 1;
	m_waveformat.nSamplesPerSec = 16000;
	m_waveformat.nAvgBytesPerSec = 32000;
	m_waveformat.nBlockAlign = 2;
	m_waveformat.wBitsPerSample = 16;
	m_waveformat.cbSize = 0;

	m_completeSignal = ::CreateEvent(0, TRUE, FALSE, 0);
	m_abortSignal = ::CreateEvent(0, TRUE, FALSE, 0);
}

//--------------------------------------------------------------------------------------------
VoiceRecorder::~VoiceRecorder()
{
	::CloseHandle(m_completeSignal); m_completeSignal = NULL;
	::CloseHandle(m_abortSignal); m_abortSignal = NULL;
}

//--------------------------------------------------------------------------------------------
bool VoiceRecorder::beginRecord(unsigned int voiceID, 
		const std::string& localWavFile, 
		const std::string& localAmrFile, 
		ON_COMPLETE_CALLBACK cbComplete)
{
	if (voiceID == m_currentVoiceID) return true; //already record

	if (m_threadHandle != NULL && WAIT_TIMEOUT == WaitForSingleObject(m_threadHandle, 0)) //still running
	{
		//begin another recorder? close this now
		::SetEvent(m_abortSignal);
		::WaitForSingleObject(m_threadHandle, INFINITE);
		::CloseHandle(m_threadHandle); m_threadHandle=0;
	}

	//create record thread
	UINT nThreadID;
	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, _recordThread, this, CREATE_SUSPENDED, &nThreadID);
	if (m_threadHandle == 0) return false;

	m_strLocalWavFile = localWavFile;
	m_strLocalAmrFile = localAmrFile;
	m_currentVoiceID = voiceID;
	m_recordSuccess = false;
	m_errorCode = "";
	m_completeCallback = cbComplete;

	//begin
	ResumeThread(m_threadHandle);

	return true;
}

//--------------------------------------------------------------------------------------------
void VoiceRecorder::completeRecord(unsigned int voiceID)
{
	//voice id not correct
	if (m_currentVoiceID != voiceID) return;

	//wait for quit
	if (m_threadHandle != NULL && WAIT_TIMEOUT == WaitForSingleObject(m_threadHandle, 0)) //still running
	{
		//close this now
		::SetEvent(m_completeSignal);
	}
}

//--------------------------------------------------------------------------------------------
UINT CALLBACK VoiceRecorder::_recordThread(void* pParam)
{
	VoiceRecorder* pThis = getInstance();

	bool ret = pThis->recordThread(pThis->m_errorCode);

	::ResetEvent(pThis->m_completeSignal);
	::ResetEvent(pThis->m_abortSignal);

	::_endthreadex(0);
	return 0;
}

//--------------------------------------------------------------------------------------------
bool VoiceRecorder::recordThread(std::string& error)
{
	int ret = 0;

	//open wave device
	ret = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveformat, (DWORD)_waveOutProc, 0L, CALLBACK_FUNCTION);
	if (ret != MMSYSERR_NOERROR){
		error = "waveInOpen is failed";
		return false;
	}

	DWORD datasize = 1024*1024; //1MB(about 1 minute)
	m_bufRec = (char *)GlobalLock(GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, datasize));
	if (m_bufRec == NULL){
		error = "GlobalAlloc Failed";
		return false;
	}
	memset(m_bufRec, 0, datasize);

	m_pWaveHdr.lpData = m_bufRec;
	m_pWaveHdr.dwBufferLength = datasize;
	m_pWaveHdr.dwBytesRecorded = 0;
	m_pWaveHdr.dwUser = 0;
	m_pWaveHdr.dwFlags = 0;
	m_pWaveHdr.dwLoops = 0;

	ret = waveInPrepareHeader(m_hWaveIn, &m_pWaveHdr, sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR){
		error = "waveInPrepareHeader Failed";
		return false;
	}

	ret = waveInAddBuffer(m_hWaveIn, &m_pWaveHdr, sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR){
		error = "waveInAddBuffer Failed";
		return false;
	}

	ret = waveInStart(m_hWaveIn);
	if (ret != MMSYSERR_NOERROR){
		error = "waveInStart Failed";
		return false;
	}

	//-----------------------
	//wait for end
	//-----------------------
	HANDLE handle[2];
	handle[0] = m_completeSignal;
	handle[1] = m_abortSignal;

	ret = ::WaitForMultipleObjects(2, handle, FALSE, INFINITE);
	if (ret == WAIT_OBJECT_0){
		//complete
		m_recordSuccess = _completeRecord(true, error);
	}
	else if (ret == WAIT_OBJECT_0 + 1){
		//abort!
		_completeRecord(false, error);
		m_recordSuccess = false;
	}

	//get md5
	std::string md5;
	if(m_recordSuccess)
	{
		md5 = md5File(m_strLocalAmrFile.c_str());
		if(md5=="") m_recordSuccess=false;
	}

	//callback
	if(m_completeCallback)
		m_completeCallback(m_currentVoiceID, m_recordSuccess, md5);

	//push message
	std::vector< std::string > params;
	params.push_back("complete");
	params.push_back(m_recordSuccess ? "true" : "false");
	params.push_back(md5);
	MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_RECORD_MSG, m_currentVoiceID, params);

	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceRecorder::_completeRecord(bool save, std::string& error)
{
	MMTIME mmt;
	mmt.wType = TIME_BYTES;

	int ret;
	ret = waveInGetPosition(m_hWaveIn, &mmt, sizeof(MMTIME));
	if (ret != MMSYSERR_NOERROR){
		error = "waveInGetPosition";
		return false;
	}

	if (mmt.wType != TIME_BYTES) {
		error = "TIME_BYTES format not supported!";
		return false;
	}

	if (waveInReset(m_hWaveIn))	{
		error = "waveInReset Failed";
		return false;
	}

	m_pWaveHdr.dwBytesRecorded = mmt.u.cb;

	if(save)
	{
		//save local wav file first
		if(!_writeToWavFile(error)) 
		{
			return false;
		}

		//encode to amr file
		if(!encodePCMToAMR(m_pWaveHdr.lpData, m_pWaveHdr.dwBytesRecorded, m_strLocalAmrFile.c_str()))
		{
			//TODO: ERROR
			return false;
		}
	}

	waveInUnprepareHeader(m_hWaveIn, &m_pWaveHdr, sizeof(WAVEHDR));
	GlobalFree(GlobalHandle(m_bufRec));
	waveInClose(m_hWaveIn);
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceRecorder::_writeToWavFile(std::string& error)
{
	HANDLE hFile = CreateFileA(m_strLocalWavFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, 
                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwNumToWrite=0;

	writeWavFileHead(hFile, m_pWaveHdr.dwBytesRecorded);

	//write data
	WriteFile(hFile, m_pWaveHdr.lpData, m_pWaveHdr.dwBytesRecorded, &dwNumToWrite, NULL);

	SetEndOfFile(hFile); CloseHandle(hFile);
	return true;
}

//--------------------------------------------------------------------------------------------
void CALLBACK VoiceRecorder::_waveOutProc(
	HWAVEOUT hwo,
	UINT uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
	)
{
	if (uMsg == WIM_DATA){
		//TODO: overflow!
		WAVEHDR* pWaveHDR = (WAVEHDR*)dwParam1;
		//AxTrace("_waveOutProc(Overflow), size=%dK", (int)(pWaveHDR->dwBytesRecorded/1024));
	}
}
