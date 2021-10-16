////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#import <AppKit/AppKit.h>

/**
 * Delegate for our app which will handle window creation
 */
@interface MetalAppDelegate : NSObject <NSApplicationDelegate>
{
}

/**
 * Initialise a new MetalAppDelegate with an OpenGl window of the specified
 * dimensions.
 *
 * @param rect
 *   Dimensions of new window.
 */
- (id)initWithRect:(NSRect)rect;

/** Width of OpenGl window. */
@property(assign) CGFloat width;

/** Height of OpenGl window. */
@property(assign) CGFloat height;

@end
