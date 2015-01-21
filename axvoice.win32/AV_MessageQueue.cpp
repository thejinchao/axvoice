#include "stdafx.h"
#include "AV_MessageQueue.h"
#include "AV_Util.h"

//--------------------------------------------------------------------------------------------
MessageQueue* MessageQueue::s_Instance = 0;

//--------------------------------------------------------------------------------------------
MessageQueue::MessageQueue()
{
	s_Instance = this;
	InitializeCriticalSection(&g_lockQueue);
}

//--------------------------------------------------------------------------------------------
MessageQueue::~MessageQueue()
{
	DeleteCriticalSection(&g_lockQueue);
}

//--------------------------------------------------------------------------------------------
void MessageQueue::pushMessage(AxVoiceMessage::Type type, unsigned int voiceID, const std::vector< std::string >& params)
{
	AutoLock autolock(&g_lockQueue);

	Message msg;
	msg.type = type;
	msg.voiceID = voiceID;
	msg.params = params;
	m_msgQueue.push(msg);
}

//--------------------------------------------------------------------------------------------
void MessageQueue::pushMessage(AxVoiceMessage::Type type, unsigned int voiceID, const std::string& p1)
{
	AutoLock autolock(&g_lockQueue);

	Message msg;
	msg.type = type;
	msg.voiceID = voiceID;

	std::vector< std::string > params;
	params.push_back(p1);
	msg.params = params;
	m_msgQueue.push(msg);
}

//--------------------------------------------------------------------------------------------
void MessageQueue::pushMessage(AxVoiceMessage::Type type, unsigned int voiceID, const std::string& p1, bool p2, const std::string& p3)
{
	AutoLock autolock(&g_lockQueue);

	Message msg;
	msg.type = type;
	msg.voiceID = voiceID;

	std::vector< std::string > params;
	params.push_back(p1);
	params.push_back(p2?"true":"false");
	params.push_back(p3);

	msg.params = params;
	m_msgQueue.push(msg);
}

//--------------------------------------------------------------------------------------------
void MessageQueue::dispatchMessage(AxVoiceCallback* cb)
{
	if(cb==0) return;

	AutoLock autolock(&g_lockQueue);

	while(!m_msgQueue.empty())
	{
		Message msg = m_msgQueue.front();
		m_msgQueue.pop();

		cb->onMessage(&msg);
	}
}


