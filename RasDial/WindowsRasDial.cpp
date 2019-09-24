#include "stdafx.h"
#include "WindowsRasDial.hpp"
#include "Utils.hpp"
#include "spdlog/include/spdlog/fmt/fmt.h"
#include "spdlog/include/spdlog/spdlog.h"
#include "spdlog/include/spdlog/sinks/stdout_color_sinks.h"
std::shared_ptr<windows_ras_dial> windows_ras_dial::instance_;
std::mutex windows_ras_dial::ras_dial_mutex_;

std::shared_ptr<windows_ras_dial> windows_ras_dial::get_instance()
{
	return instance_;
}

void windows_ras_dial::init_logger()
{
	auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto my_logger = std::make_shared<spdlog::logger>("console", sink);
	my_logger->set_level(spdlog::level::trace);
	my_logger->info("HelloWorld");
	my_logger->info("debug test");

	spdlog::register_logger(my_logger);
}

void windows_ras_dial::destroy_logger()
{
	spdlog::drop("console");
}

bool windows_ras_dial::create_instance(std::wstring adapter_name)
{
	{
		std::lock_guard<std::mutex> locker{ras_dial_mutex_};
		if (!instance_)
		{
			instance_ = std::make_shared<windows_ras_dial>();
			instance_->init_logger();
		}
		windows_ras_dial::get_instance()->name_ = adapter_name;
	}
	return true;
}

bool windows_ras_dial::destroy_instance()
{
	{
		std::lock_guard<std::mutex> locker{ras_dial_mutex_};
		if (instance_.use_count() != 0)
		{
			instance_->destroy_logger();
			instance_.reset();
		}
	}
	return true;
}

void windows_ras_dial::dial(std::wstring const& username, std::wstring const& password)
{
	RASDIALPARAMSW params;
	std::wstringstream error_message;

	auto logger = spdlog::get("console");

	try
	{
		prepare_phone_book_entry(name_);
	}
	catch (std::logic_error e)
	{
		error_message << TEXT("拨号连接适配器初始化失败 ") << e.what();
		error_message << std::endl;
		error_message << TEXT("请尝试手动创建名为 ") << name_ << TEXT(" 的适配器") << std::endl;
		throw std::logic_error{Utils::ws2s(this->error_msg)};
	}

	memset(&params, 0, sizeof(params));
	params.dwSize = sizeof(RASDIALPARAMSW);
	wcscpy_s(params.szEntryName, name_.c_str());
	wcscpy_s(params.szPhoneNumber, TEXT(""));
	wcscpy_s(params.szCallbackNumber, TEXT(""));
	wcscpy_s(params.szUserName, username.c_str());
	wcscpy_s(params.szPassword, password.c_str());
	wcscpy_s(params.szDomain, TEXT(""));

	// ReSharper disable once CppUseAuto
	DWORD const result_code = RasDialW(nullptr, nullptr, &params, NULL, nullptr, &ras_connection_handle_);

	logger->debug("PPPoE return code: {}\n", result_code);

	if (result_code != 0)
	{
		/*****************/
		if (result_code == ERROR_FROM_DEVICE /*防止下次756*/
			|| result_code == ERROR_NO_ANSWER)
		{
			RasHangUpW(ras_connection_handle_);
		}
		/*****************/
		wchar_t szError[RASDAIL_BUFFER_SIZE];

		if (RasGetErrorStringW(result_code, szError, sizeof(szError)) == ERROR_SUCCESS)
		{
			error_message << szError << std::endl;
		}
		else
		{
			error_message << TEXT("未知错误");
		}

		// 将Error Code下移，强迫用户看常用错误帮助文档
		// ReSharper disable once IdentifierTypo
		std::wstringstream errorcode_pppoe_message;

		errorcode_pppoe_message << static_cast<uint32_t>(result_code);
		switch (result_code) {
		case 651:
			errorcode_pppoe_message
				<< TEXT("1.检查网络线路是否通畅(尝试使用其他可正常拨号PC连接该网口拨号)\n")
				<< TEXT("2.请确保交换机及其连线完好.\n")
				<< TEXT("3.请确保网线已插好.\n")
				<< TEXT("若如上做问题仍未解决，请上报修网报修\n")
				<< TEXT("网址为: http://bx.neusoft.edu.cn/ \n")
				<< TEXT("┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈\n");
			break;
		case 691:
			errorcode_pppoe_message
				<< TEXT("1.请检查账号或密码以及套餐设置是否正确.\n")
				<< TEXT("2.请检查套餐是否还未生效或已经过期.\n")
				<< TEXT("3.请确保当前没有连接到其他Wifi网络.\n")
				<< TEXT("┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈\n");
			break;
		case 756:
			errorcode_pppoe_message
				<< TEXT("请尝试重新启动计算机.\n");
			break;
		case 668:
			errorcode_pppoe_message
				<< TEXT("请重新尝试拨号.\n")
				<< TEXT("\n┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈\n");
			break;
		case 1062:
			errorcode_pppoe_message
				<< TEXT("可能是一些拨号所依赖的服务无法启动所致\n")
				<< TEXT("\n┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈\n");
			break;
		}
		ras_connection_handle_ = nullptr;
		throw std::logic_error{Utils::ws2s(errorcode_pppoe_message.str() + error_message.str())};
	}
}

