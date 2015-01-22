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
    NSString *_text;
    LocalStatus _localStatus;
    ServerStatus _serverStatus;
    ToTextStatus _toTextStatus;
}


@end

@implementation VoiceItem
@synthesize voiceID = _voiceID;
@synthesize localWavFile = _localWavFile;
@synthesize localAmrFile = _localAmrFile;
@synthesize serverURL = _serverURL;
@synthesize fileMD5 = _fileMD5;
@synthesize text = _text;
@synthesize localStatus = _localStatus;
@synthesize serverStatus = _serverStatus;
@synthesize toTextStatus = _toTextStatus;

- (id)initWithParam : (uint)voiceID
       localWavFile : (NSString*) localWavFile
        localAmrFile: (NSString*)localAmrFile
          serverURL : (NSString*) serverURL
            fileMD5 : (NSString*) fileMD5
               text : (NSString*) text
        localStatus : (LocalStatus) localStatus
       serverStatus : (ServerStatus) serverStatus
       toTextStatus : (ToTextStatus)toTextStatus
{
    if (self = [super init]) {
        
        _voiceID = voiceID;
        _localWavFile = localWavFile;
        _localAmrFile = localAmrFile;
        _serverURL = serverURL;
        _fileMD5 = fileMD5;
        _text = text;
        _localStatus = localStatus;
        _serverStatus = serverStatus;
        _toTextStatus = toTextStatus;
    }
    return self;
}

- (void)startRecord
{
    //TODO: Check status
    _localStatus = RECORDING;
}

- (void)stopRecord
{
    //TODO: Check status
    _localStatus = STOP_RECORDING;
}

- (void)confirmLocalFile :  (NSString*) amrMD5
{
    //TODO: Check status
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
    //TODO: Check status
    _serverURL = serverURL;
    _serverStatus = HAS_SERVER_FILE;
}

- (void)beginDownload
{
    //TODO: Check status
    _localStatus = DOWNLOADING;
}

- (void)beginConvertToText
{
    if([self localStatus] != HAS_LOCAL_FILE ||
       [self toTextStatus] != NO_TEXT)
    {
        return; //TODO: ERROR
    }
    _toTextStatus = CONVERTING;
}

- (void)setText : (bool) success
           text : (NSString *)text
{
    if([self localStatus] != HAS_LOCAL_FILE ||
       [self toTextStatus] != CONVERTING)
    {
        return; //TODO: ERROR
    }
    
    if(success) {
        _toTextStatus = HAS_TEXT;
        _text = text;
    }else {
        _toTextStatus = NO_TEXT;
    }
}
@end