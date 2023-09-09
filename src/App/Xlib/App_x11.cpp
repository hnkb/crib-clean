
#include "App.h"
#include <stdexcept>

using namespace Crib::Platform;


Display* X11::App::display = nullptr;
Atom X11::App::windowClosed;
XContext X11::App::windowClass;

namespace
{
	int numOpenConnections = 0;
}


void X11::App::open()
{
	if (numOpenConnections++ == 0)
	{
		display = XOpenDisplay(nullptr);

		if (!display)
			throw std::runtime_error("unable to open X11 display server");

		windowClosed = XInternAtom(display, "WM_DELETE_WINDOW", false);
	}
}

void X11::App::close()
{
	if (--numOpenConnections == 0)
	{
		XCloseDisplay(display);
		display = nullptr;
	}
}

int Crib::App::run()
{
	auto& disp = X11::App::display;

	while (true)
	{
		XEvent event;
		XNextEvent(disp, &event);

		XPointer ptr;
		XFindContext(disp, event.xany.window, X11::App::windowClass, &ptr);

		if (event.type == ClientMessage
			&& event.xclient.data.l[0] == (long)X11::App::windowClosed)
		{
			if (ptr)
			{
				// TODO: also call Window::proc() with some close/destroy message
				((X11::Window*)ptr)->close();
			}

			if (numOpenConnections < 1)
				return 0;
		}
		else if (ptr)
		{
			((X11::Window*)ptr)->proc(event);
		}
	}
}
