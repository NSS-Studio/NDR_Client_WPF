#pragma once
#include "stdafx.h"

class windows_ras_dial final
{
public:
	static std::shared_ptr<windows_ras_dial> get_instance();
	static bool create_instance(std::wstring adapter_name);
	static bool destroy_instance();

	bool is_disconnected() const;
	void dial(std::wstring const& username, std::wstring const& password);
	void hang_up();
	LPCTSTR get_error_msg();
	void set_error_msg(std::string);

private:
	static void init_logger();
	static void destroy_logger();
	void prepare_phone_book_entry(std::wstring const& entry_name) const;
	static std::mutex ras_dial_mutex_;
	static std::shared_ptr<windows_ras_dial> instance_;
	HRASCONN ras_connection_handle_ = nullptr;
	std::wstring name_;
	std::wstring error_msg;
};
