//
//  VoiceConverter.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/25.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_VoiceConverter_h
#define axvoice_ios_VoiceConverter_h


@interface VoiceConverter : NSObject

+ (bool)wavToAmr:(NSString*)wavPathName
     amrPathName:(NSString*)amrPathName;

+ (bool)amrToWav:(NSString*)amrPathName
     wavPathName:(NSString*)wavPathName;


@end

#endif
