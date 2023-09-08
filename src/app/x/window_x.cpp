
#include "app_x.h"
#include "../../graphics/gl/context_gl.h"
#include <stdexcept>

using namespace crib::platform;

using crib::app::window;


x11::window::window(const crib::app::window::options& opt)
{
	app::open();
	auto& disp = app::display;

	screen = DefaultScreen(disp);

	const auto black = BlackPixel(disp, screen);
	const auto white = WhitePixel(disp, screen);

	wnd = XCreateSimpleWindow(
		disp,
		DefaultRootWindow(disp),
		opt.pos.x,
		opt.pos.y,
		opt.size.x,
		opt.size.y,
		5,
		white,
		white);

	if (!wnd)
	{
		close();
		throw std::runtime_error("failed to create window");
	}


	XStoreName(disp, wnd, opt.title.c_str());

	XSaveContext(disp, wnd, app::window_class, (XPointer)this);

	XSelectInput(
		disp,
		wnd,
		StructureNotifyMask | ExposureMask | ButtonPressMask | KeyPressMask);

	XSetWMProtocols(disp, wnd, &app::window_closed, 1);

	gc = XCreateGC(disp, wnd, 0, nullptr);
	XSetBackground(disp, gc, white);
	XSetForeground(disp, gc, black);

	XClearWindow(disp, wnd);
	XMapRaised(disp, wnd);
}

x11::window::~window()
{
	close();
}

void x11::window::close()
{
	if (already_deleted)
		return;
	already_deleted = true;

	XFreeGC(app::display, gc);
	XDestroyWindow(app::display, wnd);

	app::close();
}

void x11::window::proc(XEvent& event)
{
	auto& disp = app::display;

	switch (event.type)
	{
		case Expose:
			break;

		case KeyPress:
			break;

		case ButtonPress:
			break;

		default:
			break;
	}
}

void x11::window::set_title(const std::string& title)
{
	XStoreName(app::display, wnd, title.c_str());
}


window::window() : window(options {})
{}

window::window(options opt)
{
	if (opt.size.x <= 0)
	{
		opt.size.x = 640;
		opt.size.y = 480;
	}
	opt.pos.x = std::max(0, opt.pos.x);
	opt.pos.y = std::max(0, opt.pos.y);
	if (opt.title.empty())
		opt.title = "crib";

	impl = new x11::window(opt);
}

window::~window()
{
	if (impl)
		delete (x11::window*)impl;
}

window::options window::get_options() const
{
	return {};
}

window& window::set_options(const options& opt)
{
	if (impl)
		((x11::window*)impl)->set_title(opt.title);
	return *this;
}
