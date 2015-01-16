//
//  VoicePlayer.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#import "VoicePlayer.h"

@interface VoicePlayer() <AVAudioPlayerDelegate, AVAudioSessionDelegate>
{
    uint _voiceID;
    NSString *_localWAVFile;
    AVAudioPlayer *_mediaPlayer;
    id<PlayCallBackProtocol> _delegate;
}
@end

@implementation VoicePlayer

+ (id)sharedInstance
{
    static VoicePlayer *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        sharedInstance = [[self alloc] init];
        [sharedInstance setupSelf];
    });
    
    return sharedInstance;
}

- (void) setupSelf
{
    _mediaPlayer = nil;
}

- (bool)playVoiceFile : (uint) voiceID
         localWavFile : (NSString*)localWavFile
             delegate : (id<PlayCallBackProtocol>) delegate
{
    _voiceID = voiceID;
    _localWAVFile = localWavFile;
    _delegate = delegate;
    
    NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:_localWAVFile];
    _mediaPlayer  = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
    _mediaPlayer.volume = 100.0f;
    
    if (_mediaPlayer.duration < 1)
    {
        //[self audioPlayerDidFinishPlaying:nil successfully:YES];
        return true;
    }
    
    _mediaPlayer.delegate = self;
    
    [_mediaPlayer prepareToPlay];
    
    [_mediaPlayer play];
    
    return true;
}

- (void)stopPlay
{
    if(_mediaPlayer != nil)
    {
        [_mediaPlayer stop];
        _mediaPlayer = nil;
    }
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player
                       successfully:(BOOL)flag  {
    [_delegate onPlayComplete:_voiceID];
}

@end