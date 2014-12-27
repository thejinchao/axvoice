//
//  VoiceItem.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VoiceItem.h"

@interface VoiceItem()
{
    uint _voiceID;
    NSString *_localWavFile;
    NSString *_localAmrFile;
    NSString *_fileMD5;
    NSString *_serverURL;
    LocalStatus _localStatus;
    ServerStatus _serverStatus;
}


@end

@implementation VoiceItem
@synthesize voiceID = _voiceID;
@synthesize localWavFile = _localWavFile;
@synthesize localAmrFile = _localAmrFile;
@synthesize serverURL = _serverURL;
@synthesize fileMD5 = _fileMD5;
@synthesize localStatus = _localStatus;
@synthesize serverStatus = _serverStatus;

- (id)initWithParam : (uint)voiceID
       localWavFile : (NSString*) localWavFile
        localAmrFile: (NSString*)localAmrFile
          serverURL : (NSString*) serverURL
            fileMD5 : (NSString*) fileMD5
        localStatus : (LocalStatus) localStatus
       serverStatus : (ServerStatus) serverStatus
{
    if (self = [super init]) {
        
        _voiceID = voiceID;
        _localWavFile = localWavFile;
        _localAmrFile = localAmrFile;
        _serverURL = serverURL;
        _fileMD5 = fileMD5;
        _localStatus = localStatus;
        _serverStatus = serverStatus;

    }
    return self;
}

- (void)startRecord
{
    _localStatus = RECORDING;
}

- (void)stopRecord
{
    _localStatus = STOP_RECORDING;
}

- (void)confirmLocalFile :  (NSString*) amrMD5
{
    _fileMD5 = amrMD5;
    _localStatus = HAS_LOCAL_FILE;
}

- (void)beginUpload
{
    if([self localStatus] != HAS_LOCAL_FILE ||
       [self serverStatus] != NO_SERVER_FILE)
    {
        return; //TODO: ERROR
    }
    
    _serverStatus = UPLOADING;
}

- (void)updateServerURL:(NSString *)serverURL
{
    _serverURL = serverURL;
    _serverStatus = HAS_SERVER_FILE;
}

- (void)beginDownload
{
    _localStatus = DOWNLOADING;
}
@end