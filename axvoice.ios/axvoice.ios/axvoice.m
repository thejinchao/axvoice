//
//  axvoice_ios.m
//  axvoice.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import "axvoice.h"
#import "VoiceItem.h"
#import "VoiceManager.h"
#import "VoiceRecorder.h"
#import "UtilFunctions.h"
#import "UploadFile.h"
#import "Config.h"
#import "DownloadFile.h"
#import "VoicePlayer.h"
#import "MessageQueue.h"


@interface UploadCallback : NSObject <UploadCallBackProtocol>
{
}


- (void) onUploading : (uint)voiceID;
- (void) onUploadComplete : (uint)voiceID
                serverURL : (NSString*) serverURL;
- (void) onUploadFailed : (uint)voiceID
                 reason : (NSString*) reason;

@end

@implementation UploadCallback

- (void) onUploading : (uint)voiceID
{
    
}
- (void) onUploadComplete : (uint)voiceID
                serverURL : (NSString*) serverURL
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    
    @synchronized(manager) {
        VoiceItem * item  = [manager findItem:voiceID];
        if(item == nil)return; //TODO: ERROR
        
        //TODO: CHECK STATUS
        
        //update server url
        [item updateServerURL:serverURL];
    
        [[MessageQueue sharedInstance] pushMessage:MT_UPLOAD_MSG
                                           voiceID:voiceID
                                                p1:@"complete"
                                                p2:true
                                                p3:serverURL];
        //NSLog(@"onUploadComplete voiceID=%d ret=%@", voiceID, serverURL);
    
    }
}

- (void) onUploadFailed : (uint)voiceID
                  reason:(NSString *)reason
{
    //NSLog(@"onUploadFailed voiceID=%d reason=%@", voiceID, reason);
    [[MessageQueue sharedInstance] pushMessage:MT_UPLOAD_MSG
                                       voiceID:voiceID
                                            p1:@"complete"
                                            p2:false
                                            p3:reason];
}

@end


@interface DownloadCallback : NSObject <DownloadCallBackProtocol>
{
}

- (void) onDownloading : (uint)voiceID;
- (void) onDownloadComplete : (uint)voiceID;
- (void) onDownloadFailed : (uint)voiceID
                   reason : (NSString*) reason;
@end

@implementation DownloadCallback

- (void) onDownloading : (uint)voiceID
{

}

- (void) onDownloadComplete : (uint)voiceID
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    
    @synchronized(manager) {
        VoiceItem * item  = [manager findItem:voiceID];
        if(item == nil)return; //TODO: ERROR
    
        //after download
        [item confirmLocalFile:[UtilFunctions getMD5ByFile:item.localAmrFile]];
    
        [[MessageQueue sharedInstance] pushMessage:MT_DOWNLOAD_MSG
                                           voiceID:voiceID
                                                p1:@"complete"
                                                p2:true
                                                p3:item.fileMD5];
//        NSLog(@"download complete: id=%d, MD5=%@", voiceID, item.fileMD5);
    }
}

- (void) onDownloadFailed : (uint)voiceID
                   reason : (NSString*) reason
{
    [[MessageQueue sharedInstance] pushMessage:MT_DOWNLOAD_MSG
                                       voiceID:voiceID
                                            p1:@"complete"
                                            p2:false
                                            p3:reason];
}


@end


@interface PlayCallback : NSObject <PlayCallBackProtocol>
{
}

- (void) onPlayComplete:(uint)voiceID;

@end

@implementation PlayCallback

- (void) onPlayComplete:(uint)voiceID
{
    //push message
    [[MessageQueue sharedInstance] pushMessage:MT_PLAY_MSG
                                       voiceID:voiceID
                                            p1:@"complete"];
}

@end


@implementation axvoice


+(void)initAxVoice:(NSString *)cachePath
         uploadURL:(NSString *)uploadURL
{
    @synchronized(self) {
        [Config sharedInstance : cachePath
                      uploadURL:uploadURL];
        
        [VoiceManager sharedInstance];
        
        NSLog(@"***axvoice init: cachePath=%@, uploadURL=%@", [Config cacheAudioPath], [Config uploadURL]);
    }
}

