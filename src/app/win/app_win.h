
#pragma once

#include <crib/platform/win>


namespace crib::platform::win
{

	enum class window_message : UINT { closed = (WM_APP + 501), quit = (WM_APP + 502) };

}
