package com.axia;

import java.io.FileInputStream;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.iflytek.cloud.ErrorCode;
import com.iflytek.cloud.InitListener;
import com.iflytek.cloud.RecognizerListener;
import com.iflytek.cloud.RecognizerResult;
import com.iflytek.cloud.SpeechConstant;
import com.iflytek.cloud.SpeechError;
import com.iflytek.cloud.SpeechRecognizer;
import com.iflytek.cloud.SpeechUtility;


public class Voice2TextWrap {
	public interface Callback {
		public void onConvertComplete(long voiceID, String result);
		public void onConvertFailed(long voiceID, String reason);
	}	
	
	private Callback callback;
	private SpeechRecognizer mIat;
	private final String TAG = "Voice2TextWrap";
	private String mResultText="";
	private long voiceID;
	
	private InitListener mInitListener = new InitListener() {
		@Override
		public void onInit(int code) {
			Log.d(TAG, "SpeechRecognizer init() code = " + code);
			
			if (code != ErrorCode.SUCCESS) {
				Log.e(TAG, "SpeechRecognizer init failed, code=" + code);
        	}
		}
	};
	
	void initEngine(Context context, String iflyID) {
		//init ifly engine
		try {
		SpeechUtility.createUtility(context, SpeechConstant.APPID + "=" + iflyID + ", " +
				SpeechConstant.FORCE_LOGIN + "=true");
		}catch(Exception ex) {
			String msg = ex.getMessage();
			Log.e(TAG, "createUtility init failed, ex=" + msg);
		}
		//create speech recognizer object
		mIat = SpeechRecognizer.createRecognizer(context, mInitListener);
	}
	
	private void setIatParam(){
		// Çå¿Õ²ÎÊý
		mIat.setParameter(SpeechConstant.PARAMS, null);
		
		mIat.setParameter(SpeechConstant.ENGINE_TYPE, SpeechConstant.TYPE_CLOUD);
		mIat.setParameter(SpeechConstant.RESULT_TYPE, "plain");
		mIat.setParameter(SpeechConstant.LANGUAGE, "zh_cn");
		mIat.setParameter(SpeechConstant.ACCENT, "mandarin");
		mIat.setParameter(SpeechConstant.SAMPLE_RATE, "8000");
		mIat.setParameter(SpeechConstant.ASR_PTT, "1");
		mIat.setParameter(SpeechConstant.AUDIO_SOURCE, "-1");
	}
	
	private RecognizerListener recognizerListener=new RecognizerListener(){
		@Override
		public void onBeginOfSpeech() {	
		}

		@Override
		public void onError(SpeechError error) {
			//showTip(error.getPlainDescription(true));
			callback.onConvertFailed(voiceID, error.getPlainDescription(true));
		}

		@Override
		public void onEndOfSpeech() {
		}
		
		@Override
		public void onResult(RecognizerResult results, boolean isLast) {		
			mResultText += results.getResultString();
			if(isLast) {
				callback.onConvertComplete(voiceID, mResultText);
			}
		}

		@Override
		public void onVolumeChanged(int volume) {
		}

		@Override
		public void onEvent(int eventType, int arg1, int arg2, Bundle obj) {
		}
	};
	
	private class IFlyProcess implements Runnable {
		private final String amrFile;
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			AmrDecoder decoder = new AmrDecoder();
			if(!decoder.OpenAmrFile(amrFile)) {
				callback.onConvertFailed(voiceID, "open amr failed");
				return;
			}
			
			setIatParam();
			
			int ret = mIat.startListening(recognizerListener);
			if(ret != ErrorCode.SUCCESS){
				callback.onConvertFailed(voiceID, "start ifly listener failed");
				return;
			}
			
