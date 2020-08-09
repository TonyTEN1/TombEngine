#include "framework.h"
#include "winmain.h"
#include "init.h"
#include "resource.h"
#include "draw.h"
#include "sound.h"
#include "inventory.h"
#include "control.h"
#include "gameflow.h"
#include "savegame.h"
#include "level.h"
#include "configuration.h"
#include "Renderer11.h"
using namespace T5M::Renderer;
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
int App_Unk00D9ABFD;
extern int IsLevelLoading;
extern GameFlow* g_GameFlow;
extern GameScript* g_GameScript;
extern GameConfiguration g_Configuration;
DWORD DebugConsoleThreadID;
DWORD MainThreadID;
bool BlockAllInput = true;

int lua_exception_handler(lua_State* L, sol::optional<const exception&> maybe_exception, sol::string_view description)
{
	return luaL_error(L, description.data());
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
			g_Renderer.toggleFullScreen();
			ResumeThread((HANDLE)ThreadHandle);

			if (g_Renderer.isFullsScreen())
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

void HandleScriptMessage(WPARAM wParam)
{
	string ErrorMessage;
	string message = *(string*)(wParam);
	bool status;

	//check whether line starts with "lua "
	if (message.find("lua ") == 0) {
		string scriptSubstring = message.substr(4);
		status = g_GameScript->ExecuteScript(scriptSubstring, ErrorMessage);
	}
	else {
		status = g_GameScript->ExecuteString(message, ErrorMessage);
	}
	if (!status)
		cout << ErrorMessage << endl;
}

LRESULT CALLBACK WinAppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_USER + 0)
	{
		HandleScriptMessage(wParam);
		return 0;
	}

	// Disables ALT + SPACE
	if (msg == WM_SYSCOMMAND && wParam == SC_KEYMENU)
		return 0;

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
			BlockAllInput = false;
			if (!Debug)
				ResumeThread((HANDLE)ThreadHandle);

			App_Unk00D9ABFD = 0;
			return 0;
		}
	}
	else
	{
		//DB_Log(6, "WM_INACTIVE");
		//DB_Log(5, "HangGameThread");
		BlockAllInput = true;
		App_Unk00D9ABFD = 1;

		if (!Debug)
			SuspendThread((HANDLE)ThreadHandle);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int RetVal;
	int n;

	// Process the command line
	bool setup = false;

	//TrLevel* level = new TrLevel(string("Data\\andrea1.t5m"));
	//level->Load();
	
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

	_CrtSetReportMode(0, 2);
	_CrtSetDbgFlag(-1);
	
	// Initialise the new scripting system
	sol::state luaState;
	luaState.open_libraries(sol::lib::base);
	luaState.set_exception_handler(lua_exception_handler);

	g_GameFlow = new GameFlow(&luaState);
	LoadScript();

	g_GameScript = new GameScript(&luaState);

	// Initialise chunks for savegames
	SaveGame::Start();

	INITCOMMONCONTROLSEX commCtrlInit;
	commCtrlInit.dwSize = sizeof(INITCOMMONCONTROLSEX);
	commCtrlInit.dwICC = ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&commCtrlInit);

	// Initialise main window
	App.hInstance = hInstance;
	App.WindowClass.hIcon = NULL;
	App.WindowClass.lpszMenuName = NULL;
	App.WindowClass.lpszClassName = "TR5Main";
	App.WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	App.WindowClass.hInstance = hInstance;
	App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	App.WindowClass.lpfnWndProc = WinAppProc;
	App.WindowClass.cbClsExtra = 0;
	App.WindowClass.cbWndExtra = 0;
	App.WindowClass.hCursor = LoadCursor(App.hInstance, IDC_ARROW);

	if (!RegisterClass(&App.WindowClass))
	{
		printf("Unable To Register Window Class\n");
		return false;
	}

	// Create the renderer and enumerate adapters and video modes
	g_Renderer.Create();
	g_Renderer.EnumerateVideoModes();

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
		"TR5Main",
		g_GameFlow->GetSettings()->WindowTitle.c_str(),
		WS_POPUP,
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
		printf("Unable To Create Window: %d\n", GetLastError());
		return false;
	}

	WindowsHandle = App.WindowHandle;
	// Initialise the renderer
	g_Renderer.Initialise(g_Configuration.Width, g_Configuration.Height, g_Configuration.RefreshRate, g_Configuration.Windowed, App.WindowHandle);

	// Initialize audio
	if (g_Configuration.EnableSound)	
		Sound_Init();

	// Initialise the new inventory
	//g_Inventory = Inventory();
	
	App.bNoFocus = false;
	App.isInScene = false;

	UpdateWindow(WindowsHandle);
	ShowWindow(WindowsHandle, nShowCmd);

	//Create debug script terminal
	if (Debug)
	{
		MainThreadID = GetWindowThreadProcessId(WindowsHandle, NULL);
		AllocConsole();
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		DWORD consoleModeIn;
		int hCrt = _open_osfhandle((long)handle_in, _O_BINARY);
		FILE* hf_in = _fdopen(hCrt, "r");
		setvbuf(hf_in, NULL, _IONBF, 512);
		GetConsoleMode(handle_in, &consoleModeIn);
		consoleModeIn = consoleModeIn | ENABLE_LINE_INPUT;
		SetConsoleMode(handle_in, consoleModeIn);
		freopen_s(&hf_in, "CONIN$", "r", stdin);

		HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
		FILE* COutputHandle = _fdopen(SystemOutput, "w");
		freopen_s(&COutputHandle, "CONOUT$", "w", stdout);

		LPTHREAD_START_ROUTINE readConsoleLoop = [](LPVOID params) -> DWORD {
			DWORD read;
			CHAR buffer[4096];
			while (true)
			{
				BOOL success = ReadFile(params, &buffer, 4096, &read, NULL);
				if (success && read > 2)
				{
					//Only send the actual written message minus \r\n
					string msg(buffer, read-2);
					SendMessage(WindowsHandle, WM_USER, (WPARAM)&msg, NULL);
				}
			};
			return 0;
		};
		CreateThread(NULL, 0, readConsoleLoop, handle_in, 0, &DebugConsoleThreadID);
	}

	SetCursor(NULL);
	ShowCursor(FALSE);
	hAccTable = LoadAccelerators(hInstance, (LPCSTR)0x65);

	//g_Renderer->Test();

	SoundActive = false;
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
	DestroyAcceleratorTable(hAccTable);

	if (g_Configuration.EnableSound)
		Sound_DeInit();
	
	delete g_GameScript;
	delete g_GameFlow;

	SaveGame::End();
}