+(uint)beginRecord
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    @synchronized(manager) {
        VoiceItem* item = [manager allocateNewItem];
    
        [item startRecord];
    
        VoiceRecorder* recorder = [VoiceRecorder sharedInstance];
        [recorder beginRecord:item.voiceID
                 localWavFile:item.localWavFile
                 localAMRFile:item.localAmrFile];
    
        return item.voiceID;
    }
}

+(void)completeRecord:(uint)voiceID
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    @synchronized(manager) {
        VoiceItem * item  = [manager findItem:voiceID];
        if(item == nil)return; //TODO: ERROR
    
        if(item.localStatus != RECORDING) return; //TODO: ERROR
    
        [item stopRecord];
    
        //TODO: SHOULD ASYNC
        VoiceRecorder* recorder = [VoiceRecorder sharedInstance];
        [recorder completeRecord:voiceID];
        [item confirmLocalFile:[UtilFunctions getMD5ByFile:item.localAmrFile]];
   
        //on record complete
        [[MessageQueue sharedInstance] pushMessage:MT_RECORD_MSG
                                           voiceID:voiceID
                                                p1:@"complete"
                                                p2:true
                                                p3:[item fileMD5]];
        
        //NSLog(@"completeRecord: id=%d, MD5=%@", voiceID, item.fileMD5);
    }

}

+(void)uploadVoice:(uint)voiceID
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    @synchronized(manager) {
        VoiceItem * item  = [manager findItem:voiceID];
        if(item == nil)return; //TODO: ERROR
    
        if(item.localStatus != HAS_LOCAL_FILE) return; //TODO: ERROR
        if(item.serverStatus != NO_SERVER_FILE) return; //TODO: ERROR
    
        [item beginUpload];
    
        UploadCallback* callback = [[UploadCallback alloc] init];
        UploadFile* uploader = [[UploadFile alloc] initWithParam:item.voiceID
                                                       localFile:item.localAmrFile
                                                       serverURL:[Config uploadURL]
                                                         fileMD5:item.fileMD5
                                                        delegate:callback];
    
        [uploader run];
    }
}

+(uint)createVoice: (NSString*)serverURL
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    @synchronized(manager) {
        VoiceItem* item = [manager allocateNewItem];
    
        [item updateServerURL:serverURL];
    
        return [item voiceID];
    }
}

+(void)downloadVoice: (uint)voiceID
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    @synchronized(manager) {
        VoiceItem * item  = [manager findItem:voiceID];
        if(item == nil)return; //TODO: ERROR
    
        if(item.localStatus != NO_LOCAL_FILE) return; //TODO: ERROR
        if(item.serverStatus != HAS_SERVER_FILE) return; //TODO: ERROR
    
        [item beginDownload];
    
        DownloadCallback* callback = [[DownloadCallback alloc] init];
        
        DownloadFile* downloader = [[DownloadFile alloc] initWithParam:voiceID
                                                          localAMRFile:[item localAmrFile]
                                                          localWAVFile:[item localWavFile]
                                                             serverURL:[item serverURL]
                                                              delegate:callback];
    
    
        [downloader run];
    }
    
}

+(void)playVoice: (uint)voiceID
{
    VoiceManager* manager = [VoiceManager sharedInstance];
    @synchronized(manager) {
        VoiceItem * item  = [manager findItem:voiceID];
        if(item == nil)return; //TODO: ERROR

        if(item.localStatus != HAS_LOCAL_FILE) return; //TODO: ERROR
        
        PlayCallback *callback = [[PlayCallback alloc] init];
    
        [[VoicePlayer sharedInstance] playVoiceFile:[item voiceID]
                                       localWavFile:[item localWavFile]
                                           delegate:callback];
    }
}

+(void)stopVoice
{
    @synchronized(self) {
        [[VoicePlayer sharedInstance] stopPlay];
    }
}

+(void)dispatchMessage : (id<MessageCallBack>) delegate
{
    @synchronized(self) {
        [[MessageQueue sharedInstance] dispatchMessage:delegate];
    }
}

+(void)dispatchMessage_Unity : (NSString*) callbackObject
{
    @synchronized(self) {
        [[MessageQueue sharedInstance] dispatchMessage_Unity:callbackObject];
    }
}

@end



