#include "platform/osx/runloop_helper.hpp"

#import <Foundation/Foundation.h>

void run_runloop()
{
    [[NSRunLoop mainRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
}

