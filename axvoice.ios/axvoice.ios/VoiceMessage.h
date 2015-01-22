//
//  VoiceMessage.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_VoiceMessage_h
#define axvoice_ios_VoiceMessage_h

typedef enum {
    MT_RECORD_MSG = 1,
    MT_UPLOAD_MSG = 2,
    MT_DOWNLOAD_MSG = 3,
    MT_PLAY_MSG = 4,
    MT_TOTXT_MSG = 5
} MessageType;

@interface VoiceMessage : NSObject

- (id)initWithParam : (uint)voiceID
            msgType : (MessageType)msgType
              params: (NSArray*)params;


@property (nonatomic, assign, readonly) uint    voiceID;
@property (nonatomic, assign, readonly) MessageType msgType;
@property (nonatomic, copy, readonly) NSArray* params;

@end


@protocol MessageCallBack <NSObject>

@required
- (void)onMessage : (VoiceMessage*) msg;

@end

#endif
