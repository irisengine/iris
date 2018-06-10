#import <AppKit/AppKit.h>

#import "osx/OpenGLView.h"

/**
 * Delegate for our app which will handle window creation
 */
@interface AppDelegate : NSObject<NSApplicationDelegate>
{ }

/**
 * Initialise a new AppDelegate with an OpenGl window of the specified
 * dimensions.
 *
 * @param rect
 *   Dimensions of new window.
 */
- (id)initWithRect:(NSRect)rect;

/** Width of OpenGl window. */
@property (assign) CGFloat width;

/** Height of OpenGl window. */
@property (assign) CGFloat height;

@end

