#pragma once

#include "types.h"

// BOX.CPP
#define Legacy_CreatureActive ((int (__cdecl*)(short)) 0x00408630)
#define Legacy_CreatureUnderwater ((void (__cdecl*)(ITEM_INFO*, int)) 0x0040B400)
#define Legacy_CreatureFloat ((int (__cdecl*)(short)) 0x0040B2C0)
#define Legacy_CreatureDie ((void (__cdecl*)(short, int)) 0x0040A090)
#define Legacy_CreatureAIInfo ((void (__cdecl*)(ITEM_INFO*, AI_INFO*)) 0x004086C0)
#define Legacy_CreatureMood ((void (__cdecl*)(ITEM_INFO*, AI_INFO*, int)) 0x00409370)
#define Legacy_GetCreatureMood ((void (__cdecl*)(ITEM_INFO*, AI_INFO*, int)) 0x004090A0)
#define Legacy_CreatureEffect ((short (__cdecl*)(ITEM_INFO*, BITE_INFO*, short(*)(int, int, int, short, short, short))) 0x0040B4D0)
#define Legacy_CreatureEffect2 ((short (__cdecl*)(ITEM_INFO*, BITE_INFO*, short, short, short(*)(int, int, int, short, short, short))) 0x0040B550)
#define Legacy_CreatureTilt ((void (__cdecl*)(ITEM_INFO*, short)) 0x0040B1B0)
#define Legacy_CreatureJoint ((void (__cdecl*)(ITEM_INFO*, short, short)) 0x0040B240)
#define Legacy_CreatureAnimation ((void (__cdecl*)(short, short, short)) 0x0040A1D0)
#define Legacy_InitialiseCreature ((void (__cdecl*)(short)) 0x00408550)
#define Legacy_CreatureKill ((void (__cdecl*)(ITEM_INFO*, int, int, int)) 0x0040B820)
#define Legacy_AlertAllGuards ((void (__cdecl*)(short)) 0x0040BA70)
#define Legacy_AlertNearbyGuards ((void (__cdecl*)(ITEM_INFO*)) 0x0040BB10)
#define Legacy_MoveCreature3DPos ((int(__cdecl*)(PHD_3DPOS*, PHD_3DPOS*, int, short, short)) 0x0040C460)
#define Legacy_CalculateTarget ((int(__cdecl*)(PHD_VECTOR*, ITEM_INFO*, LOT_INFO*)) 0x004098B0)
#define Legacy_ValidBox ((int(__cdecl*)(ITEM_INFO*, short, short)) 0x00408FD0)
#define Legacy_StalkBox ((int(__cdecl*)(ITEM_INFO*, ITEM_INFO*, short)) 0x00409770)
#define Legacy_TargetBox ((int(__cdecl*)(LOT_INFO*, short)) 0x00408E20)
#define Legacy_EscapeBox ((int(__cdecl*)(ITEM_INFO*, ITEM_INFO*, short)) 0x00408EF0)
#define Legacy_BadFloor ((int(__cdecl*)(int, int, int, int, int, short, LOT_INFO*)) 0x00409FB0)
#define Legacy_CreatureCreature ((int(__cdecl*)(short)) 0x00409E20)
#define Legacy_DropBaddyPickups ((int(__cdecl*)(ITEM_INFO*)) 0x0040C5A0)
#define Legacy_CreatureCollision ((void (__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x004124E0)
#define Legacy_CreatureTurn ((short (__cdecl*)(ITEM_INFO*, short)) 0x0040AE90)
#define Legacy_AIGuard ((short(__cdecl*)(CREATURE_INFO*)) 0x0040BBE0)
#define Legacy_GetAITarget ((void(__cdecl*)(CREATURE_INFO*)) 0x0040BCC0)
#define Legacy_FindAIObject ((int(__cdecl*)(CREATURE_INFO*,short)) 0x0040C070)
#define Legacy_SameZoneAIObject ((int(__cdecl*)(CREATURE_INFO*,short)) 0x0040C460)

