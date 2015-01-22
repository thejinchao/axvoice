//
//  VoiceManager.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VoiceManager.h"
#import "VoiceItem.h"
#import "Config.h"

@interface VoiceManager()
{
    NSMutableDictionary* voiceMap;
}

@end



@implementation VoiceManager


+ (id)sharedInstance
{
    static VoiceManager *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        sharedInstance = [[self alloc] init];
        [sharedInstance setupSelf];
    });
    
    return sharedInstance;
}

- (void) setupSelf
{
    voiceMap = [[NSMutableDictionary alloc] init];
}

- (VoiceItem*) allocateNewItem
{

    uint voiceID = (uint)[[NSDate date] timeIntervalSince1970];
    
    NSString *documentDir = [Config cacheAudioPath];
    NSString *wavFile = [NSString stringWithFormat:@"%@/%d.wav", documentDir, voiceID];
    NSString *amrFile = [NSString stringWithFormat:@"%@/%d.amr", documentDir, voiceID];
    
    
    VoiceItem* newItem = [[VoiceItem alloc] initWithParam:voiceID
                                                localWavFile:wavFile
                                             localAmrFile:amrFile
                                                serverURL:@""
                                                  fileMD5:@""
                                                     text:@""
                                              localStatus:NO_LOCAL_FILE
                                             serverStatus:NO_SERVER_FILE
                                             toTextStatus:NO_TEXT];
    NSLog(@"Alloc new voice item:id=%d, local=%@", newItem.voiceID, newItem.localWavFile);
    
    [voiceMap setObject:newItem forKey:[NSString stringWithFormat:@"%d", voiceID]];
    
    return newItem;
}

- (VoiceItem*) findItem: (unsigned int) voiceID
{
    VoiceItem* item = [voiceMap objectForKey:[NSString stringWithFormat:@"%d", voiceID]];
    
    return item;
}



@end