package com.axia;

import java.io.File;

public class VoiceItem {
	public enum LocalStatus { NO_LOCAL_FILE, RECORDING, STOP_RECORDING, DOWNLOADING, HAS_LOCAL_FILE }
	public enum ServerStatus { NO_SERVER_FILE, UPLOADING, HAS_SERVER_FILE }
	
	private long voiceID;				// unique id (local)
	private String localFilePath;		// local file path
	private String fileMD5;				// hash file 
	private String serverURL;			// server URL
	private LocalStatus localStatus;	// local file status
	private ServerStatus serverStatus;	// server file status
	
	public AudioRecorder recorder;		// recorder object
	
	public long getVoiceID() { return voiceID; }
	public String getLocalFile() { return localFilePath; }
	public String getServerFile() { return serverURL; }
	public String getMD5() { return fileMD5; }
	public LocalStatus getLocalStatus() { return localStatus; }
	public ServerStatus getServerStatus() { return serverStatus; }
	
	public void startRecord() {
		this.localStatus = LocalStatus.RECORDING;
	}
	
	public void stopRecord() {
		this.localStatus = LocalStatus.STOP_RECORDING;
	}
	
	public void confirmLocalFile(String md5)
	{ 
		this.fileMD5 = md5; 
		this.localStatus = LocalStatus.HAS_LOCAL_FILE;
	}
	
	public void beginUpload() {
		this.serverStatus = ServerStatus.UPLOADING;
	}
	
	public void updateServerURL(String serverURL) {
		this.serverURL = serverURL;
		this.serverStatus = ServerStatus.HAS_SERVER_FILE;
	}
	
	public void beginDownload() {
		this.localStatus = LocalStatus.DOWNLOADING;
	}
	
	//Create new item
	public VoiceItem(long voiceID, 
			String localFilePath, 
			String fileMD5, 
			String serverURL,
			LocalStatus localStatus, 
			ServerStatus serverStatus)
	{
		this.voiceID = voiceID;
		this.localFilePath = localFilePath;
		this.fileMD5 = fileMD5;
		this.serverURL = serverURL;
		this.localStatus = localStatus;
		this.serverStatus = serverStatus;
		
		this.recorder = null;
	}
}
