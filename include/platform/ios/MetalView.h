#import <MetalKit/MetalKit.h>

@interface MetalView : UIView

@property (nonatomic, strong) id<MTLDevice> device;
@property (nonatomic, weak) CAMetalLayer *metalLayer;

@end

