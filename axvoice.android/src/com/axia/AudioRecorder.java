package com.axia;

import java.io.File;
import java.io.IOException;

import android.media.MediaRecorder;

public class AudioRecorder {
	///////////////////////////////////////////////////////
	//public function
	///////////////////////////////////////////////////////

	public void start(long voiceID, String localFileName)
	{
		this.voiceID = voiceID;
		this.localFileName = localFileName;

		try {
			recorder.reset(); //stop current recording...
			
			File directory = new File(this.localFileName).getParentFile();
			if (!directory.exists() && !directory.mkdirs()) 
			{ 
				throw new IOException("Path to file could not be created"); 
			}
			
			recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
			recorder.setOutputFormat(MediaRecorder.OutputFormat.AMR_NB);
			recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
			recorder.setAudioEncodingBitRate(ENCODING_BIT_RATE);
			recorder.setAudioSamplingRate(SAMPLE_RATE_IN_HZ);
			recorder.setOutputFile(this.localFileName);
			recorder.prepare();
			recorder.start();
		}catch(IOException e) {
			//push message
			MessageQueue.getInstance().pushMessage(
					VoiceMessage.MT_RECORD_MSG, voiceID, "complete", false, e.getMessage());
			
		}
	}
	
	public void stop(final Runnable onComplete)
	{
		AsynThreadPool.getInstance().runSingleThread(new Runnable() {
			@Override
			public void run() {
				try {
					recorder.stop();
					
					//confirm local file
					onComplete.run();
					
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});				
	}

	public void abort()
	{
		recorder.reset();
	}

	///////////////////////////////////////////////////////
	//private values
	///////////////////////////////////////////////////////
	private static final int ENCODING_BIT_RATE = 800;
	private static final int SAMPLE_RATE_IN_HZ = 8000; 

	private final MediaRecorder recorder = new MediaRecorder();
	private String localFileName;
	private long voiceID;
	
	///////////////////////////////////////////////////////
	//get instance
	///////////////////////////////////////////////////////
	private static class AudioRecorderInstance {
		private static final AudioRecorder sInstance=new AudioRecorder();
	}
	
	//thread safe(from Effective Java)
	public static AudioRecorder getInstance() {
		return AudioRecorderInstance.sInstance;
	}
	
	//private construct 
	private AudioRecorder() {
	}

}
