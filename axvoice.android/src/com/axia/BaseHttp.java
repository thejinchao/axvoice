package com.axia;

import java.net.HttpURLConnection;
import java.net.URL;

public class BaseHttp {
	protected HttpURLConnection connection = null;
	protected String serverURL;
	
	protected BaseHttp(String serverURL)
	{
		this.serverURL = serverURL;
		try {
			URL url = new URL(serverURL);
			this.connection = (HttpURLConnection) url.openConnection();
			//TODO: use proxy server if failed
		}catch (Exception e) {
			
		}
	}
	
	protected void close() {
		if (connection != null) {
			try {
				connection.disconnect();
			} catch (Exception e) {
				//LogHelper.e(tag + "_basehttp", e.getMessage());
			}
		}
	}	
	
}
