
#include <Crib/Platform/Win>


// Unfortunately, MinGW std::system_category() does not correctly report Windows error messages,
// so we have to do it ourselves.
#if defined(__MINGW32__)
#	include <string>

namespace
{
	std::string getWindowsErrorMessage(DWORD code)
	{
		char* msg;

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&msg,
			0,
			nullptr);

		std::string output;

		if (msg)
		{
			output = msg;
			LocalFree(msg);
		}
		else
		{
			output = "Unknown error " + std::to_string(code);
		}

		return output;
	}
}
#endif


crib::platform::win::error::error(std::error_code code) : system_error(std::move(code))
{
#if defined(__MINGW32__)
	message = getWindowsErrorMessage(code().value());
#else
	message = system_error::code().message();
#endif
}
