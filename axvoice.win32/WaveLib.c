/***********************************************************************
 * wavelib.c
 *  
 *    Audio Library
 *
 *
 *  Supports .WAV files, Very Simplistic Parser
 *
 *
 * Toby Opferman Copyright (c) 2003
 *
 ***********************************************************************/
 
 
 #include <windows.h>
 #include <mmsystem.h>
 #include <strsafe.h>
 #include "wavelib.h"
 
 
 /***********************************************************************
  * Internal Structures
  ***********************************************************************/
typedef struct {
    
    UCHAR IdentifierString[4];
    DWORD dwLength;

} RIFF_CHUNK, *PRIFF_CHUNK;


typedef struct {

    WORD  wFormatTag;         // Format category
    WORD  wChannels;          // Number of channels
    DWORD dwSamplesPerSec;    // Sampling rate
    DWORD dwAvgBytesPerSec;   // For buffer estimation
    WORD  wBlockAlign;        // Data block size
    WORD  wBitsPerSample;
    

} WAVE_FILE_HEADER, *PWAVE_FILE_HEADER;


typedef struct _wave_sample {

     WAVEFORMATEX WaveFormatEx;
     char *pSampleData;
     UINT Index;
     UINT Size;
     DWORD dwId;
     DWORD bPlaying;
     struct _wave_sample *pNext;

} WAVE_SAMPLE, *PWAVE_SAMPLE;
 
#define SAMPLE_SIZE				(2*2*2000) 
#define SAMPLE_BLOCK_COUNTS		(8)

typedef struct {
     
     HWAVEOUT hWaveOut;
     HANDLE hEvent;
     HANDLE hThread;
     WAVE_SAMPLE WaveSample;
     BOOL bWaveShouldDie;
	 INT nActiveBlockCounts;
     WAVEHDR WaveHdr[SAMPLE_BLOCK_COUNTS];
     char AudioBuffer[SAMPLE_BLOCK_COUNTS][SAMPLE_SIZE];
     BOOL bPaused;

} WAVELIB, *PWAVELIB;



 /***********************************************************************
  * Internal Functions
  ***********************************************************************/
void CALLBACK WaveLib_WaveOutputCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
BOOL WaveLib_OpenWaveSample(LPCSTR pFileName, PWAVE_SAMPLE pWaveSample);
void WaveLib_WaveOpen(HWAVEOUT hWaveOut, PWAVELIB pWaveLib);
void WaveLib_WaveDone(HWAVEOUT hWaveOut, PWAVELIB pWaveLib);
DWORD WINAPI WaveLib_AudioThread(PVOID pDataInput);
void WaveLib_CreateThread(PWAVELIB pWaveLib);
void WaveLib_SetupAudio(PWAVELIB pWaveLib);
void WaveLib_WaveClose(HWAVEOUT hWaveOut, PWAVELIB pWaveLib);
BOOL WaveLib_AudioBuffer(PWAVELIB pWaveLib, UINT Index);


 /***********************************************************************
  * WaveLib_Init
  *  
  *    Audio!
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
 HWAVELIB WaveLib_Init(LPCSTR pWaveFile, BOOL bPause)
 {
     PWAVELIB pWaveLib = NULL;
 
     if(pWaveLib = (PWAVELIB)LocalAlloc(LMEM_ZEROINIT, sizeof(WAVELIB)))
     {
         pWaveLib->bPaused = bPause;

         if(WaveLib_OpenWaveSample(pWaveFile, &pWaveLib->WaveSample))
         {
             if(waveOutOpen(&pWaveLib->hWaveOut, WAVE_MAPPER, &pWaveLib->WaveSample.WaveFormatEx, (ULONG)WaveLib_WaveOutputCallback, (ULONG)pWaveLib, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
             {
                WaveLib_UnInit((HWAVELIB)pWaveLib);
                pWaveLib = NULL;
             }
             else
             {
 
                 if(pWaveLib->bPaused)
                 {
                     waveOutPause(pWaveLib->hWaveOut);
                 }

                 WaveLib_CreateThread(pWaveLib);
             }
         }
         else
         {
             WaveLib_UnInit((HWAVELIB)pWaveLib);
             pWaveLib = NULL;
         }
     }

     return (HWAVELIB)pWaveLib;
 }



  /***********************************************************************
  * WaveLib_GetThreadHandle
  *  
  *    Audio!
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Thread Handle To This Audio Session
  *
  ***********************************************************************/