// CAMERA.CPP
#define Legacy_InitialiseCamera ((void (__cdecl*)()) 0x0040C690)
#define Legacy_MoveCamera ((int(__cdecl*)(GAME_VECTOR*,int)) 0x0040C7A0)
#define Legacy_ChaseCamera ((int(__cdecl*)(ITEM_INFO*)) 0x0040D150)
#define Legacy_CombatCamera ((int(__cdecl*)(ITEM_INFO*)) 0x0040D640)
#define Legacy_LookCamera ((void(__cdecl*)(ITEM_INFO*)) 0x0040DC10)
#define Legacy_FixedCamera ((void(__cdecl*)()) 0x0040E890)
#define Legacy_CalculateCamera ((void(__cdecl*)()) 0x0040ED30)
#define Legacy_BinocularCamera ((void(__cdecl*)()) 0x0040FC20)
#define Legacy_RefreshCamera ((void(__cdecl*)(short,short*)) 0x004165E0)
#define Legacy_CameraCollisionBounds ((int (__cdecl*)(GAME_VECTOR*,int,int)) 0x0040F5C0)
#define Legacy_do_new_cutscene_camera ((void(__cdecl*)()) 0x00421480)
#define Legacy_SaveD3DCameraMatrix ((void(__cdecl*)()) 0x00497280)
#define Legacy_UnknownCamera ((void(__cdecl*)()) 0x004975D0)

// COLLIDE.CPP
#define Legacy_GenericSphereBoxCollision ((void (__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x00413A90)

// CONTROL.CPP
#define Legacy_GetCeiling ((int (__cdecl*)(FLOOR_INFO*, int, int, int)) 0x00417640)
#define Legacy_GetRandomControl ((int (__cdecl*)()) 0x004A7C10)
#define Legacy_AnimateItem ((void (__cdecl*)(ITEM_INFO*)) 0x00415300)
#define Legacy_GetChange ((int (__cdecl*)(ITEM_INFO*, ANIM_STRUCT*)) 0x00415890)
#define Legacy_KillMoveItems ((void (__cdecl*)()) 0x00414620)
#define Legacy_KillMoveEffects ((void (__cdecl*)()) 0x004146A0)
#define Legacy_ClearDynamics ((void (__cdecl*)()) 0x00431530)
#define Legacy_ClearFires ((void (__cdecl*)()) 0x00481B10)
#define Legacy_UpdateSparks ((void (__cdecl*)()) 0x0042E8B0)	
#define Legacy_UpdateFireSparks ((void (__cdecl*)()) 0x004813B0)	
#define Legacy_UpdateSmoke ((void (__cdecl*)()) 0x00481DD0)	
#define Legacy_UpdateBlood ((void (__cdecl*)()) 0x00482610)	
#define Legacy_UpdateBubbles ((void (__cdecl*)()) 0x00483540)
#define Legacy_UpdateSplashes ((void (__cdecl*)()) 0x00430710)
#define Legacy_UpdateDebris ((void (__cdecl*)()) 0x0041D500)
#define Legacy_UpdateDrips ((void (__cdecl*)()) 0x00483D90)
#define Legacy_UpdateGunShells ((void (__cdecl*)()) 0x00482D80)
#define Legacy_UpdateBats ((void (__cdecl*)()) 0x00407FD0)
#define Legacy_UpdateRats ((void (__cdecl*)()) 0x0046AC70)
#define Legacy_LavaBurn ((void (__cdecl*)(ITEM_INFO*)) 0x0048ADD0)
#define Legacy_RefreshCamera ((void (__cdecl*)(short, short*)) 0x004165E0)
#define Legacy_DoFlipMap ((void (__cdecl*)(short)) 0x00418910)
#define Legacy_PlaySoundTrack ((void (__cdecl*)(short, short)) 0x00418B90)
#define Legacy_AlterFloorHeight ((void (__cdecl*)(ITEM_INFO*, int)) 0x004159F0)
#define Legacy_ObjectOnLOS2 ((int (__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, PHD_VECTOR*, BITE_INFO*)) 0x00419110)
#define Legacy_LOS ((int (__cdecl*)(GAME_VECTOR*, GAME_VECTOR*)) 0x00417CF0)
#define Legacy_GetTargetOnLOS ((int (__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, int, int)) 0x0041A170)
#define Legacy_RumbleScreen ((void (__cdecl*)()) 0x00442C90)
#define Legacy_UpdateShockwaves ((void (__cdecl*)()) 0x004849A0)
#define Legacy_GetFloor ((FLOOR_INFO* (__cdecl*)(int, int, int, short*)) 0x00415B20)
#define Legacy_GetFloorHeight ((int (__cdecl*)(FLOOR_INFO*, int, int, int)) 0x00415FB0)
#define Legacy_GetWaterHeight ((int (__cdecl*)(int, int, int, short)) 0x00415DA0)
#define Legacy_UpdateSpiders ((void (__cdecl*)()) 0x0047A340)
#define Legacy_UpdateLightning ((void (__cdecl*)()) 0x00484CB0)

