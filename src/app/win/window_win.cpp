
#include <crib/app>
#include <crib/platform/win>
#include "../../graphics/gl/context_gl.h"

using crib::app::window;


namespace
{

	LRESULT CALLBACK proc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto wnd = (window*)GetWindowLongPtrW(handle, GWLP_USERDATA);

		if (wnd)
		{
			switch (message)
			{
				case WM_SIZE:
					if (wnd->context)
						wnd->context->on_resize({ LOWORD(lParam), HIWORD(lParam) });
					return 0;

				case WM_ERASEBKGND:
					// This handler is not necessary. The window class was created with
					// hbrBackground set to nullptr, so the default processing (by
					// DefWindowProcW) is to do nothing.
					return TRUE;

				case WM_PAINT:
					if (wnd->context)
						wnd->context->draw();
					return 0;

				case WM_DESTROY:
					if (wnd->context)
					{
						delete wnd->context;
						wnd->context = nullptr;
					}
					wnd->impl = nullptr;
					PostMessageW(
						nullptr,
						(UINT)crib::platform::win::window_message::closed,
						0,
						0);
					return 0;
			}
		}

		return DefWindowProcW(handle, message, wParam, lParam);
	}

}


window::window() : window(options {})
{}

window::window(options opt)
{
	if (opt.size.x <= 0)
		opt.size.x = CW_USEDEFAULT;
	if (opt.pos.x <= 0)
		opt.pos.x = CW_USEDEFAULT;
	if (opt.title.empty())
		opt.title = "crib";
	if (opt.type_id.empty())
		opt.type_id = "crib";

	if (opt.size.x != CW_USEDEFAULT)
	{
		RECT rect { 0, 0, opt.size.x, opt.size.y };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		opt.size.x = rect.right - rect.left;
		opt.size.y = rect.bottom - rect.top;
	}

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.lpszClassName = platform::win::wide_string(opt.type_id);
	wcex.lpfnWndProc = proc;
	RegisterClassExW(&wcex);

	impl = CreateWindowEx(
		0,
		wcex.lpszClassName,
		platform::win::wide_string(opt.title),
		WS_OVERLAPPEDWINDOW,
		opt.pos.x,
		opt.pos.y,
		opt.size.x,
		opt.size.y,
		nullptr,
		nullptr,
		wcex.hInstance,
		nullptr);

	if (!impl)
		throw platform::win::error();  //"CreateWindow");

	SetWindowLongPtrW((HWND)impl, GWLP_USERDATA, LONG_PTR(this));

	{
		if (opt.prefer_engine == engine::any || opt.prefer_engine == engine::opengl)
	context = new graphics::gl::context(*this);
	if (context)
		SetWindowTextW((HWND)impl, platform::win::wide_string(context->description));
	}

	ShowWindow((HWND)impl, SW_SHOWDEFAULT);

	{
		RECT rect;
		GetClientRect((HWND)impl, &rect);
		PostMessageW(
			(HWND)impl,
			WM_SIZE,
			SIZE_RESTORED,
			MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top));
	}
}

window::~window()
{
	if (context)
		delete context;
	if (impl)
		DestroyWindow((HWND)impl);
}


window::options window::get_options() const
{
	return {};
}

window& window::set_options(const options& opt)
{
	SetWindowTextW((HWND)impl, platform::win::wide_string(opt.title));
	return *this;
}