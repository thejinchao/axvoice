#include "stdafx.h"
#include "AV_OpenCoreWrap.h"

#include <interf_enc.h>
#include <interf_dec.h>

#define AMR_MAGIC_NUMBER "#!AMR\n"
#define MAX_AMR_FRAME_SIZE 32
#define PCM_FRAME_SIZE 160 // 8khz 8000*0.02=160

//--------------------------------------------------------------------------------------------
bool writeWavFileHead(HANDLE hFile, size_t fmtDataSize)
{
	typedef struct
	{
		char chRiffID[4];
		int nRiffSize;
		char chRiffFormat[4];
	}RIFFHEADER;

	typedef struct
	{
		char chChunkID[4];
		int nChunkSize;
	}XCHUNKHEADER;

	typedef struct
	{
		short nFormatTag;
		short nChannels;
		int nSamplesPerSec;
		int nAvgBytesPerSec;
		short nBlockAlign;
		short nBitsPerSample;
	}WAVE_FORMAT;

	if(hFile==0) return false;

	DWORD dwNumToWrite=0;

	//write riff head
	RIFFHEADER head;
	memcpy(head.chRiffID, "RIFF", 4);
	head.nRiffSize = sizeof(RIFFHEADER) + 
		sizeof(XCHUNKHEADER) + 
		sizeof(WAVE_FORMAT) +
		sizeof(XCHUNKHEADER) +
		fmtDataSize;
	memcpy(head.chRiffFormat, "WAVE", 4);
	WriteFile(hFile, &head, sizeof(head), &dwNumToWrite, NULL);

	//write wave format
	WAVE_FORMAT wf= {WAVE_FORMAT_PCM, 
		1, //mono channel
		8000, //16khz
		16000, 
		2, 
		16};
	XCHUNKHEADER chunk;
	memcpy(chunk.chChunkID, "fmt ", 4);
	chunk.nChunkSize = sizeof(WAVE_FORMAT);
	WriteFile(hFile, &chunk, sizeof(chunk), &dwNumToWrite, NULL);
	WriteFile(hFile, &wf, sizeof(wf), &dwNumToWrite, NULL);

	//data trunk
	memcpy(chunk.chChunkID, "data", 4);
	chunk.nChunkSize = fmtDataSize;
	WriteFile(hFile, &chunk, sizeof(chunk), &dwNumToWrite, NULL);

	return true;
}

//--------------------------------------------------------------------------------------------
int _caclAMRFrameSize(unsigned char frameHeader)
{
	int amrEncodeMode[] = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200}; // amr 编码方式
	#define myround(x) ((int)((x)+0.5))
	#define AMR_FRAME_COUNT_PER_SECOND 50

	int mode;
	int temp1 = 0;
	int temp2 = 0;
	int frameSize;
	
	temp1 = frameHeader;
	
	// 编码方式编号 = 帧头的3-6位
	temp1 &= 0x78; // 0111-1000
	temp1 >>= 3;
	
	mode = amrEncodeMode[temp1];
	
	// 计算amr音频数据帧大小
	// 原理: amr 一帧对应20ms，那么一秒有50帧的音频数据
	temp2 = myround((double)(((double)mode / (double)AMR_FRAME_COUNT_PER_SECOND) / (double)8));
	
	frameSize = myround((double)temp2 + 0.5);
	return frameSize;
}

//--------------------------------------------------------------------------------------------
// read first amr frame @return 0-error; 1-success
int _readAMRFrameFirst(FILE* fpamr, unsigned char frameBuffer[], int* stdFrameSize, unsigned char* stdFrameHeader)
{
	memset(frameBuffer, 0, sizeof(frameBuffer));
	
	// 先读帧头
	fread(stdFrameHeader, 1, sizeof(unsigned char), fpamr);
	if (feof(fpamr)) return 0;
	
	// 根据帧头计算帧大小
	*stdFrameSize = _caclAMRFrameSize(*stdFrameHeader);
	
	// 读首帧
	frameBuffer[0] = *stdFrameHeader;
	fread(&(frameBuffer[1]), 1, (*stdFrameSize-1)*sizeof(unsigned char), fpamr);
	if (feof(fpamr)) return 0;
	
	return 1;
}