// DOOR.CPP
#define Legacy_InitialiseDoor ((void (__cdecl*)(short)) 0x0043DB60)
#define Legacy_DrawLiftDoor ((void (__cdecl*)(ITEM_INFO*)) 0x0045AAF0)
#define Legacy_DoubleDoorControl ((void (__cdecl*)(short)) 0x00429840)
#define Legacy_InitialiseSteelDoor ((void (__cdecl*)(short)) 0x0043F180)
#define Legacy_SteelDoorControl ((void (__cdecl*)(short)) 0x00486BE0)
#define Legacy_SteelDoorCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x00487AD0)

// DRAW.CPP
#define Legacy_IsRoomOutside ((int (__cdecl*)(int, int, int)) 0x00418E90) 
#define Legacy_UpdateStorm ((void (__cdecl*)()) 0x0042A310)  
#define Legacy_InitInterpolate ((void(__cdecl*)(int,int)) 0x0042BE90)
#define Legacy_Sync ((int (__cdecl*)()) 0x004D1A40)
#define Legacy_GetBoundsAccurate ((short* (__cdecl*)(ITEM_INFO*)) 0x0042CF80)
#define Legacy_GetBestFrame ((short* (__cdecl*)(ITEM_INFO*)) 0x0042D020)
#define Legacy_phd_PushMatrix ((void(__cdecl*)(void)) 0x0048F9C0)
#define Legacy_phd_RotYXZ ((void(__cdecl*)(short, short, short)) 0x00490150)
#define Legacy_phd_TranslateRel ((void(__cdecl*)(int, int, int)) 0x0048FB20)
#define Legacy_phd_TranslateRel_I ((void(__cdecl*)(int, int, int)) 0x0042C110)
#define Legacy_phd_TranslateRel_ID ((void(__cdecl*)(int,int,int,int,int,int)) 0x0042C190)
#define Legacy_phd_TranslateAbs ((void(__cdecl*)(int, int, int)) 0x004903F0)
#define Legacy_phd_PopMatrix_I ((void(__cdecl*)(void)) 0x0042BF00)
#define Legacy_phd_RotY ((void(__cdecl*)(short)) 0x0048FCD0)
#define Legacy_phd_RotX ((void(__cdecl*)(short)) 0x0048FBE0)
#define Legacy_phd_RotZ ((void(__cdecl*)(short)) 0x0048FDC0)
#define Legacy_phd_RotY_I ((void(__cdecl*)(short)) 0x0042BFC0)
#define Legacy_phd_RotX_I ((void(__cdecl*)(short)) 0x0042C030)
#define Legacy_phd_RotZ_I ((void(__cdecl*)(short)) 0x0042C0A0)
#define Legacy_phd_SetTrans ((void (__cdecl*)(int, int, int)) 0x0048FA40)
#define Legacy_phd_PushMatrix_I ((void(__cdecl*)(void)) 0x0042BF50)
#define Legacy_phd_PushUnitMatrix ((void (__cdecl*)()) 0x0048FA90)
#define Legacy_phd_GetVectorAngles ((void(__cdecl*)(int, int, int, short*)) 0x004904B0)
#define Legacy_phd_RotYXZpack ((void(__cdecl*)(int)) 0x0048FEB0)
#define Legacy_gar_RotYXZsuperpack ((void(__cdecl*)(short**,int)) 0x0042C310)
#define Legacy_gar_RotYXZsuperpack_I ((void(__cdecl*)(short**,short**,int)) 0x0042C290)
#define Legacy_phd_RotBoundingBoxNoPersp ((void (__cdecl*)(short*, short*)) 0x0042E240)

