#pragma once
class VoiceItem;

class VoiceManager
{
public:
	VoiceItem* allocateNewItem(void);
	VoiceItem* findItem(unsigned int voiceID);

private:
	typedef std::map< unsigned int, VoiceItem* > VoiceMap;
	VoiceMap m_voiceMap;

public:
	VoiceManager();
	~VoiceManager();

	static VoiceManager* getInstance(void) { return s_Instance; }
	static VoiceManager* s_Instance;
};