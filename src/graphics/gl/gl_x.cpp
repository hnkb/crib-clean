
#include "context_gl.h"
#include "../../app/x/app_x.h"
#include <stdexcept>

using namespace crib::platform;

using crib::graphics::gl::context;


namespace
{
	static bool ctxErrorOccurred = false;
	static int ctxErrorHandler(Display*, XErrorEvent*)
	{
		ctxErrorOccurred = true;
		return 0;
	}
}

context::context(const app::window& window) : owner(window)
{
	description = "OpenGL  |  no or unknown device";

	auto& disp = x11::app::display;
	auto& xwnd = *(x11::window*)owner.impl;
	auto& fbc = xwnd.pixel_format;

	if (!GLAD_GLX_ARB_create_context || !GLAD_GLX_ARB_create_context_profile)
	{
		// using old-style GLX context as modern GLX is not found
		ctx = glXCreateNewContext(disp, fbc, GLX_RGBA_TYPE, 0, True);
	}
	else
	{
		// Install an X error handler so the application won't exit if GL 3.3 context creation
		// fails.
		//
		// Note this error handler is global.  All display connections in all threads of a
		// process use the same error handler, so be sure to guard against other threads issuing
		// X commands while this code is running.
		ctxErrorOccurred = false;
		auto oldHandler = XSetErrorHandler(&ctxErrorHandler);

		GLint context_attributes[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			None
		};

		ctx = glXCreateContextAttribsARB(disp, fbc, 0, True, context_attributes);

		// Sync to ensure any errors generated are processed.
		XSync(disp, False);

		if (ctxErrorOccurred || !ctx)
		{
			// Couldn't create GL 3.3 context.  Fall back to old-style 2.x context.
			// When a context version below 3.0 is requested, implementations will return the
			// newest context version compatible with OpenGL versions less than version 3.0.
			context_attributes[1] = 1;
			context_attributes[3] = 0;
			ctx = glXCreateContextAttribsARB(disp, fbc, 0, True, context_attributes);

			XSync(disp, False);
		}

		// Restore the original error handler
		XSetErrorHandler(oldHandler);
	}

	if (!ctx)
		throw std::runtime_error("Failed to create an OpenGL context");

	glXMakeCurrent(disp, xwnd.wnd, ctx);

	if (!gladLoadGL())
		throw std::runtime_error("Unable to load OpenGL");

	read_device_name(0);
}

context::~context()
{
	if (ctx)
	{
		glXMakeCurrent(x11::app::display, 0, 0);
		glXDestroyContext(x11::app::display, ctx);
	}
}

void context::draw()
{
	if (ctx)
	{
		draw_platform_independent();
		glXSwapBuffers(x11::app::display, ((x11::window*)owner.impl)->wnd);
	}
}


GLXFBConfig x11::glx::choose_pixel_format(Display* disp)
{
	if (!gladLoadGLX(disp, DefaultScreen(disp)))
		throw std::runtime_error("Unable to load OpenGL");

	if (!glad_glXChooseFBConfig)
		throw std::runtime_error("invalid GLX version: GLX 1.3 or higher is required");


	// attributes we require
	static int visualAttribs[] = {
		GLX_X_RENDERABLE,  True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE,      8,
		GLX_GREEN_SIZE,    8,
		GLX_BLUE_SIZE,     8,
		GLX_ALPHA_SIZE,    8,
		GLX_DEPTH_SIZE,    24,
		GLX_STENCIL_SIZE,  8,
		GLX_DOUBLEBUFFER,  True,
		//GLX_SAMPLE_BUFFERS, 1,
		//GLX_SAMPLES,        4,
		None
	};

	int fbcount;
	auto fbc = glXChooseFBConfig(disp, DefaultScreen(disp), visualAttribs, &fbcount);

	if (!fbc)
		throw std::runtime_error("failed to retrieve a framebuffer config");


	// Pick the FB config/visual with the most samples per pixel
	int selectedIdx = -1, selectedSamples = -1;

	for (int i = 0; i < fbcount; i++)
	{
		if (auto vi = glXGetVisualFromFBConfig(disp, fbc[i]))
		{
			int samp_buf, samples;

			glXGetFBConfigAttrib(disp, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib(disp, fbc[i], GLX_SAMPLES, &samples);

			if (selectedIdx < 0 || ((samp_buf > 0) && (samples > selectedSamples)))
				selectedIdx = i, selectedSamples = samples;

			XFree(vi);
		}
	}

	auto retVal = fbc[selectedIdx];
	XFree(fbc);

	return retVal;
}