			short[] pcmData = new short[1024*8];
			while(true) {
				int length = decoder.ReadNextPCMData(pcmData);
				if(length==0) break;
				
				byte[] byteData = new byte[length*2];
				for(int i=0; i<length; i++) {
					short s = pcmData[i];
					byteData[i*2+1] = (byte) (s >> 8);  
					byteData[i*2+0] = (byte) (s);
				}
				
				ret = mIat.writeAudio(byteData, 0, length*2);
				if(ret != ErrorCode.SUCCESS) {
					callback.onConvertFailed(voiceID, "ifly write audio data failed");
					return;
				}
				try { 
					Thread.sleep(160);
				}catch(Exception ex) {
					
				}
			}
			
			mIat.stopListening();
		}
		
		public IFlyProcess(String amrFile) {
			this.amrFile = amrFile;
		}
	}
	
	void beginConvert(long voiceID, Voice2TextWrap.Callback callback) {
		VoiceItem item = VoiceManager.getInstance().findItem(voiceID);
		if(item.getLocalStatus() != VoiceItem.LocalStatus.HAS_LOCAL_FILE) return;
		if(item.getTextStatus() == VoiceItem.TextStatus.HAS_TEXT) {
			//already has text
			//TODO: ...
		} else if(item.getTextStatus() == VoiceItem.TextStatus.CONVERTING) {
			//converting...
			//TODO: maybe more information
			return;
		}
		
		if(mIat.isListening()) {
			//TODO: error(busy)
			return;
		}
		
		//begin convert thread
		this.voiceID = voiceID;
		this.callback = callback;
		mResultText="";
		item.beginConvertToText();
		new Thread(new IFlyProcess(item.getLocalFile())).start();
		
		//*/
		
		//-----------------------------------------------
		/*
		AmrDecoder decoder = new AmrDecoder();
		decoder.OpenAmrFile("/sdcard/changyou/iflytek09.amr");
		
		try {
			FileOutputStream fos = new FileOutputStream("/sdcard/changyou/test_1k.pcm");
			int block_index=0;
			int block_index_per_k = 0;
			
			short[] pcmData = new short[1024*1];
			while(true) {
				int length = decoder.ReadNextPCMData(pcmData, (block_index==0));
				if(length==0) break;
				
				byte[] byteData = new byte[length*2];
				for(int i=0; i<length; i++) {
					short s = pcmData[i];
					byteData[i*2+1] = (byte) (s >> 8);  
					byteData[i*2+0] = (byte) (s);
				}
				
				fos.write(byteData);
				
				int file_point=0;
				while(true){
					int data_size = byteData.length;
					int block_size = 2048;
					if(file_point+block_size > data_size) block_size = data_size-file_point;
					if(block_size<=0) break;
					
					String block_name = String.format("/sdcard/changyou/test_1k.%02d.perk", block_index_per_k++);
					FileOutputStream fos_block = new FileOutputStream(block_name);

					fos_block.write(byteData, file_point, block_size);
					fos_block.close();
					
					file_point += block_size;
				}
			
				block_index ++;
				
			}
			
			fos.close();
		
		}catch(Exception ex) {
			
		}
		return;
		*/		
		//-----------------------------------------------

		/*
		setIatParam();
		mResultText="";
		
		int ret = mIat.startListening(recognizerListener);
		if(ret != ErrorCode.SUCCESS){
			//TODO: ERROR
			return;
		}
		
		FileInputStream fis=null;
		try {
			fis = new FileInputStream("/sdcard/changyou/test_win.pcm");
			
			byte[] buffer = new byte[1024*16];
			int length = -1;
			while((length = fis.read(buffer))!=-1) {
				mIat.writeAudio(buffer, 0, length);
				Thread.sleep(160);
			}
		}
		catch(Exception ex)	{
			
		}
		
		mIat.stopListening();
		//*/
	}	
	///////////////////////////////////////////////////////
	//get instance
	///////////////////////////////////////////////////////
	private static class Voice2TextWrapInstance {
		private static final Voice2TextWrap sInstance=new Voice2TextWrap();
	}
	
	//thread safe(from Effective Java)
	public static Voice2TextWrap getInstance() {
		return Voice2TextWrapInstance.sInstance;
	}
	
	//private construct 
	private Voice2TextWrap() {
	}
}
