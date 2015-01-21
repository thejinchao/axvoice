#include "stdafx.h"
#include "AV_VoiceItem.h"
#include "AV_Util.h"
#include "AV_UploadFile.h"
#include "AV_Global.h"
#include "AV_DownloadFile.h"

//--------------------------------------------------------------------------------------------
VoiceItem::VoiceItem(unsigned int _voiceID, 
		const char* _localAmrFile,
		const char* _localWavFile,
		const char* _fileMD5, 
		const char* _serverURL,
		LocalStatus _localStatus, 
		ServerStatus _serverStatus,
		ToTextStatus _toTextStatus)
	: voiceID(_voiceID)
	, localAmrFile(_localAmrFile ? _localAmrFile : "")
	, localWavFile(_localWavFile ? _localWavFile : "")
	, fileMD5(_fileMD5 ? _fileMD5 : "" )
	, serverURL(_serverURL ? _serverURL : "")
	, localStatus(_localStatus)
	, serverStatus(_serverStatus)
	, toTextStatus(_toTextStatus)
	, uploader(0)
	, downloader(0)
{
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::startRecord(void) 
{ 
	//TODO: Check status
	localStatus = RECORDING; 
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::stopRecord(void)
{
	localStatus = STOP_RECORDING;
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::confirmLocalFile(const std::string& md5)
{ 
	//TODO: Check status

	this->fileMD5 = md5; 
	this->localStatus = HAS_LOCAL_FILE;
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::beginUpload(UploadFile::ON_COMPLETE_CALLBACK cbComplete)
{
	//check status
	if( getLocalStatus() != HAS_LOCAL_FILE ||
		getServerStatus() != NO_SERVER_FILE ||
		uploader != 0) return false;

	//upload name
	char szName[64];
	StringCbPrintfA(szName, 64, "-%s-", fileMD5.c_str());

	//create uploader
	this->uploader = new UploadFile(getVoiceID(), g_uploadURL, szName, getLocalAmrFile(), cbComplete);
	if(!(this->uploader->beginUploadThread())) return false;

	this->serverStatus = UPLOADING;
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::updateURL(const char* szURL)
{
	//TODO: check status
	if(getServerStatus() == HAS_SERVER_FILE) return false;

	this->serverURL = szURL;
	this->serverStatus = HAS_SERVER_FILE;
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::beginDownload(DownloadFile::ON_COMPLETE_CALLBACK cbComplete)
{
	//check status
	if( getLocalStatus() != NO_LOCAL_FILE ||
		getServerStatus() != HAS_SERVER_FILE ||
		downloader != 0) return false;

	//create downloader
	this->downloader = new DownloadFile(this->getVoiceID(), this->getServerFile(), 
		this->getLocalAmrFile(), this->getLocalWavFile(),
		cbComplete);
	if(!(this->downloader->beginDownloadThread())) return false;

	this->localStatus = DOWNLOADING;
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::beginConvertToText(void)
{
	//check status
	if(	getLocalStatus() != HAS_LOCAL_FILE ||
		getTextStatus() != NO_TEXT) return false;

	toTextStatus = CONVERTING;
	return true;
}

//--------------------------------------------------------------------------------------------
bool VoiceItem::setText(const char* text)
{
	//check status
	if(	getLocalStatus() != HAS_LOCAL_FILE ||
		getTextStatus() != CONVERTING) return false;

	toTextStatus = HAS_TEXT;
	strText = text;
	return true;
}
