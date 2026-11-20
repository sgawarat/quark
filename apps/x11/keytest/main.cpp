#include <print>
#include <thread>

#include <cstdlib>

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

int main() {
  auto* display = XOpenDisplay(nullptr);
  auto window = DefaultRootWindow(display);

  XGrabKeyboard(display, window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
  XSelectInput(display, window, KeyPressMask | KeyReleaseMask);

  while (true) {
    XPending(display);

    XEvent event{};
    XNextEvent(display, &event);

    switch (event.type) {
      case KeyPress: {
        std::println("press: {}", event.xkey.keycode);
        break;
      }
      case KeyRelease: {
        std::println("release: {}", event.xkey.keycode);
        break;
      }
    }

    if (event.type == KeyPress &&
        XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0) == XK_Escape) {
      break;
    }

    std::this_thread::yield();
  }

  XUngrabKeyboard(display, CurrentTime);

  return EXIT_SUCCESS;
}
