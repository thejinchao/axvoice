//
//  UploadFile.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/25.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "UploadFile.h"

@interface UploadFile()
{
    uint _voiceID;
    NSString* _localFile;
    NSString* _fileMD5;
    NSString* _serverURL;
    id<UploadCallBackProtocol> _delegate;
}

@end

@implementation UploadFile

@synthesize voiceID = _voiceID;
@synthesize localFile = _localFile;
@synthesize fileMD5 = _fileMD5;
@synthesize serverURL = _serverURL;

#define BOUNDARY @"00content0boundary00"

- (id)initWithParam : (uint)voiceID
          localFile : (NSString*) localFile
          serverURL : (NSString*) serverURL
            fileMD5 : (NSString*) fileMD5
           delegate : (id<UploadCallBackProtocol>) delegate
{
    if (self = [super init]) {
        _voiceID = voiceID;
        _localFile = localFile;
        _serverURL = serverURL;
        _fileMD5 = fileMD5;
        _delegate = delegate;
    }
    return self;
}


- (void) run
{
    NSData *fileData = [NSData dataWithContentsOfFile:_localFile];
    
    NSMutableURLRequest *myRequest = [ [NSMutableURLRequest alloc] initWithURL: [NSURL URLWithString:_serverURL]
                                                                   cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                               timeoutInterval:60*3];
    [myRequest setHTTPMethod:@"POST"];
    [myRequest setValue:[@"multipart/form-data; boundary=" stringByAppendingString:BOUNDARY]
     forHTTPHeaderField:@"Content-Type"];
    
    NSMutableData *body = [NSMutableData data];

    NSString *param = [NSString stringWithFormat:@"--%@\r\nContent-Disposition: form-data; name=\"%@\";filename=\"%@\"\r\n\r\n",
                       BOUNDARY,
                       [NSString stringWithFormat:@"-%@-", _fileMD5],
                       @"amrfile"];
    [body appendData:[param dataUsingEncoding:NSUTF8StringEncoding]];
    [body appendData:fileData];
    [body appendData:[@"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
    

    NSString *endString = [NSString stringWithFormat:@"--%@--\r\n",BOUNDARY];
    [body appendData:[endString dataUsingEncoding:NSUTF8StringEncoding]];
    [myRequest setHTTPBody:body];
    
    NSString *strLength = [NSString stringWithFormat:@"%ld", (long)body.length];
    [myRequest setValue:strLength forHTTPHeaderField:@"Content-Length"];
   
    
    NSOperationQueue *queue = [[NSOperationQueue alloc] init];
    [NSURLConnection sendAsynchronousRequest:myRequest queue:queue completionHandler:^(NSURLResponse *response, NSData *result, NSError *connectionError){
        if (result && response)
        {
            NSInteger statusCode = [(NSHTTPURLResponse *)response statusCode];
            if (statusCode == 200 || statusCode==201)
            {
                NSString *responseString = [[NSString alloc] initWithData:result encoding:NSASCIIStringEncoding];
                
                if (responseString && ![responseString isEqual:[NSNull null]])
                {
                    NSArray *arrayRet = [responseString componentsSeparatedByString:@"|"];
                    if(arrayRet.count==2)
                    {
                        NSString* f = [arrayRet objectAtIndex:0];
                        if( [f compare: @"SUCCESS"]==NSOrderedSame)
                        {
                            [_delegate onUploadComplete:_voiceID serverURL:[arrayRet objectAtIndex:1]];
                        }
                        else if( [f compare: @"ERROR"] == NSOrderedSame)
                        {
                            [_delegate onUploadFailed:_voiceID reason:[arrayRet objectAtIndex:1]];
                        }
                        else{
                            [_delegate onUploadFailed:_voiceID reason:responseString];
                        }
                    }
                    
                }
            }
            else
            {
                [_delegate onUploadFailed:_voiceID
                                   reason:[NSString stringWithFormat:@"HTTP_CODE=%ld", (long)statusCode]];
            }
        }
        else{
            [_delegate onUploadFailed:_voiceID
                               reason:[NSString stringWithFormat:@"ERROR=%@", connectionError]];
            
        }
    }];
    
    /*
    NSError *error;
    NSHTTPURLResponse *response;
    NSData* result = [NSURLConnection sendSynchronousRequest:myRequest returningResponse:&response error:&error];
    
    if (result && response)
    {
        if (response.statusCode == 200 || response.statusCode==201)
        {
            NSString *responseString = [[NSString alloc] initWithData:result encoding:NSASCIIStringEncoding];
            
            if (responseString && ![responseString isEqual:[NSNull null]])
            {
                NSLog(@"ret=%@", responseString);
            }
        }
        else
        {
            NSLog(@"response_code=%d", response.statusCode);
        }
    }
     */
}

@end