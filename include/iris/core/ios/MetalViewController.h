#import <MetalKit/MetalKit.h>

#include <UIKit/UIKit.h>

#include <queue>

#include "events/event.h"

@interface MetalViewController : UIViewController
{

  @public
    std::queue<iris::Event> events_;
}

@end
