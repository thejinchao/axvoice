//
//  Config.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Config.h"


@interface Config()
{
    NSString* _cacheAudioPath;
    NSString* _uploadURL;
}

@end

@implementation Config


+ (id)sharedInstance : (NSString*) cacheAudioPath
           uploadURL : (NSString*) uploadURL;
{
    static Config *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        sharedInstance = [[self alloc] init];
        [sharedInstance setupSelf:cacheAudioPath
                        uploadURL:uploadURL];
    });
    
    return sharedInstance;
}

- (void) setupSelf : (NSString*) cacheAudioPath
         uploadURL : (NSString*) uploadURL
{
    if(cacheAudioPath==nil)
    {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        _cacheAudioPath = [documentsDirectory stringByAppendingPathComponent:@"axvoice_cache"];
        
        NSFileManager *fileManager = [NSFileManager defaultManager];
            BOOL isDir;
        if (![fileManager fileExistsAtPath:_cacheAudioPath isDirectory:&isDir])
        {
            [fileManager createDirectoryAtPath:_cacheAudioPath withIntermediateDirectories:YES attributes:nil error:nil];
        }
    }
    else{
        _cacheAudioPath = cacheAudioPath;
    }
 
    if(uploadURL==nil) {
        _uploadURL = @"http://www.dashengine.com/upload_voice";
    }
    else{
        _uploadURL = uploadURL;
    }
    
}

+ (NSString*) cacheAudioPath
{
    Config* config = [Config sharedInstance:nil uploadURL:nil];
    return config->_cacheAudioPath;
}

+ (NSString*) uploadURL
{
    Config* config = [Config sharedInstance:nil uploadURL:nil];
    return config->_uploadURL;
}

@end