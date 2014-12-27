package com.axia;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.util.Map;

public class UploadFile implements Runnable {
	public interface Callback {
		public void onUploading(long voiceID);
		public void onUploadComplete(long voiceID, String serverURL);
		public void onUploadFailed(long voiceID, String reason);
	}
	
	private long voiceID;
	private String localFileName;
	private String fileMD5;
	private String serverURL;
	private Map<String, String> mapRequest; // http request params
	private Callback callBack;

	
	public UploadFile(long voiceID, 
			String localFileName, 
			String fileMD5,
			String serverURL, 
			Map<String, String> mapRequest,
			Callback callBack) throws Exception
	{
		this.voiceID = voiceID;
		this.localFileName = localFileName;
		this.fileMD5 = fileMD5;
		this.serverURL = serverURL;
		this.mapRequest = mapRequest;
		this.callBack = callBack;
	}
	
	@Override
	public void run() {
		try {
			OpenHttpRequest httpRequest = OpenHttpRequest.post(serverURL);
			for (Map.Entry<String, String> entry : mapRequest.entrySet()) {
				httpRequest.part(entry.getKey(), entry.getValue());
			}
	
			//begin send
			httpRequest.part("-" + fileMD5 + "-", "amrfile", new File(localFileName));
			
			int statusCode = httpRequest.code();
			String result = httpRequest.body();
			
			if (statusCode == 200 || statusCode==201) {
				String[] r = result.split("\\|");
				if(r.length==2)
				{
				 	if(r[0].equals("SUCCESS"))
						callBack.onUploadComplete(voiceID, r[1].trim());
					else if(r[0].equals("ERROR"))
						callBack.onUploadFailed(voiceID, r[1].trim());
					else
						callBack.onUploadFailed(voiceID, result);
				}
			}else {
				callBack.onUploadFailed(voiceID, "HTTP_CODE="+statusCode);
			}
		}catch(Exception e) {
			callBack.onUploadFailed(voiceID, "Exception=" + e.toString());
		}

	}
}
