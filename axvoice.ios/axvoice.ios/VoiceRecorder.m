//
//  VoiceRecorder.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
//#import <AudioSession/AudioSession.h>

#import "VoiceRecorder.h"
#import "VoiceConverter.h"


@interface VoiceRecorder() <AVAudioRecorderDelegate,
AVAudioPlayerDelegate,
AVAudioSessionDelegate>
{
    uint _voiceID;
    NSURL *_localWAVFile;
    NSString *_localAMRFile;
    AVAudioRecorder *recorder;
}
@end


@implementation VoiceRecorder

+ (id)sharedInstance
{
    static VoiceRecorder *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        sharedInstance = [[self alloc] init];
        [sharedInstance setupSelf];
    });
    
    return sharedInstance;
}

- (void) setupSelf
{
    
}

- (bool)beginRecord : (uint) vid
       localWavFile : (NSString*)localWavFile
        localAMRFile: (NSString*)localAMRFile
{
    if(_voiceID == vid) return true; //already record
    
    _voiceID = vid;
    _localWAVFile = [NSURL fileURLWithPath:localWavFile];
    _localAMRFile = localAMRFile;
    //NSLog(@"Using File called: %@", _localFile);
    
    NSMutableDictionary *settings = [[NSMutableDictionary alloc] init];
    [settings setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM] forKey:AVFormatIDKey];
    [settings setValue:[NSNumber numberWithFloat:8000.0] forKey:AVSampleRateKey];
    [settings setValue:[NSNumber numberWithInt:1] forKey:AVNumberOfChannelsKey];
    [settings setValue:[NSNumber numberWithInt:16] forKey:AVLinearPCMBitDepthKey];
    [settings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
    [settings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];
    
    //Setup the recorder to use this file and record to it.
    recorder = [[ AVAudioRecorder alloc] initWithURL:_localWAVFile settings:settings error:nil];
    
    [recorder setDelegate:self];
    [recorder setMeteringEnabled:YES];
    
    [recorder prepareToRecord];

    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error: nil];
    [[AVAudioSession sharedInstance] setActive: YES error: nil];

    //Start the actual Recording
    [recorder record];
     
     //There is an optional method for doing the recording for a limited time see
    //[recorder recordForDuration:(NSTimeInterval) 60];
     
    return true;
}

- (void) completeRecord : (uint)vid
{
    if(_voiceID != vid) return; //error
    
    //Stop the recorder.
    [recorder stop];
    
    //convert to amr
    [VoiceConverter wavToAmr:[_localWAVFile path]
                 amrPathName:_localAMRFile];
}

@end