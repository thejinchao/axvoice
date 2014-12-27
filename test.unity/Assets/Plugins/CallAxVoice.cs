using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using com.axia;

namespace com.axia
{
	public class CallAxVoice 
	{
			
	#if UNITY_IPHONE
        [DllImport ("__Internal")]
        private static extern void _initAxVoice(string cachePath, string uploadURL);

        [DllImport ("__Internal")]
        private static extern uint _beginRecord();

        [DllImport ("__Internal")]
        private static extern void _completeRecord(uint voiceID);

        [DllImport ("__Internal")]
        private static extern void _uploadVoice(uint voiceID);

        [DllImport ("__Internal")]
        private static extern uint _createVoice(string serverURL);

        [DllImport ("__Internal")]
        private static extern void _downloadVoice(uint voiceID);

        [DllImport ("__Internal")]
        private static extern void _playVoice(uint voiceID);

        [DllImport ("__Internal")]
        private static extern void _stopVoice();

        [DllImport ("__Internal")]
        private static extern void _dispatchMessage(string callbackObjName);

        public static void Init(string cachePath, string uploadURL)
        {
        	_initAxVoice(cachePath, uploadURL);
        }
        
			  public static uint BeginRecord() {
			    uint voiceID = (uint)_beginRecord();
			    return voiceID;
			  }

			  public static void CompleteRecord(uint voiceID) {
			    _completeRecord(voiceID);
			    
			    //hack
			    UploadVoice(voiceID);
			  }

			  public static void UploadVoice(uint voiceID) {
			    _uploadVoice(voiceID);
			  }
			
			  public static uint CreatVoice(string serverURL) {
			    uint voiceID = (uint)_createVoice(serverURL);
			    return voiceID;
			  }
			
			  public static void DownloadVoice(uint voiceID) {
			    _downloadVoice(voiceID); 
			  }
			
			  public static void PlayVoice(uint voiceID) {
			    _playVoice(voiceID); 
			  }
			
			  public static void StopVoice() {
			    _stopVoice(); 
			  }
			
			  public static void DispatchMessage(string callbackObjName) {
			  	_dispatchMessage(callbackObjName);
			  }
	
	#elif UNITY_ANDROID
        static private AndroidJavaClass clsAxVoice = new AndroidJavaClass("com.axia.AxVoice");
        
        public static void Init(string cachePath, string uploadURL)
        {
        	clsAxVoice.CallStatic("init", cachePath, uploadURL); 
        }
        
			  public static uint BeginRecord() {
			    uint voiceID = (uint)(clsAxVoice.CallStatic<long>("beginRecord") & 0xFFFFFFFF); 
			    return voiceID;
			  }

			  public static void CompleteRecord(uint voiceID) {
			    clsAxVoice.CallStatic("completeRecord", (long)voiceID, false); 
			  }

			  public static void UploadVoice(uint voiceID) {
			    clsAxVoice.CallStatic("uploadVoice", (long)voiceID); 
			  }
			
			  public static uint CreatVoice(string szServerURL) {
			    uint voiceID = (uint)(clsAxVoice.CallStatic<long>("createVoice", szServerURL)&0xFFFFFFFF); 
			    return voiceID;
			  }
			
			  public static void DownloadVoice(uint voiceID) {
			    clsAxVoice.CallStatic("downloadVoice", (long)voiceID); 
			  }
			
			  public static void PlayVoice(uint voiceID) {
			    clsAxVoice.CallStatic("playVoice", (long)voiceID); 
			  }
			
			  public static void StopVoice() {
			    clsAxVoice.CallStatic("stopVoice"); 
			  }
			
			  public static void DispatchMessage(string callbackObj) {
			  	clsAxVoice.CallStatic("dispatchMessage_Unity", callbackObj); 
			  }

	#elif UNITY_STANDALONE_WIN		
		    public class Callback : AxVoiceCallback
		    {
		    	GameObject callbackObj;
		    	public void setCallbackObj(string obj) { 
		    		callbackObj = GameObject.Find(obj);
		    	}
		    	
		    	public override void onMessage(AxVoiceMessage msg)
		    	{
		    		if(msg.getType() == AxVoiceMessage.Type.MT_RECORD_MSG)
		    		{
		    			callbackObj.SendMessage("OnAxVoiceRecordMessage", new string[]{ ""+msg.getVoiceID(), msg.getParam(0), msg.getParam(1), msg.getParam(2)});
		    		} 
		    		else if(msg.getType() == AxVoiceMessage.Type.MT_UPLOAD_MSG)
		    		{
		    			callbackObj.SendMessage("OnAxVoiceUploadMessage", new string[]{ ""+msg.getVoiceID(), msg.getParam(0), msg.getParam(1), msg.getParam(2)});
		    		}
		    		else if(msg.getType() == AxVoiceMessage.Type.MT_DOWNLOAD_MSG)
		    		{
		    			callbackObj.SendMessage("OnAxVoiceDownloadMessage", new string[]{ ""+msg.getVoiceID(), msg.getParam(0), msg.getParam(1), msg.getParam(2)});
		    		}
		    		else if(msg.getType() == AxVoiceMessage.Type.MT_PLAY_MSG)
		    		{
		    			callbackObj.SendMessage("OnAxVoicePlayMessage", new string[]{ ""+msg.getVoiceID(), msg.getParam(0)});
		    		}
		    	}
		    }
		    
		    static private Callback callback = new Callback();
		    
			  public static void Init(string cachePath, string uploadURL) {
			  	axvoicePINVOKE.AxVoice_Init(cachePath, uploadURL);
			  }
			
			  public static uint BeginRecord() {
			    uint ret = axvoicePINVOKE.AxVoice_BeginRecord();
			    return ret;
			  }
			
			  public static void CompleteRecord(uint voiceID) {
			    axvoicePINVOKE.AxVoice_CompleteRecord(voiceID);
			  }
			
			  public static void UploadVoice(uint voiceID) {
			    axvoicePINVOKE.AxVoice_UploadVoice(voiceID);
			  }
			
			  public static uint CreatVoice(string szServerURL) {
			    uint ret = axvoicePINVOKE.AxVoice_CreatVoice(szServerURL);
			    return ret;
			  }
			
			  public static void DownloadVoice(uint voiceID) {
			    axvoicePINVOKE.AxVoice_DownloadVoice(voiceID);
			  }
			
			  public static void PlayVoice(uint voiceID) {
			    axvoicePINVOKE.AxVoice_PlayVoice(voiceID);
			  }
			
			  public static void StopVoice() {
			    axvoicePINVOKE.AxVoice_StopVoice();
			  }
			
			  public static void DispatchMessage(string callbackObj) {
			  	callback.setCallbackObj(callbackObj);
			    axvoicePINVOKE.AxVoice_DispatchMessage(AxVoiceCallback.getCPtr(callback));
			  }
	#else
	//NOT SUPPORT YET!
			  public static void Init(string cachePath, string uploadURL) {
			  	
			  }
			
			  public static uint BeginRecord() {
			    return 0;
			  }
			
			  public static void CompleteRecord(uint voiceID) {
			  }
			
			  public static void UploadVoice(uint voiceID) {
			  }
			
			  public static uint CreatVoice(string szServerURL) {
			  	return 0;
			  }
			
			  public static void DownloadVoice(uint voiceID) {
			  }
			
			  public static void PlayVoice(uint voiceID) {
			  }
			
			  public static void StopVoice() {
			  }
			
			  public static void DispatchMessage(string callbackObj) {
			  }
			  
	#endif
	}
	
}