// EFFECT2.CPP
#define Legacy_DetatchSpark ((SPARKS* (__cdecl*)(int, int)) 0x0042E6A0)
#define Legacy_AddWaterSparks ((void (__cdecl*)(int, int, int, int)) 0x00483180)
#define Legacy_TriggerUnderwaterExplosion ((void (__cdecl*)(ITEM_INFO*)) 0x0044F500)
#define Legacy_TriggerExplosionSparks ((void (__cdecl*)(int, int, int, int, int, int, short)) 0x0042F610)
#define Legacy_GetFreeSpark ((short (__cdecl*)()) 0x0042E790)
#define Legacy_GetFreeDrip ((short (__cdecl*)()) 0x00483D00)
#define Legacy_GetFreeSmokeSpark ((short (__cdecl*)()) 0x00481D40)
#define Legacy_TriggerDartSmoke ((void (__cdecl*)(int, int, int, int, int, int)) 0x00430D90)
#define Legacy_TriggerGunShell ((void (__cdecl*)(short, int, int)) 0x00482A60)
#define Legacy_TriggerLaraDrips ((void (__cdecl*)()) 0x00483F00)
#define Legacy_SetupRipple ((void (__cdecl*)(int, int, int, int, int)) 0x00430910)
#define Legacy_TriggerShockwave ((void (__cdecl*)(PHD_3DPOS*, int, int, int, int, int)) 0x00484670)
#define Legacy_TriggerExplosionBubbles ((void (__cdecl*)(int, int, int, short)) 0x00431070)
#define Legacy_AddFire ((void (__cdecl*)(int, int, int, byte, short, int)) 0x00481B40)
#define Legacy_ControlWaterfallMist ((void (__cdecl*)(short)) 0x00432CA0)
#define Legacy_TriggerRicochetSparks ((void (__cdecl*)(GAME_VECTOR*, short, int, int)) 0x0042F060)

// EFFECTS.CPP
#define Legacy_DoBloodSplat ((short (__cdecl*)(int, int, int, short, short, short)) 0x00432760)
#define Legacy_CreateBubble ((short (__cdecl*)(PHD_3DPOS*, short, int)) 0x0043C6C0); 
#define Legacy_SetupRipple ((void (__cdecl*)(int, int, int, byte, byte)) 0x00430910); 

// EFFECT2.CPP
#define Legacy_InitialiseSmokeEmitter ((void (__cdecl*)(short)) 0x0043D9D0)
#define Legacy_SmokeEmitterControl ((void (__cdecl*)(short)) 0x00431560)

// HEALT.CPP
#define Legacy_FlashIt ((int (__cdecl*)()) 0x00439C10)
#define Legacy_UpdateHealtBar ((void (__cdecl*)(int)) 0x00439E50)
#define Legacy_UpdateAirBar ((void (__cdecl*)(int)) 0x00439FC0)

// ITEMS.CPP
#define Legacy_ItemNewRoom ((void (__cdecl*)(short, short)) 0x00440DA0)
#define Legacy_EffectNewRoom ((void (__cdecl*)(short, short)) 0x004412F0)
#define Legacy_KillEffect ((void (__cdecl*)(short)) 0x00441180)
#define Legacy_CreateNewEffect ((short (__cdecl*)(short)) 0x004410F0)
#define Legacy_InitialiseFXArray ((void (__cdecl*)(int)) 0x00441080)
#define Legacy_AddActiveItem ((void (__cdecl*)(short)) 0x00440D10)
#define Legacy_RemoveActiveItem ((void (__cdecl*)(short)) 0x00440B60)
#define Legacy_RemoveDrawnItem ((void (__cdecl*)(short)) 0x00440C40)
#define Legacy_InitialiseItem ((void (__cdecl*)(short)) 0x004408B0)
#define Legacy_CreateItem ((short (__cdecl*)()) 0x00440840)
#define Legacy_KillItem ((void (__cdecl*)(short)) 0x00440620)
#define Legacy_ItemNearLara ((int (__cdecl*)(PHD_3DPOS*, int)) 0x00432580)
#define Legacy_TranslateItem ((void (__cdecl*)(ITEM_INFO*, int, int, int)) 0x00415960)

