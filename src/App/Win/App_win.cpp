
#include "app_win.h"
#include <crib/app>


int crib::app::run()
{
	MSG msg;

	if (EnumThreadWindows(
			GetCurrentThreadId(),
			[](HWND, LPARAM) { return FALSE; },
			0))
		throw std::logic_error("[crib::application::run] At least one window is required.");

	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if ((msg.message == (UINT)platform::win::window_message::quit)
			|| (msg.message == (UINT)platform::win::window_message::closed
				&& EnumThreadWindows(
					GetCurrentThreadId(),
					[](HWND, LPARAM) { return FALSE; },
					0)))
			PostQuitMessage(0);
	}

	return (int)msg.wParam;
}
