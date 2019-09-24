#include "interface.h"
#include "WindowsRasDial.hpp"
#include "stdafx.h"

extern "C" {
bool DLL_EXPORT create_ras_dial_instance(LPCTSTR adapter_name)
{
	return windows_ras_dial::create_instance(std::wstring( adapter_name ));
}

bool DLL_EXPORT destroy_ras_dial_instance()
{
	return windows_ras_dial::destroy_instance();
}

bool DLL_EXPORT ras_dial_connection_state()
{
	return windows_ras_dial::get_instance()->is_disconnected();
}
bool DLL_EXPORT ras_dial(LPCTSTR username, LPCTSTR password)
{
	try 
	{
		windows_ras_dial::get_instance()->dial(std::wstring(username), std::wstring(password));
		return true;
	}
	catch (std::logic_error& e) 
	{
		windows_ras_dial::get_instance()->set_error_msg(e.what());
		return false;
	}
}
void DLL_EXPORT ras_hang_up() 
{
	windows_ras_dial::get_instance()->hang_up();
}
LPCTSTR DLL_EXPORT ras_get_error_msg()
{
	LPCTSTR tmp = windows_ras_dial::get_instance()->get_error_msg();
	return tmp;
}
}
