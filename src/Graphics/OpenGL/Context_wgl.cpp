
#include "Context.h"
#include "../../App/Win/App.h"
#include <glad/glad_wgl.h>
#include <VersionHelpers.h>

using Crib::Graphics::OpenGL::Context;


namespace
{

	bool isAlreadyInitialized = false;
	PIXELFORMATDESCRIPTOR pfd = {};

	void loadOpenGL()
	{
		// In order to be able to choose advanced context creation options (like multisampling)
		// we need to load WGL extensions via glad, but to do that, we need an HDC with a
		// PixelFormat already chosen. Problem is, after we obtain pointer to extensions, we
		// can't change the PixelFormat for that window anymore.
		// Therefore, we create a dummy window here for initializing glad with a not-so-perfect
		// PixelFormat.

		if (isAlreadyInitialized)
			return;

		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cDepthBits = 32;
		pfd.cColorBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;
		if (IsWindowsVistaOrGreater())
			pfd.dwFlags |= PFD_SUPPORT_COMPOSITION;

		auto dummy = CreateWindowEx(
			0,
			Crib::Platform::Win::windowClassName,
			L"dummy",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			0,
			CW_USEDEFAULT,
			0,
			nullptr,
			nullptr,
			GetModuleHandleW(nullptr),
			nullptr);

		auto hdc = GetDC(dummy);
		SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);

		auto ctx = wglCreateContext(hdc);
		wglMakeCurrent(hdc, ctx);

		// initialize extensions and latest OpenGL version
		isAlreadyInitialized = gladLoadGL() && gladLoadWGL(hdc);

		wglMakeCurrent(hdc, nullptr);
		wglDeleteContext(ctx);
		DestroyWindow(dummy);

		if (!isAlreadyInitialized)
			throw std::runtime_error("Unable to load OpenGL");
	}

	int selectBetterPixelFormat(HDC hdc)
	{
		float fAttributes[] = { 0, 0 };
		int iAttributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 24,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 0,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 4,
			0, 0 };

		// try to get the highest multi-sampling we can
		int pixelFormat;
		UINT numFormats;
		for (int samples = 8; samples > 0; samples /= 2)
		{
			iAttributes[19] = samples;
			if (wglChoosePixelFormatARB(
					hdc,
					iAttributes,
					fAttributes,
					1,
					&pixelFormat,
					&numFormats)
				&& numFormats > 0)
				return pixelFormat;
		}

		// or fall back to non-extended in case wglChoosePixelFormatARB fails
		return ChoosePixelFormat(hdc, &pfd);
	}

}


Context::Context(const App::Window& window) : owner((HWND)window.impl)
{
	description = "OpenGL  |  no or unknown device";

	loadOpenGL();

	auto hdc = GetDC(owner);
	SetPixelFormat(hdc, selectBetterPixelFormat(hdc), &pfd);

	// Try to create the ideal context, but if it fails, fall back to whatever we can get
	if (GLAD_WGL_ARB_create_context && GLAD_WGL_ARB_create_context_profile)
	{
		const int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0 };

		ctx = wglCreateContextAttribsARB(hdc, nullptr, attribList);
	}
	if (!ctx)
		ctx = wglCreateContext(hdc);
	if (!ctx)
		throw std::runtime_error("Failed to create an OpenGL context");

	wglMakeCurrent(hdc, ctx);


	// enable V-Sync
	if (GLAD_WGL_EXT_swap_control)
		wglSwapIntervalEXT(1);  // or 0 to disable V-Sync

	if (GLAD_WGL_EXT_swap_control_tear)
		wglSwapIntervalEXT(-1);  // to enable adaptive sync

	readDeviceDescription(wglGetSwapIntervalEXT());
}

Context::~Context()
{
	wglMakeCurrent(GetDC(owner), nullptr);
	wglDeleteContext(ctx);
}

void Context::draw()
{
	PAINTSTRUCT ps;
	auto handle = owner;
	auto hdc = BeginPaint(handle, &ps);

	drawPlatformIndependent();

	SwapBuffers(hdc);
	EndPaint(handle, &ps);
}
