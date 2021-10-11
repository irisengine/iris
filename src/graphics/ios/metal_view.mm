////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#import "graphics/ios/metal_view.h"

#include <functional>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "core/root.h"
#include "graphics/ios/ios_window.h"
#include "graphics/ios/ios_window_manager.h"

@implementation MetalView

CADisplayLink *_displayLink;

+ (id)layerClass
{
    // we need to change the backing layer so metal can be used
    return [CAMetalLayer class];
}

- (instancetype)init
{
    if ((self = [super initWithFrame:[[UIScreen mainScreen] bounds]]))
    {
        // basic metal setup
        _metalLayer = (CAMetalLayer *)[self layer];
        _device = MTLCreateSystemDefaultDevice();
        _metalLayer.device = _device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        _metalLayer.frame = self.layer.frame;
        self.contentScaleFactor = iris::Root::window_manager().current_window()->screen_scale();
        [self setUserInteractionEnabled:YES];
        [super setUserInteractionEnabled:YES];
        [self setBackgroundColor:[UIColor redColor]];
    }

    return self;
}

@end
