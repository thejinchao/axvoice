//
//  Voice2TextWrap.m
//  axvoice.ios
//
//  Created by JinChao on 15/1/21.
//  Copyright (c) 2015年 thecodeway. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Voice2TextWrap.h"
#import "VoiceManager.h"
#import "VoiceItem.h"
#import "Config.h"
#import "MessageQueue.h"

#import "iflyMSC/IFlySetting.h"
#import "iflyMSC/IFlySpeechUtility.h"
#import "iflyMSC/IFlySpeechRecognizer.h"
#import "iflyMSC/IFlySpeechConstant.h"


@interface IFlyEngine()
{
    NSMutableDictionary* sessionMap;
    IFlySpeechRecognizer * iFlySpeechRecognizer;
    uint currentVoiceID;
    NSString* wavFile;
    NSMutableString* strResult;
    id<ConvertCallBackProtocol> _delegate;
}

@end

@implementation IFlyEngine

+ (id)sharedInstance
{
    static IFlyEngine *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        sharedInstance = [[self alloc] init];
        [sharedInstance setupSelf];
    });
    
    return sharedInstance;
}

/**
 创建识别对象
 domain:识别的服务类型
 iat,search,video,poi,music,asr;iat,普通文本听写; search,热词搜索;video,视频音乐搜索;asr: 关键词识别
 */
+(id) createRecognizer:(id)delegate
                Domain:(NSString*) domain
{
    IFlySpeechRecognizer * recognizer = [IFlySpeechRecognizer sharedInstance];
    
    //请不要删除这句,createRecognizer是单例方法，需要重新设置代理
    recognizer.delegate = delegate;
    
    [recognizer setParameter:domain forKey:[IFlySpeechConstant IFLY_DOMAIN]];
    
    //设置采样率
    [recognizer setParameter:@"8000" forKey:[IFlySpeechConstant SAMPLE_RATE]];
    
    //设置录音保存文件
    //    [iflySpeechRecognizer setParameter:@"asr.pcm" forKey:[IFlySpeechConstant ASR_AUDIO_PATH]];
    
    //设置为非语义模式
    [recognizer setParameter:@"0" forKey:[IFlySpeechConstant ASR_SCH]];
    
    //设置返回结果的数据格式，可设置为json，xml，plain，默认为json。
    [recognizer setParameter:@"plain" forKey:[IFlySpeechConstant RESULT_TYPE]];
    
    //设置为麦克风输入模式
    [recognizer setParameter:IFLY_AUDIO_SOURCE_MIC forKey:@"audio_source"];
    
    return recognizer;
}

- (void) setupSelf
{
    sessionMap = [[NSMutableDictionary alloc] init];
    iFlySpeechRecognizer = nil;
}

- (void) initEngine : (NSString*)appID
{
    //set log level
    [IFlySetting setLogFile:LVL_ALL];
    
    //set console switch
    [IFlySetting showLogcat:YES];
    
    //set cache path
    [IFlySetting setLogFilePath:[Config cacheAudioPath]];
    
    //init appid
    NSString *initString = [[NSString alloc] initWithFormat:@"appid=%@, timeout=%@", appID, @"20000"];
    
    //所有服务启动前，需要确保执行createUtility
    [IFlySpeechUtility createUtility:initString];
    
    //创建识别
    iFlySpeechRecognizer = [IFlyEngine createRecognizer:self Domain:@"iat"];
}

- (bool) beginConvert2Voice : (uint)voiceID
                   delegate : (id<ConvertCallBackProtocol>) delegate
{
    //is engine inited?
    if(iFlySpeechRecognizer==nil) return false; //TODO: ERROR
    
    //is engine busy?
    if([iFlySpeechRecognizer isListening])
    {
        [[MessageQueue sharedInstance] pushMessage:MT_TOTXT_MSG voiceID:voiceID p1:@"failed" p2:false p3:@"busy"];
        return false;
    }
    
    _delegate = delegate;
    
    VoiceItem * item  = [[VoiceManager sharedInstance] findItem:voiceID];
    if(item == nil)return false; //TODO: ERROR
    if(item.localStatus != HAS_LOCAL_FILE) return false; //TODO: ERROR
    
    if(item.toTextStatus==HAS_TEXT)
    {
        //already has text
        
        [[MessageQueue sharedInstance] pushMessage:MT_TOTXT_MSG voiceID:voiceID p1:@"complete" p2:true p3:[item text]];
        return true;
    } else if(item.toTextStatus==CONVERTING)
    {
        //already converting...
        //TODO: maybe give more information
        return true;
    }
    
    //begin convert
    currentVoiceID = voiceID;
    wavFile = [item localWavFile];
    strResult = [[NSMutableString alloc] init];
    [item beginConvertToText];

    //启动发送数据线程
    [NSThread detachNewThreadSelector:@selector(sendAudioThread) toTarget:self withObject:nil];
    
    return true;
}

