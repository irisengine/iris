////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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
