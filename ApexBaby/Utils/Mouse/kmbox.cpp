#include "kmbox.h"

#include <iostream>

#include "context.h"

namespace kmbox
{
	HANDLE serial_handle = nullptr;

	bool connected = false;

	int clamp(int i)
	{
		if (i > 127)
			i = 127;
		if (i < -128)
			i = -128;

		return i;
	}

	std::string find_port(const std::string& targetDescription)
	{
		HDEVINFO hDevInfo = SetupDiGetClassDevsA(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
		if (hDevInfo == INVALID_HANDLE_VALUE) return "";

		SP_DEVINFO_DATA deviceInfoData;
		deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceInfoData); ++i)
		{
			char buf[512];
			DWORD nSize = 0;

			if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)buf, sizeof(buf), &nSize) && nSize > 0)
			{
				buf[nSize] = '\0';
				std::string deviceDescription = buf;

				size_t comPos = deviceDescription.find("COM");
				size_t endPos = deviceDescription.find(")", comPos);

				if (comPos != std::string::npos && endPos != std::string::npos && deviceDescription.find(targetDescription) != std::string::npos)
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
					return deviceDescription.substr(comPos, endPos - comPos);
				}
			}
		}
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return "";
	}

	void initialize(LPCSTR port)
	{
		connected = false;

		serial_handle = CreateFileA(port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (serial_handle == INVALID_HANDLE_VALUE)
		{
			ctx.m_logger->Error("Failed to open serial port!\n");
			return;
		}

		if (!SetupComm(serial_handle, 8192, 8192))
		{
			ctx.m_logger->Error("Failed to setup serial port!\n");
			return;
		}

		COMMTIMEOUTS timeouts = {0};
		if (!GetCommTimeouts(serial_handle, &timeouts))
		{
			ctx.m_logger->Error("Failed to get serial timeouts!\n");
			return;
		}
		timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = 0;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 2000;

		if (!SetCommTimeouts(serial_handle, &timeouts))
		{
			ctx.m_logger->Error("Failed to set serial parameters!\n");
			CloseHandle(serial_handle);
			return;
		}

		PurgeComm(serial_handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

		DCB dcbSerialParams = {0};
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		if (!GetCommState(serial_handle, &dcbSerialParams))
		{
			ctx.m_logger->Error("Failed to get serial state!\n");
			CloseHandle(serial_handle);
			return;
		}

		int baud = ctx.m_settings.hardware_kmbox_baudrate;
		dcbSerialParams.BaudRate = baud;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;

		if (!SetCommState(serial_handle, &dcbSerialParams))
		{
			ctx.m_logger->Error("Failed to set serial parameters!\n");
			CloseHandle(serial_handle);
			return;
		}

		ctx.m_logger->Succes("Connected to KMBOX on port %s\n", std::string(port).c_str());
		connected = true;
	}

	void SendCommand(const std::string& command)
	{
		DWORD bytesWritten;
		if (!WriteFile(serial_handle, command.c_str(), command.length(), &bytesWritten, NULL))
		{
			ctx.m_logger->Error("Failed to write to serial port!\n");
		}
	}

	void move(int x, int y)
	{
		if (!connected)
		{
			printf("not connected?\n");
			return;
		}
		x = clamp(x);
		y = clamp(y);
		std::string command = "km.move(" + std::to_string(x) + "," + std::to_string(y) + ")\r\n";
		SendCommand(command.c_str());
	}

	bool IsDown(int virtual_key)
	{
		if (!connected)
			return false;
		return false;
		std::string command = virtual_key == 3 ? "km.middle()\r\n" : "km.side" + std::to_string(virtual_key) + std::string("()\r\n");
		SendCommand(command);
		char readBuffer[256];
		DWORD bytesRead;
		if (!ReadFile(serial_handle, readBuffer, sizeof(readBuffer) - 1, &bytesRead, NULL))
		{
			std::cerr << "Failed to read from serial port!" << std::endl;
			return false;
		}

		if (bytesRead > 0)
		{
			readBuffer[bytesRead] = '\0';
			std::string command2 = virtual_key == 3 ? "km.middle()\r\n1" : "km.side" + std::to_string(virtual_key) + std::string("()\r\n1");
			if (strstr(readBuffer, command2.c_str()))
				return true;
		}
		return false;
	}
}