- (void)sendAudioThread
{
    //从文件中读取音频
    NSData *data = [NSData dataWithContentsOfFile:wavFile];
    if(data==nil) {
        [_delegate onConvertFailed:currentVoiceID reason:@"open wav file failed"];
        [[MessageQueue sharedInstance] pushMessage:MT_TOTXT_MSG voiceID:currentVoiceID p1:@"failed" p2:false p3:@"open wav file failed"];
        return;
    }
    
    //设置音频数据模式为音频流
    [iFlySpeechRecognizer setParameter:IFLY_AUDIO_SOURCE_STREAM forKey:@"audio_source"];
    
    //启动服务
    BOOL ret = [iFlySpeechRecognizer startListening];
    if (!ret)  {
        [_delegate onConvertFailed:currentVoiceID reason:@"startListening failed"];
        [[MessageQueue sharedInstance] pushMessage:MT_TOTXT_MSG voiceID:currentVoiceID p1:@"failed" p2:false p3:@"startListening failed"];
        return;
    }
    
    int count = 10;
    unsigned long audioLen = data.length/count;
    
    //分割音频
    for (int i =0 ; i< count-1; i++) {
        char * part1Bytes = malloc(audioLen);
        NSRange range = NSMakeRange(audioLen*i, audioLen);
        [data getBytes:part1Bytes range:range];
        NSData * part1 = [NSData dataWithBytes:part1Bytes length:audioLen];
        //写入音频，让SDK识别
        int ret = [iFlySpeechRecognizer writeAudio:part1];
        free(part1Bytes);
        
        //检测数据发送是否正常
        if(!ret)
        {
            //NSLog(@"sendAudioThread[ERROR]");
            [iFlySpeechRecognizer stopListening];
            [_delegate onConvertFailed:currentVoiceID reason:@"writeAudio"];
            [[MessageQueue sharedInstance] pushMessage:MT_TOTXT_MSG voiceID:currentVoiceID p1:@"failed" p2:false p3:@"writeAudio"];
            return;
        }
    }
    
    //处理最后一部分
    unsigned long writtenLen = audioLen * (count-1);
    char * part3Bytes = malloc(data.length-writtenLen);
    NSRange range = NSMakeRange(writtenLen, data.length-writtenLen);
    [data getBytes:part3Bytes range:range];
    NSData * part3 = [NSData dataWithBytes:part3Bytes length:data.length-writtenLen];
    
    [iFlySpeechRecognizer writeAudio:part3];
    
    free(part3Bytes);
    
    //音频数据写入完成，进入等待状态
    [iFlySpeechRecognizer stopListening];
    
    //NSLog(@"sendAudioThread[OUT]");
}

- (void) onResults : (NSArray *) results
            isLast : (BOOL) isLast
{
    NSDictionary *dic = results[0];
    
    for (NSString *key in dic) {
        [strResult appendFormat:@"%@", key];
    }
}

- (void) onError : (IFlySpeechError *) error
{
    NSString *status;
    NSString *result;
    bool success;
    
    if (error.errorCode ==0 ) {
        if (strResult.length==0) {
            status = @"failed";
            result = @"empty result";
            success = false;
        }
        else{
            status = @"complete";
            result = [NSString stringWithString:strResult];
            success = true;
            NSLog(@"Final=%@", strResult);
        }
    }
    else
    {
        status = @"failed";
        result = [NSString stringWithFormat:@"code=%d desc=%@", error.errorCode, error.errorDesc];
        success = false;
    }
    if (success) {
        [_delegate onConvertComplete:currentVoiceID strResult:result];
    }
    else {
        [_delegate onConvertFailed:currentVoiceID reason:result];
    }
        
    [[MessageQueue sharedInstance] pushMessage:MT_TOTXT_MSG voiceID:currentVoiceID p1:status p2:success p3:result];
}

@end