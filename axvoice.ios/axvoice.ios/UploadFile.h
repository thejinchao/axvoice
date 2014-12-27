//
//  UploadFile.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/25.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_UploadFile_h
#define axvoice_ios_UploadFile_h

@protocol UploadCallBackProtocol <NSObject>

@required
- (void) onUploading : (uint)voiceID;
- (void) onUploadComplete : (uint)voiceID
                serverURL : (NSString*) serverURL;
- (void) onUploadFailed : (uint)voiceID
                 reason : (NSString*) reason;
@end


@interface UploadFile : NSObject

@property (nonatomic, assign, readonly) uint    voiceID;
@property (nonatomic, strong, readonly) NSString* localFile;
@property (nonatomic, strong, readonly) NSString* serverURL;
@property (nonatomic, strong, readonly) NSString* fileMD5;


- (id)initWithParam : (uint)voiceID
          localFile : (NSString*) localFile
          serverURL : (NSString*) serverURL
            fileMD5 : (NSString*) fileMD5
           delegate : (id<UploadCallBackProtocol>) delegate;

- (void) run;

@end

#endif
