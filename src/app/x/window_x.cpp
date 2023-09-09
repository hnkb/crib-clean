
#include "app_x.h"
#include "../../graphics/gl/context_gl.h"
#include <stdexcept>
#include <cstring>

using namespace crib::platform;

using crib::app::window;


x11::window::window(crib::app::window* owner, const crib::app::window::options& opt)
	: owner(owner)
{
	app::open();
	auto& disp = app::display;

	pixel_format = glx::choose_pixel_format(disp);
	auto vi = glXGetVisualFromFBConfig(disp, pixel_format);

	// create color map
	color_map = XCreateColormap(disp, RootWindow(disp, vi->screen), vi->visual, AllocNone);

	XSetWindowAttributes swa;
	swa.colormap = color_map;
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;

	wnd = XCreateWindow(
		disp,
		RootWindow(disp, vi->screen),
		opt.pos.x,
		opt.pos.y,
		opt.size.x,
		opt.size.y,
		0,
		vi->depth,
		InputOutput,
		vi->visual,
		CWBorderPixel | CWColormap | CWEventMask,
		&swa);

	XFree(vi);

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

	XMapRaised(disp, wnd);
}

x11::window::~window()
{
	close();
	owner->impl = nullptr;
}

void x11::window::close()
{
	if (owner->context)
	{
		delete owner->context;
		owner->context = nullptr;
	}

	if (already_deleted)
		return;
	already_deleted = true;

	XDestroyWindow(app::display, wnd);
	XFreeColormap(app::display, color_map);

	app::close();
}

void x11::window::proc(XEvent& event)
{
	auto& disp = app::display;

	switch (event.type)
	{
		case Expose:
			owner->draw();
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

	impl = new x11::window(this, opt);

	create_graphics_context(opt);

	if (context)
	{
		XWindowAttributes gwa;
		XGetWindowAttributes(x11::app::display, ((x11::window*)impl)->wnd, &gwa);
		context->on_resize({ gwa.width, gwa.height });
	}
}

window::~window()
{
	if (context)
	{
		delete context;
		context = nullptr;
	}
	if (impl)
		delete (x11::window*)impl;
}

window& window::operator=(window&& other)
{
	if (this != &other)
	{
		impl = other.impl;
		context = other.context;
		other.impl = nullptr;
		other.context = nullptr;

		if (impl)
			((platform::x11::window*)impl)->owner = this;
	}
	return *this;
}


void window::close()
{
	XEvent ev;
	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = ((x11::window*)impl)->wnd;
	ev.xclient.message_type = XInternAtom(x11::app::display, "WM_PROTOCOLS", true);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = x11::app::window_closed;
	ev.xclient.data.l[1] = CurrentTime;
	XSendEvent(x11::app::display, ev.xclient.window, False, NoEventMask, &ev);
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
