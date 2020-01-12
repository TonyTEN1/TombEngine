﻿#pragma once

#include "..\Game\sound.h"



#define Camera						VAR_U_(0x00EEF940, CAMERA_INFO)
#define ForcedFixedCamera			VAR_U_(0x00EEFA20, GAME_VECTOR)
#define UseForcedFixedCamera		VAR_U_(0x00EEFA50, char)
#define MallocBuffer				VAR_U_(0x00E4B10C, char*)
#define MallocSize					VAR_U_(0x00E4B058, int)
#define MallocPtr					VAR_U_(0x00E4B0DC, char*)
#define MallocFree					VAR_U_(0x00E4B0F4, int)
#define MallocUsed					VAR_U_(0x00E4B0F0, int)
#define Items						VAR_U_(0x00EEEFF0, ITEM_INFO*)
#define Effects						VAR_U_(0x00E5C5E0, FX_INFO*)
#define InItemControlLoop			VAR_U_(0x0051CA53, byte)
#define ItemNewRoomNo				VAR_U_(0x0051CA54, short)
#define ItemNewRooms				ARRAY_(0x00EEF4C0, short, [512])
#define NextFxActive				VAR_U_(0x00E5C5FC, short)
#define NextFxFree					VAR_U_(0x00E5C5F4, short)
#define NextItemActive				VAR_U_(0x00E5BF22, short)
#define NextItemFree				VAR_U_(0x00E5BF16, short)
//#define SlotsUsed					VAR_U_(0x0051CEE8, int)
#define nAIObjects					VAR_U_(0x00E5B842, short)
#define TriggerIndex				VAR_U_(0x00EEEF9C, short*)
#define Boxes						VAR_U_(0x00EEFB64, BOX_INFO*)
#define NumberBoxes					VAR_U_(0x00EEFB68, int)
#define GroundZones					ARRAY_(0x00EEFB20, short*, [10])
#define Overlaps					VAR_U_(0x00EEFB5C, short*)
#define LevelItems					VAR_U_(0x0051CEB8, int)
#define BaddieSlots					VAR_U_(0x00E5B834, CREATURE_INFO*)
#define AIObjects					VAR_U_(0x00E5B844, AIOBJECT*)
#define Rooms						VAR_U_(0x00875154, ROOM_INFO*)
//#define Objects						ARRAY_(0x0086CF50, OBJECT_INFO, [460])
#define Anims						VAR_U_(0x00875158, ANIM_STRUCT*)
#define Bones						VAR_U_(0x00875178, int*)
#define Changes						VAR_U_(0x0087515C, CHANGE_STRUCT*)
#define Ranges						VAR_U_(0x00875160, RANGE_STRUCT*)
#define MeshBase					VAR_U_(0x0087516C, short*)
#define Meshes						VAR_U_(0x00875170, short**)
#define Frames						VAR_U_(0x00875174, short*)
#define Commands					VAR_U_(0x0087517C, short*)
#define MeshesCount					VAR_U_(0x00875140, int)
#define AnimationsCount				VAR_U_(0x00875150, int)
#define FloorData					VAR_U_(0x00875168, short*)
#define ObjectTextures				VAR_U_(0x008751B0, OBJECT_TEXTURE*)
#define RoomLightsCount				VAR_U_(0x0087B0EC, int)
#define NumberRooms					VAR_U_(0x0087514C, short)
#define nAnimUVRanges				VAR_U_(0x0087495C, int)
#define LevelFilePtr				VAR_U_(0x00875164, FILE*)
#define NumberCameras				VAR_U_(0x00EEFAC0, int)
#define Cameras						VAR_U_(0x00EEF9A2, OBJECT_VECTOR*)
//#define NumberSpotcams				VAR_U_(0x00E4F428, int)
#define LaraItem					VAR_U_(0x00E5BF08, ITEM_INFO*)
#define Lara						VAR_U_(0x00E5BD60, LARA_INFO)
#define LaraDrawType				VAR_U_(0x00EEEAD0, char)
#define Hairs						ARRAY_(0x00E5C000, HAIR_STRUCT, [DOUBLE_HAIR_SEGMENTS])
#define gfNumMips					VAR_U_(0x0051CE37, char)
#define Frames						VAR_U_(0x00875174, short*)
#define gfMips						ARRAY_(0x00E5C2C0, char, [8])
#define SetDebounce					VAR_U_(0x00EEEA38, int)
#define FlipStatus					VAR_U_(0x00EEEAE0, int)
#define FlipStats					ARRAY_(0x00EEF060, int, [255])
#define WindowsHandle				VAR_U_(0x00D9AB3C, HWND)
#define SoundActive					VAR_U_(0x0051D004, int)
#define DoTheGame					VAR_U_(0x00876C40, int)
#define CurrentLevel				VAR_U_(0x00E5C2D0, char)
#define CutSeqNum					VAR_U_(0x0051CAA8, int)
#define CutSeqTriggered				VAR_U_(0x0051CAAC, int)
#define GlobalPlayingCutscene		VAR_U_(0x0051CAB0, int)
#define Wibble						VAR_U_(0x0051CDF0, int)
#define DbInput						VAR_U_(0x00878DAC, int)
#define TrInput						VAR_U_(0x00878D98, int)
#define MatrixStack					ARRAY_(0x0055D66C, int, [480])
#define MatrixPtr					VAR_U_(0x00E4B0F8, int*)
#define DxMatrixPtr					VAR_U_(0x0055DA2C, byte*)
#define Unk_876C48					VAR_U_(0x00876C48, int)
#define Unk_007E7FE8				VAR_U_(0x007E7FE8, int)
#define SampleLUT					VAR_U_(0x00E528A4, short*)
#define SampleInfo					VAR_U_(0x00E528A8, SAMPLE_INFO*)
#define SamplePointer               ARRAY_(0x0086BEF0, HSAMPLE, [SOUND_MAX_SAMPLES])
#define SoundSlot					ARRAY_(0x00E52430, SoundEffectSlot, [SOUND_MAX_CHANNELS])
#define TrackNamePrefix				VAR_U_(0x00511828, char)
#define IsAtmospherePlaying			VAR_U_(0x00EEEFFC, byte)
#define CurrentAtmosphere			VAR_U_(0x00EEEB90, byte)
#define GlobalMusicVolume			VAR_U_(0x00517B68, unsigned int)
#define GlobalFXVolume				VAR_U_(0x00517B6C, unsigned int)
#define gfFilenameOffset			VAR_U_(0x00E5C34C, short*)
#define gfFilenameWad				VAR_U_(0x00E5C2CC, char*)
#define AllStrings					VAR_U_(0x00E5C310, char*)
#define AllStringsOffsets			VAR_U_(0x00E5C2B8, short*)
#define RenderLoadBar				VAR_U_(0x008FBDC0, int)
#define WeatherType					VAR_U_(0x00EEF4A0, byte)
#define CreditsDone					VAR_U_(0x00E6D838, byte)
#define CanLoad						VAR_U_(0x0051CE54, byte)
#define Savegame					VAR_U_(0x00E52EB3, SAVEGAME_INFO)
#define LastInventoryItem			VAR_U_(0x00508E14, int)
#define DelCutSeqPlayer				VAR_U_(0x0051CE2C, short)
#define TitleControlsLockedOut		VAR_U_(0x00E5C2A8, byte)
#define TrackCameraInit				VAR_U_(0x0051D010, int)
#define CheckTrigger				VAR_U_(0x0051D014, int)
#define UseSpotCam					VAR_U_(0x0051D018, int)
#define DisableLaraControl			VAR_U_(0x0051D01C, byte)
#define FramesCount					VAR_U_(0x0051CA78, int)
#define GlobalCounter				VAR_U_(0x0051CA58, short)
#define LevelComplete				VAR_U_(0x00E5C2F0, byte)
#define ResetFlag					VAR_U_(0x0051CA6C, int)
#define LegacyGameFlow				VAR_U_(0x00E5C2BC, GAMEFLOW*)
#define GameMode					VAR_U_(0x005082C0, byte)
#define nFrames						VAR_U_(0x005082C4, int)
#define GameStatus					VAR_U_(0x00E5C2B0, int)
//#define GotLaraSpheres				VAR_U_(0x00E51F2E, byte)
#define EffectRoutines				ARRAY_(0x00507964, EFFECT_ROUTINE, [MAX_EFFECTS])
//#define DashTimer					VAR_U_(0x00E5BF04, short)
#define LaraCollisionRoutines		ARRAY_(0x005089A8, LARA_COLLISION_ROUTINE, [500])
#define LaraControlRoutines			ARRAY_(0x0050877C, LARA_CONTROL_ROUTINE, [500])
#define CurrentFOV					VAR_U_(0x00E4F504, short)
//#define SpotCam						ARRAY_(0x00E4F6C0, SPOTCAM, [64])
//#define CurrentSplineCamera			VAR_U_(0x00E4F500, short)
//#define OldHitPoints				VAR_U_(0x005084DC, short)
#define GameTimer					VAR_U_(0x00E5C27C, int)
#define InventoryObjectsList		ARRAY_(0x00508E38, INVOBJ, [100])
#define Friggrimmer					VAR_U_(0x00E598F4, byte)
#define InventoryRings				ARRAY_(0x00E59900, INVENTORYRING*, [2])
#define GnFrameCounter				VAR_U_(0x00E4B0FC, int)
#define	gfLevelFlags				VAR_U_(0x00E5C2A0, int)
#define OptionAutoTarget			VAR_U_(0x00D9AC30, int)
#define PhdLeft						VAR_U_(0x0055D20C, int)
#define PhdTop						VAR_U_(0x0051D0A8, int)
#define PhdRight					VAR_U_(0x0055DA3C, int)
#define PhdBottom					VAR_U_(0x0055D204, int)
#define BoundList					ARRAY_(0x0051CB5C, int, [128])
#define BoundStart					VAR_U_(0x0051CD60, int)
#define BoundEnd					VAR_U_(0x0051CD64, int)
#define NumberDrawnRooms			VAR_U_(0x00E6CAFC, int)
#define DrawnRooms					ARRAY_(0x00E6D760, short, [256])
#define CurrentRoom					VAR_U_(0x00E6D754, short)
#define Outside2					VAR_U_(0x00E6CAF8, int)
#define OutsideTop					VAR_U_(0x00E6E4E0, int)
#define OutsideLeft					VAR_U_(0x00E6D83C, int)
#define OutsideBottom				VAR_U_(0x00E6D738, int)
#define OutsideRight				VAR_U_(0x00E6E4C0, int)
#define Underwater					VAR_U_(0x00E6E4B4, int)
#define Unknown_00E6CAE8			VAR_U_(0x00E6CAE8, int)
#define PhdWindowXmax				VAR_U_(0x0055D218, short)
#define PhdWindowYmax				VAR_U_(0x0055D240, short)
//#define Fires						ARRAY_(0x00E4C7A0, FIRE_LIST, [MAX_FIRE_LIST])
//#define Dynamics					ARRAY_(0x00E6C3E0, DYNAMIC, [MAX_DYNAMICS])
//#define NumDynamics					VAR_U_(0x00E6D82C, int)
#define Sprites						VAR_U_(0x008751B4, SPRITE*)
#define GunFlashes                  ARRAY_(0x00E4D660, GUNFLASH_STRUCT, [MAX_GUNFLASH])
//#define Splashes					ARRAY_(0x00E6CA00, SPLASH_STRUCT, [MAX_SPLASH])
//#define Ripples						ARRAY_(0x00E5C600, RIPPLE_STRUCT, [MAX_RIPPLES])
//#define NextFireSpark               VAR_U_(0x0050A17C, int)
//#define NextSmokeSpark              VAR_U_(0x0051D084, int)
//#define NextGunShell                VAR_U_(0x0051D08C, int)
//#define NextBubble                  VAR_U_(0x0051D090, int)
//#define NextDrip                    VAR_U_(0x0051D094, int)
#define NextDebris                  VAR_U_(0x0051CA8C, int)
//#define NextBlood                   VAR_U_(0x0051D088, int)
#define NextItem                    VAR_U_(0x00EEF044, short)
#define SkyPos1						VAR_U_(0x00E6E4B0, short)
#define SkyPos2						VAR_U_(0x00E6D73E, short)
#define SkyVelocity1				VAR_U_(0x00E5C276, signed char)
#define SkyVelocity2				VAR_U_(0x00E5C275, signed char)
#define SkyColor1					VAR_U_(0x00E5C2A4, CVECTOR)
#define SkyColor2					VAR_U_(0x00E5C280, CVECTOR)
#define SkyStormColor				ARRAY_(0x00E6CAF0, short, [3])
#define SkyStormColor2				ARRAY_(0x00E6E4B8, short, [3])
#define StormTimer					VAR_U_(0x0051CD5C, short)
#define Unk_00E6D74C				VAR_U_(0x00E6D74C, short)
#define Unk_00E6D73C				VAR_U_(0x00E6D73C, short)
#define Unk_00E6D74C				VAR_U_(0x00E6D74C, short)
#define Unk_00E6E4DC				VAR_U_(0x00E6E4DC, short)
#define CurrentAtmosphere			VAR_U_(0x00EEEB90, byte)
#define IsAtmospherePlaying			VAR_U_(0x00EEEFFC, byte)
//#define SmokeCountL					VAR_U_(0x00E6C9E8, byte)
//#define SmokeCountR					VAR_U_(0x00E6C9EC, byte)
//#define SplashCount					VAR_U_(0x0051CDF8, byte)
#define WeaponDelay					VAR_U_(0x0051CA52, byte)
//#define HealtBarTimer				VAR_U_(0x0051CEA8, int)
#define AnimatedTextureRanges		VAR_U_(0x00D9ADA0, short*)
#define RequiredStartPos			VAR_U_(0x00E5C2B4, byte)
#define InitialiseGame				VAR_U_(0x005082C1, byte)
#define TorchRoom					VAR_U_(0x00507AB0, short)
#define WeaponEnemyTimer			VAR_U_(0x0080E016, byte)
#define HeightType					VAR_U_(0x00EEEFF4, int)
#define HeavyTriggered				VAR_U_(0x00EEEA34, byte)
#define FlipEffect					VAR_U_(0x00506C60, int)
#define FlipTimer					VAR_U_(0x0051CA68, int)
#define Unknown_00EEF99A			VAR_U_(0x00EEF99A, byte)
//#define SpotCamRemap				ARRAY_(0x00E4F4F0, byte, [16])
//#define CameraCnt					ARRAY_(0x00E51F10, byte, [16])
//#define LastSpotCam					VAR_U_(0x00E51F2C, short)
#define FlipMap						ARRAY_(0x00EEEBA0, int, [255])
#define TriggerTimer				VAR_U_(0x0051CA5A, byte)
#define JustLoaded					VAR_U_(0x0051D001, byte)
//#define SmokeWeapon					VAR_U_(0x00E6CAA0, int)
#define HKTimer						VAR_U_(0x0051CEC9, int)
#define HKFlag						VAR_U_(0x0051CECC, int)
#define HKFlag2						VAR_U_(0x0051CEC8, byte)
#define Ropes						ARRAY_(0x00E54CC0, ROPE_STRUCT, [12])
#define NumRopes					VAR_U_(0x0051CA60, int)
#define PhdWidth					VAR_U_(0x0055D29C, int)
#define PhdHeight					VAR_U_(0x0055D164, int)
#define PhdPerspective				VAR_U_(0x0055D208, int)
#define Bats						VAR_U_(0x00EEEFE8, BAT_STRUCT*)
#define Rats						VAR_U_(0x00EEEFEC, RAT_STRUCT*)
#define Spiders						VAR_U_(0x00EEF45C, SPIDER_STRUCT*)
#define PoisonFlags					VAR_U_(0x00E5BF3E, byte)
#define SmashedMeshCount			VAR_U_(0x0051CA5C, short)
#define SmashedMesh					ARRAY_(0x00EEF8C0, MESH_INFO*, [16])
#define SmashedMeshRoom				ARRAY_(0x00EEF480, short, [16])
#define Debris						ARRAY_(0x00E8CAC0, DEBRIS_STRUCT, [MAX_DEBRIS])
#define SniperCameraActive			VAR_U_(0x0051CA1D, byte)
#define BinocularRange				VAR_U_(0x0051CA30, int)
#define BinocularOn					VAR_U_(0x0051CA34, int)
#define BinocularOldCamera			VAR_U_(0x0051CA38, CAMERA_TYPE)
#define LaserSight					VAR_U_(0x0051CA3C, int)
#define SniperCount					VAR_U_(0x0051CA1C, byte)
#define ExitingBinocular			VAR_U_(00051CA20, int)
#define OldLaraBusy					VAR_U_(0x00E59898, byte)
#define Infrared					VAR_U_(0x0051CA40, int)
#define SpotcamOverlay				VAR_U_(0x0051D020, byte)
#define SpotcamDontDrawLara			VAR_U_(0x0051D021, byte)
#define SniperOverlay				VAR_U_(0x0051D022, byte)
#define TargetGraphicsMeshPointer	VAR_U_(0x00E6E460, short*)
#define BinocularGraphicsMeshPtr	VAR_U_(0x00E6D744, short*)
#define CollidedItems				ARRAY_(0x00E88EA0, ITEM_INFO*, [1024])
#define CollidedMeshes				ARRAY_(0x00E89EA0, MESH_INFO*, [1024])
#define FlashFadeR					VAR_U_(0x0051D07A, short)
#define FlashFadeG					VAR_U_(0x0051D07C, short)
#define FlashFadeB					VAR_U_(0x0051D07E, short)
#define FlashFader					VAR_U_(0x0051D080, short)
#define GlobalCollisionBounds		VAR_U_(0x00EEF910, BOUNDING_BOX)
#define SphereList					ARRAY_(0x00E51FE0, SPHERE, [34])
#define IsRoomOutsideNo				VAR_U_(0x007FE6C2, short)
#define App_Unk00D9AC2B				VAR_U_(0x00D9AC2B, byte)
#define App_Unk00D9AC19				VAR_U_(0x00D9AC19, byte)
#define App_Unk00D9ABFD				VAR_U_(0x00D9ABFD, int)
#define WaterfallTextures			ARRAY_(0x00E59720, OBJECT_TEXTURE*, [6])
#define WaterfallY					ARRAY_(0x00E59760, float, [6])
#define KeyboardLayout0				ARRAY_(0x00516C3C, short, [18])
#define KeyboardLayout1				ARRAY_(0x00516C60, short, [18])
#define KeyMap						ARRAY_(0x0086BB8C, byte, [256])
#define ConflictingKeys				ARRAY_(0x00878C4C, byte, [256])
/*#define LaraAir						VAR_U_(0x00E4F6A8, int)
#define LaraHealth					VAR_U_(0x00E51F20, int)

#define LastSequence				VAR_U_(0x00E51F2C, short)
#define SpotcamTimer				VAR_U_(0x0051D00C, short)
#define SpotcamLoopCnt				VAR_U_(0x00E4F488, short)
#define CameraFade					VAR_U_(0x00E4F36C, int)
#define LaraFixedPosition			VAR_U_(0x00E4F360, PHD_VECTOR)
#define InitialCameraRoom			VAR_U_(0x00E4F588, short)
#define LastFOV						VAR_U_(0x00E4F506, short)
#define InitialCameraPosition		VAR_U_(0x00E4F3D0, PHD_VECTOR)
#define InitialCameraTarget			VAR_U_(0x00E4F350, PHD_VECTOR)
#define CurrentSplinePosition		VAR_U_(0x00E51F28, int)
#define SplineToCamera				VAR_U_(0x00E4F48C, int)
#define FirstCamera					VAR_U_(0x00E51F24, short)
#define LastCamera					VAR_U_(0x00E4F6AC, short)
#define CurrentCameraCnt			VAR_U_(0x00E4EAA0, int)
#define CameraXposition				ARRAY_(0x00E4F43C, int, [MAX_CAMERA])
#define CameraYposition				ARRAY_(0x00E4F5FC, int, [MAX_CAMERA])
#define CameraZposition				ARRAY_(0x00E4F53C, int, [MAX_CAMERA])
#define CameraXtarget				ARRAY_(0x00E4F3DC, int, [MAX_CAMERA])
#define CameraYtarget				ARRAY_(0x00E51EBC, int, [MAX_CAMERA])
#define CameraZtarget				ARRAY_(0x00E4F59C, int, [MAX_CAMERA])
#define CameraRoll					ARRAY_(0x00E4F49C, int, [MAX_CAMERA])
#define CameraFOV					ARRAY_(0x00E4F37C, int, [MAX_CAMERA])
#define CameraSpeed					ARRAY_(0x00E4F65C, int, [MAX_CAMERA])
#define SpotcamDontDrawLara			VAR_U_(0x0051D021, byte)
#define QuakeCam					VAR_U_(0x00E4F520, QUAKE_CAMERA)
#define SplineFromCamera			VAR_U_(0x00E4F3DC, int)
#define Unk_0051D024				VAR_U_(0x0051D024, byte)

*/
//#define PoisonFlag					VAR_U_(0x00E5BF3E, byte)
//#define SplashSetup					VAR_U_(0x00E6CAC0, SPLASH_SETUP)
#define NullRotations               ARRAY_(0x0051CD68, short, [12])
#define FirstHair                   VAR_U_(0x00E5BF60, int)

