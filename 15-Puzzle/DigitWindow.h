#pragma once

#include <Windows.h>

class CMainWindow;

class CDigitWindow {
	friend class CMainWindow;

public:
	CDigitWindow() = default;
	~CDigitWindow();

	static bool Register();
	bool Create( HWND _parentWindowHandle, int left, int top, int width, int heigth, CMainWindow* _parent, int _row, int _column );
	void Show( int windowShowMode );

protected:
	void OnDestroy();
	void OnCreate();
	void OnNCCreate( const HWND handle );
	void OnPaint();
	void OnLButtonDown();

private:
	HWND windowHandle{};
	HWND parentWindowHandle{};
	CMainWindow* parent{ nullptr };
	int row{ 0 };
	int column{ 0 };

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
	static CDigitWindow* getThis( HWND handle );
};
