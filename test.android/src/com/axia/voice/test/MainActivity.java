package com.axia.voice.test;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.http.util.EncodingUtils;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.TextView;

import com.axia.AxVoice;
import com.axia.VoiceMessage;

public class MainActivity extends Activity implements OnTouchListener, OnClickListener
{
	private static final int DISPATCH_VOICE_MESSAGE = 0;
	private static final int DISPATCH_SERVER_URL_DOWNLOAD = 1;
	private static final int DISPATCH_VOICE_ID_DOWNLOAD = 2;
	private TextView voiceIDView, voiceURLView, resultView;
	private long currentVoiceID;
	private Timer mTimer;
	private TimerTask mTimerTask;
	private Handler mHandler;
	
	private class VoiceMessageCallback implements AxVoice.MessageCallBack
	{
		@Override
		public void onMessage(VoiceMessage msg)
		{
			switch(msg.msgType)
			{
			case VoiceMessage.MT_RECORD_MSG:
				{
					long voiceID = msg.voiceID;
					String type = msg.params[0];
					String success = msg.params[1];
					String result = msg.params[2];
					if(type=="complete")
					{
						Log.i("axvoice", "record complete, id=" + voiceID + ", suc=" + success + ",result=" + result);
						
						TextView _voiceTextView = (TextView)findViewById(R.id.editText_voice_id);
						_voiceTextView.setText(String.valueOf(voiceID));
						
						//begin upload
						AxVoice.uploadVoice(voiceID);
					}
				}
				break;
				
			case VoiceMessage.MT_UPLOAD_MSG:
				{
					long voiceID = msg.voiceID;
					String type = msg.params[0];
					String success = msg.params[1];
					String result = msg.params[2];
					if(type=="complete")
					{
						Log.i("axvoice", "upload complete, id=" + voiceID + ", suc=" + success + ",result=" + result);
						
						voiceURLView.setText(result);
					}
				}
				break;
			case VoiceMessage.MT_DOWNLOAD_MSG:
				{
					long voiceID = msg.voiceID;
					String type = msg.params[0];
					String success = msg.params[1];
					String result = msg.params[2];
					if(type=="complete")
					{
						Log.i("axvoice", "download complete, id=" + voiceID + ", suc=" + success + ",result=" + result);
						((Button)findViewById(R.id.button_play_voice)).setEnabled(true);
						((Button)findViewById(R.id.button_stop_voice)).setEnabled(true);
					}
				}
				break;
				
			case VoiceMessage.MT_PLAY_MSG:
				{
					long voiceID = msg.voiceID;
					String type = msg.params[0];
					if(type=="complete")
					{
						Log.i("axvoice", "play complete, id=" + voiceID);
					}
				}
				break;
				
			case VoiceMessage.MT_TOTEXT_MSG:
				{
					long voiceID = msg.voiceID;
					String type = msg.params[0];
					String success = msg.params[1];
					String result = msg.params[2];
					Log.i("axvoice", "text complete, id=" + voiceID + ", suc=" + success + ",result=" + result);

					if(type=="complete" && success=="true")
					{
						resultView.setText(result); 
					} else if(type=="failed") {
						resultView.setText(result); 
					}
				}
				break;
			}
		}
	}
	private VoiceMessageCallback msgCallback = new VoiceMessageCallback();
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
		((Button)findViewById(R.id.button_get_url)).setOnClickListener(this);
		((Button)findViewById(R.id.button_create_voice)).setOnClickListener(this);
		((Button)findViewById(R.id.button_get_voiceid)).setOnClickListener(this);
		((Button)findViewById(R.id.button_download)).setOnClickListener(this);
		((Button)findViewById(R.id.button_play_voice)).setOnClickListener(this);
		((Button)findViewById(R.id.button_stop_voice)).setOnClickListener(this);
		((Button)findViewById(R.id.button_record)).setOnTouchListener(this);
		((Button)findViewById(R.id.button_to_text)).setOnClickListener(this);
		
		voiceIDView = (TextView)findViewById(R.id.editText_voice_id);
		voiceURLView = (TextView)findViewById(R.id.editText_voice_url);
		resultView = (TextView)findViewById(R.id.result_text);
		
