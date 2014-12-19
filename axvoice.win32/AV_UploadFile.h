#pragma once

class UploadFile
{
public:
	typedef void (CALLBACK *ON_COMPLETE_CALLBACK)(unsigned int, bool, const char*);  

	bool beginUploadThread(void);

private:
	static UINT CALLBACK _uploadThread(void* pParam);
	void uploadThread(void);

	static size_t _curlWriteMemory(void *ptr, size_t size, size_t nmemb, void *data);

	bool _getDownloadURL(const char* szResult, std::string& downloadURL);

private:
	ON_COMPLETE_CALLBACK m_completeCallback;
	unsigned int m_voiceID;
	std::string m_uploadURL;
	std::string m_uploadName;
	std::string m_localFile;
	bool m_success;

	HANDLE m_hThread;

	enum { MAX_RECEIVE_LENGTH=1024 };
	char m_result[MAX_RECEIVE_LENGTH];
	int m_currentMemoryPoint;

	std::string m_errorCode;

public:
	UploadFile(unsigned int voiceID, 
		const std::string& uploadURL, const std::string& uploadName, const std::string& localFile, 
		ON_COMPLETE_CALLBACK cbComplete);
	~UploadFile();
};
