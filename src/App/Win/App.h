
#pragma once

#include <Crib/Platform/Win>


namespace crib::platform::win
{

	enum class window_message : UINT { closed = (WM_APP + 501), quit = (WM_APP + 502) };

}
