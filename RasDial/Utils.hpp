#pragma once
#include <string>
#include <codecvt>

namespace Utils
{
	// ReSharper disable once CppInconsistentNaming
	std::wstring s2ws(const std::string& str);
	// ReSharper disable once CppInconsistentNaming
	std::string ws2s(const std::wstring& wstr);
};
