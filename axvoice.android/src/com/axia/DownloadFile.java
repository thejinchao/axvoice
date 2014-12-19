package com.axia;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;


public class DownloadFile extends BaseHttp implements Runnable{
	
	public interface Callback {
		public void onDownloading(long voiceID);
		public void onDownloadComplete(long voiceID);
		public void onDownloadFailed(long voiceID, String reason);
	}	
	public static final int BUFFER_SIZE = 2 * 1024;

	private long voiceID;
	private String localFile;
	private Callback callBack;
	
	public DownloadFile(long voiceID, String serverURL, String localFile, Callback callBack) {
		super(serverURL);
		
		this.voiceID = voiceID;
		this.localFile = localFile;
		this.callBack = callBack;
	}

	@Override
	public void run() {
		if (connection != null) {
			InputStream is = null;
			try {
				connection.connect();
				long length = connection.getContentLength();
				is = connection.getInputStream();
				writeInputSteamToSD(is, length);
				
				// download complete
				callBack.onDownloadComplete(voiceID);
			} catch (Exception e) {
				// download failed
				callBack.onDownloadFailed(voiceID, e.toString());
			} finally {
				if (is != null) {
					try {
						is.close();
					} catch (IOException e) {
					}
				}
				super.close();
			}
		}		
	}
	
	private void writeInputSteamToSD(InputStream is, long length) {
		File cacheFile = new File(localFile);
		File tmpFile =  new File(cacheFile.getAbsolutePath() + ".tmp");
		FileOutputStream fos = null;
		try {
			if (!tmpFile.getParentFile().exists()) {
				tmpFile.getParentFile().mkdirs();
			}

			if (cacheFile.exists()) {
				cacheFile.delete();
			}

			if (tmpFile.exists()) {
				tmpFile.delete();
			}

			fos = new FileOutputStream(tmpFile);
			byte[] buf = new byte[BUFFER_SIZE];
			int len = 0;
			//int downloaded = 0;
			while ((len = is.read(buf)) != -1) {
				fos.write(buf, 0, len);
				//downloaded += len;
				//core.onDownloadFileProgress(length, downloaded, flag);
			}

			if (!tmpFile.renameTo(cacheFile)) {
				tmpFile.delete();
			}
		} catch (IOException e) {
			tmpFile.delete();
			callBack.onDownloadFailed(voiceID, e.toString());
		} finally {
			try {
				if (fos != null) {
					fos.close();
					fos = null;
				}
			} catch (IOException e) {
			}
		}
	}
}
