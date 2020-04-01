// dllmain.cpp: definisce il punto di ingresso per l'applicazione DLL.
#include <Windows.h>
#include <stdio.h>
#include "Global\global.h"
#include "Specific\setup.h"
#include "Game\anim.h"
#include "Game\box.h"
#include "Game\items.h"
#include "Game\deltapak.h"
#include "Game\lot.h"
#include "Game\collide.h"
#include "Game\larafire.h"
#include "Game\Camera.h"
#include "Game\hair.h"
#include "Game\Lara.h"
#include "Game\text.h"
#include "Game\healt.h"
#include "Game\pickup.h"
#include "Game\sound.h"
#include "Game\control.h"
#include "Game\effect2.h"
#include "Game\lara1gun.h"
#include "Game\spotcam.h"
#include "Game\objects.h"
#include "Game\door.h"
#include "Game\switch.h"
#include "Game\lion.h"
#include "Game\people.h"
#include "Game\effects.h"
#include "Game\sphere.h"
#include "Game\tomb4fx.h"

#include "Specific\game.h"
#include "Specific\roomload.h"
#include "Specific\patch.h"
#include "Specific\input.h"
#include "Specific\winmain.h"

#include "Game\draw.h"

HMODULE g_DllHandle;

// Dummy functions for replacing binkw32.dll
extern "C" __declspec(dllexport) int _BinkCopyToBufferX28()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkCloseX4()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkNextFrameX4()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkWaitX4()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkDoFrameX4()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkDDSurfaceTypeX4()
{
	return 0;
}
extern "C" __declspec(dllexport) int _BinkOpenX8()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkSetSoundSystemX8()
{
	return 0;
}

extern "C" __declspec(dllexport) int _BinkOpenDirectSoundX4()
{
	return 0;
}

// Test function
int __cdecl PlayFMV()
{
	return 0;
}

// Injection function
void InjectAll()
{
	INJECT(0x004A75A0, PlayFMV);

	Inject_Malloc();
	Inject_Box();
	Inject_Items();
	Inject_Lot();
	Inject_DeltaPak();
	Inject_Setup();
	Inject_RoomLoad();
	Inject_Collide();
	Inject_Draw();
	Inject_LaraFire();
	Inject_Input();
	Inject_WinMain();
	Inject_Sound();
	Inject_Camera();
	Inject_Lara();
	Inject_Hair();
	Inject_Text();
	Inject_Healt();
	Inject_Pickup();
	Inject_Control();
	Inject_Effect2();
	Inject_Lara1Gun();
	Inject_Spotcam();
	Inject_Objects();
	Inject_Door();
	Inject_Switch();
	Inject_Lion();
	Inject_People();
	Inject_Tomb4FX();
	Inject_Effects();
	Inject_Sphere();
}

//#define ORIGINAL_CODE

// The DLL entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		// We are ready!
		printf("TR5Main DLL attached correctly.\n");

		g_DllHandle = hModule;

#ifndef ORIGINAL_CODE
		// Patch the game code on the fly for some advanced features
		PatchGame();
		
		// Inject all decompiled function
		InjectAll();
#endif

		break;
    case DLL_THREAD_ATTACH:
		break;

    case DLL_THREAD_DETACH:
		break;

    case DLL_PROCESS_DETACH:	
        break;
    }
    return TRUE;
}

