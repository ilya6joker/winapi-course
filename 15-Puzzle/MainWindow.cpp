#include "MainWindow.h"
#include "PuzzleSolver.h"

#include "resource.h"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>

CMainWindow::~CMainWindow()
{
	if( windowHandle != NULL ) {
		CloseHandle( windowHandle );
	}
}

bool CMainWindow::Register()
{
	WNDCLASSEX windowClass{};

	windowClass.cbSize = sizeof( windowClass );
	windowClass.lpfnWndProc = windowProc;
	windowClass.hInstance = GetModuleHandle( NULL );
	windowClass.lpszClassName = L"15-Puzzle";
	windowClass.lpszMenuName = MAKEINTRESOURCE( IDR_MENU1 );

	return RegisterClassEx( &windowClass ) != 0;
}

bool CMainWindow::Create()
{
	return CreateWindowEx( 0, L"15-Puzzle", L"15-Puzzle", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 240, NULL, NULL, GetModuleHandle( NULL ), static_cast<LPVOID>( this ) ) != NULL;
}

void CMainWindow::Show( int windowShowMode )
{
	ShowWindow( windowHandle, windowShowMode );
	for( int i = 0; i < degree; ++i ) {
		for( int j = 0; j < degree; ++j ) {
			digitWindows[i][j].Show( windowShowMode );
		}
	}
}

void CMainWindow::OnDestroy()
{
	PostQuitMessage( 0 );
}

void CMainWindow::OnCreate()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = ( rectangle.right - rectangle.left ) / degree;
	const int height = ( rectangle.bottom - rectangle.top ) / degree;

	auto permutation = getInitialState( degree );

	CDigitWindow::Register();
	for( int i = 0; i < degree; ++i ) {
		for( int j = 0; j < degree; ++j ) {
			const int left = rectangle.left + j * width;
			const int top = rectangle.top + i * height;
			const int index = i * degree + j;

			digits[i][j] = permutation[index];
			digitWindows[i][j].Create( windowHandle, left, top, width, height, this, i, j );
		}
	}

	SetFocus( digitWindows[degree - 1][degree - 1].windowHandle );
}

void CMainWindow::OnNCCreate( const HWND handle )
{
	windowHandle = handle;

	RECT rectangle{};
	GetWindowRect( windowHandle, &rectangle );

	const int heigth = rectangle.bottom - rectangle.top;
	const int width = rectangle.right - rectangle.left;
	const int size = min( heigth, width );

	SetWindowPos( windowHandle, HWND_TOP, rectangle.left, rectangle.top, size, size, SWP_SHOWWINDOW );
}

void CMainWindow::OnSize()
{
	RECT rectangle{};
	GetClientRect( windowHandle, &rectangle );

	const int width = ( rectangle.right - rectangle.left ) / degree;
	const int height = ( rectangle.bottom - rectangle.top ) / degree;

	for( int i = 0; i < degree; ++i ) {
		for( int j = 0; j < degree; ++j ) {
			const int left = rectangle.left + j * width;
			const int top = rectangle.top + i * height;

			SetWindowPos( digitWindows[i][j].windowHandle, HWND_TOP, left, top, width, height, SWP_SHOWWINDOW );
		}
	}

	InvalidateRect( windowHandle, &rectangle, TRUE );
}

void CMainWindow::OnSizing( WPARAM wParam, RECT* rect )
{
	const int heigth = max( 160, min( 320, rect->bottom - rect->top ) );
	const int width = max( 160, min( 320, rect->right - rect->left ) );
	const int size = min( heigth, width );

	switch( wParam ) {
		case WMSZ_BOTTOM:
			rect->bottom = rect->top + width;
			rect->right = rect->left + heigth;
			break;
		case WMSZ_TOP:
			rect->top = rect->bottom - width;
			rect->right = rect->left + heigth;
			break;
		case WMSZ_LEFT:
			rect->bottom = rect->top + width;
			rect->left = rect->right - heigth;
			break;
		case WMSZ_RIGHT:
			rect->bottom = rect->top + width;
			rect->right = rect->left + heigth;
			break;
		case WMSZ_BOTTOMLEFT:
			rect->bottom = rect->top + size;
			rect->left = rect->right - size;
			break;
		case WMSZ_BOTTOMRIGHT:
			rect->bottom = rect->top + size;
			rect->right = rect->left + size;
			break;
		case WMSZ_TOPLEFT:
			rect->top = rect->bottom - size;
			rect->left = rect->right - size;
			break;
		case WMSZ_TOPRIGHT:
			rect->top = rect->bottom - size;
			rect->right = rect->left + size;
			break;
	}
}

