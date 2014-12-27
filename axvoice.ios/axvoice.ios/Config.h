//
//  Config.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/25.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_Config_h
#define axvoice_ios_Config_h

@interface Config : NSObject


+ (id)sharedInstance : (NSString*) cacheAudioPath
           uploadURL : (NSString*) uploadURL;

+ (NSString*) cacheAudioPath;
+ (NSString*) uploadURL;


@end

#endif
