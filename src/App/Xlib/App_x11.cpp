
#include "App.h"
#include <stdexcept>

using namespace crib::platform;


Display* x11::app::display = nullptr;
Atom x11::app::window_closed;
XContext x11::app::window_class;

namespace
{
	int numOpenConnections = 0;
}


void x11::app::open()
{
	if (numOpenConnections++ == 0)
	{
		display = XOpenDisplay(nullptr);

		if (!display)
			throw std::runtime_error("unable to open X11 display server");

		window_closed = XInternAtom(display, "WM_DELETE_WINDOW", false);
	}
}

void x11::app::close()
{
	if (--numOpenConnections == 0)
	{
		XCloseDisplay(display);
		display = nullptr;
	}
}

int crib::app::run()
{
	auto& disp = x11::app::display;

	while (true)
	{
		XEvent event;
		XNextEvent(disp, &event);

		XPointer ptr;
		XFindContext(disp, event.xany.window, x11::app::window_class, &ptr);

		if (event.type == ClientMessage
			&& event.xclient.data.l[0] == (long)x11::app::window_closed)
		{
			if (ptr)
			{
				// TODO: also call window::proc() with some close/destroy message
				((x11::window*)ptr)->close();
			}

			if (numOpenConnections < 1)
				return 0;
		}
		else if (ptr)
		{
			((x11::window*)ptr)->proc(event);
		}
	}
}
