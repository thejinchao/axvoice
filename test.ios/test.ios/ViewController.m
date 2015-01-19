//
//  ViewController.m
//  test.ios
//
//  Created by JinChao on 14/12/24.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#import "ViewController.h"
#import "axvoice.h"

@interface VoiceMessageCallback : NSObject <MessageCallBack>
{
    __weak ViewController* _viewControl;
}

@property (weak, nonatomic) ViewController *viewControl;

- (void)onMessage : (VoiceMessage*) msg;

@end



@interface ViewController ()
{
    uint voice_id;
    NSTimer *_callbackTimer;
    VoiceMessageCallback *_callback;
}

@property (weak, nonatomic) IBOutlet UIButton *btnRecord;
@property (weak, nonatomic) IBOutlet UITextField *textURL;
@property (weak, nonatomic) IBOutlet UITextField *textVoiceID;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *directory = [documentsDirectory stringByAppendingPathComponent:@"axvoice_temp"];
    
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDir;
    if (![fileManager fileExistsAtPath:directory isDirectory:&isDir])
    {
        [fileManager createDirectoryAtPath:directory withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    [axvoice initAxVoice:directory uploadURL:nil] ; 
    
    _callback = [[VoiceMessageCallback alloc] init];
    _callback.viewControl = self;
    
    _callbackTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(callbackTimerUpdate:) userInfo:nil repeats:YES];
    [_callbackTimer fire];
    
}

- (void) callbackTimerUpdate:(id) sender
{
    [axvoice dispatchMessage:_callback];
    //NSLog(@"%@", sender);
    //self.recordLengthLabel.text = [NSString stringWithFormat:@"%.2f", _recorder.currentTime];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)onRecordBtnDown {
    voice_id = [axvoice beginRecord];
    [_btnRecord setTitle:@"Recording..." forState:UIControlStateNormal];
}

- (IBAction)onRecordBtnUp:(id)sender {
    [_btnRecord setTitle:@"Record" forState:UIControlStateNormal];
    [axvoice completeRecord:voice_id];
}

- (IBAction)onCreateClick {
    NSString* serverURL = [_textURL text];
    
    voice_id = [axvoice createVoice:serverURL];
    
    [_textVoiceID setText:[NSString stringWithFormat:@"%d", voice_id]];
}
- (IBAction)onDownloadClick {
    
    voice_id = [[_textVoiceID text] intValue];
    [axvoice downloadVoice:voice_id];
}
- (IBAction)onPlayClick {
    voice_id = [[_textVoiceID text] intValue];
    [axvoice playVoice:voice_id];
}
- (IBAction)onStopClick {
    [axvoice stopVoice];
}

- (IBAction)onServerURLDebugGet {
    NSMutableURLRequest *myRequest = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:@"http://www.dashengine.com/download/server_url.txt"]
                                                                  cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                              timeoutInterval:60];
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
                //NSLog(@"ret=%@", responseString);
                [_textURL setText:responseString];
            }
        }
        else
        {
            NSLog(@"response_code=%ld", (long)response.statusCode);
        }
    }
    
}

- (IBAction)onVoiceIDDebugGet {
}

@end


@implementation VoiceMessageCallback
@synthesize viewControl = _viewControl;

- (void)onMessage : (VoiceMessage*) msg
{
    MessageType type = [msg msgType];
    uint voiceID = [msg voiceID];
    
    switch (type) {
        case MT_RECORD_MSG:
        {
            NSString* type = [[msg params] objectAtIndex:0];
            NSString* success = [[msg params] objectAtIndex:1];
            NSString* result = [[msg params] objectAtIndex:2];
            
            NSLog(@"MT_RECORD_MSG: id=%d type=%@, suc=%@, result=%@", voiceID, type, success, result);
            if([type compare:@"complete"]==NSOrderedSame)
            {
                [_viewControl.textVoiceID setText:[NSString stringWithFormat:@"%d", voiceID]];

                [axvoice uploadVoice:voiceID];
            }
        }
            break;
            
        case MT_UPLOAD_MSG:
        {
            
            NSString* type = [[msg params] objectAtIndex:0];
            NSString* success = [[msg params] objectAtIndex:1];
            NSString* result = [[msg params] objectAtIndex:2];

            NSLog(@"MT_UPLOAD_MSG: id=%d type=%@, suc=%@, result=%@", voiceID, type, success, result);
            if([type compare:@"complete"]==NSOrderedSame)
            {
                [_viewControl.textURL setText:result];
            }
        }
            break;
            
        case MT_DOWNLOAD_MSG:
        {
            NSString* type = [[msg params] objectAtIndex:0];
            NSString* success = [[msg params] objectAtIndex:1];
            NSString* result = [[msg params] objectAtIndex:2];
            
            NSLog(@"MT_DOWNLOAD_MSG: id=%d type=%@, suc=%@, result=%@", voiceID, type, success, result);
            
        }
            break;
            
        case MT_PLAY_MSG:
        {
            NSString* type = [[msg params] objectAtIndex:0];
            
            NSLog(@"MT_PLAY_MSG: id=%d type=%@", voiceID, type);
            
        }
            break;
        default:
            break;
    }
}

@end
