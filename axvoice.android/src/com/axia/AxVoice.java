package com.axia;

import java.io.File;
import java.util.HashMap;
import java.util.NoSuchElementException;

import com.thecodeway.axtrace.AxTrace;
//import com.unity3d.player.UnityPlayer;

public class AxVoice {
	
	//////////////////////////////////////////////////////
	//public functions
	//////////////////////////////////////////////////////
	//-------------------------------------------------------------------
	static public void init(String dataPath, String uploadURL)
	{
		AxTrace.SetTraceServer("192.168.0.100", 1978);

		Config.cacheAudioPath = dataPath+"/";
		Config.uploadUrl = uploadURL;
		
		AxTrace.Trace("Init cache=%s, upload=%s", dataPath, uploadURL);
	}
	
	//-------------------------------------------------------------------
	static public long beginRecord()
	{
		AxTrace.Trace("Begin Record");
		
		synchronized(VoiceManager.getInstance().thelock)  {
			//allocate new voice
			VoiceItem newItem = VoiceManager.getInstance().allocateNewItem();
			
			//create recorder
			newItem.startRecord();
			AudioRecorder.getInstance().start(newItem.getVoiceID(), newItem.getLocalFile());
			
			return newItem.getVoiceID();
		} 
	}
	
	public static class OnRecordCompleteCallback implements Runnable
	{
		@Override
		public void run() {
			synchronized(VoiceManager.getInstance().thelock)  {
				try {
					//find the voice item
					VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
					
					//confirm local file
					String md5 = UtilFunctions.getMd5ByFile(new File(item.getLocalFile()));
					item.confirmLocalFile(md5);
			
					//push message
					MessageQueue.getInstance().pushMessage(
							VoiceMessage.MT_RECORD_MSG, voiceID, "complete", true, md5);
					
					AxTrace.Trace("Complete Record");
				}catch(NoSuchElementException e){
					//TODO: error
					return;
				}catch(Exception e) {
					//TODO: error
					return;
				}
			}			
		}
		
		OnRecordCompleteCallback(long voiceID) {
			this.voiceID = voiceID;
		}
		private long voiceID;
	}
	//-------------------------------------------------------------------
	static public void completeRecord(long voiceID, boolean abort)
	{
		AxTrace.Trace("Begin Complete Record");
		synchronized(VoiceManager.getInstance().thelock)  {
			//find the voice item
			VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
			//check status
			if(item.getLocalStatus() != VoiceItem.LocalStatus.RECORDING)
			{
				//TODO: ERROR
				return;
			}
			//begin stop record
			item.stopRecord();
			
			if(abort) {
				AudioRecorder.getInstance().abort();
			}
			else {
				AudioRecorder.getInstance().stop(new OnRecordCompleteCallback(voiceID));
			}
		}
		return;
	}
	
	//-------------------------------------------------------------------
	static private class UploadCallback implements UploadFile.Callback {
		@Override
		public void onUploading(long voiceID) {
		}
		
		@Override
		public void onUploadFailed(long voiceID, String reason) {
			//AxTrace.TraceEx(0, 1, "Upload failed! reason=|%s|", reason);
			//push message
			MessageQueue.getInstance().pushMessage(
					VoiceMessage.MT_UPLOAD_MSG, voiceID, "complete", false, reason);
		}
		
		@Override
		public void onUploadComplete(long voiceID, String serverURL) {
			//AxTrace.Trace("Upload Complete! SERVERURL=%s", serverURL);
			synchronized(VoiceManager.getInstance().thelock)  {
				VoiceItem _item = VoiceManager.getInstance().findItem(voiceID);
				//TODO: check status
				_item.updateServerURL(serverURL);
				
				//push message
				MessageQueue.getInstance().pushMessage(
						VoiceMessage.MT_UPLOAD_MSG, voiceID, "complete", true, serverURL);
				
			}
		}
	}
	
