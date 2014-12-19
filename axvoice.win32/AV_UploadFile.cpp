#include "stdafx.h"
#include "AV_UploadFile.h"
#include "AV_Interface.h"
#include "AV_MessageQueue.h"

//--------------------------------------------------------------------------------------------
UploadFile::UploadFile(unsigned int voiceID, 
	const std::string& uploadURL, const std::string& uploadNameconst, 
	const std::string& localFile, 
	ON_COMPLETE_CALLBACK cbComplete)
	: m_uploadURL(uploadURL)
	, m_uploadName(uploadNameconst)
	, m_localFile(localFile)
	, m_hThread(0)
	, m_currentMemoryPoint(0)
	, m_success(false)
	, m_completeCallback(cbComplete)
	, m_voiceID(voiceID)
{
	memset(m_result, 0, MAX_RECEIVE_LENGTH);
}

//--------------------------------------------------------------------------------------------
UploadFile::~UploadFile()
{
}

//--------------------------------------------------------------------------------------------
bool UploadFile::beginUploadThread(void)
{
	//create record thread
	UINT nThreadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, _uploadThread, this, CREATE_SUSPENDED, &nThreadID);
	if (m_hThread == 0) return false;

	::ResumeThread(m_hThread);
	return true;
}

//--------------------------------------------------------------------------------------------
UINT CALLBACK UploadFile::_uploadThread(void* pParam)
{
	UploadFile* pThis = (UploadFile*)pParam;
	pThis->uploadThread();
	::_endthreadex(0);
	return 0;
}

//--------------------------------------------------------------------------------------------
size_t UploadFile::_curlWriteMemory(void *ptr, size_t size, size_t nmemb, void *data)
{
	UploadFile* pThis = (UploadFile*)data;

	size_t total_size = size*nmemb;
	memcpy(pThis->m_result + pThis->m_currentMemoryPoint, ptr, total_size);
	pThis->m_currentMemoryPoint += total_size;

	return total_size;
}

//--------------------------------------------------------------------------------------------
void UploadFile::uploadThread(void)
{
	CURL* curl_handle = curl_easy_init();

	curl_httppost *post_handle=0, *post_last=0;

	curl_formadd(&post_handle, &post_last, CURLFORM_PTRNAME, m_uploadName.c_str(), CURLFORM_FILE, 
		m_localFile.c_str(), CURLFORM_END);

	curl_easy_setopt(curl_handle, CURLOPT_URL, m_uploadURL.c_str());

	if(post_handle!=0)
		curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, post_handle);

	//开始接收数据
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _curlWriteMemory); 
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)this); 
	curl_easy_perform(curl_handle);

	long retcode=0;
	CURLcode ret = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &retcode); 
    if((ret == CURLE_OK) && (retcode == 200 || retcode==201) )
		m_success = true;
	else 
		m_success = false;

	//cleanup
	curl_easy_cleanup(curl_handle);

	//get download url
	if(m_success)
		m_success = _getDownloadURL(m_result, m_errorCode);

	//call complete callbcak function
	if(m_completeCallback) m_completeCallback(m_voiceID, m_success, m_errorCode.c_str());

	//callback message
	MessageQueue::getInstance()->pushMessage(AxVoiceMessage::MT_UPLOAD_MSG, m_voiceID, "complete", m_success, m_errorCode);
}

//--------------------------------------------------------------------------------------------
bool UploadFile::_getDownloadURL(const char* szResult, std::string& errorCode)
{
	const char* SUCCESS_CODE = "SUCCESS";
	const int SUCCESS_CODE_LEN = 7;

	const char* ERR_CODE = "ERROR";
	const int ERR_CODE_LEN = 5;

	const char* dot = strchr(szResult, '|');
	if(dot==0) return false;

	if(dot-szResult==SUCCESS_CODE_LEN && strncmp(SUCCESS_CODE, szResult, SUCCESS_CODE_LEN)==0)
	{
		errorCode = dot+1;
		errorCode = errorCode.substr(0, errorCode.find_last_not_of("\r\n")+1);
		return true;
	}

	if(dot-szResult==ERR_CODE_LEN && strncmp(ERR_CODE, szResult, ERR_CODE_LEN)==0)
	{
		errorCode = dot+1;
		return false;
	}
	
	errorCode="<ERROR_FORMAT>";
	return false;
}

