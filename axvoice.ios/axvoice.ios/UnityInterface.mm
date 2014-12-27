#include "axvoice.h"
#include "UnityInterface.h"

extern "C" {
    void UnitySendMessage(const char* obj, const char* method, const char* msg);
}


// Converts C style string to NSString
NSString* CreateNSString (const char* string)
{
    if (string)
        return [NSString stringWithUTF8String: string];
    else
        return [NSString stringWithUTF8String: ""];
}

@implementation UnityInterface

+ (void) UnitySendMessageWrap : (NSString*) nameObject
                 functionName : (NSString*) functionName
                       params : (NSString*) params
{
    UnitySendMessage([nameObject cStringUsingEncoding:NSASCIIStringEncoding],
                     [functionName cStringUsingEncoding:NSASCIIStringEncoding],
                     [params cStringUsingEncoding:NSASCIIStringEncoding]);
  // */
}

@end

extern "C" {
    void _initAxVoice(const char* cachePath, const char* uploadURL)
    {
        [axvoice initAxVoice : CreateNSString(cachePath)
                    uploadURL: CreateNSString(uploadURL)];
    }
    
    unsigned int _beginRecord(void)
    {
        return [axvoice beginRecord];
    }
    
    void _completeRecord(unsigned int voiceID)
    {
        [axvoice completeRecord:voiceID];
    }
    
    void _uploadVoice(unsigned int voiceID)
    {
        [axvoice uploadVoice:voiceID];
    }
    
    unsigned int _createVoice(const char* serverURL)
    {
        return [axvoice createVoice:CreateNSString(serverURL)];
    }
    
    void _downloadVoice(unsigned int voiceID)
    {
        [axvoice downloadVoice:voiceID];
    }
    
    void _playVoice(unsigned int voiceID)
    {
        NSLog(@"***play voice%d", voiceID);
        [axvoice playVoice:voiceID];
    }
    
    void _stopVoice(void)
    {
        [axvoice stopVoice];
    }
    
    void _dispatchMessage(const char* callbackObject)
    {
        [axvoice dispatchMessage_Unity : CreateNSString(callbackObject)];
    }
}