	//-------------------------------------------------------------------
	static public void uploadVoice(long voiceID)
	{
		//AxTrace.Trace("Begin Upload voice(id=%d)", voiceID);
		
		synchronized(VoiceManager.getInstance().thelock)  {
			try {
				//find the voice item
				VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
				if(item.getLocalStatus() != VoiceItem.LocalStatus.HAS_LOCAL_FILE)
				{
					throw new Exception("local file invalid");
				}
				if(item.getServerStatus() != VoiceItem.ServerStatus.NO_SERVER_FILE)
				{
					throw new Exception("server file busy");
				}
				
				item.beginUpload();
				
				//begin upload
				UploadFile uploadFile = new UploadFile(item.getVoiceID(), 
						item.getLocalFile(), item.getMD5(), Config.uploadUrl, 
						new HashMap<String, String>(),
						new UploadCallback());
				
				//begin upload
				new Thread(uploadFile).start();		
				
			}catch(NoSuchElementException e){
				//TODO: ERROR
				return;
			}catch(Exception e) {
				//TODO: ERROR
				return;
			}
		}
		
		return;
	}
	//-------------------------------------------------------------------
	static public long createVoice(String serverURL)
	{
		synchronized(VoiceManager.getInstance().thelock)  {
			//allocate new voice
			VoiceItem newItem = VoiceManager.getInstance().allocateNewItem();
			
			newItem.updateServerURL(serverURL);
			
			return newItem.getVoiceID();
		}
	}
	
	//-------------------------------------------------------------------
	static private class DownloadCallback implements DownloadFile.Callback {
		@Override
		public void onDownloading(long voiceID) {
			// TODO Auto-generated method stub
			
		}
		
		@Override
		public void onDownloadFailed(long voiceID, String reason) {
			//push message
			MessageQueue.getInstance().pushMessage(
					VoiceMessage.MT_DOWNLOAD_MSG, voiceID, "complete", false, reason);
		}
		
		@Override
		public void onDownloadComplete(long voiceID) {
			synchronized(VoiceManager.getInstance().thelock)  {
				VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
				
				//TODO: Check status
				
				//confirm local file
				String md5 = UtilFunctions.getMd5ByFile(new File(item.getLocalFile()));
				item.confirmLocalFile(md5);

				MessageQueue.getInstance().pushMessage(
						VoiceMessage.MT_DOWNLOAD_MSG, voiceID, "complete", true, md5);
			}
		}
	}
	//-------------------------------------------------------------------
	static public void downloadVoice(long voiceID)
	{
		synchronized(VoiceManager.getInstance().thelock)  {
			try {
				//find the voice item
				VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
				
				if(item.getLocalStatus() != VoiceItem.LocalStatus.NO_LOCAL_FILE)
				{
					return;
				}
				if(item.getServerStatus() != VoiceItem.ServerStatus.HAS_SERVER_FILE)
				{
					return;
				}
				
				item.beginDownload();
				
				//begin download file and play
				DownloadFile downloader = new DownloadFile(
						item.getVoiceID(),
						item.getServerFile(), 
						item.getLocalFile(),
						new DownloadCallback());
				
				//begin upload
				new Thread(downloader).start();		
				//TODO: push message
			}catch(NoSuchElementException e){
				//TODO: ERROR
				return;
			}catch(Exception e) {
				//TODO: ERROR
				return;
			}
			
		}		
	}
	
	//-------------------------------------------------------------------
	static private class PlayVoiceCallback implements AudioPlayer.Callback
	{
		@Override
		public void onComplete(long voiceID)
		{
			//push message
			MessageQueue.getInstance().pushMessage(VoiceMessage.MT_PLAY_MSG, voiceID, "complete");
		}
	}
	
	//-------------------------------------------------------------------
	static public void playVoice(long voiceID)
	{
		synchronized(VoiceManager.getInstance().thelock)  {
			try {
				//find the voice item
				VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
				
				if(item.getLocalStatus() != VoiceItem.LocalStatus.HAS_LOCAL_FILE)
				{
					return;
				}

				//play
				AudioPlayer.getInstance().playAudioFile(item.getVoiceID(), item.getLocalFile(), new PlayVoiceCallback());
				
				//TODO: push message
			}catch(NoSuchElementException e){
				//TODO: ERROR
				return;
			}catch(Exception e) {
				//TODO: ERROR
				return;
			}
			
		}		
	}
	
	//-------------------------------------------------------------------
	static public void stopVoice()
	{
		synchronized(VoiceManager.getInstance().thelock)  {
			AudioPlayer.getInstance().stop();
		}
	}	
	//-------------------------------------------------------------------
	public interface MessageCallBack
	{
		public void onMessage(VoiceMessage msg);
	}
	
	//-------------------------------------------------------------------
	static public void dispatchMessage(MessageCallBack cb)
	{
		MessageQueue.getInstance().dispatchMessage(cb);
	}
	
	//-------------------------------------------------------------------
	static public void dispatchMessage_Unity(String cbObjectName)
	{
		MessageQueue.getInstance().dispatchMessage_Unity(cbObjectName);
	}
}