//
//  VoicePlayer.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_VoicePlayer_h
#define axvoice_ios_VoicePlayer_h

@protocol PlayCallBackProtocol <NSObject>

@required
- (void) onPlayComplete : (uint)voiceID;
@end

@interface VoicePlayer : NSObject

- (bool)playVoiceFile : (uint) voiceID
         localWavFile : (NSString*)localWavFile
             delegate : (id<PlayCallBackProtocol>) delegate;

- (void)stopPlay;

+ (id)sharedInstance;

@end

#endif
