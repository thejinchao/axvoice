//
//  VoiceManager.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_VoiceManager_h
#define axvoice_ios_VoiceManager_h

//pre-define
@class VoiceItem;


@interface VoiceManager : NSObject

- (VoiceItem*) allocateNewItem;
- (VoiceItem*) findItem: (unsigned int) voiceID;

+ (id)sharedInstance;

@end

#endif
