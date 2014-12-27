//
//  DownloadFile.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DownloadFile.h"
#import "VoiceConverter.h"


@interface DownloadFile()
{
    uint _voiceID;
    NSString* _localWAVFile;
    NSString* _localAMRFile;
    NSString* _serverURL;
    id<DownloadCallBackProtocol> _delegate;
}

@end

@implementation DownloadFile

@synthesize voiceID = _voiceID;
@synthesize localAMRFile = _localAMRFile;
@synthesize localWAVFile = _localWAVFile;
@synthesize serverURL = _serverURL;


- (id)initWithParam : (uint)voiceID
       localAMRFile : (NSString*) localAMRFile
       localWAVFile : (NSString*) localWAVFile
          serverURL : (NSString*) serverURL
           delegate : (id<DownloadCallBackProtocol>) delegate
{
    if (self = [super init]) {
        _voiceID = voiceID;
        _localAMRFile = localAMRFile;
        _localWAVFile = localWAVFile;
        _serverURL = serverURL;
        _delegate = delegate;
    }
    return self;
}

- (void) run
{
    NSMutableURLRequest *myRequest = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:_serverURL]
                                                                  cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                              timeoutInterval:60 * 3];

    NSOperationQueue *queue = [[NSOperationQueue alloc] init];
    [NSURLConnection sendAsynchronousRequest:myRequest queue:queue completionHandler:^(NSURLResponse *response, NSData *result, NSError *error){
        if(result)
        {
            if ([result writeToFile:_localAMRFile atomically:YES])
            {
                if([VoiceConverter amrToWav:_localAMRFile wavPathName:_localWAVFile])
                {
                    //ok
                    [_delegate onDownloadComplete:_voiceID];
                }
                else{
                    [_delegate onDownloadFailed:_voiceID
                                         reason:@"Convert To Wav error"];
                    
                }
            }
            else{
                [_delegate onDownloadFailed:_voiceID
                                     reason:@"Write AMR file error"];
                
            }
        }
        else {
            [_delegate onDownloadFailed:_voiceID
                                 reason:[NSString stringWithFormat:@"NSError=%@", error]];
        }
    }];
/*
    NSError *error;
    NSHTTPURLResponse *response;
    NSData* result = [NSURLConnection sendSynchronousRequest:myRequest returningResponse:&response error:&error];
    if (result)
    {
        if ([result writeToFile:_localAMRFile atomically:YES])
        {
            [VoiceConverter amrToWav:_localAMRFile wavPathName:_localWAVFile];
        }
    }
*/
}

@end