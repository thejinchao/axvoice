/***********************************************************************
 * testaudio.h
 *  
 *    Audio Library
 *
 *
 * Toby Opferman Copyright (c) 2003
 *
 ***********************************************************************/


#ifndef __WAVELIB_H__
#define __WAVELIB_H__


typedef PVOID HWAVELIB;

#ifdef __cplusplus
extern "C" {
#endif


HWAVELIB WaveLib_Init(LPCSTR pAudioFile, BOOL bPause);
HANDLE WaveLib_GetThreadHandle(HWAVELIB hWaveLib);
void WaveLib_UnInit(HWAVELIB hWaveLib);
void WaveLib_Pause(HWAVELIB hWaveLib, BOOL bPause);

#ifdef __cplusplus
}
#endif



#endif

