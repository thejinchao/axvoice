package com.axia;

import java.util.HashMap;
import java.util.NoSuchElementException;

import android.annotation.SuppressLint;

public class VoiceManager {
	

	///////////////////////////////////////////////////////
	//public functions
	///////////////////////////////////////////////////////
	@SuppressLint("DefaultLocale")
	public VoiceItem allocateNewItem()
	{
		long uniqueID = System.currentTimeMillis() & 0xFFFFFF;
		String localFilePath = Config.cacheAudioPath + uniqueID + VOICE_FILE_EXT;
		
		VoiceItem newItem = new VoiceItem(uniqueID, localFilePath, "", "", "",
				VoiceItem.LocalStatus.NO_LOCAL_FILE, 
				VoiceItem.ServerStatus.NO_SERVER_FILE, 
				VoiceItem.TextStatus.NO_TEXT);
		
		synchronized(voiceHashMap){
			voiceHashMap.put(newItem.getVoiceID(), newItem);
		}
		
		return newItem;
	}
	
	public VoiceItem findItem(long voiceID) throws NoSuchElementException
	{
		synchronized(voiceHashMap){
			if(voiceHashMap.containsKey(voiceID))
				return voiceHashMap.get(voiceID);
			else
				throw new NoSuchElementException();
		}
	}
	

	///////////////////////////////////////////////////////
	//private values
	///////////////////////////////////////////////////////
	public static final String VOICE_FILE_EXT = ".amr";

	//hash map to store voice data
	private HashMap< Long, VoiceItem > voiceHashMap;
	public Object thelock;
	
	///////////////////////////////////////////////////////
	//get instance
	///////////////////////////////////////////////////////
	private static class VoiceManagerInstance {
		private static final VoiceManager sInstance=new VoiceManager();
	}
	
	//thread safe(from Effective Java)
	public static VoiceManager getInstance() {
		return VoiceManagerInstance.sInstance;
	}
	
	//private construct 
	private VoiceManager() {
		voiceHashMap = new HashMap<Long, VoiceItem>();
		thelock = new Object();
	}

}
