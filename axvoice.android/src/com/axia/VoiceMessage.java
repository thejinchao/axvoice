package com.axia;

public class VoiceMessage {
	public static final int MT_RECORD_MSG = 1;
	public static final int MT_UPLOAD_MSG = 2;
	public static final int MT_DOWNLOAD_MSG = 3;
	public static final int MT_PLAY_MSG = 4;
	
	public long voiceID;
	public int msgType;
	public String[] params;
	
	public VoiceMessage(long voiceID, int msgType, String[] params) {
		this.voiceID = voiceID;
		this.msgType = msgType;
		this.params = params;
	}
}
