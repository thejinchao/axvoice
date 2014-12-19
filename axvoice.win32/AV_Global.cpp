#include "stdafx.h"
#include "AV_Global.h"

std::string g_cacheAudioPath = "";
//std::string g_uploadURL = "http://dashengine.com/upload_voice";
std::string g_uploadURL = "http://10.1.8.123/upload_voice";
CRITICAL_SECTION g_lockInterface;