// JeepBaddieCollision
#define Unk_0080DE1A				VAR_U_(0x0080DE1A, short)
#define Unk_0080DDE8				VAR_U_(0x0080DDE8, int)
#define Unk_0080DE24				VAR_U_(0x0080DE24, short)
// End
#define ShatterItem					VAR_U_(0x00EEFAE0, SHATTER_ITEM)
#define Unk_00EEFB6C				VAR_U_(0x00EEFB6C, short)
#define OnObject					VAR_U_(0x00EEEAC8, int)
#define DontUnlockBox				VAR_U_(0x0051CB40, byte)
//#define CurrentSpotcamSequence		VAR_U_(0x00E4F502, short)
#define TiltXOffset					VAR_U_(0x00EEEAD4, int)
#define TiltYOffset					VAR_U_(0x00EEEADC, int)

//#define NextSpider					VAR_U_(0x0051D008, int)
//#define NextSpark					VAR_U_(0x0051CDFC, int)
#define KillEverythingFlag			VAR_U_(0x0051CDF4, int)
#define SmokeWindX					VAR_U_(0x00E6C9E0, int)
#define SmokeWindZ					VAR_U_(0x00E6C9E4, int)
#define NextRat						VAR_U_(0x0051CF94, int)
//#define NextFireSpark				VAR_U_(0x0050A17C, int)
#define LaraNodeUnderwater			ARRAY_(0x00E862FE, byte, [15])
#define OnFloor						VAR_U_(0x00EEEAC8, int)
#define SlowMotion					VAR_U_(0x0051CA50, short)
#define ShatterSounds				ARRAY_(0x00506BAC, unsigned char, [18][10])
#define TargetMesh					VAR_U_(0x00EEFAF8, int)
#define WeaponsArray				ARRAY_(0x005085B0, WEAPON_INFO, [9])
#define GetLaraOnLOS				VAR_U_(0x0051CA64, char)

extern bool MonksAttackLara;

// Remapped variables
extern int NumItems;
extern int dword_874254;
extern int unk_87435C;
extern ITEM_INFO* Targets[NUM_SLOTS];
extern STATIC_INFO StaticObjects[NUM_STATICS];

#define LegacyObjects						ARRAY_(0x0086CF50, OBJECT_INFO, [460])
extern OBJECT_INFO Objects[2048];