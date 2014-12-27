//
//  VoiceConverter.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/25.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VoiceConverter.h"
#import "opencore-amr/OpenCoreWrap.h"


@implementation VoiceConverter

+ (bool)wavToAmr:(NSString *)wavPathName
     amrPathName:(NSString*)amrPathName
{
    if (EncodeWAVEFileToAMRFile([wavPathName cStringUsingEncoding:NSASCIIStringEncoding],
                                [amrPathName cStringUsingEncoding:NSASCIIStringEncoding], 1, 16) != 0)
    {
        return true;
    }
    
    
    return false;
}


+ (bool)amrToWav:(NSString*)amrPathName
    wavPathName:(NSString*)wavPathName
{
    if (DecodeAMRFileToWAVEFile([amrPathName cStringUsingEncoding:NSASCIIStringEncoding],
                                [wavPathName cStringUsingEncoding:NSASCIIStringEncoding]) != 0)
    {
        return true;
    }
    
    return false;
    
}

@end