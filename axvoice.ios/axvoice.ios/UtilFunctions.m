//
//  UtilFunctions.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/25.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonDigest.h>

#import "UtilFunctions.h"

@implementation UtilFunctions

+(NSString*)getMD5ByFile:(NSString *)fileName
{
    NSData *fileData = [NSData dataWithContentsOfFile:fileName];
    
    unsigned char md5Buffer[CC_MD5_DIGEST_LENGTH];
    CC_MD5(fileData.bytes, fileData.length, md5Buffer);
    
    NSMutableString *output = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for(int i = 0; i < CC_MD5_DIGEST_LENGTH; i++)
        [output appendFormat:@"%02x",md5Buffer[i]];
    
    return [NSString stringWithString:output];
}

@end