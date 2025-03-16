#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include <thread>
#include <setupapi.h>
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")
namespace kmbox
{
	extern bool connected;
	extern std::string find_port(const std::string& targetDescription);
	extern void initialize(LPCSTR port);

	extern void move(int x, int y);
	extern bool IsDown(int virtual_key);
}
