////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#import "graphics/macos/metal_view.h"

@implementation MetalView
{
}

- (BOOL)acceptsFirstResponder
{
    // make sure we receive key events
    return YES;
}

- (void)keyDown:(NSEvent *)theEvent
{
    // once we receive a key event we completely ignore it! This is because we
    // are handling these elsewhere, but having the view accept and ignore key
    // events prevents the annoying OS X doonk noise
}

@end
