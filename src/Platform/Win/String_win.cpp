
#include <crib/platform/win>
#include <strsafe.h>

using namespace crib::platform::win;



// wide_string& wide_string::operator=(const std::string& utf8)
wide_string& wide_string::operator=(std::string_view utf8)
{
	// if (utf8.empty())
	//{
	//	utf16.resize(1);
	//	utf16.front() = 0;
	// }
	// else
	{
		auto buffer_size = MultiByteToWideChar(
			CP_UTF8,
			0,
			utf8.data(),
			(int)utf8.size()
				+ 1,  // add 1 to input size, so null-terminator is also added to output
			nullptr,
			0);

		if (buffer_size == 0)
			throw error();

		utf16.resize((size_t)buffer_size);

		MultiByteToWideChar(
			CP_UTF8,
			0,
			utf8.data(),
			(int)utf8.size() + 1,
			utf16.data(),
			capacity());
	}

	return *this;
}


std::string wide_string::to_utf8(const wchar_t* utf16)
{
	// we either have size (like when FormatMessageW returns size, or when using
	// std::wstring)
	// 			   => directly use size+1 to also copy null-terminator
	// or we     have bounds (like when we have vector<> buffer size)
	//			   => use StringCchLengthW to get size?
	// or we     don't have any knowledge (what to do here?)
	//             => use -1? is it safe?

	auto buffer_size = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, nullptr, 0, nullptr, nullptr);

	if (buffer_size == 0 && *utf16 != 0)
		throw error();

	std::string utf8(
		(size_t)buffer_size - 1,
		0);  // buffer_size includes null-terminator, so we subtract one for string

	WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16,
		-1,
		&utf8[0],
		(int)utf8.size() + 1,  // adding one to output size, to include null-terminator
		nullptr,
		nullptr);

	return utf8;
}


int wide_string::length() const
{
	size_t len = 0;

	throw_on_error(StringCchLengthW(utf16.data(), utf16.size(), &len));

	return (int)len;
}
