#include "stdafx.h"
#include "AV_DownloadFile.h"
#include "AV_Global.h"
#include "AV_Util.h"
#include "md5.h"
#include "AV_VoiceItem.h"
#include "AV_VoiceManager.h"
#include "AV_Interface.h"
#include "AV_MessageQueue.h"
#include "AV_OpenCoreWrap.h"

//--------------------------------------------------------------------------------------------
DownloadFile::DownloadFile(unsigned int voiceID, 
	const std::string& downloadURL, 
	const std::string& localAmrFile, 
	const std::string& localWavFile, 
	ON_COMPLETE_CALLBACK cbComplete)
	: m_downloadURL(downloadURL)
	, m_localAmrFile(localAmrFile)
	, m_localWavFile(localWavFile)
	, m_hThread(0)
	, m_curlTempMemory(0)
	, m_curlTempMemorySize(0)
	, m_curlTempMemoryTail(0)
	, m_voiceID(voiceID)
	, m_completeCallback(cbComplete)
{
}

//--------------------------------------------------------------------------------------------
DownloadFile::~DownloadFile()
{
	if(m_curlTempMemory)
	{
		free(m_curlTempMemory);
		m_curlTempMemory = 0;
	}
}

//--------------------------------------------------------------------------------------------
bool DownloadFile::beginDownloadThread(void)
{
	//create record thread
	UINT nThreadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, _downloadThread, this, CREATE_SUSPENDED, &nThreadID);
	if (m_hThread == 0) return false;

	::ResumeThread(m_hThread);
	return true;
}

//--------------------------------------------------------------------------------------------
UINT CALLBACK DownloadFile::_downloadThread(void* pParam)
{
	DownloadFile* pThis = (DownloadFile*)pParam;
	pThis->downloadThread();
	::_endthreadex(0);
	return 0;
}

//--------------------------------------------------------------------------------------------
void DownloadFile::downloadThread(void)
{
	CURL* curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, m_downloadURL.c_str());

	//receive data
	m_curlTempMemoryTail = 0;
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _curlWriteMemory); 
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)this); 
	curl_easy_perform(curl_handle);

	bool success=false;
	long retcode=0;
	CURLcode ret = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &retcode); 
    if((ret == CURLE_OK) && (retcode == 200 || retcode==201) )
	{
		//write local file
		success = _writeLocalFile();
	}

	//decode to wav file
	if(success)
	{
		success = convertAMRtoWAV(m_localAmrFile.c_str(), m_localWavFile.c_str());
	}

	//cleanup
	curl_easy_cleanup(curl_handle);

	//callback
	if(m_completeCallback)
		m_completeCallback(m_voiceID, success, m_md5);

	//callback message
	MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_DOWNLOAD_MSG, m_voiceID, "complete", success, success?m_md5.c_str():"");
}

//--------------------------------------------------------------------------------------------
bool DownloadFile::_writeLocalFile(void)
{
	FILE* fp=0;
	fopen_s(&fp, m_localAmrFile.c_str(), "wb");
	fwrite(m_curlTempMemory, 1, m_curlTempMemoryTail, fp);
	fclose(fp);

	//get md5
	MD5_CTX ctx;
	MD5Update(&ctx, m_curlTempMemory, m_curlTempMemoryTail);

	unsigned char md5_code[16];
	MD5Final(&ctx, md5_code);

	char chBuffer[128]={0};
	for(register int nCount = 0; nCount < 16; nCount++)
	{
		StringCbPrintfA(chBuffer+nCount*2, 128-nCount*2, "%02x", md5_code[nCount]);
	}
	m_md5 = chBuffer;

	return true;
}

//--------------------------------------------------------------------------------------------
size_t DownloadFile::_curlWriteMemory(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realSize = size * nmemb;
	DownloadFile* pThis = (DownloadFile*)data;

	//总内存长度
	size_t memSize = pThis->m_curlTempMemorySize;
	//已使用内存
	size_t oldSize = pThis->m_curlTempMemoryTail;

	//重新分配内存
	if(memSize-oldSize < realSize + 1)
	{
		size_t newSize = oldSize + realSize + 1;
		unsigned char* new_memory = (unsigned char*)malloc(newSize);

		//拷贝旧数据
		if(oldSize>0)
		{
			memcpy(new_memory, pThis->m_curlTempMemory, oldSize);
			free(pThis->m_curlTempMemory);
		}
		pThis->m_curlTempMemory = new_memory;
		pThis->m_curlTempMemorySize = newSize;
	}

	//接收新数据
	memcpy(pThis->m_curlTempMemory+oldSize, ptr, realSize);
	pThis->m_curlTempMemoryTail += realSize;

	return realSize;
} 
