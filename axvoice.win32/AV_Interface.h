/***************************************************

				AXIA|Voice

						(C) Copyright  JinChao. 2014
***************************************************/


#ifndef __AXVOICE_INTERFACE__
#define __AXVOICE_INTERFACE__

/** Init axvoice */
bool AxVoice_Init(const char* cachePath, const char* uploadURL);

/** Begin record @return = voiceID */
unsigned int AxVoice_BeginRecord(void);

/** Complete current record */
void AxVoice_CompleteRecord(unsigned int voiceID);

/** Begin Upload */
void AxVoice_UploadVoice(unsigned int voiceID);

/** create a voice from server url @return = voiceID */
unsigned int AxVoice_CreatVoice(const char* szServerURL);

/** download a voice */
void AxVoice_DownloadVoice(unsigned int voiceID);

/** play a voice*/
void AxVoice_PlayVoice(unsigned int voiceID);

/** stop play current voice*/
void AxVoice_StopVoice(void);

class AxVoiceMessage
{
public:
	enum Type
	{
		MT_RECORD_MSG=1,
		MT_UPLOAD_MSG=2,
		MT_DOWNLOAD_MSG=3,
		MT_PLAY_MSG=4,
	};

	virtual Type getType(void) const = 0;
	virtual unsigned int getVoiceID(void) const = 0;
	virtual int getParamCounts(void) const = 0;
	virtual const char* getParam(int index) const = 0;

	//virtual ~AxVoiceMessage() {}
};

class AxVoiceCallback
{
public:
	virtual ~AxVoiceCallback() {}
	virtual void onMessage(const AxVoiceMessage* message) = 0;
};

//call from main thread
void AxVoice_DispatchMessage(AxVoiceCallback* cb);

#endif