//--------------------------------------------------------------------------------------------
int _readAMRFrame(FILE* fpamr, unsigned char frameBuffer[], int stdFrameSize, unsigned char stdFrameHeader)
{
	int bytes = 0;
	unsigned char frameHeader; // 帧头
	
	memset(frameBuffer, 0, sizeof(frameBuffer));
	
	// 读帧头
	// 如果是坏帧(不是标准帧头)，则继续读下一个字节，直到读到标准帧头
	while(true)
	{
		bytes = fread(&frameHeader, 1, sizeof(unsigned char), fpamr);
		if (feof(fpamr)) return 0;
		if (frameHeader == stdFrameHeader) break;
	}
	
	// 读该帧的语音数据(帧头已经读过)
	frameBuffer[0] = frameHeader;
	bytes = fread(&(frameBuffer[1]), 1, (stdFrameSize-1)*sizeof(unsigned char), fpamr);
	if (feof(fpamr)) return 0;
	
	return 1;
}

//--------------------------------------------------------------------------------------------
bool convertAMRtoWAV(const char* szAMR, const char* szWAV)
{
	//open amr file
	FILE* fpamr = 0;
	fopen_s(&fpamr, szAMR, "rb");
	if(fpamr==0) return false;

	// check file head
	char magic[8];
	fread(magic, sizeof(char), strlen(AMR_MAGIC_NUMBER), fpamr);
	if (strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
	{
		fclose(fpamr);
		return 0;
	}

	// temp wav file
	int nFrameCount = 0;
	DWORD dwNumWrited=0;
	HANDLE hFile = CreateFileA(szWAV, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	writeWavFileHead(hFile, 0);
	
	// init decoder
	void* handle_amr = Decoder_Interface_init();

	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];
	short pcmFrame[PCM_FRAME_SIZE];
	int stdFrameSize;
	unsigned char stdFrameHeader;

	// read first frame
	memset(amrFrame, 0, sizeof(amrFrame));
	memset(pcmFrame, 0, sizeof(pcmFrame));
	_readAMRFrameFirst(fpamr, amrFrame, &stdFrameSize, &stdFrameHeader);

	//decode amr to pcm
	Decoder_Interface_Decode(handle_amr, amrFrame, pcmFrame, 0);
	nFrameCount++;
	WriteFile(hFile, pcmFrame, sizeof(short)*PCM_FRAME_SIZE, &dwNumWrited, 0);

	// decode other frame
	while(true)
	{
		memset(amrFrame, 0, sizeof(amrFrame));
		memset(pcmFrame, 0, sizeof(pcmFrame));
		if (!_readAMRFrame(fpamr, amrFrame, stdFrameSize, stdFrameHeader)) break;
		
		// 解码一个AMR音频帧成PCM数据 (8k-16b-单声道)
		Decoder_Interface_Decode(handle_amr, amrFrame, pcmFrame, 0);
		nFrameCount++;
		WriteFile(hFile, pcmFrame, sizeof(short)*PCM_FRAME_SIZE, &dwNumWrited, 0);
	}
	Decoder_Interface_exit(handle_amr);

	// reset wave head
	DWORD dwCurrentPoint = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
	if(INVALID_SET_FILE_POINTER== SetFilePointer(hFile, 0, 0, FILE_BEGIN)) return false;

	writeWavFileHead(hFile, nFrameCount*160*sizeof(short));

	//reset to prev pointer
	SetFilePointer(hFile, dwCurrentPoint, 0, FILE_BEGIN);
	::SetEndOfFile(hFile); CloseHandle(hFile);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//for encoder
///////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
int _readPCMFrame(short speech[], const char* PCM, const char* PCM_end, int& pcmPoint, int nChannels, int nBitsPerSample)
{
	int nRead = 0;
	int x = 0, y = 0;

	// 原始PCM音频帧数据
	unsigned char  pcmFrame_8b1[PCM_FRAME_SIZE];
	unsigned char  pcmFrame_8b2[PCM_FRAME_SIZE << 1];
	unsigned short pcmFrame_16b1[PCM_FRAME_SIZE];
	unsigned short pcmFrame_16b2[PCM_FRAME_SIZE << 1];

	#define READ_MEMORY(pBegin, pEnd, point, nSize, target) \
		nRead = nSize; \
		if (pBegin + point + nRead > pEnd) nRead = pEnd - pBegin - point; \
		memcpy(target, pBegin + point, nRead); \
		point += nRead; 


	if (nBitsPerSample == 8 && nChannels == 1)
	{
		//nRead = fread(pcmFrame_8b1, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
		READ_MEMORY(PCM, PCM_end, pcmPoint, (nBitsPerSample / 8)*PCM_FRAME_SIZE*nChannels, pcmFrame_8b1);

//		nRead = (nBitsPerSample / 8)*PCM_FRAME_SIZE*nChannels;
	//	memcpy(pcmFrame_8b1, PCM + pcmPoint, nRead);
		//pcmPoint += nRead;

		for (x = 0; x < PCM_FRAME_SIZE; x++)
		{
			speech[x] = (short)((short)pcmFrame_8b1[x] << 7);
		}
	}
	else if (nBitsPerSample == 8 && nChannels == 2)
	{
		//nRead = fread(pcmFrame_8b2, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
		READ_MEMORY(PCM, PCM_end, pcmPoint, (nBitsPerSample / 8)*PCM_FRAME_SIZE*nChannels, pcmFrame_8b2);

		//nRead = (nBitsPerSample / 8)*PCM_FRAME_SIZE*nChannels;
		//memcpy(pcmFrame_8b2, PCM + pcmPoint, nRead);
		//pcmPoint += nRead;

		for (x = 0, y = 0; y < PCM_FRAME_SIZE; y++, x += 2)
		{
			// 1 - 取两个声道之左声道
			speech[y] = (short)((short)pcmFrame_8b2[x + 0] << 7);
			// 2 - 取两个声道之右声道
			//speech[y] =(short)((short)pcmFrame_8b2[x+1] << 7);
			// 3 - 取两个声道的平均值
			//ush1 = (short)pcmFrame_8b2[x+0];
			//ush2 = (short)pcmFrame_8b2[x+1];
			//ush = (ush1 + ush2) >> 1;
			//speech[y] = (short)((short)ush << 7);
		}
	}
	else if (nBitsPerSample == 16 && nChannels == 1)
	{
		//nRead = fread(pcmFrame_16b1, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
		READ_MEMORY(PCM, PCM_end, pcmPoint, (nBitsPerSample / 8)*PCM_FRAME_SIZE*nChannels, pcmFrame_16b1);
		//nRead = (nBitsPerSample / 8) * PCM_FRAME_SIZE*nChannels;
		//memcpy(pcmFrame_16b1, PCM + pcmPoint, nRead);
		//pcmPoint += nRead;

		for (x = 0; x < PCM_FRAME_SIZE; x++)
		{
			speech[x] = (short)pcmFrame_16b1[x + 0];
		}
	}
	else if (nBitsPerSample == 16 && nChannels == 2)
	{
		//nRead = fread(pcmFrame_16b2, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
		nRead = (nBitsPerSample / 8) * PCM_FRAME_SIZE*nChannels;
		memcpy(pcmFrame_16b2, PCM + pcmPoint, nRead);
		pcmPoint += nRead;

		for (x = 0, y = 0; y < PCM_FRAME_SIZE; y++, x += 2)
		{
			//speech[y] = (short)pcmFrame_16b2[x+0];
			speech[y] = (short)((int)((int)pcmFrame_16b2[x + 0] + (int)pcmFrame_16b2[x + 1])) >> 1;
		}
	}

	// 如果读到的数据不是一个完整的PCM帧, 就返回0
	if (nRead < PCM_FRAME_SIZE*nChannels) return 0;

	return nRead;
}

//--------------------------------------------------------------------------------------------
bool encodePCMToAMR(const char* PCM, int pcmBytes, const char* szAMRFile)
{
	const int CHANNELS = 1;
	const int BITS_PER_SAMPLE = 16;

	//init amr encoder handle
	void* handle_amr = Encoder_Interface_init(0);

	HANDLE hFileHandle =
		CreateFileA(szAMRFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileHandle == INVALID_HANDLE_VALUE) return false;

	int total_bytes = 0;
	DWORD writeBytes = 0;

	/* write magic number to indicate single channel AMR file storage format */
	WriteFile(hFileHandle, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER), &writeBytes, 0);

	int pcmPoint = 0;
	while (true) {
		/* input speech vector */
		short speech[160];
		/* bitstream filetype */
		unsigned char amrFrame[MAX_AMR_FRAME_SIZE];

		int size = _readPCMFrame(speech, PCM, PCM + pcmBytes, pcmPoint, CHANNELS, BITS_PER_SAMPLE);
		if (size) {
			int byte_counter = Encoder_Interface_Encode(handle_amr, MR475, speech, amrFrame, 0);
			total_bytes += byte_counter;
			WriteFile(hFileHandle, amrFrame, byte_counter, &writeBytes, 0);
		}
		else break;
	}
	Encoder_Interface_exit(handle_amr);
	CloseHandle(hFileHandle); hFileHandle = INVALID_HANDLE_VALUE;
	return true;
}
