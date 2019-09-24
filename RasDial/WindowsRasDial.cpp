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
		error_message << TEXT("����������������ʼ��ʧ�� ") << e.what();
		error_message << std::endl;
		error_message << TEXT("�볢���ֶ�������Ϊ ") << name_ << TEXT(" ��������") << std::endl;
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
		if (result_code == ERROR_FROM_DEVICE /*��ֹ�´�756*/
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
			error_message << TEXT("δ֪����");
		}

		// ��Error Code���ƣ�ǿ���û������ô�������ĵ�
		// ReSharper disable once IdentifierTypo
		std::wstringstream errorcode_pppoe_message;

		errorcode_pppoe_message << static_cast<uint32_t>(result_code);
		switch (result_code) {
		case 651:
			errorcode_pppoe_message
				<< TEXT("1.���������·�Ƿ�ͨ��(����ʹ����������������PC���Ӹ����ڲ���)\n")
				<< TEXT("2.��ȷ�������������������.\n")
				<< TEXT("3.��ȷ�������Ѳ��.\n")
				<< TEXT("��������������δ��������ϱ���������\n")
				<< TEXT("��ַΪ: http://bx.neusoft.edu.cn/ \n")
				<< TEXT("����������������������������������������������������������������������������������������\n");
			break;
		case 691:
			errorcode_pppoe_message
				<< TEXT("1.�����˺Ż������Լ��ײ������Ƿ���ȷ.\n")
				<< TEXT("2.�����ײ��Ƿ�δ��Ч���Ѿ�����.\n")
				<< TEXT("3.��ȷ����ǰû�����ӵ�����Wifi����.\n")
				<< TEXT("��������������������������������������������������������������������������������������������\n");
			break;
		case 756:
			errorcode_pppoe_message
				<< TEXT("�볢���������������.\n");
			break;
		case 668:
			errorcode_pppoe_message
				<< TEXT("�����³��Բ���.\n")
				<< TEXT("\n��������������������������������������������������������������������������������������������\n");
			break;
		case 1062:
			errorcode_pppoe_message
				<< TEXT("������һЩ�����������ķ����޷���������\n")
				<< TEXT("\n��������������������������������������������������������������������������������������������\n");
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
				error_message << TEXT("ö��RAS-CAPABLE�豸ʱ���ش���\n")
					<< static_cast<uint32_t>(dwRet);
				if (RasGetErrorStringW(dwRet, szError, sizeof(szError)) == ERROR_SUCCESS)
				{
					const std::wstring message{static_cast<wchar_t*>(szError)};
					error_message << TEXT(" ") << message;
				}
				else
				{
					error_message << TEXT(" ") << TEXT("δ֪����");
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
				error_message <<  TEXT("û�п��õ�RAS-CAPABLE�豸\n");
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
				error_message << static_cast<uint32_t>(dwRet) <<TEXT(" ") << TEXT("δ֪����") << std::endl;
			}
			throw std::logic_error{ Utils::ws2s(error_message.str()) };
		}
	}
}
