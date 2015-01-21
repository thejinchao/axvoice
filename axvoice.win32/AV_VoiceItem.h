#pragma once

#include "AV_UploadFile.h"
#include "AV_DownloadFile.h"

class VoiceItem
{
public:
	enum LocalStatus { NO_LOCAL_FILE, RECORDING, STOP_RECORDING, DOWNLOADING, HAS_LOCAL_FILE };
	enum ServerStatus { NO_SERVER_FILE, UPLOADING, HAS_SERVER_FILE };
	enum ToTextStatus { NO_TEXT, CONVERTING, HAS_TEXT };

	unsigned int getVoiceID(void) const { return voiceID;  }
	const std::string& getLocalAmrFile(void) const { return localAmrFile; }
	const std::string& getLocalWavFile(void) const { return localWavFile; }
	const std::string& getServerFile(void) const { return serverURL; }
	const std::string& getMD5(void) const { return fileMD5; }
	const std::string& getText(void) const { return strText; }  //utf8 code

	LocalStatus getLocalStatus(void) const { return localStatus; }
	ServerStatus getServerStatus(void) const { return serverStatus; }
	ToTextStatus getTextStatus(void) const { return toTextStatus; }

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

	//begin convert To text
	bool beginConvertToText(void);

	//set text
	bool setText(const char* text);

private:
	unsigned int voiceID;
	std::string localAmrFile;
	std::string localWavFile;
	std::string fileMD5;	//amr file md5
	std::string serverURL;
	std::string strText;

	LocalStatus localStatus;
	ServerStatus serverStatus;
	ToTextStatus toTextStatus;
	
	UploadFile* uploader;
	DownloadFile* downloader;

public:
	VoiceItem(unsigned int voiceID, 
		const char* localAmrFile,
		const char* localWavFile,
		const char* fileMD5, 
		const char* serverURL,
		LocalStatus localStatus, 
		ServerStatus serverStatus,
		ToTextStatus toTextStatus);
};