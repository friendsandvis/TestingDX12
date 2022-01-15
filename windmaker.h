#pragma once

#include<Windows.h>
#include<assert.h>
#include"Renderable.h"



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
