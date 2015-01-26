package com.axia;

import java.io.FileInputStream;
import java.io.FileOutputStream;

public class AmrDecoder {
    static {
        System.loadLibrary("amr-codec");
    }

    private final int MAX_AMR_FRAME_SIZE = 32;
    private final int PCM_FRAME_SIZE = 160; // 8khz 8000*0.02=160
    private final int AMR_FRAME_COUNT_PER_SECOND = 50;
    
    private final String MAGIC_HEAD = "#!AMR\n";
    
    FileInputStream fis = null;
	int stdFrameSize=0, stdFrameHeader=0;
	short[] prevPCMData;
	int prevPCMDataLength;
	
    public boolean OpenAmrFile(String fileName) {
    	init();
    	try {
    		fis = new FileInputStream(fileName);
    		
    		//read magic head
    		byte[] mgHead = new byte[MAGIC_HEAD.length()];
    		int length = fis.read(mgHead);
    		if(length!=MAGIC_HEAD.length() || MAGIC_HEAD.compareTo(new String(mgHead, "UTF8"))!=0 ) return false;
    		
    		//read first frame
    		byte[] amrFrame = new byte[MAX_AMR_FRAME_SIZE];
    		prevPCMData = new short[PCM_FRAME_SIZE];
    		if(!_readFirstFrame(amrFrame)) return false;
    		
    		decode(amrFrame, prevPCMData);
    		prevPCMDataLength = PCM_FRAME_SIZE;
    		
    	}catch(Exception ex) {
    		return false;
    	}
    	return true;
    }
    
    public int ReadNextPCMData(short[] pcmData) {
    	
    	int fillSize = 0;
    	//check last pcm data
    	if(prevPCMDataLength !=0)
    	{	
    		if(pcmData.length<prevPCMDataLength) return 0;  //not enough memory(SHOULD NOT HAPPEN)

    		System.arraycopy(prevPCMData, 0, pcmData, 0, prevPCMDataLength);
    		fillSize = prevPCMDataLength;
    		prevPCMDataLength = 0;
    	}
    	
		byte[] amrFrame = new byte[MAX_AMR_FRAME_SIZE];
    	while(fillSize+PCM_FRAME_SIZE < pcmData.length) {
	    	//read next amr frame
	    	if(!_readFrame(amrFrame)) return fillSize;
	    	
			decode(amrFrame, prevPCMData);
			
			System.arraycopy(prevPCMData, 0, pcmData, fillSize, PCM_FRAME_SIZE);
			
			fillSize += PCM_FRAME_SIZE;
    	}
    	
    	return fillSize;
    }
    
    private boolean _readFirstFrame(byte[] amrFrame) {
    	//read head
    	try {
    		if(1 != fis.read(amrFrame, 0, 1)) return false;
    		stdFrameHeader = (int)amrFrame[0];
    		stdFrameSize = _caclAMRFrameSize(amrFrame[0]);
    		if((stdFrameSize-1) != fis.read(amrFrame, 1, stdFrameSize-1)) return false;
    		
    		return true;
    	}catch(Exception ex) {
    		return false;
    	}
    }
    
    private boolean _readFrame(byte[] amrFrame) {
    	try {
    		while(true) {
    			if(1 != fis.read(amrFrame, 0, 1)) return false;
    			if(amrFrame[0]==stdFrameHeader) break;
    		}
    		
    		if((stdFrameSize-1) != fis.read(amrFrame, 1, stdFrameSize-1)) return false;
    		return true;
    	}catch(Exception ex) {
    		return false;
    	}
    }
    private int _myround(double x) {
    	return ((int)((x)+0.5));
    }
    
    private int _caclAMRFrameSize(byte frameHeader) {
    	int[] amrEncodeMode = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200};
    	
    	int temp1 = frameHeader;
    	temp1 &= 0x78; // 0111-1000
    	temp1 >>= 3;
    	int mode = amrEncodeMode[temp1];
    	
    	// 计算amr音频数据帧大小
    	// 原理: amr 一帧对应20ms，那么一秒有50帧的音频数据
    	int temp2 = _myround((double)(((double)mode / (double)AMR_FRAME_COUNT_PER_SECOND) / (double)8));
    	int frameSize = _myround((double)temp2 + 0.5);
    	
    	return frameSize;
    }
    
    private static native void init();
    private static native void exit();
    private static native int decode(byte[] in, short[] out);
}
