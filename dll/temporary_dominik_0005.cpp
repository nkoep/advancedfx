#include "windows.h" // we need access to virtualprotect etc.

// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

#include "detours.h"

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s* ppmove;

// client!HUD_ProcessPlayerState

typedef cl_entity_s *(* GetLocalPlayer_t)( void );

typedef void (* HUD_ProcessPlayerState_t)( struct entity_state_s *dst, const struct entity_state_s *src );

HUD_ProcessPlayerState_t detoured_HUD_ProcessPlayerState = NULL;

bool g_bInstalled_0005 = false;

REGISTER_DEBUGCVAR(fana0005_blockall, "-1", 0);
REGISTER_DEBUGCVAR(fana0005_iuser1, "-1", 0); // observer mode
REGISTER_DEBUGCVAR(fana0005_iuser2, "-1", 0); // target 1
REGISTER_DEBUGCVAR(fana0005_iuser3, "-1", 0); // target 2
REGISTER_DEBUGCVAR(fana0005_efnodraw, "-1", 0); // force EF_NODRAW effect

void hooking_HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src )
{

	cl_entity_t *player = pEngfuncs->GetLocalPlayer();	// Get the local player's index
	if ( dst->number == player->index )
	{
		if(fana0005_blockall->value != 1.0f)
		{
			entity_state_s ns = *src;

			if(fana0005_iuser1->value != -1) ns.iuser1 = fana0005_iuser1->value;
			if(fana0005_iuser2->value != -1) ns.iuser2 = fana0005_iuser2->value;
			if(fana0005_iuser3->value != -1) ns.iuser3 = fana0005_iuser3->value;
			if(fana0005_efnodraw->value != -1) ns.effects |= EF_NODRAW;
			detoured_HUD_ProcessPlayerState(dst,&ns);
		}
	}
	else
		detoured_HUD_ProcessPlayerState(dst,src);
}

bool Install_0005()
{
	if(g_bInstalled_0005)
		return true;

	HMODULE hm = GetModuleHandle("client.dll");
	FARPROC fp = GetProcAddress(hm,"HUD_ProcessPlayerState");

	if(hm && fp)
	{
		detoured_HUD_ProcessPlayerState = (HUD_ProcessPlayerState_t)DetourApply((BYTE *)fp, (BYTE *)hooking_HUD_ProcessPlayerState,0x06);
		g_bInstalled_0005 = true;
	}

	return g_bInstalled_0005;
}

REGISTER_DEBUGCMD_FUNC(fana0005_install)
{
	if(Install_0005())
		pEngfuncs->Con_Printf("[OK] Hook installed.\n");
	else
		pEngfuncs->Con_Printf("[ERROR] Hook installation failed.\n");
}

/////////////////


typedef void (* Demo_ReadBuffer_t)( int size, unsigned char *buffer );

Demo_ReadBuffer_t detoured_Lupus0005 = NULL;

bool g_bInstalledLupus0005 = false;

REGISTER_DEBUGCVAR(lupus0005_enable, "0", 0);

void hookingLupus0005( int size, unsigned char *buffer )
{
	int id = *( int * )buffer;

//	pEngfuncs->Con_DPrintf("Demo_ReadBuffer with id: %u\n", id);

	if(lupus0005_enable->value != 1.0f || 1 != id)
		detoured_Lupus0005(size, buffer);
}


bool Install_Lupus0005()
{
	if(g_bInstalledLupus0005)
		return true;

	HMODULE hm = GetModuleHandle("client.dll");
	FARPROC fp = GetProcAddress(hm,"Demo_ReadBuffer");

	if(hm && fp)
	{
		detoured_Lupus0005 = (Demo_ReadBuffer_t)DetourApply((BYTE *)fp, (BYTE *)hookingLupus0005,0x06);
		g_bInstalledLupus0005 = true;
	}

	return g_bInstalledLupus0005;
}

REGISTER_DEBUGCMD_FUNC(lupus0005_install)
{
	if(Install_Lupus0005())
		pEngfuncs->Con_Printf("[OK] Hook installed.\n");
	else
		pEngfuncs->Con_Printf("[ERROR] Hook installation failed.\n");
}