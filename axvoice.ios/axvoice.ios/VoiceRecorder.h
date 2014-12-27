//
//  VoiceRecorder.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_VoiceRecorder_h
#define axvoice_ios_VoiceRecorder_h

@interface VoiceRecorder : NSObject


- (bool)beginRecord : (uint) voiceID
       localWavFile : (NSString*)localWavFile
        localAMRFile: (NSString*)localAMRFile;

- (void) completeRecord : (uint)voiceID;


+ (id)sharedInstance;

@end

#endif
