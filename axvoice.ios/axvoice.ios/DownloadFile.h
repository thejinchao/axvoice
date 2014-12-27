//
//  DownloadFile.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/26.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_DownloadFile_h
#define axvoice_ios_DownloadFile_h

@protocol DownloadCallBackProtocol <NSObject>

@required
- (void) onDownloading : (uint)voiceID;
- (void) onDownloadComplete : (uint)voiceID;
- (void) onDownloadFailed : (uint)voiceID
                   reason : (NSString*) reason;
@end


@interface DownloadFile : NSObject

@property (nonatomic, assign, readonly) uint        voiceID;
@property (nonatomic, strong, readonly) NSString*   localAMRFile;
@property (nonatomic, strong, readonly) NSString*   localWAVFile;
@property (nonatomic, strong, readonly) NSString*   serverURL;


- (id)initWithParam : (uint)voiceID
       localAMRFile : (NSString*) localAMRFile
       localWAVFile : (NSString*) localWAVFile
          serverURL : (NSString*) serverURL
           delegate : (id<DownloadCallBackProtocol>) delegate;

- (void) run;

@end

#endif
