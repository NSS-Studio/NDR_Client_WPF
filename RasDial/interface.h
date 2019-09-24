#pragma once
#define DLL_EXPORT __declspec(dllexport)

#include <Windows.h>

// Export Interface To Invoke this DLL

extern "C" {
bool DLL_EXPORT create_ras_dial_instance(LPCTSTR adapter_name);
bool DLL_EXPORT destroy_ras_dial_instance();
bool DLL_EXPORT ras_dial_connection_state();
bool DLL_EXPORT ras_dial(LPCTSTR username, LPCTSTR password);
void DLL_EXPORT ras_hang_up();
LPCTSTR DLL_EXPORT ras_get_error_msg();
}
