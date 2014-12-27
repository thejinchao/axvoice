//
//  MessageQueue.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_MessageQueue_h
#define axvoice_ios_MessageQueue_h

#import "VoiceMessage.h"

@interface MessageQueue : NSObject

- (void) pushMessage : (MessageType) msgType
             voiceID : (uint)voiceID
              params : (NSArray*) param;

- (void) pushMessage : (MessageType) msgType
             voiceID : (uint)voiceID
                  p1 : (NSString*) p1;

- (void) pushMessage : (MessageType) msgType
             voiceID : (uint)voiceID
                  p1 : (NSString*) p1
                  p2 : (bool) p2
                  p3 : (NSString*)p3;

- (void)dispatchMessage : (id<MessageCallBack>) delegate;

- (void)dispatchMessage_Unity : (NSString*) callbackObj;

+ (id)sharedInstance;

@end

#endif
