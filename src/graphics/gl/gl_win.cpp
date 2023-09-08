
#include "context_gl.h"
#include <glad/glad_wgl.h>
#include <VersionHelpers.h>

using crib::graphics::gl::context;


context::context(const app::window& window) : owner(window)
{
	description = "OpenGL  |  no or unknown device";

	auto handle = (HWND)owner.impl;
	auto hdc = GetDC(handle);

	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 32;
	pfd.cColorBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (IsWindowsVistaOrGreater())
		pfd.dwFlags |= PFD_SUPPORT_COMPOSITION;

	// here we can choose a multi-sampling pixel format, by
	//  1- creating a dummy window
	//  2- creating an OpenGL context
	//  3- initialize OpenGL extensions
	//  4- call extended GetPixelFormat and use here instead of this pixelFormat
	// otherwise, use default Windows one
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	ctx = wglCreateContext(hdc);
	wglMakeCurrent(hdc, ctx);


	// initialize extensions and latest OpenGL version
	if (!gladLoadGL() || !gladLoadWGL(hdc))
		throw std::runtime_error("Unable to load OpenGL");


	// create a new context with version number explicitly selected and replace our old context
	// with it is it necessary? at least on my dev machine the default context is already latest
	// version
	if (GLAD_WGL_ARB_create_context && GLAD_WGL_ARB_create_context_profile)
	{
		const int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0 };

		auto betterContext = wglCreateContextAttribsARB(hdc, nullptr, attribList);
		if (betterContext)
		{
			wglMakeCurrent(hdc, betterContext);
			wglDeleteContext(ctx);
			ctx = betterContext;
		}
	}


	// enable V-Sync
	if (GLAD_WGL_EXT_swap_control)
		wglSwapIntervalEXT(1);  // or 0 to disable V-Sync

	if (GLAD_WGL_EXT_swap_control_tear)
		wglSwapIntervalEXT(-1);  // to enable adaptive sync


	// set description
	{
		std::string profile;
		{
			GLint mask;
			glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
			if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
				profile = "Core";
			else if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
				profile = "Compatibility";
		}

		std::string sync;
		{
			const int interval = wglGetSwapIntervalEXT();
			if (interval == 1)
				sync = " (V-Sync)";
			else if (interval == -1)
				sync = " (adaptive sync)";
		}

		description = std::string("GL ") + (char*)glGetString(GL_VERSION) + "  " + profile
					  + "  GLSL " + (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) + "  |  "
					  + (char*)glGetString(GL_RENDERER) + sync;
	}
}

context::~context()
{
	wglMakeCurrent(GetDC((HWND)owner.impl), nullptr);
	wglDeleteContext(ctx);
}

void context::draw()
{
	PAINTSTRUCT ps;
	auto handle = (HWND)owner.impl;
	auto hdc = BeginPaint(handle, &ps);

	glClearColor(0.6f, 0.2f, 0.15f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	SwapBuffers(hdc);
	EndPaint(handle, &ps);
}
