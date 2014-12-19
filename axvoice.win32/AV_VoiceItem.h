#pragma once

#include "AV_UploadFile.h"
#include "AV_DownloadFile.h"

class VoiceItem
{
public:
	enum LocalStatus { NO_LOCAL_FILE, RECORDING, STOP_RECORDING, DOWNLOADING, HAS_LOCAL_FILE };
	enum ServerStatus { NO_SERVER_FILE, UPLOADING, HAS_SERVER_FILE };

	unsigned int getVoiceID(void) const { return voiceID;  }
	const std::string& getLocalFile(void) const { return localFilePath; }
	const std::string& getServerFile(void) const { return serverURL; }
	const std::string& getMD5(void) const { return fileMD5; }
	LocalStatus getLocalStatus(void) const { return localStatus; }
	ServerStatus getServerStatus(void) const { return serverStatus; }

	//start record
	bool startRecord(void);
	//stop record
	bool stopRecord(void);
	//after record or download
	bool confirmLocalFile(const std::string& md5);

	//begin upload
	bool beginUpload(UploadFile::ON_COMPLETE_CALLBACK cbComplete);

	//update url
	bool updateURL(const char* szURL);

	//begin download
	bool beginDownload(DownloadFile::ON_COMPLETE_CALLBACK cbComplete);

private:
	unsigned int voiceID;
	std::string localFilePath;
	std::string fileMD5;
	std::string serverURL;
	LocalStatus localStatus;
	ServerStatus serverStatus;
	
	UploadFile* uploader;
	DownloadFile* downloader;

public:
	VoiceItem(unsigned int voiceID, const char* localFilePath, const char* fileMD5, const char* serverURL,
		LocalStatus localStatus, ServerStatus serverStatus);
};