		mTimer = new Timer();
		mTimerTask = new TimerTask() {
			@Override
			public void run() {
				mHandler.sendEmptyMessage(DISPATCH_VOICE_MESSAGE);
			}
		};
		
		mHandler = new Handler(){
			@Override
			public void handleMessage(Message msg) {
				if(msg.what == DISPATCH_VOICE_MESSAGE) {
					//call from main thread
					AxVoice.dispatchMessage(msgCallback);
				}
				else if(msg.what == DISPATCH_SERVER_URL_DOWNLOAD) {
					voiceURLView.setText(msg.getData().getString("data"));
				}
				else if(msg.what == DISPATCH_VOICE_ID_DOWNLOAD) {
					voiceIDView.setText(msg.getData().getString("data"));
				}
			}
		};
		mTimer.schedule(mTimerTask, 100, 100);
		
		//init axvoice
		AxVoice._setNativeActivity(this);
		AxVoice.init(Environment.getExternalStorageDirectory() + "/changyou/",
				"http://dashengine.com/upload_voice", 
				"54c0b179");
    }
    
	@Override
	public boolean onTouch(View v, MotionEvent event)
	{
		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			v.getParent().requestDisallowInterceptTouchEvent(true);
			((Button)findViewById(R.id.button_record)).setText("Recording...");
			beginRecord();
			break;
			
		case MotionEvent.ACTION_MOVE:
			break;
			
		case MotionEvent.ACTION_UP:
			completeRecord();
			((Button)findViewById(R.id.button_record)).setText("Record!");
			break;
		}
		
		return false;
	}
	
	private void debugGetServerURl(final String address, final int msgType)
	{
		new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					URL url = new URL(address);
					HttpURLConnection conn = (HttpURLConnection) url.openConnection();
					conn.connect();
			
					BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));   
					
					String line = reader.readLine();
					Bundle bundle = new Bundle();;
					bundle.putString("data", line);
					
					Message msg = new Message();  
					msg.what = msgType; 
					msg.setData(bundle);
					mHandler.sendMessage(msg);
					
				}catch(Exception e) {
					
				}
			}
		}).start();	
		
	}
	
	@Override
	public void onClick(View v)
	{
		switch(v.getId())
		{
		case R.id.button_get_url:
			{
				debugGetServerURl("http://www.dashengine.com/download/server_url.txt", DISPATCH_SERVER_URL_DOWNLOAD);
			}
			break;
		case R.id.button_create_voice:
			{
				long voiceID = AxVoice.createVoice(voiceURLView.getText().toString());
				voiceIDView.setText(String.valueOf(voiceID));
			}
			break;
			
		case R.id.button_get_voiceid:
			{
				debugGetServerURl("http://www.dashengine.com/download/voice_id.txt", DISPATCH_VOICE_ID_DOWNLOAD);
			}
			break;
			
		case R.id.button_download:
			{
				long voiceID = Long.parseLong(voiceIDView.getText().toString());
				AxVoice.downloadVoice(voiceID);
				
				((Button)findViewById(R.id.button_play_voice)).setEnabled(false);
				((Button)findViewById(R.id.button_stop_voice)).setEnabled(false);
			}
			break;
			
		case R.id.button_play_voice:
			{
				AxVoice.playVoice(Long.parseLong(voiceIDView.getText().toString()));
			}
			break;
		case R.id.button_stop_voice:
			{
				AxVoice.stopVoice();
			}
			break;
			
		case R.id.button_to_text:
			{
				resultView.setText("");
				AxVoice.voice2Text(Long.parseLong(voiceIDView.getText().toString()));
			}
			break;
		
		}
	}
	
	private String _readLineFromFile(String fileName)
	{
		try {
			FileInputStream fin = new FileInputStream(fileName);
			int length = fin.available();
			byte[] buffer = new byte[length];
			fin.read(buffer);
			fin.close();
			
			String line = EncodingUtils.getString(buffer,  "UFT-8").trim();
			return line;
		}catch(Exception e)	{
			return "";
		}
	}
	
	private void beginRecord()
	{
		currentVoiceID = AxVoice.beginRecord();
	}
	
	private void completeRecord()
	{
		AxVoice.completeRecord(currentVoiceID, false);
	}
}
