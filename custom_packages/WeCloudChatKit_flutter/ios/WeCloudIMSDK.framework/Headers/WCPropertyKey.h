//
//  WCPropertyKey.h
//  WCExtensionExample
//
//  Created by WC Lee on 15/8/11.
//  Copyright (c) 2015年 小码哥. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    WCPropertyKeyTypeDictionary = 0, // 字典的key
    WCPropertyKeyTypeArray // 数组的key
} WCPropertyKeyType;

/**
 *  属性的key
 */
@interface WCPropertyKey : NSObject
/** key的名字 */
@property (copy,   nonatomic) NSString *name;
/** key的种类，可能是@"10"，可能是@"age" */
@property (assign, nonatomic) WCPropertyKeyType type;

/**
 *  根据当前的key，也就是name，从object（字典或者数组）中取值
 */
- (id)valueInObject:(id)object;

@end
