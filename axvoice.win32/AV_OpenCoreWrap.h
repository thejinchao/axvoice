#pragma once

bool convertAMRtoWAV(const char* szAMR, const char* szWAV);

bool encodePCMToAMR(const char* PCM, int pcmBytes, const char* szAMRFile);

bool writeWavFileHead(HANDLE hFile, size_t fmtDataSize);
