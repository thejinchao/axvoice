/***************************************************

					AXIA|Voice

						(C) Copyright  JinChao. 2014
***************************************************/


#include "stdafx.h"
#include "AV_Interface.h"
#include "AV_VoiceManager.h"
#include "AV_VoiceItem.h"
#include "AV_VoiceRecorder.h"
#include "AV_Global.h"
#include "AV_VoicePlayer.h"
#include "AV_Util.h"
#include "AV_MessageQueue.h"
#include "AV_Voice2TextWrap.h"

//--------------------------------------------------------------------------------------------
bool AxVoice_Init(const char* cachePath, const char* uploadURL, const char* iflyID)
{
	AutoLock autoLock(&g_lockInterface);

	bool success = true;
	if(cachePath&&cachePath[0]!=0)
		g_cacheAudioPath = cachePath;

	if(uploadURL && uploadURL[0]!=0)
		g_uploadURL = uploadURL;

	if(iflyID && iflyID[0]!=0)
		success = IFlyEngine::getInstance()->initEngine(iflyID);

	return success;
}

//--------------------------------------------------------------------------------------------
void CALLBACK _onRecordCompleteCallback(unsigned int voiceID, bool success, const std::string& strMD5)
{
	if(!success) return;

	{
		AutoLock autoLock(&g_lockInterface);

		//find the voice item
		VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
		if(item==0) return; //TODO: ERROR

		//confirm local file
		item->confirmLocalFile(strMD5);
	}
}

//--------------------------------------------------------------------------------------------
unsigned int AxVoice_BeginRecord(void)
{
	AutoLock autoLock(&g_lockInterface);

	//create item
	VoiceItem* newItem = VoiceManager::getInstance()->allocateNewItem();
	if(newItem==0) return 0; //TODO: ERROR

	//begin record
	newItem->startRecord();

	VoiceRecorder* recorder = VoiceRecorder::getInstance();
	recorder->beginRecord(newItem->getVoiceID(), 
		newItem->getLocalWavFile(), 
		newItem->getLocalAmrFile(), _onRecordCompleteCallback);

	return newItem->getVoiceID();
}

//--------------------------------------------------------------------------------------------
void AxVoice_CompleteRecord(unsigned int voiceID)
{
	AutoLock autoLock(&g_lockInterface);

	//find the voice item
	VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
	if(item==0) return; //TODO: ERROR

	//check status
	if(item->getLocalStatus() != VoiceItem::RECORDING)
	{
		//TODO: ERROR
		return;
	}
	item->stopRecord();

	//complete record
	VoiceRecorder* recorder = VoiceRecorder::getInstance();
	recorder->completeRecord(voiceID);
}

//--------------------------------------------------------------------------------------------
void CALLBACK _onUploadCompleteCallback(unsigned int voiceID, bool success, const char* szServerURL)
{
	if(!success) return;

	{
		AutoLock autoLock(&g_lockInterface);

		VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
		if(item==0) return; //TODO: ERROR
	
		item->updateURL(szServerURL);
	}
}

//--------------------------------------------------------------------------------------------
void AxVoice_UploadVoice(unsigned int voiceID)
{
	AutoLock autoLock(&g_lockInterface);

	//find the voice item
	VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
	if(item==0) return; //TODO: ERROR

	if(item->getLocalStatus() != VoiceItem::HAS_LOCAL_FILE)
	{
		//TODO: ERROR, "local file invalid"
		return;
	}
	if(item->getServerStatus() != VoiceItem::NO_SERVER_FILE)
	{
		//TODO: ERROR, "server file busy"
		return;
	}

	if(!(item->beginUpload(_onUploadCompleteCallback)))
	{
		//TODO: ERROR, "begin upload thread failed"
		return;
	}
}

//--------------------------------------------------------------------------------------------
unsigned int AxVoice_CreatVoice(const char* szServerURL)
{
	AutoLock autoLock(&g_lockInterface);

	//create item
	VoiceItem* newItem = VoiceManager::getInstance()->allocateNewItem();
	if(newItem==0) return 0; //TODO: ERROR

	newItem->updateURL(szServerURL);
	return newItem->getVoiceID();
}

//--------------------------------------------------------------------------------------------
void CALLBACK _onDownloadCompleteCallback(unsigned int voiceID, bool success, const std::string& strMD5)
{
	if(!success) return;

	{
		AutoLock autoLock(&g_lockInterface);

		VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
		if(item==0) return; //TODO: ERROR
	
		item->confirmLocalFile(strMD5);
	}
}

//--------------------------------------------------------------------------------------------
void AxVoice_DownloadVoice(unsigned int voiceID)
{
	AutoLock autoLock(&g_lockInterface);

	//find the voice item
	VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
	if(item==0) return; //TODO: ERROR

	if(item->getLocalStatus() != VoiceItem::NO_LOCAL_FILE)
	{
		//TODO: ERROR
		return;
	}

	if(item->getServerStatus() != VoiceItem::HAS_SERVER_FILE)
	{
		//TODO: ERROR
		return;
	}

	//begin download
	if(!(item->beginDownload(_onDownloadCompleteCallback)))
	{
		//TODO: ERROR
		return;
	}

	//OK!
	return;
}

//--------------------------------------------------------------------------------------------
void AxVoice_PlayVoice(unsigned int voiceID)
{
	AutoLock autoLock(&g_lockInterface);

	//find the voice item
	VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
	if(item==0) return; //TODO: ERROR

	if(item->getLocalStatus() != VoiceItem::HAS_LOCAL_FILE)
	{
		//TODO: ERROR
		return;
	}

	//Play local amr file
	VoicePlayer* player = VoicePlayer::getInstance();
	
	//is playing?
	player->reset();
	player->startPlay(voiceID, item->getLocalWavFile().c_str());
}

//--------------------------------------------------------------------------------------------
void AxVoice_StopVoice(void)
{
	AutoLock autoLock(&g_lockInterface);

	VoicePlayer* player = VoicePlayer::getInstance();
	player->reset();
}

//--------------------------------------------------------------------------------------------
void AxVoice_DispatchMessage(AxVoiceCallback* cb)
{
	MessageQueue::getInstance()->dispatchMessage(cb);
}

//--------------------------------------------------------------------------------------------
void CALLBACK _onConvertToTextCompleteCallback(unsigned int voiceID, bool success, const std::string& strText)
{
	if(!success) return;

	{
		AutoLock autoLock(&g_lockInterface);

		VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
		if(item==0) return; //TODO: ERROR
	
		item->setText(strText.c_str());
	}
}

//--------------------------------------------------------------------------------------------
bool AxVoice_Voice2Text(unsigned int voiceID)
{
	AutoLock autoLock(&g_lockInterface);

	//find the voice item
	VoiceItem* item = VoiceManager::getInstance()->findItem(voiceID);
	if(item==0) return false; //TODO: ERROR

	if(item->getLocalStatus() != VoiceItem::HAS_LOCAL_FILE)
	{
		//TODO: ERROR
		return false;
	}

	//begin send to server
	return IFlyEngine::getInstance()->beginConvertVoice(voiceID, _onConvertToTextCompleteCallback);
}