HANDLE WaveLib_GetThreadHandle(HWAVELIB hWaveLib)
{
     PWAVELIB pWaveLib = (PWAVELIB)hWaveLib;
	 return pWaveLib->hThread;
}




 /***********************************************************************
  * WaveLib_Pause
  *  
  *    Audio!
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
 void WaveLib_Pause(HWAVELIB hWaveLib, BOOL bPause)
 {
     PWAVELIB pWaveLib = (PWAVELIB)hWaveLib;

     pWaveLib->bPaused = bPause;

     if(pWaveLib->bPaused)
     {
         waveOutPause(pWaveLib->hWaveOut);
     }
     else
     {
         waveOutRestart(pWaveLib->hWaveOut);
     }
 }

 /***********************************************************************
  * WaveLib_Init
  *  
  *    Audio!
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
 void WaveLib_UnInit(HWAVELIB hWaveLib)
 {
     PWAVELIB pWaveLib = (PWAVELIB)hWaveLib;

     if(pWaveLib)
     {
         if(pWaveLib->hThread)
         {
             pWaveLib->bWaveShouldDie = TRUE;

             SetEvent(pWaveLib->hEvent);
             WaitForSingleObject(pWaveLib->hThread, INFINITE);

             CloseHandle(pWaveLib->hEvent);
             CloseHandle(pWaveLib->hThread);
         }

         if(pWaveLib->hWaveOut)
         {
             waveOutClose(pWaveLib->hWaveOut);
         }


         if(pWaveLib->WaveSample.pSampleData)
         {
             LocalFree(pWaveLib->WaveSample.pSampleData);
         }

         LocalFree(pWaveLib);
     }

 }
 
 
 /***********************************************************************
  * WaveLib_WaveOutputCallback
  *  
  *    Audio Callback 
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/ 
void CALLBACK WaveLib_WaveOutputCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    PWAVELIB pWaveLib = (PWAVELIB)dwInstance;

    switch(uMsg)
    {
      case WOM_OPEN:
            WaveLib_WaveOpen(hwo, pWaveLib);
            break;

       case WOM_DONE:
            WaveLib_WaveDone(hwo, pWaveLib);
            break;

       case WOM_CLOSE:
            WaveLib_WaveClose(hwo, pWaveLib);
            break;
    }
}



 
 /***********************************************************************
  * WaveLib_WaveOpen
  *  
  *    Audio Callback 
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
void WaveLib_WaveOpen(HWAVEOUT hWaveOut, PWAVELIB pWaveLib)
{
  // Do Nothing
}


 /***********************************************************************
  * WaveLib_WaveDone
  *  
  *    Audio Callback 
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
void WaveLib_WaveDone(HWAVEOUT hWaveOut, PWAVELIB pWaveLib)
{
	SetEvent(pWaveLib->hEvent);
	pWaveLib->nActiveBlockCounts -= 1;
}


 /***********************************************************************
  * WaveLib_WaveClose
  *  
  *    Audio Callback 
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
void WaveLib_WaveClose(HWAVEOUT hWaveOut, PWAVELIB pWaveLib)
{
  // Do Nothing
}



 /***********************************************************************
  * WaveLib_OpenWaveFile
  *  
  *    Audio Callback 
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
BOOL WaveLib_OpenWaveSample(LPCSTR pFileName, PWAVE_SAMPLE pWaveSample)
{
    BOOL bSampleLoaded = FALSE;
    HANDLE hFile;
    RIFF_CHUNK RiffChunk = {0};
    DWORD dwBytes, dwReturnValue;
    WAVE_FILE_HEADER WaveFileHeader;
    DWORD dwIncrementBytes;

    if(hFile = CreateFileA(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL))
    {
        char szIdentifier[5] = {0};

        SetFilePointer(hFile, 12, NULL, FILE_CURRENT);
        

        ReadFile(hFile, &RiffChunk, sizeof(RiffChunk), &dwBytes, NULL);
        ReadFile(hFile, &WaveFileHeader, sizeof(WaveFileHeader), &dwBytes, NULL);

        pWaveSample->WaveFormatEx.wFormatTag      = WaveFileHeader.wFormatTag;         
        pWaveSample->WaveFormatEx.nChannels       = WaveFileHeader.wChannels;          
        pWaveSample->WaveFormatEx.nSamplesPerSec  = WaveFileHeader.dwSamplesPerSec;    
        pWaveSample->WaveFormatEx.nAvgBytesPerSec = WaveFileHeader.dwAvgBytesPerSec;   
        pWaveSample->WaveFormatEx.nBlockAlign     = WaveFileHeader.wBlockAlign;  
        pWaveSample->WaveFormatEx.wBitsPerSample  = WaveFileHeader.wBitsPerSample;
        pWaveSample->WaveFormatEx.cbSize          = 0;

        dwIncrementBytes = dwBytes;

        do {
             SetFilePointer(hFile, RiffChunk.dwLength - dwIncrementBytes, NULL, FILE_CURRENT);
             
             dwReturnValue = GetLastError();

             if(dwReturnValue == 0)
             {
                 dwBytes = ReadFile(hFile, &RiffChunk, sizeof(RiffChunk), &dwBytes, NULL);
             
                 dwIncrementBytes = 0;

                 memcpy(szIdentifier, RiffChunk.IdentifierString, 4); 
             }

        } while(_stricmp(szIdentifier, "data") && dwReturnValue == 0) ;

        if(dwReturnValue == 0)
        {
            pWaveSample->pSampleData = (char *)LocalAlloc(LMEM_ZEROINIT, RiffChunk.dwLength);

            pWaveSample->Size = RiffChunk.dwLength;

            ReadFile(hFile, pWaveSample->pSampleData, RiffChunk.dwLength, &dwBytes, NULL);

            CloseHandle(hFile);

            bSampleLoaded = TRUE;
        }
    }

    return bSampleLoaded;
}





 /***********************************************************************
  * WaveLib_CreateThread
  *  
  *    Audio Callback 
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
void WaveLib_CreateThread(PWAVELIB pWaveLib)
{
    DWORD dwThreadId;

    pWaveLib->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    pWaveLib->hThread = CreateThread(NULL, 0, WaveLib_AudioThread, pWaveLib, 0, &dwThreadId);

}

 /***********************************************************************
  * WaveLib_AudioThread
  *  
  *    Audio Thread
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
DWORD WINAPI WaveLib_AudioThread(PVOID pDataInput)
{
    PWAVELIB pWaveLib = (PWAVELIB)pDataInput;
    DWORD dwReturnValue = 0;
    UINT Index;

    WaveLib_SetupAudio(pWaveLib);

    while(!pWaveLib->bWaveShouldDie && pWaveLib->nActiveBlockCounts>0)
    {
        WaitForSingleObject(pWaveLib->hEvent, INFINITE);

        for(Index = 0; Index < SAMPLE_BLOCK_COUNTS; Index++)
        {
            if(pWaveLib->WaveHdr[Index].dwFlags & WHDR_DONE)
            {
               if(WaveLib_AudioBuffer(pWaveLib, Index))
			   {
				   pWaveLib->nActiveBlockCounts += 1;
					waveOutWrite(pWaveLib->hWaveOut, &pWaveLib->WaveHdr[Index], sizeof(WAVEHDR));
			   }
            }
        }
    }

    waveOutReset(pWaveLib->hWaveOut);

    return dwReturnValue;
}




 /***********************************************************************
  * WaveLib_AudioMixer
  *  
  *    Audio Mixer
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
BOOL WaveLib_AudioBuffer(PWAVELIB pWaveLib, UINT Index)
{
	UINT uiRemainBytes = pWaveLib->WaveSample.Size - pWaveLib->WaveSample.Index;
	if(uiRemainBytes==0) return FALSE;

    pWaveLib->WaveHdr[Index].dwFlags &= ~WHDR_DONE;

    if(uiRemainBytes < SAMPLE_SIZE)
    {
        memcpy(pWaveLib->AudioBuffer[Index], pWaveLib->WaveSample.pSampleData + pWaveLib->WaveSample.Index, uiRemainBytes);

		pWaveLib->WaveSample.Index = pWaveLib->WaveSample.Size;

	    pWaveLib->WaveHdr[Index].dwBufferLength = uiRemainBytes;
    }
    else
    {
       memcpy(pWaveLib->AudioBuffer[Index], pWaveLib->WaveSample.pSampleData + pWaveLib->WaveSample.Index, SAMPLE_SIZE);

       pWaveLib->WaveSample.Index += SAMPLE_SIZE;
	   pWaveLib->WaveHdr[Index].dwBufferLength = SAMPLE_SIZE;
    }

    pWaveLib->WaveHdr[Index].lpData = pWaveLib->AudioBuffer[Index];
	return TRUE;
}






 /***********************************************************************
  * WaveLib_SetupAudio
  *  
  *    Audio Thread
  *
  * Parameters
  *     
  * 
  * Return Value
  *     Handle To This Audio Session
  *
  ***********************************************************************/
void WaveLib_SetupAudio(PWAVELIB pWaveLib)
{
    UINT Index = 0;
	pWaveLib->nActiveBlockCounts = 0;
    for(Index = 0; Index < SAMPLE_BLOCK_COUNTS; Index++)
    {
        pWaveLib->WaveHdr[Index].dwBufferLength = SAMPLE_SIZE;
        pWaveLib->WaveHdr[Index].lpData         = pWaveLib->AudioBuffer[Index]; 

		if(WaveLib_AudioBuffer(pWaveLib, Index)) 
		{
			pWaveLib->nActiveBlockCounts += 1;
			waveOutPrepareHeader(pWaveLib->hWaveOut, &pWaveLib->WaveHdr[Index], sizeof(WAVEHDR));


			waveOutWrite(pWaveLib->hWaveOut, &pWaveLib->WaveHdr[Index], sizeof(WAVEHDR));
		}

    }
}


