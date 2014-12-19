#pragma once

#include "AV_Interface.h"

class MessageQueue
{
public:

	class Message : public AxVoiceMessage
	{
	public:
		virtual Type getType(void) const { return type; }
		virtual unsigned int getVoiceID(void) const { return voiceID; }
		virtual int getParamCounts(void) const { return (int)params.size(); }
		virtual const char* getParam(int index) const { return params[index].c_str(); }

		Type type;
		unsigned int voiceID;
		std::vector<std::string> params;
	};

	void dispatchMessage(AxVoiceCallback* cb);

	void pushMessage(AxVoiceMessage::Type type, unsigned int voiceID, const std::vector< std::string >& params);
	void pushMessage(AxVoiceMessage::Type type, unsigned int voiceID, const std::string& p1);
	void pushMessage(AxVoiceMessage::Type type, unsigned int voiceID, const std::string& p1, bool p2, const std::string& p3);

private:
	typedef std::queue< Message > MsgQueue;
	MsgQueue m_msgQueue;
	CRITICAL_SECTION g_lockQueue;

public:
	MessageQueue();
	~MessageQueue();

	static MessageQueue* getInstance(void) { return s_Instance; }
	static MessageQueue* s_Instance;
};
