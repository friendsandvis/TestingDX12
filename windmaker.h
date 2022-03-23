#pragma once

#include"WindowProcHook.h"
#include<assert.h>
#include"Renderable.h"

class WindProcManager
{
public:
	inline static void SetProcHook(WindowProcHook* ahook) { s_prochook=ahook; }
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	static WindowProcHook* s_prochook;
};

class WindMaker
{
public:
	 
	
	inline HWND Gethwnd() { return m_hwnd; }
	inline unsigned GetHeight() { return m_height; }
	inline unsigned GetWidth() { return m_width; }
	inline HINSTANCE GetHinstance() {
		return GetModuleHandle(nullptr);
	}
	
	void CreateWind(unsigned width,unsigned height, LPCWSTR windowname);
	void RunMessageloop(Renderable* dx12manager =nullptr);
private:
	
	HWND m_hwnd;
	unsigned m_width, m_height;
	

};
