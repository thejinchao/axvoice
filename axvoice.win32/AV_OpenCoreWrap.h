#pragma once

bool convertAMRtoWAV(const char* szAMR, const char* szWAV);

bool encodePCMToAMR(const char* PCM, int pcmBytes, const char* szAMRFile);
