// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
// ReSharper disable once IdentifierTypo
#define NOMINMAX
#include <windows.h>
// ReSharper disable once IdentifierTypo
#define RASNP_Ipv6 0x00000008
#include <ras.h>
#include <raserror.h>

#include <memory>
#include <string>
#include <iostream>
#include <mutex>
#include <sstream>
#include <exception>
#include "Utils.hpp"

// ReSharper disable once IdentifierTypo
#define SPDLOG_WCHAR_FILENAMES
// ReSharper disable once IdentifierTypo
#define RASDAIL_BUFFER_SIZE 1024
#define DEVICE_MAX_NUMBER 64

// reference additional headers your program requires here
