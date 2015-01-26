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
        private static extern void _initAxVoice(string cachePath, string uploadURL, string iflyID);

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

        [DllImport ("__Internal")]
        private static extern void _voice2Text(uint voiceID);

        public static void Init(string cachePath, string uploadURL, string iflyID)
        {
        	_initAxVoice(cachePath, uploadURL, iflyID);
        }
        
			  public static uint BeginRecord() {
			    uint voiceID = (uint)_beginRecord();
			    return voiceID;
			  }

			  public static void CompleteRecord(uint voiceID) {
			    _completeRecord(voiceID);
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
				  
				public static void Voice2Text(uint voiceID) {
			    _voice2Text(voiceID); 
			  }
	#elif UNITY_ANDROID
        static private AndroidJavaClass clsAxVoice = new AndroidJavaClass("com.axia.AxVoice");
        
        public static void Init(string cachePath, string uploadURL, string iflyID)
        {
        	clsAxVoice.CallStatic("init", cachePath, uploadURL, iflyID); 
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
			  
				public static void Voice2Text(uint voiceID) {
			    clsAxVoice.CallStatic("voice2Text", (long)voiceID); 
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
		    		else if(msg.getType() == AxVoiceMessage.Type.MT_TOTXT_MSG)
		    		{
		    			callbackObj.SendMessage("OnAxVoiceTextMessage", new string[]{ ""+msg.getVoiceID(), msg.getParam(0), msg.getParam(1), msg.getParam(2)});
		    		}
		    	}
		    }
		    
		    static private Callback callback = new Callback();
		    
			  public static bool Init(string cachePath, string uploadURL, string iflyID) {
			    bool ret = axvoicePINVOKE.AxVoice_Init(cachePath, uploadURL, iflyID);
			    return ret;
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
			
			  public static bool Voice2Text(uint voiceID) {
			    bool ret = axvoicePINVOKE.AxVoice_Voice2Text(voiceID);
			    return ret;
			  }
			
			  public static void DispatchMessage(string callbackObj) {
			  	callback.setCallbackObj(callbackObj);
			    axvoicePINVOKE.AxVoice_DispatchMessage(AxVoiceCallback.getCPtr(callback));
			  }

	#else
	//NOT SUPPORT YET!
			  public static void Init(string cachePath, string uploadURL, string iflyID) {
			  	
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
			  
				public static void Voice2Text(uint voiceID) {
			  }			  
	#endif
	}
}

#if UNITY_STANDALONE_WIN
/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace com.axia {

public class AxVoiceCallback : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal AxVoiceCallback(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(AxVoiceCallback obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~AxVoiceCallback() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          axvoicePINVOKE.delete_AxVoiceCallback(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public virtual void onMessage(AxVoiceMessage message) {
    axvoicePINVOKE.AxVoiceCallback_onMessage(swigCPtr, AxVoiceMessage.getCPtr(message));
  }

  public AxVoiceCallback() : this(axvoicePINVOKE.new_AxVoiceCallback(), true) {
    SwigDirectorConnect();
  }

  private void SwigDirectorConnect() {
    if (SwigDerivedClassHasMethod("onMessage", swigMethodTypes0))
      swigDelegate0 = new SwigDelegateAxVoiceCallback_0(SwigDirectoronMessage);
    axvoicePINVOKE.AxVoiceCallback_director_connect(swigCPtr, swigDelegate0);
  }

  private bool SwigDerivedClassHasMethod(string methodName, global::System.Type[] methodTypes) {
    global::System.Reflection.MethodInfo methodInfo = this.GetType().GetMethod(methodName, global::System.Reflection.BindingFlags.Public | global::System.Reflection.BindingFlags.NonPublic | global::System.Reflection.BindingFlags.Instance, null, methodTypes, null);
    bool hasDerivedMethod = methodInfo.DeclaringType.IsSubclassOf(typeof(AxVoiceCallback));
    return hasDerivedMethod;
  }

  private void SwigDirectoronMessage(global::System.IntPtr message) {
    onMessage((message == global::System.IntPtr.Zero) ? null : new AxVoiceMessage(message, false));
  }

  public delegate void SwigDelegateAxVoiceCallback_0(global::System.IntPtr message);

  private SwigDelegateAxVoiceCallback_0 swigDelegate0;

  private static global::System.Type[] swigMethodTypes0 = new global::System.Type[] { typeof(AxVoiceMessage) };
}

}

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace com.axia {

public class AxVoiceMessage : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal AxVoiceMessage(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(AxVoiceMessage obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~AxVoiceMessage() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          axvoicePINVOKE.delete_AxVoiceMessage(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public virtual AxVoiceMessage.Type getType() {
    AxVoiceMessage.Type ret = (AxVoiceMessage.Type)axvoicePINVOKE.AxVoiceMessage_getType(swigCPtr);
    return ret;
  }

  public virtual uint getVoiceID() {
    uint ret = axvoicePINVOKE.AxVoiceMessage_getVoiceID(swigCPtr);
    return ret;
  }

  public virtual int getParamCounts() {
    int ret = axvoicePINVOKE.AxVoiceMessage_getParamCounts(swigCPtr);
    return ret;
  }

  public virtual string getParam(int index) {
    string ret = axvoicePINVOKE.AxVoiceMessage_getParam(swigCPtr, index);
    return ret;
  }

  public AxVoiceMessage() : this(axvoicePINVOKE.new_AxVoiceMessage(), true) {
    SwigDirectorConnect();
  }

  private void SwigDirectorConnect() {
    if (SwigDerivedClassHasMethod("getType", swigMethodTypes0))
      swigDelegate0 = new SwigDelegateAxVoiceMessage_0(SwigDirectorgetType);
    if (SwigDerivedClassHasMethod("getVoiceID", swigMethodTypes1))
      swigDelegate1 = new SwigDelegateAxVoiceMessage_1(SwigDirectorgetVoiceID);
    if (SwigDerivedClassHasMethod("getParamCounts", swigMethodTypes2))
      swigDelegate2 = new SwigDelegateAxVoiceMessage_2(SwigDirectorgetParamCounts);
    if (SwigDerivedClassHasMethod("getParam", swigMethodTypes3))
      swigDelegate3 = new SwigDelegateAxVoiceMessage_3(SwigDirectorgetParam);
    axvoicePINVOKE.AxVoiceMessage_director_connect(swigCPtr, swigDelegate0, swigDelegate1, swigDelegate2, swigDelegate3);
  }

  private bool SwigDerivedClassHasMethod(string methodName, global::System.Type[] methodTypes) {
    global::System.Reflection.MethodInfo methodInfo = this.GetType().GetMethod(methodName, global::System.Reflection.BindingFlags.Public | global::System.Reflection.BindingFlags.NonPublic | global::System.Reflection.BindingFlags.Instance, null, methodTypes, null);
    bool hasDerivedMethod = methodInfo.DeclaringType.IsSubclassOf(typeof(AxVoiceMessage));
    return hasDerivedMethod;
  }

  private int SwigDirectorgetType() {
    return (int)getType();
  }

  private uint SwigDirectorgetVoiceID() {
    return getVoiceID();
  }

  private int SwigDirectorgetParamCounts() {
    return getParamCounts();
  }

  private string SwigDirectorgetParam(int index) {
    return getParam(index);
  }

  public delegate int SwigDelegateAxVoiceMessage_0();
  public delegate uint SwigDelegateAxVoiceMessage_1();
  public delegate int SwigDelegateAxVoiceMessage_2();
  public delegate string SwigDelegateAxVoiceMessage_3(int index);

  private SwigDelegateAxVoiceMessage_0 swigDelegate0;
  private SwigDelegateAxVoiceMessage_1 swigDelegate1;
  private SwigDelegateAxVoiceMessage_2 swigDelegate2;
  private SwigDelegateAxVoiceMessage_3 swigDelegate3;

  private static global::System.Type[] swigMethodTypes0 = new global::System.Type[] {  };
  private static global::System.Type[] swigMethodTypes1 = new global::System.Type[] {  };
  private static global::System.Type[] swigMethodTypes2 = new global::System.Type[] {  };
  private static global::System.Type[] swigMethodTypes3 = new global::System.Type[] { typeof(int) };
  public enum Type {
    MT_RECORD_MSG = 1,
    MT_UPLOAD_MSG = 2,
    MT_DOWNLOAD_MSG = 3,
    MT_PLAY_MSG = 4,
    MT_TOTXT_MSG = 5
  }

}

}

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace com.axia {

class axvoicePINVOKE {

  protected class SWIGExceptionHelper {

    public delegate void ExceptionDelegate(string message);
    public delegate void ExceptionArgumentDelegate(string message, string paramName);

    static ExceptionDelegate applicationDelegate = new ExceptionDelegate(SetPendingApplicationException);
    static ExceptionDelegate arithmeticDelegate = new ExceptionDelegate(SetPendingArithmeticException);
    static ExceptionDelegate divideByZeroDelegate = new ExceptionDelegate(SetPendingDivideByZeroException);
    static ExceptionDelegate indexOutOfRangeDelegate = new ExceptionDelegate(SetPendingIndexOutOfRangeException);
    static ExceptionDelegate invalidCastDelegate = new ExceptionDelegate(SetPendingInvalidCastException);
    static ExceptionDelegate invalidOperationDelegate = new ExceptionDelegate(SetPendingInvalidOperationException);
    static ExceptionDelegate ioDelegate = new ExceptionDelegate(SetPendingIOException);
    static ExceptionDelegate nullReferenceDelegate = new ExceptionDelegate(SetPendingNullReferenceException);
    static ExceptionDelegate outOfMemoryDelegate = new ExceptionDelegate(SetPendingOutOfMemoryException);
    static ExceptionDelegate overflowDelegate = new ExceptionDelegate(SetPendingOverflowException);
    static ExceptionDelegate systemDelegate = new ExceptionDelegate(SetPendingSystemException);

    static ExceptionArgumentDelegate argumentDelegate = new ExceptionArgumentDelegate(SetPendingArgumentException);
    static ExceptionArgumentDelegate argumentNullDelegate = new ExceptionArgumentDelegate(SetPendingArgumentNullException);
    static ExceptionArgumentDelegate argumentOutOfRangeDelegate = new ExceptionArgumentDelegate(SetPendingArgumentOutOfRangeException);

    [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="SWIGRegisterExceptionCallbacks_axvoice")]
    public static extern void SWIGRegisterExceptionCallbacks_axvoice(
                                ExceptionDelegate applicationDelegate,
                                ExceptionDelegate arithmeticDelegate,
                                ExceptionDelegate divideByZeroDelegate, 
                                ExceptionDelegate indexOutOfRangeDelegate, 
                                ExceptionDelegate invalidCastDelegate,
                                ExceptionDelegate invalidOperationDelegate,
                                ExceptionDelegate ioDelegate,
                                ExceptionDelegate nullReferenceDelegate,
                                ExceptionDelegate outOfMemoryDelegate, 
                                ExceptionDelegate overflowDelegate, 
                                ExceptionDelegate systemExceptionDelegate);

    [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="SWIGRegisterExceptionArgumentCallbacks_axvoice")]
    public static extern void SWIGRegisterExceptionCallbacksArgument_axvoice(
                                ExceptionArgumentDelegate argumentDelegate,
                                ExceptionArgumentDelegate argumentNullDelegate,
                                ExceptionArgumentDelegate argumentOutOfRangeDelegate);

    static void SetPendingApplicationException(string message) {
      SWIGPendingException.Set(new global::System.ApplicationException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingArithmeticException(string message) {
      SWIGPendingException.Set(new global::System.ArithmeticException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingDivideByZeroException(string message) {
      SWIGPendingException.Set(new global::System.DivideByZeroException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingIndexOutOfRangeException(string message) {
      SWIGPendingException.Set(new global::System.IndexOutOfRangeException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingInvalidCastException(string message) {
      SWIGPendingException.Set(new global::System.InvalidCastException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingInvalidOperationException(string message) {
      SWIGPendingException.Set(new global::System.InvalidOperationException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingIOException(string message) {
      SWIGPendingException.Set(new global::System.IO.IOException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingNullReferenceException(string message) {
      SWIGPendingException.Set(new global::System.NullReferenceException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingOutOfMemoryException(string message) {
      SWIGPendingException.Set(new global::System.OutOfMemoryException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingOverflowException(string message) {
      SWIGPendingException.Set(new global::System.OverflowException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingSystemException(string message) {
      SWIGPendingException.Set(new global::System.SystemException(message, SWIGPendingException.Retrieve()));
    }

    static void SetPendingArgumentException(string message, string paramName) {
      SWIGPendingException.Set(new global::System.ArgumentException(message, paramName, SWIGPendingException.Retrieve()));
    }
    static void SetPendingArgumentNullException(string message, string paramName) {
      global::System.Exception e = SWIGPendingException.Retrieve();
      if (e != null) message = message + " Inner Exception: " + e.Message;
      SWIGPendingException.Set(new global::System.ArgumentNullException(paramName, message));
    }
    static void SetPendingArgumentOutOfRangeException(string message, string paramName) {
      global::System.Exception e = SWIGPendingException.Retrieve();
      if (e != null) message = message + " Inner Exception: " + e.Message;
      SWIGPendingException.Set(new global::System.ArgumentOutOfRangeException(paramName, message));
    }

    static SWIGExceptionHelper() {
      SWIGRegisterExceptionCallbacks_axvoice(
                                applicationDelegate,
                                arithmeticDelegate,
                                divideByZeroDelegate,
                                indexOutOfRangeDelegate,
                                invalidCastDelegate,
                                invalidOperationDelegate,
                                ioDelegate,
                                nullReferenceDelegate,
                                outOfMemoryDelegate,
                                overflowDelegate,
                                systemDelegate);

      SWIGRegisterExceptionCallbacksArgument_axvoice(
                                argumentDelegate,
                                argumentNullDelegate,
                                argumentOutOfRangeDelegate);
    }
  }

  protected static SWIGExceptionHelper swigExceptionHelper = new SWIGExceptionHelper();

  public class SWIGPendingException {
    [global::System.ThreadStatic]
    private static global::System.Exception pendingException = null;
    private static int numExceptionsPending = 0;

    public static bool Pending {
      get {
        bool pending = false;
        if (numExceptionsPending > 0)
          if (pendingException != null)
            pending = true;
        return pending;
      } 
    }

    public static void Set(global::System.Exception e) {
      if (pendingException != null)
        throw new global::System.ApplicationException("FATAL: An earlier pending exception from unmanaged code was missed and thus not thrown (" + pendingException.ToString() + ")", e);
      pendingException = e;
      lock(typeof(axvoicePINVOKE)) {
        numExceptionsPending++;
      }
    }

    public static global::System.Exception Retrieve() {
      global::System.Exception e = null;
      if (numExceptionsPending > 0) {
        if (pendingException != null) {
          e = pendingException;
          pendingException = null;
          lock(typeof(axvoicePINVOKE)) {
            numExceptionsPending--;
          }
        }
      }
      return e;
    }
  }


  protected class SWIGStringHelper {

    public delegate string SWIGStringDelegate(string message);
    static SWIGStringDelegate stringDelegate = new SWIGStringDelegate(CreateString);

    [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="SWIGRegisterStringCallback_axvoice")]
    public static extern void SWIGRegisterStringCallback_axvoice(SWIGStringDelegate stringDelegate);

    static string CreateString(string cString) {
      return cString;
    }

    static SWIGStringHelper() {
      SWIGRegisterStringCallback_axvoice(stringDelegate);
    }
  }

  static protected SWIGStringHelper swigStringHelper = new SWIGStringHelper();


  static axvoicePINVOKE() {
  }


  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_Init")]
  public static extern bool AxVoice_Init(string jarg1, string jarg2, string jarg3);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_BeginRecord")]
  public static extern uint AxVoice_BeginRecord();

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_CompleteRecord")]
  public static extern void AxVoice_CompleteRecord(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_UploadVoice")]
  public static extern void AxVoice_UploadVoice(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_CreatVoice")]
  public static extern uint AxVoice_CreatVoice(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_DownloadVoice")]
  public static extern void AxVoice_DownloadVoice(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_PlayVoice")]
  public static extern void AxVoice_PlayVoice(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_StopVoice")]
  public static extern void AxVoice_StopVoice();

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_Voice2Text")]
  public static extern bool AxVoice_Voice2Text(uint jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceMessage_getType")]
  public static extern int AxVoiceMessage_getType(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceMessage_getVoiceID")]
  public static extern uint AxVoiceMessage_getVoiceID(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceMessage_getParamCounts")]
  public static extern int AxVoiceMessage_getParamCounts(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceMessage_getParam")]
  public static extern string AxVoiceMessage_getParam(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_new_AxVoiceMessage")]
  public static extern global::System.IntPtr new_AxVoiceMessage();

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_delete_AxVoiceMessage")]
  public static extern void delete_AxVoiceMessage(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceMessage_director_connect")]
  public static extern void AxVoiceMessage_director_connect(global::System.Runtime.InteropServices.HandleRef jarg1, AxVoiceMessage.SwigDelegateAxVoiceMessage_0 delegate0, AxVoiceMessage.SwigDelegateAxVoiceMessage_1 delegate1, AxVoiceMessage.SwigDelegateAxVoiceMessage_2 delegate2, AxVoiceMessage.SwigDelegateAxVoiceMessage_3 delegate3);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_delete_AxVoiceCallback")]
  public static extern void delete_AxVoiceCallback(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceCallback_onMessage")]
  public static extern void AxVoiceCallback_onMessage(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_new_AxVoiceCallback")]
  public static extern global::System.IntPtr new_AxVoiceCallback();

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoiceCallback_director_connect")]
  public static extern void AxVoiceCallback_director_connect(global::System.Runtime.InteropServices.HandleRef jarg1, AxVoiceCallback.SwigDelegateAxVoiceCallback_0 delegate0);

  [global::System.Runtime.InteropServices.DllImport("axvoice", EntryPoint="CSharp_AxVoice_DispatchMessage")]
  public static extern void AxVoice_DispatchMessage(global::System.Runtime.InteropServices.HandleRef jarg1);
}

}

#endif
