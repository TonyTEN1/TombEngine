#pragma once

// Define std::string ids
#define STRING_PASSPORT					1
#define STRING_LARA_HOME				2
#define STRING_CONTROLS					3
#define STRING_DISPLAY					4
#define STRING_SOUND					5
#define STRING_NEW_GAME					6
#define STRING_LOAD_GAME				7
#define STRING_SAVE_GAME				8
#define STRING_EXIT_GAME				9
#define STRING_EXIT_TO_TITLE			10	
#define STRING_UZI						11
#define STRING_PISTOLS					12
#define STRING_SHOTGUN					13
#define STRING_REVOLVER					14
#define STRING_REVOLVER_LASER			15
#define STRING_DESERT_EAGLE				16
#define STRING_DESERT_EAGLE_LASER		17
#define STRING_DESERT_EAGLE_AMMO		18
#define STRING_HK						19
#define STRING_HK_SILENCED				20
#define STRING_SHOTGUN_AMMO1			21
#define STRING_SHOTGUN_AMMO2			22
#define STRING_HK_SNIPER_MODE			23
#define STRING_HK_BURST_MODE			24
#define STRING_HK_RAPID_MODE			25
#define STRING_HK_AMMO					26
#define STRING_REVOLVER_AMMO			27
#define STRING_UZI_AMMO					28
#define STRING_PISTOLS_AMMO				29
#define STRING_LASERSIGHT				30
#define STRING_SILENCER					31
#define STRING_LARGE_MEDIPACK			32
#define STRING_SMALL_MEDIPACK			33
#define STRING_BINOCULARS				34
#define STRING_HEADSET					35
#define STRING_FLARES					36
#define STRING_TIMEX					37
#define STRING_CROWBAR					38
#define STRING_USE						39
#define STRING_COMBINE					40
#define STRING_SEPARE					41
#define STRING_CHOOSE_AMMO				42
#define STRING_SELECT_LEVEL				43
#define STRING_GRENADE_LAUNCHER			46
#define STRING_GRENADE_AMMO1			47
#define STRING_GRENADE_AMMO2			48
#define STRING_GRENADE_AMMO3			49
#define STRING_HARPOON_GUN				50
#define STRING_HARPOON_AMMO				51
#define STRING_ROCKET_LAUNCHER			52
#define STRING_ROCKET_AMMO				53
#define STRING_CROSSBOW					54
#define STRING_CROSSBOW_LASER			55
#define STRING_CROSSBOW_AMMO1			56
#define STRING_CROSSBOW_AMMO2			57
#define STRING_CROSSBOW_AMMO3			58
#define STRING_DIARY					59
#define STRING_ENABLED					60
#define STRING_DISABLED					61
#define STRING_MUSIC_VOLUME				62
#define STRING_SFX_VOLUME				63
#define STRING_SCREEN_RESOLUTION		64
#define STRING_SHADOWS					65
#define STRING_CAUSTICS					66
#define STRING_VOLUMETRIC_FOG			67
#define STRING_APPLY					68
#define STRING_BACK						69
#define STRING_ENABLE_SOUND				70
#define STRING_SPECIAL_SOUND_FX			71
#define STRING_WATERSKIN1_EMPTY			75
#define STRING_WATERSKIN2_EMPTY			76
#define STRING_CONTROLS_MOVE_FORWARD	78
#define STRING_CONTROLS_MOVE_BACKWARD	79
#define STRING_CONTROLS_MOVE_LEFT		80
#define STRING_CONTROLS_MOVE_RIGHT		81
#define STRING_CONTROLS_DUCK			82
#define STRING_CONTROLS_DASH			83
#define STRING_CONTROLS_WALK			84
#define STRING_CONTROLS_JUMP			85
#define STRING_CONTROLS_ACTION			86
#define STRING_CONTROLS_DRAW_WEAPON		87
#define STRING_CONTROLS_USE_FLARE		88
#define STRING_CONTROLS_LOOK			89
#define STRING_CONTROLS_ROLL			90
#define STRING_CONTROLS_INVENTORY		91
#define STRING_CONTROLS_STEP_LEFT		92
#define STRING_CONTROLS_STEP_RIGHT		93
#define STRING_TITLE_ITEMS				94
#define STRING_TITLE_PUZZLES			95
#define STRING_TITLE_SETTINGS			96
#define STRING_TITLE_CHOOSE_AMMO		97
#define STRING_TITLE_COMBINE			98
#define STRING_WAITING_FOR_KEY			99
#define STRING_WINDOWED					100
#define STRING_OK						101
#define STRING_CANCEL					102
#define STRING_RENDER_OPTIONS			103
#define STRING_DISPLAY_ADAPTER			104
#define STRING_OUTPUT_SETTINGS			105

class LanguageScript
{
public:
	std::string Name;
	std::vector<std::string> Strings;

	LanguageScript(char* name);
	~LanguageScript();
};

