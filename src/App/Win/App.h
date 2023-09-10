
#pragma once

#include <Crib/Platform/Win>


namespace Crib::Platform::Win
{

	enum class Message : UINT { closed = (WM_APP + 501), quit = (WM_APP + 502) };

	constexpr auto windowClass = L"Crib";

}