void CMainWindow::OnCommand( WPARAM wParam )
{
	if( HIWORD( wParam ) == 0 && LOWORD( wParam ) == ID_40001 ) {
		auto hintMove = CPuzzleSolver::GetHint( getVector( digits ) );

		std::wstring hint = L"���������� ����������� �������� ";
		hint += hintMove.RowDifference == -1 ? L"�����" : hintMove.RowDifference == 1 ? L"����" : hintMove.ColumnDifference == -1 ? L"�����" : L"������";
		hint += L"...";

		MessageBoxW( windowHandle, hint.c_str(), L"���������", MB_OK );
	}
}

LRESULT CMainWindow::windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_DESTROY:
		{
			getThis( handle )->OnDestroy();
			break;
		}
		case WM_CREATE:
		{
			getThis( handle )->OnCreate();
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_NCCREATE:
		{
			auto createStructPtr = reinterpret_cast< CREATESTRUCT* >( lParam );
			auto createParams = reinterpret_cast<LONG>( createStructPtr->lpCreateParams );

			SetLastError( 0 );
			if( SetWindowLong( handle, GWLP_USERDATA, createParams ) == 0 && GetLastError() != 0 ) {
				MessageBoxW( 0, std::to_wstring( GetLastError() ).c_str(), L"Error", NULL );
				return FALSE;
			}

			auto actualThis = reinterpret_cast<CMainWindow*>( createParams );
			actualThis->OnNCCreate( handle );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		case WM_SIZE:
		{
			getThis( handle )->OnSize();
			return TRUE;
		}
		case WM_SIZING:
		{
			getThis( handle )->OnSizing( wParam, reinterpret_cast<RECT*>( lParam ) );
			return 0;
		}
		case WM_COMMAND:
		{
			getThis( handle )->OnCommand( wParam );
			return DefWindowProc( handle, message, wParam, lParam );
		}
		default:
			return DefWindowProc( handle, message, wParam, lParam );
	}
	return 0;
}

CMainWindow* CMainWindow::getThis( HWND handle )
{
	return reinterpret_cast<CMainWindow*>( GetWindowLong( handle, GWLP_USERDATA ) );
}

std::vector<int> CMainWindow::getRandomPermutation( int length )
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist( 1, length );

	std::vector<int> permutation{};

	for( int i = 0; i < length - 1; ++i ) {
		int value = 0;
		do {
			value = dist( rd );
		} while( std::find( permutation.begin(), permutation.end(), value ) != permutation.end() );
		permutation.push_back( value );
	}

	int sum = std::accumulate( permutation.begin(), permutation.end(), 0 );
	int totalSum = length * ( length + 1 ) / 2;
	permutation.push_back( totalSum - sum );

	return permutation;
}

bool CMainWindow::isSolvableState( const std::vector<int>& permutation, int degree )
{
	int sum = 0;
	for( auto i = 0; i < permutation.size(); ++i ) {
		for( auto j = i + 1; j < permutation.size(); ++j ) {
			if( permutation[i] > permutation[j] ) {
				++sum;
			}
		}
	}
	return ( sum + degree ) % 2 == 0;
}

std::vector<int> CMainWindow::getInitialState( int degree )
{
	std::vector<int> permutation{};
	int sum = 0;

	do {
		permutation = getRandomPermutation( degree * degree - 1 );
	} while( !isSolvableState( permutation, degree ) || CPuzzleSolver::IsFinishPositions( permutation ) );

	permutation.push_back( 0 );

	return permutation;
}

std::vector<int> CMainWindow::getVector( const std::array<std::array<int, degree>, degree>& array )
{
	std::vector<int> positions{};
	for( auto& row : array ) {
		for( auto& cell : row ) {
			positions.push_back( cell );
		}
	}
	return positions;
}
