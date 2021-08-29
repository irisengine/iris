#import "graphics/macos/opengl_view.h"

@implementation OpenGLView
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
    // events prevents the annoying macos doonk noise
}

@end