// LARA.CPP
#define Legacy_AnimateLara ((int (__cdecl*)(ITEM_INFO*)) 0x004563F0)
#define Legacy_GetLaraJointPosition ((void (__cdecl*)(PHD_VECTOR*, int)) 0x0041E2A0)

// OBJECTS.CPP
#define Legacy_InitialiseTightRope ((void (__cdecl*)(short)) 0x0043ED30)
#define Legacy_InitialiseRaisingBlock  ((void (__cdecl*)(short)) 0x0043D730)
#define Legacy_RaisingBlockControl ((void (__cdecl*)(short)) 0x0048C3D0)
#define Legacy_InitialiseRaisingCog ((void (__cdecl*)(short)) 0x00440320)
#define Legacy_RaisingCogControl ((void (__cdecl*)(short)) 0x00406040)
#define Legacy_InitialiseSmashObject ((void (__cdecl*)(short)) 0x0043D7F0)
#define Legacy_InitialiseAnimating ((void (__cdecl*)(short)) 0x00440100)
#define Legacy_AnimatingControl ((void (__cdecl*)(short)) 0x00465590)
#define Legacy_InitialiseXRayMachine ((void (__cdecl*)(short)) 0x0043FA20)
#define Legacy_HighObject2Control ((void (__cdecl*)(short)) 0x004070D0)


// PICKUP.CPP
#define Legacy_PickupCollision ((void (__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x00467C00)
#define Legacy_InitialisePickup ((void (__cdecl*)(short)) 0x0043E260)
#define Legacy_PickupControl ((void (__cdecl*)(short)) 0x004679D0)

// ROPE.CPP
#define Legacy_CalculateRopePoints ((void (__cdecl*)(ROPE_STRUCT*)) 0x0046EC70)

// SAVEGAME.CPP
#define Legacy_RestoreGame ((int (__cdecl*)()) 0x00472060)	
#define Legacy_ReadSavegame ((int (__cdecl*)(int)) 0x004A8E10)	
#define Legacy_CreateSavegame ((void (__cdecl*)()) 0x00470FA0)	
#define Legacy_WriteSavegame ((int (__cdecl*)(int)) 0x004A8BC0)

// SPHERE.CPP
#define Legacy_TestCollision ((int (__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00479170)
#define Legacy_GetJointAbsPosition ((int (__cdecl*)(ITEM_INFO*, PHD_VECTOR*, int)) 0x00479780)
#define Legacy_GetSpheres ((int (__cdecl*)(ITEM_INFO*, SPHERE*, int)) 0x00479380)

// SPOTCAM.CPP
#define Legacy_InitSpotCamSequences ((void (__cdecl*)()) 0x0047A800)
#define Legacy_CalculateSpotCameras ((void (__cdecl*)()) 0x0047B280)
#define Legacy_InitialiseSpotCam ((void (__cdecl*)(short)) 0x0047A9D0)
#define Legacy_TriggerTitleSpotcam ((void (__cdecl*)(int)) 0x004284A0)
#define Legacy_Spline ((int (__cdecl*)(int, int*, int)) 0x0047A890)

// SWITCH.CPP
#define Legacy_InitialiseSwitch ((void (__cdecl*)(short)) 0x00440070)
#define Legacy_SequenceControl ((void (__cdecl*)(short)) 0x0047F520)
#define Legacy_SequenceCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x0047F610)
#define Legacy_LeverSwitchCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x0047EE00)
#define Legacy_InitialisePulleySwitch ((void (__cdecl*)(short)) 0x0043E1F0)
#define Legacy_InitialiseCrowDoveSwitch ((void (__cdecl*)(short)) 0x0043ECF0)

// MISC
#define Legacy_SQRT_ASM ((int(__cdecl*)(int)) 0x0048F980)
