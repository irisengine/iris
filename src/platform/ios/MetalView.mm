#import "platform/ios/MetalView.h"

#include <functional>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include "platform/macos/macos_ios_utility.h"

@implementation MetalView

CADisplayLink *_displayLink;

+ (id)layerClass {
    // we need to change the backing layer so metal can
    // be used
    return [CAMetalLayer class];
}

- (instancetype)init {
    if ((self = [super initWithFrame:[[UIScreen mainScreen] bounds]]))
    {
        // basic metal setup
        _metalLayer = (CAMetalLayer *)[self layer];
        _device = MTLCreateSystemDefaultDevice();
        _metalLayer.device = _device;
        _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        self.contentScaleFactor = eng::platform::utility::screen_scale();
    }

    return self;
}

@end
