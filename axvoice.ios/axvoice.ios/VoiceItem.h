//
//  VoiceItem.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_VoiceItem_h
#define axvoice_ios_VoiceItem_h


typedef enum  {
    NO_LOCAL_FILE = 0,
    RECORDING,
    STOP_RECORDING,
    DOWNLOADING,
    HAS_LOCAL_FILE
} LocalStatus;

typedef enum  {
    NO_SERVER_FILE = 0,
    UPLOADING,
    HAS_SERVER_FILE
} ServerStatus;

typedef enum {
    NO_TEXT=0,
    CONVERTING,
    HAS_TEXT
} ToTextStatus;

@interface VoiceItem : NSObject


@property (nonatomic, assign, readonly) uint    voiceID;
@property (nonatomic, copy, readonly) NSString* localWavFile;
@property (nonatomic, copy, readonly) NSString* localAmrFile;
@property (nonatomic, copy, readonly) NSString* serverURL;
@property (nonatomic, copy, readonly) NSString* fileMD5;
@property (nonatomic, copy, readonly) NSString* text;
@property (nonatomic, assign, readonly) LocalStatus localStatus;
@property (nonatomic, assign, readonly) ServerStatus serverStatus;
@property (nonatomic, assign, readonly) ToTextStatus toTextStatus;

- (id)initWithParam : (uint)voiceID
       localWavFile : (NSString*) localWavFile
        localAmrFile: (NSString*)localAmrFile
          serverURL : (NSString*) serverURL
            fileMD5 : (NSString*) fileMD5
               text : (NSString*) text
        localStatus : (LocalStatus) localStatus
       serverStatus : (ServerStatus) serverStatus
       toTextStatus : (ToTextStatus) toTextStatus;

- (void)startRecord;
- (void)stopRecord;
- (void)confirmLocalFile : (NSString*) amrMD5;
- (void)beginUpload;
- (void)updateServerURL: (NSString*)serverURL;
- (void)beginDownload;
- (void)beginConvertToText;
- (void)setText : (bool) success
           text : (NSString *)text;

@end




#endif
