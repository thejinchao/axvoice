package com.axia;

import java.util.LinkedList;
import java.util.Queue;

import com.unity3d.player.UnityPlayer;

public class MessageQueue {
	
	public void pushMessage(int type, long voiceID, String[] params)
	{
		synchronized(msgQueue) {
			VoiceMessage msg = new VoiceMessage(voiceID, type, params);
			msgQueue.offer(msg);
		}
	}
	
	public void pushMessage(int type, long voiceID, String p1)
	{
		synchronized(msgQueue) {
			String[] params = new String[]{p1};
			VoiceMessage msg = new VoiceMessage(voiceID, type, params);
			msgQueue.offer(msg);
		}
	}

	public void pushMessage(int type, long voiceID, String p1, boolean p2, String p3)
	{
		synchronized(msgQueue) {
			String[] params = new String[]{p1, p2?"true":"false", p3};
			VoiceMessage msg = new VoiceMessage(voiceID, type, params);
			msgQueue.offer(msg);
		}
	}

	public void dispatchMessage(AxVoice.MessageCallBack cb)
	{
		synchronized(msgQueue) {
			for(VoiceMessage msg : msgQueue) {
				cb.onMessage(msg);
			}
			msgQueue.clear();
		}		
	}
	
	private String _union_params(VoiceMessage msg)
	{
		String ret = String.valueOf(msg.voiceID);
		for(String p : msg.params) {
			ret += "|";
			ret += p;
		}
		return ret;
	}
	
	public void dispatchMessage_Unity(String cbObjectName)
	{
		//Call UnityEngine Send Message
		synchronized(msgQueue) {
			for(VoiceMessage msg : msgQueue) {
				switch(msg.msgType)
				{
				case VoiceMessage.MT_RECORD_MSG:
					UnityPlayer.UnitySendMessage(cbObjectName, "_OnAxVoiceRecordMessage", _union_params(msg));
					break;
					
				case VoiceMessage.MT_UPLOAD_MSG:
					UnityPlayer.UnitySendMessage(cbObjectName, "_OnAxVoiceUploadMessage", _union_params(msg));
					break;
					
				case VoiceMessage.MT_DOWNLOAD_MSG:
					UnityPlayer.UnitySendMessage(cbObjectName, "_OnAxVoiceDownloadMessage", _union_params(msg));
					break;
				case VoiceMessage.MT_PLAY_MSG:
					UnityPlayer.UnitySendMessage(cbObjectName, "_OnAxVoicePlayMessage", _union_params(msg));
					break;
				}
			}
			msgQueue.clear();
		}		
	}
	
	private Queue<VoiceMessage> msgQueue;
	
	///////////////////////////////////////////////////////
	//get instance
	///////////////////////////////////////////////////////
	private static class MessageQueueInstance {
		private static final MessageQueue sInstance=new MessageQueue();
	}
	
	//thread safe(from Effective Java)
	public static MessageQueue getInstance() {
		return MessageQueueInstance.sInstance;
	}
	
	//private construct 
	private MessageQueue() {
		msgQueue = new LinkedList<VoiceMessage>();
	}
}

