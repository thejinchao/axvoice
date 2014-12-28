#include "stdafx.h"
#include "AV_Global.h"

std::string g_cacheAudioPath = "";
std::string g_uploadURL = "http://www.dashengine.com/upload_voice";
CRITICAL_SECTION g_lockInterface;
