#import <AppKit/AppKit.h>

/**
 * Delegate for our app which will handle window creation
 */
@interface OpenGLAppDelegate : NSObject <NSApplicationDelegate>
{
}

/**
 * Initialise a new OpenGLAppDelegate with an OpenGl window of the specified
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