void windows_ras_dial::hang_up()
{
	RasHangUpW(ras_connection_handle_);
	ras_connection_handle_ = nullptr;
}

LPCTSTR windows_ras_dial::get_error_msg()
{
	return this->error_msg.c_str();
}

void windows_ras_dial::set_error_msg(std::string s)
{
	this->error_msg = Utils::s2ws(s);
}

bool windows_ras_dial::is_disconnected() const
{
	RASCONNSTATUSW rcs{};
	rcs.dwSize = sizeof(RASCONNSTATUSW);
	const auto ret = RasGetConnectStatusW(ras_connection_handle_, &rcs);
	auto logger = spdlog::get("console");
	logger->debug("is_disconnected: {}", ret || (rcs.rasconnstate == RASCS_Disconnected));
	return ret || (rcs.rasconnstate == RASCS_Disconnected);
}

void windows_ras_dial::prepare_phone_book_entry(std::wstring const& entry_name) const
{
	DWORD dwBufferSize = sizeof(RASENTRYW);
	DWORD dwRet = 0;
	wchar_t szEntryName[RASDAIL_BUFFER_SIZE];
	RASENTRYW rasEntry;
	std::wstringstream error_message;

	auto logger = spdlog::get("console");

	memset(&rasEntry, 0, sizeof(RASENTRYW));
	szEntryName[0] = TEXT('\0');
	rasEntry.dwSize = sizeof(RASENTRYW);
	wcscpy_s(szEntryName, entry_name.c_str());
	dwRet = RasGetEntryPropertiesW(nullptr, szEntryName, &rasEntry, &dwBufferSize, nullptr, nullptr);

	logger->debug("dwRet: {}\n", dwRet);
	if (ERROR_CANNOT_FIND_PHONEBOOK_ENTRY == dwRet)
	{
		logger->debug("Phone book entry not found");

		DWORD cb = sizeof(RASENTRYW);
		rasEntry.dwSize = cb;
		rasEntry.dwfOptions = RASEO_RemoteDefaultGateway |
			// RASEO_PreviewPhoneNumber|
			// RASEO_PreviewUserPw|
			RASEO_ModemLights;

		wcscpy_s(rasEntry.szDeviceType, RASDT_Modem);
		// TODO fill rasEntry.szDeviceName by RasEnumDevices ;
		{
			DWORD dwSize, dwDevices;
			RASDEVINFOW arrDevInfo[DEVICE_MAX_NUMBER];
			dwSize = sizeof(arrDevInfo);
			arrDevInfo[0].dwSize = sizeof(RASDEVINFOW);
			DWORD ret = RasEnumDevicesW(arrDevInfo, &dwSize, &dwDevices);
			if (ret)
			{
				logger->debug("RasEnumDevicesW() error {}\n", ret);
				wchar_t szError[RASDAIL_BUFFER_SIZE];
				error_message << TEXT("枚举RAS-CAPABLE设备时返回错误\n")
					<< static_cast<uint32_t>(dwRet);
				if (RasGetErrorStringW(dwRet, szError, sizeof(szError)) == ERROR_SUCCESS)
				{
					const std::wstring message{static_cast<wchar_t*>(szError)};
					error_message << TEXT(" ") << message;
				}
				else
				{
					error_message << TEXT(" ") << TEXT("未知错误");
				}
				//----- return RasGetErrorStringW()
				throw std::logic_error{Utils::ws2s(error_message.str())};
			}
			logger->debug("RasEnumDevicesW() succeed code: {}", ret);
			bool bFound = false;
			DWORD i = 0;
			for (; i < dwDevices; ++i)
			{
				logger->debug("Device name: {}\n", Utils::ws2s(std::wstring{arrDevInfo[i].szDeviceName}));
				logger->debug("Device type: {}\n", Utils::ws2s(std::wstring{arrDevInfo[i].szDeviceType}));
				if (!wcscmp(arrDevInfo[i].szDeviceType, TEXT("pppoe")))
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				logger->debug("Device not found");
				error_message <<  TEXT("没有可用的RAS-CAPABLE设备\n");
				throw std::logic_error{ Utils::ws2s(error_message.str()) };
			}

			wcscpy_s(rasEntry.szDeviceName, arrDevInfo[i].szDeviceName);
			logger->debug("Device name {}\n", Utils::ws2s(std::wstring{arrDevInfo[i].szDeviceName}));
		}
		rasEntry.dwfNetProtocols = RASNP_Ip;
		rasEntry.dwFramingProtocol = RASFP_Ppp;
		rasEntry.dwfNetProtocols = RASNP_Ipv6 | RASNP_Ip;
		dwRet = RasSetEntryPropertiesW(nullptr, szEntryName, &rasEntry, cb, nullptr, 0);
		if (dwRet)
		{
			wchar_t szError[RASDAIL_BUFFER_SIZE];
			if (RasGetErrorStringW(dwRet, szError, sizeof(szError)) == ERROR_SUCCESS)
			{
				const std::wstring message{ static_cast<wchar_t*>(szError) };
				error_message << static_cast<uint32_t>(dwRet) << TEXT(" ") << message << std::endl;
			}
			else
			{
				error_message << static_cast<uint32_t>(dwRet) <<TEXT(" ") << TEXT("未知错误") << std::endl;
			}
			throw std::logic_error{ Utils::ws2s(error_message.str()) };
		}
	}
}
