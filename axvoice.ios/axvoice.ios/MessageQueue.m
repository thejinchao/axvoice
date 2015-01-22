//
//  MessageQueue.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MessageQueue.h"
#import "UnityInterface.h"


@interface MessageQueue()
{
    NSMutableArray *_messageQueueArray;
}

@end


@implementation MessageQueue

+ (id)sharedInstance
{
    static MessageQueue *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        sharedInstance = [[self alloc] init];
        [sharedInstance setupSelf];
    });
    
    return sharedInstance;
}

- (void) setupSelf
{
    _messageQueueArray = [[NSMutableArray alloc] init];
}

- (void) pushMessage : (MessageType) msgType
             voiceID : (uint)voiceID
              params : (NSArray*) param
{
    VoiceMessage* msg = [[VoiceMessage alloc] initWithParam:voiceID msgType:msgType params:param];
    @synchronized(_messageQueueArray) {
        [_messageQueueArray addObject:msg];
    }
}

- (void) pushMessage : (MessageType) msgType
             voiceID : (uint)voiceID
                  p1 : (NSString*) p1
{
    NSArray* params = [NSArray arrayWithObjects : p1, nil];
    [self pushMessage : msgType
              voiceID : voiceID
               params : params];
}

- (void) pushMessage : (MessageType) msgType
             voiceID : (uint)voiceID
                  p1 : (NSString*) p1
                  p2 : (bool) p2
                  p3 : (NSString*)p3
{
    NSArray* params = [NSArray arrayWithObjects : p1, p2?@"true":@"false", p3, nil];
    [self pushMessage : msgType
              voiceID : voiceID
               params : params];
    
}

-(void)dispatchMessage : (id<MessageCallBack>) delegate
{
    @synchronized(_messageQueueArray) {
        for(VoiceMessage *msg in _messageQueueArray)
        {
            [delegate onMessage:msg];
        }
        
        [_messageQueueArray removeAllObjects];
    }
}

- (NSString *) union_params : (VoiceMessage*) msg
{
    NSString* ret = [NSString stringWithFormat:@"%d", msg.voiceID];
    
    for(NSString* p in msg.params) {
        ret = [ret stringByAppendingFormat : @"|%@", p];
    }
    return ret;
}

- (void)dispatchMessage_Unity : (NSString*) callbackObj
{
    @synchronized(_messageQueueArray) {
        for(VoiceMessage *msg in _messageQueueArray)
        {
            switch([msg msgType])
            {
                case MT_RECORD_MSG:
                    [UnityInterface UnitySendMessageWrap : callbackObj
                                            functionName : @"_OnAxVoiceRecordMessage"
                                                  params : [self union_params:msg]];
                    break;
                    
                case MT_UPLOAD_MSG:
                    [UnityInterface UnitySendMessageWrap : callbackObj
                                            functionName : @"_OnAxVoiceUploadMessage"
                                                  params : [self union_params:msg]];
                    break;
                    
                case MT_DOWNLOAD_MSG:
                    [UnityInterface UnitySendMessageWrap : callbackObj
                                            functionName : @"_OnAxVoiceDownloadMessage"
                                                  params : [self union_params:msg]];
                    
                    break;
                    
                case MT_PLAY_MSG:
                    [UnityInterface UnitySendMessageWrap : callbackObj
                                            functionName : @"_OnAxVoicePlayMessage"
                                                  params : [self union_params:msg]];
                    break;
                    
                case MT_TOTXT_MSG:
                    [UnityInterface UnitySendMessageWrap : callbackObj
                                            functionName : @"_OnAxVoiceTextMessage"
                                                  params : [self union_params:msg]];
                    break;
                    
            }
        }
        
        [_messageQueueArray removeAllObjects];
    }
}
@end