#include"windmaker.h"
#include<imgui.h>
#include<backends/imgui_impl_dx12.h>
#include<backends/imgui_impl_win32.h>
#include"DXUtils.h"
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WindowProcHook* WindProcManager::s_prochook = nullptr;
Renderable* WindProcManager::s_dx12manager = nullptr;
LRESULT CALLBACK WindProcManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef USEIMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
	{
		return true;
	}
#endif // USEIMGUI
	if (s_prochook != nullptr)
	{
		s_prochook->ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
	}
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT:
	{
			//draw here
			if (s_dx12manager)
			{
				s_dx12manager->Render();
			}
			return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		
	}

	return 0;
	
}

void WindMaker::CreateWind(unsigned width, unsigned height,LPCWSTR windowname)
{
	
	m_width = width;
	m_height = height;
	wchar_t CLASS_NAME[] = L" Window Class";
	WNDCLASS wndclass = {0};
	wndclass.hInstance = GetModuleHandle(nullptr);
	wndclass.lpszClassName = CLASS_NAME;
	wndclass.lpfnWndProc = WindProcManager::WindowProc;
	ATOM atomres= RegisterClass(&wndclass);
	assert(atomres != 0);
	 m_hwnd =
	CreateWindowEx(0, CLASS_NAME, windowname, WS_OVERLAPPEDWINDOW, 0, 0, m_width,m_height, NULL, NULL, wndclass.hInstance, NULL);
	assert(m_hwnd != NULL);
	ShowWindow(m_hwnd, SW_SHOW);
	
}

void WindMaker::RunMessageloop(Renderable* dx12manager)
{
	WindProcManager::SetDX12Manager(dx12manager);
	

	//using hwnd here instead of null causes window created to be in stuck state
		MSG msg = {0};
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					bool requestquit = true;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		
		
		
		
		
		
	
}