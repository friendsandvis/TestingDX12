#include"windmaker.h"

WindowProcHook* WindProcManager::s_prochook = nullptr;
LRESULT CALLBACK WindProcManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (s_prochook != nullptr)
	{
		s_prochook->ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
	}
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
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
	
	

	//using hwnd here instead of null causes window created to be in stuck state
	
	
	 bool running = true;
	while(running)
	{
		if (IsWindow(m_hwnd))
		{
			//draw here
			if (dx12manager)
			{
				dx12manager->Render();
			}
		}
		MSG msg = {0};
		while (PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE))
		{




			if (msg.message == WM_QUIT)
			{
				running = false;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		
		

		
		//do check for the validity of window before drawing upon it(if you try to prresent upon a destroyed window then app crashs.
		
		
		
		
		
		
	}
	
}