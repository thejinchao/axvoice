//
//  VoiceMessage.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VoiceMessage.h"

@interface VoiceMessage()
{
    uint _voiceID;
    MessageType _msgType;
    NSArray* _params;
}

@end

@implementation VoiceMessage

@synthesize voiceID = _voiceID;
@synthesize msgType = _msgType;
@synthesize params = _params;


- (id)initWithParam : (uint)voiceID
            msgType : (MessageType)msgType
              params: (NSArray*)params
{
    if (self = [super init]) {
        
        _voiceID = voiceID;
        _msgType = msgType;
        _params = params;
    }
    return self;
}

@end
