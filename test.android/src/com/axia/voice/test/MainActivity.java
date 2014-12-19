package com.axia.voice.test;

import java.io.FileInputStream;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.http.util.EncodingUtils;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
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
	private TextView voiceIDView, voiceURLView;
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
		
		voiceIDView = (TextView)findViewById(R.id.editText_voice_id);
		voiceURLView = (TextView)findViewById(R.id.editText_voice_url);
		
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
			}
		};
		mTimer.schedule(mTimerTask, 100, 100);
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
	
	@Override
	public void onClick(View v)
	{
		switch(v.getId())
		{
		case R.id.button_get_url:
			{
				String url = _readLineFromFile("/data/local/axvoice_url.txt");
				voiceURLView.setText(url);
			}
			break;
		case R.id.button_create_voice:
			{
				long voiceID = AxVoice.createVoice(voiceURLView.getText().toString());
				voiceIDView.setText(String.valueOf(voiceID));
			}
			break;
			
		case R.id.button_download:
			{
				long voiceID = Long.parseLong(voiceIDView.getText().toString());
				AxVoice.downloadVoice(voiceID);
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
