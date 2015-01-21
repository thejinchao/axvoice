#include "stdafx.h"
#include "AV_Global.h"
#include "AV_VoiceManager.h"
#include "AV_VoiceRecorder.h"
#include "AV_VoicePlayer.h"
#include "AV_MessageQueue.h"
#include "AV_Voice2TextWrap.h"

//--------------------------------------------------------------------------------------------
void initGlobalValue(HINSTANCE hinstDLL)
{
	InitializeCriticalSection(&g_lockInterface);

	char szTempPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, szTempPath);

	PathAppendA(szTempPath, "axvoice_cache_audio");
	::CreateDirectoryA(szTempPath, 0);

	g_hModuleHandle = hinstDLL;
	g_cacheAudioPath = szTempPath;

	//create message queue
	new MessageQueue();

	//create voice manager
	new VoiceManager();

	//create audio recorder
	new VoiceRecorder();

	//create audio player
	new VoicePlayer();

	//create ifly engine
	new IFlyEngine();
}

//--------------------------------------------------------------------------------------------
void releaseGlobalValue(void)
{
	delete VoicePlayer::getInstance();
	delete VoiceRecorder::getInstance();
	delete VoiceManager::getInstance();

	DeleteCriticalSection(&g_lockInterface);
}

//--------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(
	_In_  HINSTANCE hinstDLL,
	_In_  DWORD fdwReason,
	_In_  LPVOID lpvReserved
	)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// init global value
		initGlobalValue(hinstDLL);
		break;

	case DLL_PROCESS_DETACH:
		//release global value
		releaseGlobalValue();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

