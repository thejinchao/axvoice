//
//  axvoice_ios.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VoiceMessage.h"


@interface axvoice : NSObject

+(void)initAxVoice: (NSString*)cachePath
        uploadURL : (NSString*)uploadURL
           iFlyID : (NSString*)iFlyID;

+(uint)beginRecord;

+(void)completeRecord: (uint)voiceID;

+(void)uploadVoice: (uint)voiceID;

+(uint)createVoice: (NSString*)serverURL;

+(void)downloadVoice: (uint)voiceID;

+(void)playVoice: (uint)voiceID;

+(void)stopVoice;

+(void)dispatchMessage : (id<MessageCallBack>) delegate;

+(void)dispatchMessage_Unity : (NSString*) callbackObject;

+(void)voice2Text: (uint) voiceID;

@end
