#include "stdafx.h"

// BEGIN HLSDK includes
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/engine/cdll_int.h>
#include <hlsdk/multiplayer/common/cvardef.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"
#include "hl_addresses.h"

#include <hooks/shared/detours.h>

extern cl_enginefuncs_s *pEngfuncs;


#define TYPE_ZOOM 1
#define DTOURSZ_Demo_ReadBuffer 0x06

typedef void (* Demo_ReadBuffer_t)( int size, unsigned char *buffer );

Demo_ReadBuffer_t detoured_Demo_ReadBuffer = NULL;

enum DemoZoomMode {
	DZM_DEFAULT,
	DZM_BLOCK,
	DZM_SET
} g_DemoZoomMode = DZM_DEFAULT;

float g_DemoZoomMode_SetValue;

void touring_Demo_ReadBuffer( int size, unsigned char *buffer )
{
	if(DZM_DEFAULT != g_DemoZoomMode) {

		int id = *(int *)buffer;

		if(TYPE_ZOOM == id) {
			if(DZM_BLOCK == g_DemoZoomMode)
				return;
			
			*(float *)(&buffer[sizeof(int)]) = g_DemoZoomMode_SetValue;
		}
	}
	detoured_Demo_ReadBuffer(size, buffer);
}

bool detour_Demo_ReadBuffer()
{
	static bool bFirstRun = true;
	static bool bSuccess = false;

	if(bFirstRun) {
		bFirstRun = false;

		HMODULE hm = GetModuleHandle("client.dll");
		FARPROC fp = GetProcAddress(hm,"Demo_ReadBuffer");

		if(hm && fp)
		{
			detoured_Demo_ReadBuffer = (Demo_ReadBuffer_t)DetourApply((BYTE *)fp, (BYTE *)touring_Demo_ReadBuffer, DTOURSZ_Demo_ReadBuffer);
			bSuccess = true;
		}
	}

	return bSuccess;
}

REGISTER_DEBUGCMD_FUNC(demozoom)
{
	if(!detour_Demo_ReadBuffer()) {
		pEngfuncs->Con_Printf("Error.\n");
		return;
	}

	int argc = pEngfuncs->Cmd_Argc();

	if(2 <= argc) {
		if(!stricmp("block",pEngfuncs->Cmd_Argv(1))) {
			g_DemoZoomMode = DZM_BLOCK;
			return;
		}
		else if(!stricmp("default",pEngfuncs->Cmd_Argv(1))) {
			g_DemoZoomMode = DZM_DEFAULT;
			return;
		}
		else if(3 <= argc && !stricmp("set",pEngfuncs->Cmd_Argv(1))) {
			g_DemoZoomMode = DZM_SET;
			g_DemoZoomMode_SetValue = atof(pEngfuncs->Cmd_Argv(2));
			return;
		}
	}

	pEngfuncs->Con_Printf(
		"Commands:\n"
		"__mirv_demozoom block - blocks demozoom demo events.\n"
		"__mirv_demozoom ride <fov> - overrides the event with a float value.\n"
		"__mirv_demozoom default - restores the game's default behaviour.\n"
		"Current mode: "
	);

	switch(g_DemoZoomMode) {
	case DZM_BLOCK:
		pEngfuncs->Con_Printf("block\n");
		break;
	case DZM_SET:
		pEngfuncs->Con_Printf("ride %f\n", g_DemoZoomMode_SetValue);
		break;
	default:
		pEngfuncs->Con_Printf("default\n");
	}
}