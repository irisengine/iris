#import "platform/ios/MetalViewController.h"

#include <queue>
#include <tuple>

#import "platform/ios/MetalView.h"

@interface MetalViewController ()
@end

@implementation MetalViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setMultipleTouchEnabled:YES];
}

- (void) viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeLeft];
    [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
}

- (void) loadView {
    MetalView *view = [[MetalView alloc] init];
    [self setView:view];
}

- (void)touchesBegan:(NSSet<UITouch*> *)touches withEvent:(UIEvent *)event {
    for(UITouch *touch in touches)
    {
        CGPoint touchPosition = [touch locationInView:[touch window]];
        events_.emplace(eng::TouchEvent(
            reinterpret_cast<std::uintptr_t>(touch),
            eng::TouchType::BEGIN,
            touchPosition.x,
            touchPosition.y));
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
   for(UITouch *touch in touches)
   {
        CGPoint touchPosition = [touch locationInView:[touch window]];
        events_.emplace(eng::TouchEvent(
            reinterpret_cast<std::uintptr_t>(touch),
            eng::TouchType::MOVE,
            touchPosition.x,
            touchPosition.y));
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
   for(UITouch *touch in touches)
   {
        CGPoint touchPosition = [touch locationInView:[touch window]];
        events_.emplace(eng::TouchEvent(
            reinterpret_cast<std::uintptr_t>(touch),
            eng::TouchType::END,
            touchPosition.x,
            touchPosition.y));
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

@end

