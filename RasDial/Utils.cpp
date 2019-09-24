#include "stdafx.h"
#include "Utils.hpp"

std::wstring Utils::s2ws(const std::string& str)
{
	static std::locale loc{"en_US.UTF-8"};
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(str);
}

std::string Utils::ws2s(const std::wstring& wstr)
{
	static std::locale loc{"en_US.UTF-8"};
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstr);
}
