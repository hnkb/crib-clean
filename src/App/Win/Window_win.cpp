
#include "App.h"
#include <Crib/App>
#include "../../Graphics/OpenGL/Context.h"
#include <Crib/Platform/Win>

using Crib::App::Window;


namespace
{

	LRESULT CALLBACK proc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto window = (Window*)GetWindowLongPtrW(handle, GWLP_USERDATA);

		if (window)
		{
			// if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) || message ==
			// WM_KEYDOWN
			//	|| message == WM_KEYUP)
			//{
			//	if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN
			//		|| message == WM_MBUTTONDOWN)
			//		SetCapture(handle);

			//	if (message == WM_LBUTTONUP || message == WM_RBUTTONUP
			//		|| message == WM_MBUTTONUP)
			//		ReleaseCapture();

			//	input.push(message, wParam, lParam, timer.now());
			//	return 0;
			//}

			switch (message)
			{
				case WM_CHAR:
				{
					Crib::Platform::Win::WideString buffer(4);
					memset((wchar_t*)buffer, 0, sizeof(wchar_t) * buffer.capacity());
					buffer[0] = (wchar_t)wParam;

					if (IS_HIGH_SURROGATE(wParam))
					{
						MSG next;
						if (PeekMessageW(&next, handle, WM_CHAR, WM_CHAR, TRUE)
							&& IS_LOW_SURROGATE(next.wParam))
							buffer[1] = (wchar_t)next.wParam;
					}
					window->onKeyChar(buffer);
					return 0;
				}

				case WM_MOVE:
				{
					RECT rect;
					GetWindowRect(handle, &rect);
					window->onPositionChanged({ rect.left, rect.top });
					return 0;
				}

				case WM_SIZE:
					window->onSizeChanged({ LOWORD(lParam), HIWORD(lParam) });
					InvalidateRect(handle, nullptr, FALSE);
					return 0;

				case WM_ERASEBKGND:
					// This handler is not necessary. The window class was created with
					// hbrBackground set to nullptr, so the default processing (by
					// DefWindowProcW) is to do nothing.
					return TRUE;

				case WM_PAINT:
					window->draw();
					return 0;

				case WM_DESTROY:
					if (window->context)
					{
						delete window->context;
						window->context = nullptr;
					}
					window->impl = nullptr;
					PostMessageW(
						nullptr,
						(UINT)Crib::Platform::Win::WindowMessage::closed,
						0,
						0);
					return 0;
			}
		}

		return DefWindowProcW(handle, message, wParam, lParam);
	}

}


Window::Window() : Window(Options {})
{}

Window::Window(Options opt)
{
	if (opt.size.x <= 0)
		opt.size.x = CW_USEDEFAULT;
	if (opt.pos.x <= 0)
		opt.pos.x = CW_USEDEFAULT;
	if (opt.title.empty())
		opt.title = "Crib";

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
	wcex.lpszClassName = Platform::Win::windowClassName;
	wcex.lpfnWndProc = proc;
	RegisterClassExW(&wcex);

	impl = CreateWindowEx(
		0,
		wcex.lpszClassName,
		Platform::Win::WideString(opt.title),
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
		throw Platform::Win::Error();  //"CreateWindow");

	SetWindowLongPtrW((HWND)impl, GWLP_USERDATA, LONG_PTR(this));

	createGraphicsContext(opt);

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

Window::~Window()
{
	if (context)
	{
		delete context;
		context = nullptr;
	}
	if (impl)
		DestroyWindow((HWND)impl);
}

Window& Window::operator=(Window&& other)
{
	if (this != &other)
	{
		impl = other.impl;
		context = other.context;
		other.impl = nullptr;
		other.context = nullptr;

		SetWindowLongPtrW((HWND)impl, GWLP_USERDATA, LONG_PTR(this));
	}
	return *this;
}


void Window::close()
{
	DestroyWindow((HWND)impl);
}


Window::Options Window::getOptions() const
{
	return {};
}

Window& Window::setOptions(const Options& opt)
{
	SetWindowTextW((HWND)impl, Platform::Win::WideString(opt.title));
	return *this;
}
