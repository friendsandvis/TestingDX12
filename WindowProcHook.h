#pragma once
/*
* this class is simply an interface used to hook into the running window's procedure.
*/
#include<Windows.h>
class WindowProcHook
{
public:
	WindowProcHook();
	~WindowProcHook();
	virtual void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {};
private:

};

