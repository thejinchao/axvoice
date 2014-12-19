#include "stdafx.h"
#include "AV_VoiceManager.h"
#include "AV_Global.h"
#include "AV_VoiceItem.h"

//--------------------------------------------------------------------------------------------
VoiceManager* VoiceManager::s_Instance = 0;

//--------------------------------------------------------------------------------------------
VoiceManager::VoiceManager()
{
	s_Instance = this;
}

//--------------------------------------------------------------------------------------------
VoiceManager::~VoiceManager()
{
}

//--------------------------------------------------------------------------------------------
VoiceItem* VoiceManager::allocateNewItem(void)
{
	__time32_t currentTime = _time32(0);

	char localFilePath[MAX_PATH];
	::StringCbCopyA(localFilePath, MAX_PATH, g_cacheAudioPath.c_str());
	
	char localFileName[MAX_PATH];
	StringCchPrintfA(localFileName, MAX_PATH, "%d.amr", currentTime);

	PathAppendA(localFilePath, localFileName);

	VoiceItem *item = new VoiceItem(currentTime, localFilePath, 0, 0, 
		VoiceItem::NO_LOCAL_FILE, VoiceItem::NO_SERVER_FILE);
	{
		m_voiceMap.insert(std::make_pair(item->getVoiceID(), item));
	}
	return item;
}

//--------------------------------------------------------------------------------------------
VoiceItem* VoiceManager::findItem(unsigned int voiceID)
{
	VoiceMap::iterator it = m_voiceMap.find(voiceID);
	if(it==m_voiceMap.end()) return 0;

	return it->second;
}
