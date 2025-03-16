#pragma once

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <string>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

enum class EType
{
	TYPE_DX11,
	TYPE_DX12
};

static int const NUM_FRAMES_IN_FLIGHT = 3;
static int const NUM_BACK_BUFFERS = 3;

class Fuser
{
private:
	static inline HWND m_hwnd;
	static inline HINSTANCE m_hInstance;
	static inline EType m_type;
	WNDCLASSEXW wc;
	bool m_is_initialized = false;

	//debug things
	RECT game_rect = {0, 0, 0, 0};
	ImVec2 game_size;

public:
	Fuser()
	{
	};

	~Fuser()
	{
	};

	EType GetType() { return m_type; }
	HWND GetHWND() { return m_hwnd; }
	HINSTANCE GetHINSTANCE() { return m_hInstance; }

	bool Initialize(ImVec2 window_size);

	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	bool LoadTextureFromWebpFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	bool LoadTextureFromMemory(const unsigned char* buffer, size_t size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	/*
	*The call back isbetween ImGui Render & New.
	*so you can do anything imgui related within the callback
	*preferably used for example player drawing ect.
	*/
	void Run(void (*callback)());

	ImVec4 clear_color = ImVec4(0.f, 0.f, 0.0f, 1.00f);
};

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern UINT g_ResizeWidth;
extern UINT g_ResizeHeight;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
inline Fuser fuser;
