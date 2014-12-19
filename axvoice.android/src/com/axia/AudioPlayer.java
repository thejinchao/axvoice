package com.axia;

import java.io.IOException;

import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;

public class AudioPlayer {
	public interface Callback {
		public void onComplete(long voiceID);
	}	
	
	private class MediaPlayerCompletionListenter implements OnCompletionListener {
		@Override
		public void onCompletion(MediaPlayer mp) {
			mp.reset();
			callback.onComplete(voiceID);
		}
		
		public MediaPlayerCompletionListenter(Callback callback, long voiceID) {
			this.callback = callback;
			this.voiceID = voiceID;
		}
		
		private Callback callback;
		private long voiceID;
	}

	public void stop() {
		mediaPlayer.reset();
	}
	
	public void playAudioFile(long voiceID, String localFile, Callback callback) {
		try {
			mediaPlayer.reset();
			
			this.voiceID = voiceID;
			this.callback = callback;
			
			mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
			mediaPlayer.setDataSource(localFile);
			mediaPlayer.setVolume(1.0f, 1.0f);
			mediaPlayer.prepare();
			mediaPlayer.start();
		
			// set on complete listener
			mediaPlayer.setOnCompletionListener(new MediaPlayerCompletionListenter(this.callback, this.voiceID));
			
		} catch (IllegalArgumentException e) {
			//e.printStackTrace();
		} catch (IllegalStateException e) {
			//e.printStackTrace();
		} catch (IOException e) {
			//e.printStackTrace();
		}
	}
	
	private MediaPlayer mediaPlayer = new MediaPlayer();
	private Callback callback;
	private long voiceID;
	
	///////////////////////////////////////////////////////
	//get instance
	///////////////////////////////////////////////////////
	private static class AudioPlayerInstance {
		private static final AudioPlayer sInstance=new AudioPlayer();
	}
	
	//thread safe(from Effective Java)
	public static AudioPlayer getInstance() {
		return AudioPlayerInstance.sInstance;
	}
	
	//private construct 
	private AudioPlayer() {
	}
}
