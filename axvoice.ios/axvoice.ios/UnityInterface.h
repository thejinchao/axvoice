//
//  UnityInterface.h
//  axvoice.ios
//
//  Created by JinChao on 14/12/27.
//  Copyright (c) 2014年 thecodeway. All rights reserved.
//

#ifndef axvoice_ios_UnityInterface_h
#define axvoice_ios_UnityInterface_h

@interface UnityInterface : NSObject


+ (void) UnitySendMessageWrap : (NSString*) nameObject
                 functionName : (NSString*) functionName
                       params : (NSString*) params;

@end

#endif
