//
//  Voice2TextWrap.h
//  axvoice.ios
//
//  Created by JinChao on 15/1/21.
//  Copyright (c) 2015年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_Voice2TextWrap_h
#define axvoice_ios_Voice2TextWrap_h

#import "iflyMSC/IFlySpeechRecognizerDelegate.h"

@class IFlySpeechRecognizer;

@protocol ConvertCallBackProtocol <NSObject>

@required

- (void) onConvertComplete : (uint)voiceID
                 strResult : (NSString*) strResult;
- (void) onConvertFailed : (uint)voiceID
                 reason : (NSString*) reason;
@end

@interface IFlyEngine : NSObject<IFlySpeechRecognizerDelegate>

- (void) initEngine : (NSString*)appID;

- (bool) beginConvert2Voice : (uint)voiceID
                   delegate : (id<ConvertCallBackProtocol>) delegate;

+ (id)sharedInstance;


@end

#endif
