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

	char locaAmrlFilePath[MAX_PATH];
	::StringCbCopyA(locaAmrlFilePath, MAX_PATH, g_cacheAudioPath.c_str());
	
	char localAmrFileName[MAX_PATH];
	StringCchPrintfA(localAmrFileName, MAX_PATH, "%d.amr", currentTime);

	PathAppendA(locaAmrlFilePath, localAmrFileName);

	char localWavFilePath[MAX_PATH];
	StringCbCopyA(localWavFilePath, MAX_PATH, locaAmrlFilePath);
	::PathRenameExtensionA(localWavFilePath, ".wav");

	VoiceItem *item = new VoiceItem(currentTime, locaAmrlFilePath, localWavFilePath, 0, 0, 
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
