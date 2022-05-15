#include "framework.h"
#include "Specific/winmain.h"

#include <CommCtrl.h>
#include <process.h>
#include <iostream>
#include "Game/control/control.h"
#include "Game/savegame.h"
#include "Renderer/Renderer11.h"
#include "resource.h"
#include "Sound/sound.h"
#include "Specific/level.h"
#include "Specific/configuration.h"
#include "LanguageScript.h"
#include "ScriptInterfaceState.h"

using namespace TEN::Renderer;
using std::exception;
using std::string;
using std::cout;
using std::endl;

WINAPP App;
unsigned int ThreadID;
uintptr_t ThreadHandle;
HACCEL hAccTable;
byte receivedWmClose = false;
bool Debug = false;
HWND WindowsHandle;
DWORD MainThreadID;

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#if _DEBUG
string commit;
#endif

Vector2Int GetScreenResolution()
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	Vector2Int resolution;
	resolution.x = desktop.right;
	resolution.y = desktop.bottom;
	return resolution;
}

std::vector<Vector2Int> GetAllSupportedScreenResolutions()
{
	std::vector<Vector2Int> result;

	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);
	for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++)
	{
		bool add = true;
		for (auto m : result)
		{
			if (m.x == dm.dmPelsWidth && m.y == dm.dmPelsHeight)
			{
				add = false;
				break;
			}
		}
		if (add)
		{
			Vector2Int resolution;
			resolution.x = dm.dmPelsWidth;
			resolution.y = dm.dmPelsHeight;
			result.push_back(resolution);
		}
	}

	std::sort(
		result.begin(),
		result.end(),
		[](Vector2Int& a, Vector2Int& b)
		{
			if (a.x == b.x)
			{
				return (a.y < b.y);
			}
			else
			{
				return (a.x < b.x);
			}
		}
	);

	return result;
}

