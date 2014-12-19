package com.axia;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;

import android.os.Handler;
import android.os.HandlerThread;

public class AsynThreadPool extends HandlerThread {
	private static final String	THREAD_NAME	= AsynThreadPool.class.getName();
	private Handler handler;
	private ScheduledExecutorService scheduledSingleThreadPool = Executors.newSingleThreadScheduledExecutor();  
	public static AsynThreadPool asynThreadPool;
	
	public AsynThreadPool() {
		super(THREAD_NAME);
		start();
		handler=new Handler(getLooper());
	}
	
	public static AsynThreadPool getInstance() {
		if (asynThreadPool == null) {
			asynThreadPool = new AsynThreadPool();
		}

		return asynThreadPool;
	}

	public void runSingleThread(Runnable run){
		scheduledSingleThreadPool.submit(run);
	}
}
