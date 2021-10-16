////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#import <MetalKit/MetalKit.h>

@interface MetalView : UIView

@property(nonatomic, strong) id<MTLDevice> device;
@property(nonatomic, weak) CAMetalLayer *metalLayer;

@end