void WinProcMsg()
{
	MSG Msg;

	do
	{
		GetMessage(&Msg, 0, 0, 0);
		if (!TranslateAccelerator(WindowsHandle, hAccTable, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	while (!ThreadEnded && Msg.message != WM_QUIT);
}

void CALLBACK HandleWmCommand(unsigned short wParam)
{
	if (wParam == 8)
	{
		if (!IsLevelLoading)
		{
			SuspendThread((HANDLE)ThreadHandle);
			g_Renderer.ToggleFullScreen();
			ResumeThread((HANDLE)ThreadHandle);

			if (g_Renderer.IsFullsScreen())
			{
				SetCursor(0);
				ShowCursor(false);
			}
			else
			{
				SetCursor(LoadCursorA(App.hInstance, (LPCSTR)0x68));
				ShowCursor(true);
			}
		}
	}
}

LRESULT CALLBACK WinAppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Disables ALT + SPACE
	if (msg == WM_SYSCOMMAND && wParam == SC_KEYMENU)
	{
		return 0;
	}

	// Manually handle ALT + ENTER toggle fullscreen
	if ((msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
		&& wParam == VK_RETURN
		&& (HIWORD(lParam) & KF_ALTDOWN))
	{
		g_Renderer.ToggleFullScreen();
		return 0;
	}

	if (msg > 0x10)
	{
		if (msg == WM_COMMAND)
			HandleWmCommand((unsigned short)wParam);

		return DefWindowProcA(hWnd, msg, wParam, (LPARAM)lParam);
	}

	if (msg == WM_CLOSE)
	{
		receivedWmClose = true;
		PostQuitMessage(0);
		DoTheGame = false;

		return DefWindowProcA(hWnd, 0x10u, wParam, (LPARAM)lParam);
	}

	if (msg != WM_ACTIVATE)
		return DefWindowProcA(hWnd, msg, wParam, (LPARAM)lParam);

	if (receivedWmClose)
	{
		return DefWindowProcA(hWnd, msg, wParam, (LPARAM)lParam);
	}

	if ((short)wParam)
	{
		if ((signed int)(unsigned short)wParam > 0 && (signed int)(unsigned short)wParam <= 2)
		{
			//DB_Log(6, "WM_ACTIVE");
			if (!Debug)
				ResumeThread((HANDLE)ThreadHandle);

			return 0;
		}
	}
	else
	{
		//DB_Log(6, "WM_INACTIVE");
		//DB_Log(5, "HangGameThread");
		if (!Debug)
			SuspendThread((HANDLE)ThreadHandle);
	}

	return 0;
}

int main() 
{
	return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), SW_SHOW);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int RetVal;
	int n;

	// Process the command line
	bool setup = false;

	LPWSTR* argv;
	int argc;
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	for (int i = 1; i < argc; i++)
	{
		if (wcscmp(argv[i], L"/setup") == 0)
			setup = true;
		if (wcscmp(argv[i], L"/debug") == 0)
			Debug = true;
	}
	LocalFree(argv);

	// Clear Application Structure
	memset(&App, 0, sizeof(WINAPP));
	
	InitTENLog();

	// Collect numbered tracks
	EnumerateLegacyTracks();

	// Initialise the new scripting system
	ScriptInterfaceState::Init();

	try 
	{
		// todo make sure the right objects are deleted at the end
		g_GameFlow = ScriptInterfaceState::CreateFlow();
		g_GameFlow->LoadFlowScript();
		g_GameScript = ScriptInterfaceState::CreateGame();
		g_GameScriptEntities = ScriptInterfaceState::CreateObjectsHandler();
		g_GameStringsHandler = ScriptInterfaceState::CreateStringsHandler();
	}
	catch (TENScriptException const& e)
	{
		std::string msg = std::string{ "An unrecoverable error occurred in " } + __func__ + ": " + e.what();
		TENLog(msg, LogLevel::Error, LogConfig::All);
		ShutdownTENLog();
		return 0;
	}

	INITCOMMONCONTROLSEX commCtrlInit;
	commCtrlInit.dwSize = sizeof(INITCOMMONCONTROLSEX);
	commCtrlInit.dwICC = ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&commCtrlInit);

	// Initialise main window
	App.hInstance = hInstance;
	App.WindowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	App.WindowClass.lpszMenuName = NULL;
	App.WindowClass.lpszClassName = "TombEngine";
	App.WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	App.WindowClass.hInstance = hInstance;
	App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	App.WindowClass.lpfnWndProc = WinAppProc;
	App.WindowClass.cbClsExtra = 0;
	App.WindowClass.cbWndExtra = 0;
	App.WindowClass.hCursor = LoadCursor(App.hInstance, IDC_ARROW);

	if (!RegisterClass(&App.WindowClass))
	{
		TENLog("Unable To Register Window Class", LogLevel::Error);
		return 0;
	}

	// Create the renderer and enumerate adapters and video modes
	g_Renderer.Create();

	// Load configuration and optionally show the setup dialog
	InitDefaultConfiguration();
	if (setup || !LoadConfiguration())
	{
		if (!SetupDialog())
		{
			WinClose();
			return 0;
		}

		LoadConfiguration();
	}

	RECT Rect;
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = g_Configuration.Width;
	Rect.bottom = g_Configuration.Height;
	AdjustWindowRect(&Rect, WS_CAPTION, false);

	App.WindowHandle = CreateWindowEx(
		0,
		"TombEngine",
		g_GameFlow->GetString(STRING_WINDOW_TITLE),
		WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT, // TODO: change this to center of screen !!!
		CW_USEDEFAULT,
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		NULL,
		NULL,
		App.hInstance,
		NULL
	);

	if (!App.WindowHandle)
	{
		TENLog("Unable To Create Window" + std::to_string(GetLastError()), LogLevel::Error);
		return false;
	}

	WindowsHandle = App.WindowHandle;
	// Initialise the renderer
	g_Renderer.Initialise(g_Configuration.Width, g_Configuration.Height, g_Configuration.Windowed, App.WindowHandle);

	// Initialize audio
	if (g_Configuration.EnableSound)	
		Sound_Init();
	
	App.bNoFocus = false;
	App.isInScene = false;

	UpdateWindow(WindowsHandle);
	ShowWindow(WindowsHandle, nShowCmd);

	SetCursor(NULL);
	ShowCursor(FALSE);
	hAccTable = LoadAccelerators(hInstance, (LPCSTR)0x65);

	//g_Renderer->Test();

	DoTheGame = true;

	ThreadEnded = false;
	ThreadHandle = BeginThread(GameMain, ThreadID);
	WinProcMsg();
	ThreadEnded = true;

	while (DoTheGame);
	
	WinClose();
	exit(EXIT_SUCCESS);
}

void WinClose()
{
	WaitForSingleObject((HANDLE)ThreadHandle, 5000);

	DestroyAcceleratorTable(hAccTable);

	if (g_Configuration.EnableSound)
		Sound_DeInit();
	
	delete g_GameScript;
	g_GameScript = nullptr;

	delete g_GameFlow;
	g_GameFlow = nullptr;

	delete g_GameScriptEntities;
	g_GameScriptEntities = nullptr;

	delete g_GameStringsHandler;
	g_GameStringsHandler = nullptr;

	ShutdownTENLog();
}