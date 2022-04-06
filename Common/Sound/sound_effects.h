#pragma once

enum SOUND_EFFECTS
{
	SFX_TR4_LARA_FEET = 0,
	SFX_TR4_LARA_CLIMB2 = 1,
	SFX_TR4_LARA_NO = 2,
	SFX_TR4_LARA_SLIPPING = 3,
	SFX_TR4_LARA_LAND = 4,
	SFX_TR4_LARA_CLIMB1 = 5,
	SFX_TR4_LARA_DRAW = 6,
	SFX_TR4_LARA_HOLSTER = 7,
	SFX_TR4_LARA_FIRE = 8,
	SFX_TR4_LARA_RELOAD = 9,
	SFX_TR4_LARA_RICOCHET = 10,
	SFX_TR4_PUSH_BLOCK_END = 11,
	SFX_TR4_METAL_SCRAPE_LOOP1 = 12,
	SFX_TR4_SMALL_SCARAB_FEET = 13,
	SFX_TR4_BIG_SCARAB_ATTACK = 14,
	SFX_TR4_BIG_SCARAB_DEATH = 15,
	SFX_TR4_BIG_SCARAB_FLYING = 16,
	SFX_TR4_LARA_WET_FEET = 17,
	SFX_TR4_LARA_WADE = 18,
	SFX_TR4_RUMBLE_LOOP = 19,
	SFX_TR4_METAL_SCRAPE_LOOP2 = 20,
	SFX_TR4_CRICKET_LOOP = 21,
	SFX_TR4_WOOD_BRIDGE_FALL = 22,
	SFX_TR4_STARGATE_SWIRL = 23,
	SFX_TR4_LARA_KNEES_SHUFFLE = 24,
	SFX_TR4_PUSH_SPX_SWITCH = 25,
	SFX_TR4_LARA_CLIMB3 = 26,
	SFX_TR4_RESERVED = 27,
	SFX_TR4_LARA_SHIMMY2 = 28,
	SFX_TR4_LARA_JUMP = 29,
	SFX_TR4_LARA_FALL = 30,
	SFX_TR4_LARA_INJURY = 31,
	SFX_TR4_LARA_ROLL = 32,
	SFX_TR4_LARA_SPLASH = 33,
	SFX_TR4_LARA_GETOUT = 34,
	SFX_TR4_LARA_SWIM = 35,
	SFX_TR4_LARA_BREATH = 36,
	SFX_TR4_LARA_BUBBLES = 37,
	SFX_TR4_SPINNING_PUZZLE = 38,
	SFX_TR4_LARA_KEY = 39,
	SFX_TR4_COG_RESAW_LIBRARY = 40,
	SFX_LARA_GENERAL_DEATH = 41,
	SFX_TR4_LARA_KNEES_DEATH = 42,
	SFX_TR4_LARA_UZI_FIRE = 43,
	SFX_TR4_LARA_UZI_STOP = 44,
	SFX_TR4_LARA_SHOTGUN = 45,
	SFX_TR4_LARA_BLOCK_PUSH1 = 46,
	SFX_TR4_LARA_BLOCK_PUSH2 = 47,
	SFX_TR4_SARLID_PALACES = 48,
	SFX_TR4_LARA_SHOTGUN_SHELL = 49,
	SFX_TR4_UNDERWATER_DOOR = 50,
	SFX_TR4_LARA_BLKPULL = 51,
	SFX_TR4_LARA_FLOATING = 52,
	SFX_LARA_HIGH_FALL_DEATH = 53,
	SFX_TR4_LARA_GRABHAND = 54,
	SFX_TR4_LARA_GRABBODY = 55,
	SFX_TR4_LARA_GRABFEET = 56,
	SFX_TR4_RATCHET_3SHOT = 57,
	SFX_TR4_RATCHET_1SHOT = 58,
	SFX_TR4_WATER_LOOP_NOTINUSE = 59,
	SFX_TR4_UNDERWATER = 60,
	SFX_TR4_UNDERWATER_SWITCH = 61,
	SFX_TR4_LARA_PICKUP = 62,
	SFX_TR4_PUSHABLE_SOUND = 63,
	SFX_TR4_DOOR_GENERAL = 64,
	SFX_TR4_HELICOPTER_LOOP = 65,
	SFX_TR4_ROCK_FALL_CRUMBLE = 66,
	SFX_TR4_ROCK_FALL_LAND = 67,
	SFX_TR4_PENDULUM_BLADES = 68,
	SFX_TR4_STALEGTITE = 69,
	SFX_TR4_LARA_THUD = 70,
	SFX_TR4_GENERIC_SWOOSH = 71,
	SFX_TR4_GENERIC_HEAVY_THUD = 72,
	SFX_TR4_CROC_FEET = 73,
	SFX_TR4_SWINGING_FLAMES = 74,
	SFX_TR4_STONE_SCRAPE = 75,
	SFX_TR4_BLAST_CIRCLE = 76,
	SFX_TR4_BAZOOKA_FIRE = 77,
	SFX_LARA_HK_FIRE= 78,
	SFX_TR4_WATERFALL_LOOP = 79,
	SFX_TR4_CROC_ATTACK = 80,
	SFX_TR4_CROC_DEATH = 81,
	SFX_TR4_PORTCULLIS_UP = 82,
	SFX_TR4_PORTCULLIS_DOWN = 83,
	SFX_TR4_DOUBLE_DOORS_BANG = 84,
	SFX_TR4_DOUBLE_DOORS_CREAK = 85,
	SFX_TR4_PETES_PYRA_STONE = 86,
	SFX_TR4_PETES_PYRA_PNEU = 87,
	SFX_TR4_AHMET_DIE = 88,
	SFX_TR4_AHMET_ATTACK = 89,
	SFX_TR4_AHMET_HANDS = 90,
	SFX_TR4_AHMET_FEET = 91,
	SFX_TR4_AHMET_SWIPE = 92,
	SFX_TR4_AHMET_WAIT = 93,
	SFX_TR4_GUIDE_JUMP = 94,
	SFX_TR4_GENERAL_FOOTSTEPS1 = 95,
	SFX_TR4_GUIDE_LAND_USENOT = 96,
	SFX_TR4_POUR = 97,
	SFX_TR4_SCALE1 = 98,
	SFX_TR4_SCALE2 = 99,
	SFX_TR4_BEETLARA_WINDUP = 100,
	SFX_TR4_BEETLE_CLK_WHIRR = 101,
	SFX_TR4_BEETLE_CLK_EXP = 102,
	SFX_TR4_MINE_EXP_OVERLAY = 103,
	SFX_TR4_HECKLER_AND_KOCH_STOP = 104,
	SFX_TR4_EXPLOSION1 = 105,
	SFX_TR4_EXPLOSION2 = 106,
	SFX_TR4_EARTHQUAKE_LOOP = 107,
	SFX_TR4_MENU_ROTATE = 108,
	SFX_TR4_MENU_SELECT = 109,
	SFX_TR4_Menu_Empty1 = 110,
	SFX_TR4_MENU_CHOOSE = 111,
	SFX_TR4_TICK_TOCK = 112,
	SFX_TR4_Menu_Empty2 = 113,
	SFX_TR4_MENU_COMBINE = 114,
	SFX_TR4_Menu_Empty3 = 115,
	SFX_TR4_MENU_MEDI = 116,
	SFX_TR4_LARA_CLIMB_WALLS_NOISE = 117,
	SFX_TR4_WATER_LOOP = 118,
	SFX_TR4_VONCROY_JUMP = 119,
	SFX_TR4_LOCUSTS_LOOP = 120,
	SFX_TR4_DESSERT_EAGLE_FIRE = 121,
	SFX_TR4_BOULDER_FALL = 122,
	SFX_TR4_LARA_MINI_LOAD = 123,
	SFX_TR4_LARA_MINI_LOCK = 124,
	SFX_TR4_LARA_MINI_FIRE = 125,
	SFX_TR4_GATE_OPENING = 126,
	SFX_TR4_LARA_ELECTRIC_LOOP = 127,
	SFX_TR4_LARA_ELECTRIC_CRACKLES = 128,
	SFX_TR4_BLOOD_LOOP = 129,
	SFX_TR4_BIKE_START = 130,
	SFX_TR4_BIKE_IDLE = 131,
	SFX_TR4_BIKE_ACCELERATE = 132,
	SFX_TR4_BIKE_MOVING = 133,
	SFX_TR4_BIKE_SIDE_IMPACT = 134,
	SFX_TR4_BIKE_FRONT_IMPACT = 135,
	SFX_TR4_SOFT_WIND_LOOP = 136,
	SFX_TR4_BIKE_LAND = 137,
	SFX_TR4_CROCGOD_ROAR = 138,
	SFX_TR4_CROCGOD_WINGS = 139,
	SFX_TR4_CROCGOD_LAND = 140,
	SFX_TR4_CROCGOD_FIRE_ROAR = 141,
	SFX_TR4_BIKE_STOP = 142,
	SFX_TR4_GENERIC_BODY_SLAM = 143,
	SFX_TR4_HECKER_AND_KOCH_OVERLAY = 144,
	SFX_TR4_LARA_SPIKE_DEATH = 145,
	SFX_TR4_LARA_DEATH3 = 146,
	SFX_TR4_ROLLING_BALL = 147,
	SFX_TR4_BLK_PLAT_RAISE_AND_LOW = 148,
	SFX_TR4_RUMBLE_NEXTDOOR = 149,
	SFX_TR4_LOOP_FOR_SMALL_FIRES = 150,
	SFX_TR4_CHAINS_LIBRARY = 151,
	SFX_TR4_JEEP_START = 152,
	SFX_TR4_JEEP_IDLE = 153,
	SFX_TR4_JEEP_ACCELERATE = 154,
	SFX_TR4_JEEP_MOVE = 155,
	SFX_TR4_JEEP_STOP = 156,
	SFX_TR4_BATS_1 = 157,
	SFX_TR4_ROLLING_DOOR = 158,
	SFX_TR4_LAUNCHER_1 = 159,
	SFX_TR4_LAUNCHER_2 = 160,
	SFX_TR4_TRAPDOOR_OPEN = 161,
	SFX_TR4_TRAPDOOR_CLOSE = 162,
	SFX_TR4_HK_STOP= 163,
	SFX_TR4_BABOON_STAND_WAIT = 164,
	SFX_TR4_BABOON_ATTACK_LOW = 165,
	SFX_TR4_BABOON_ATTACK_JUMP = 166,
	SFX_TR4_BABOON_JUMP = 167,
	SFX_TR4_BABOON_DEATH = 168,
	SFX_TR4_BAT_SQUEAL_FULL = 169,
	SFX_TR4_BAT_SQK = 170,
	SFX_TR4_BAT_FLAP = 171,
	SFX_TR4_SPHINX_NOSE_RASP = 172,
	SFX_TR4_SPHINX_WALK = 173,
	SFX_TR4_SPHINX_NOISE = 174,
	SFX_TR4_DOG_HOWL = 175,
	SFX_TR4_DOG_HIT_GROUND = 176,
	SFX_TR4_FOUNTAIN_LOOP = 177,
	SFX_TR4_DOG_FOOT_1 = 178,
	SFX_TR4_DOG_JUMP = 179,
	SFX_TR4_DOG_BITE = 180,
	SFX_TR4_DOG_DEATH = 181,
	SFX_TR4_THUNDER_RUMBLE = 182,
	SFX_TR4_THUNDER_CRACK = 183,
	SFX_TR4_WRAITH_WHISPERS = 184,
	SFX_LARA_FOOSTEPS_SAND = 185,
	SFX_TR4_Empty2 = 186,
	SFX_TR4_SKEL_FOOTSTEPS = 187,
	SFX_TR4_SKEL_ATTACK = 188,
	SFX_TR4_GENERIC_SWORD_SWOOSH = 189,
	SFX_TR4_SKEL_SWORD_CLANG = 190,
	SFX_TR4_SKEL_STICK_GROUND = 191,
	SFX_TR4_GEN_PULL_SWORD = 192,
	SFX_TR4_SKEL_LAND_HEAVY = 193,
	SFX_TR4_GUIDE_SCARE = 194,
	SFX_TR4_JEEP_DOOR_OPEN = 195,
	SFX_TR4_JEEP_DOOR_CLOSE = 196,
	SFX_TR4_ELEC_ARCING_LOOP = 197,
	SFX_TR4_ELEC_ONE_SHOT = 198,
	SFX_TR4_Empty3 = 199,
	SFX_TR4_LIBRARY_COG_LOOP = 200,
	SFX_TR4_JEEP_SIDE_IMPACT = 201,
	SFX_TR4_JEEP_FRONT_IMPACT = 202,
	SFX_TR4_JEEP_LAND = 203,
	SFX_TR4_SPINNING_GEM_SLOTS = 204,
	SFX_TR4_RUMMBLE = 205,
	SFX_TR4_WARTHOG_HEADBUTT = 206,
	SFX_TR4_WARTHOG_DEATH = 207,
	SFX_TR4_SET_SPIKE_TIMER = 208,
	SFX_TR4_WARTHOG_SQUEAL = 209,
	SFX_TR4_WARTHOG_FEET = 210,
	SFX_TR4_WARTHOG_GRUNT = 211,
	SFX_TR4_SAVE_CRYSTAL = 212,
	SFX_TR4_HORSE_RICOCHETS = 213,
	SFX_TR4_METAL_SHUTTERS_SMASH = 214,
	SFX_TR4_GEM_DROP_ON_FLOOR = 215,
	SFX_TR4_SCORPION_SCREAM = 216,
	SFX_TR4_SCORPION_FEET = 217,
	SFX_TR4_SCORPION_CLAWS = 218,
	SFX_TR4_SCORPION_TAIL_WHIP = 219,
	SFX_TR4_SCORPION_SMALL_FEET = 220,
	SFX_TR4_METAL_GATE_OPEN = 221,
	SFX_TR4_HORSE_TROTTING = 222,
	SFX_TR4_KN_TEMPLAR_WALK = 223,
	SFX_TR4_KN_TEMPLAR_GURGLES = 224,
	SFX_TR4_KN_SWORD_SCRAPE = 225,
	SFX_TR4_KN_TEMPLAR_ATTACK = 226,
	SFX_TR4_KN_SWORD_CLANG = 227,
	SFX_TR4_KN_SWORD_SWOOSH = 228,
	SFX_TR4_MUMMY_ATTACK = 229,
	SFX_TR4_MUMMY_WALK = 230,
	SFX_TR4_MUMMY_GURGLES = 231,
	SFX_TR4_MUMMY_TAKE_HIT = 232,
	SFX_TR4_SMALL_FAN = 233,
	SFX_TR4_LARGE_FAN = 234,
	SFX_TR4_LARA_CROSSBOW = 235,
	SFX_TR4_SMALL_CREATURE_FEET = 236,
	SFX_TR4_SAS_GADGIE_DIE = 237,
	SFX_TR4_WATER_FLUSHES = 238,
	SFX_TR4_GUID_ZIPPO = 239,
	SFX_TR4_LEAP_SWITCH = 240,
	SFX_TR4_OLD_SWITCH = 241,
	SFX_TR4_DEMIGODS_FEET = 242,
	SFX_TR4_DEMIGODS_BULL_SNORT = 243,
	SFX_TR4_DEMIGODS_BULL_HAMMER = 244,
	SFX_TR4_DEMIGODS_S_WAVE_RUMB = 245,
	SFX_TR4_DEMIGOD_WEAP_SWOOSH = 246,
	SFX_TR4_DEMIGOD_FALCON_SQUEAL = 247,
	SFX_TR4_DEMIGOD_FALCON_PLAS = 248,
	SFX_TR4_DEMIGOD_RISE = 249,
	SFX_TR4_DEMI_TUT_PLASMA_SPRAY = 250,
	SFX_TR4_DEMI_SIREN_SWAVE = 251,
	SFX_TR4_DEMIGODS_TUT_GROWL = 252,
	SFX_TR4_JOBY_ELECTRIC_INSERT = 253,
	SFX_TR4_BAD_LAND = 254,
	SFX_TR4_DOOR_GEN_THUD = 255,
	SFX_TR4_BAD_GRUNTS = 256,
	SFX_TR4_BAD_DIE = 257,
	SFX_TR4_BAD_JUMP = 258,
	SFX_TR4_BAD_TROOP_STUN = 259,
	SFX_TR4_BAD_SWORDAWAY = 260,
	SFX_TR4_BAD_TROOP_UZI = 261,
	SFX_TR4_BAD_SWORD_RICO = 262,
	SFX_TR4_BAD_TROOP_UZI_END = 263,
	SFX_TR4_TROOP_SCORP_CRIES = 264,
	SFX_TR4_SAS_TROOP_FEET = 265,
	SFX_TR4_GENERIC_NRG_CHARGE = 266,
	SFX_TR4_SAS_MG_FIRE = 267,
	SFX_TR4_HAMMER_HEAD_WADE = 268,
	SFX_TR4_SMALL_SWITCH = 269,
	SFX_TR4_Empty4 = 270,
	SFX_TR4_SIREN_WING_FLAP = 271,
	SFX_TR4_SIREN_NOIZES = 272,
	SFX_TR4_SIREN_ATTACK = 273,
	SFX_TR4_SIREN_DEATH = 274,
	SFX_TR4_SIREN_GEN_NOISES = 275,
	SFX_TR4_SETT_SIREN_PLASMA = 276,
	SFX_TR4_HAMMER_HEAD_ATK = 277,
	SFX_TR4_SMALL_DOOR_SUBWAY = 278,
	SFX_TR4_TRAIN_DOOR_OPEN = 279,
	SFX_TR4_TRAIN_DOOR_CLOSE = 280,
	SFX_TR4_VONCROY_KNIFE_SWISH = 281,
	SFX_TR4_TRAIN_UNLINK_BREAK = 282,
	SFX_TR4_OBJ_BOX_HIT = 283,
	SFX_TR4_OBJ_BOX_HIT_CHANCE = 284,
	SFX_TR4_OBJ_GEM_SMASH = 285,
	SFX_TR4_CATBLADES_DRAW = 286,
	SFX_TR4_SWIRLY_LONG_MOVE_SFX = 287,
	SFX_LARA_FOOTSTEPS_MUD = 288,
	SFX_TR4_HORSEMAN_HORSE_NEIGH = 289,
	SFX_LARA_FOOTSTEPS_GRAVEL = 290,
	SFX_LARA_FOOTSTEPS_GRASS = 291,
	SFX_LARA_FOOTSTEPS_WOOD = 292,
	SFX_LARA_FOOTSTEPS_MARBLE = 293,
	SFX_LARA_FOOTSTEPS_METAL = 294,
	SFX_TR4_GEN_SPHINX_DOORTHD = 295,
	SFX_TR4_SETT_PLASMA_1 = 296,
	SFX_TR4_SETT_BOLT_1 = 297,
	SFX_TR4_SETT_FEET = 298,
	SFX_TR4_SETT_NRG_CHARGE = 299,
	SFX_TR4_SETT_NRG_CHARGE2 = 300,
	SFX_TR4_HORSEMAN_TAKEHIT = 301,
	SFX_TR4_HORSEMAN_WALK = 302,
	SFX_TR4_HORSEMAN_GRUNT = 303,
	SFX_TR4_HORSEMAN_FALL = 304,
	SFX_TR4_HORSEMAN_DIE = 305,
	SFX_TR4_MAPPER_SWITCH_ON = 306,
	SFX_TR4_MAPPER_OPEN = 307,
	SFX_TR4_MAPPER_LAZER = 308,
	SFX_TR4_MAPPER_MOVE = 309,
	SFX_TR4_MAPPER_CLUNK = 310,
	SFX_TR4_BLADES_DRAW = 311,
	SFX_TR4_BLADES_CLASH_LOUD = 312,
	SFX_TR4_BLADES_CLASH_QUIET = 313,
	SFX_TR4_HAMMER_TRAP_BANG = 314,
	SFX_TR4_DOOR_BIG_STONE = 315,
	SFX_TR4_SETT_BIG_ROAR = 316,
	SFX_TR4_BABOON_CHATTER = 317,
	SFX_TR4_BABOON_ROLL = 318,
	SFX_TR4_SWOOSH_SWIRLY_DOUBLE = 319,
	SFX_TR4_DOOR_SETTDOOR_SQK = 320,
	SFX_TR4_DOOR_SETTDOOR_CLANK = 321,
	SFX_TR4_SETT_JUMP_ATTACK = 322,
	SFX_TR4_JOBY_BLOCK = 323,
	SFX_TR4_SETT_TAKE_HIT = 324,
	SFX_TR4_DART_SPITT = 325,
	SFX_TR4_LARA_CROWBAR_GEM = 326,
	SFX_TR4_CROWBAR_DOOR_OPEN = 327,
	SFX_TR4_LARA_LEVER_GEN_SQKS = 328,
	SFX_TR4_HORSEMAN_GETUP = 329,
	SFX_TR4_EXH_BASKET_OPEN = 330,
	SFX_TR4_EXH_MUMCOFF_OPE1 = 331,
	SFX_TR4_EXH_MUMCOFF_OPE2 = 332,
	SFX_TR4_EXH_MUM_JOLT = 333,
	SFX_TR4_EXH_MUMHEAD_SPIN = 334,
	SFX_TR4_EXH_MUMMY_RAHHH = 335,
	SFX_TR4_EXH_ROLLER_BLINDS = 336,
	SFX_TR4_LARA_LEVER_PART1 = 337,
	SFX_TR4_LARA_LEVER_PART2 = 338,
	SFX_TR4_LARA_POLE_CLIMB = 339,
	SFX_TR4_LARA_POLE_LOOP = 340,
	SFX_TR4_TRAP_SPIKEBALL_SPK = 341,
	SFX_TR4_LARA_PULLEY = 342,
	SFX_TR4_TEETH_SPIKES = 343,
	SFX_TR4_SAND_LOOP = 344,
	SFX_TR4_LARA_USE_OBJECT = 345,
	SFX_TR4_LIBRARY_COG_SQKS = 346,
	SFX_TR4_HIT_ROCK = 347,
	SFX_TR4_LARA_NO_FRENCH = 348,
	SFX_TR4_LARA_NO_JAPAN = 349,
	SFX_TR4_LARA_CROW_WRENCH = 350,
	SFX_TR4_LARA_ROPE_CREAK = 351,
	SFX_TR4_BOWLANIM = 352,
	SFX_TR4_SPHINX_DOOR_WOODCRACK = 353,
	SFX_TR4_BEETLE_CLK_WHIRR2 = 354,
	SFX_TR4_MAPPER_PYRAMID_OPEN = 355,
	SFX_TR4_LIGHT_BEAM_JOBY = 356,
	SFX_TR4_GUIDE_FIRE_LIGHT = 357,
	SFX_TR4_AUTOGUNS = 358,
	SFX_TR4_PULLEY_ANDY = 359,
	SFX_TR4_STEAM = 360,
	SFX_TR4_JOBY_GARAGE_DOOR = 361,
	SFX_TR4_JOBY_WIND = 362,
	SFX_TR4_SANDHAM_IN_THE_HOUSE = 363,
	SFX_TR4_SANDHAM_CONVEYS = 364,
	SFX_TR4_CRANKY_GRAPE_CRUSH = 365,
	SFX_TR4_BIKE_HIT_OBJECTS = 366,
	SFX_TR4_BIKE_HIT_ENEMIES = 367,
	SFX_TR4_FLAME_EMITTER = 368,
	SFX_TR4_LARA_CLICK_SWITCH = 369,
	SFX_LARA_FOOTSTEPS_ICE = 370,
	SFX_LARA_FOOTSTEPS_SNOW = 371,
	SFX_LARA_HK_SILENCED = 372,
	SFX_LARA_HARPOON_LOAD_LAND = 373,
	SFX_LARA_HARPOON_FIRE_LAND = 374,
	SFX_LARA_HARPOON_LOAD_WATER = 375,
	SFX_LARA_HARPOON_FIRE_WATER = 376,
	SFX_BINOCULARS_ZOOM = 377,
	SFX_FLARE_BURN_LAND = 378,
	SFX_FLARE_IGNITE_LAND = 379,
	SFX_FLARE_IGNITE_WATER = 380,
	SFX_FLARE_BURN_WATER = 381,
		//Tomb Raider 1 Sounds
	SFX_TR1_BEAR_GROWL = 382,
	SFX_TR1_BEAR_FEET = 383,
	SFX_TR1_BEAR_ATTACK = 384,
	SFX_TR1_BEAR_SNARL = 385,
	SFX_TR1_BEAR_HURT = 386,
	SFX_TR1_BEAR_DEATH = 387,
	SFX_TR1_BAT_SQUEAK = 388,
	SFX_TR1_BAT_FLAP = 389,
	SFX_TR1_STONE_DOOR = 390,
	SFX_TR1_PENDULUM_BLADES = 391,
	SFX_TR1_TREX_DEATH = 392,
	SFX_TR1_TREX_FOOTSTOMP = 393,
	SFX_TR1_TREX_ROAR = 394,
	SFX_TR1_TREX_ATTACK = 395,
	SFX_TR1_WOLF_JUMP = 396,
	SFX_TR1_WOLF_HURT = 397,
	SFX_TR1_WOLF_DEATH = 398,
	SFX_TR1_WOLF_HOWL = 399,
	SFX_TR1_WOLF_ATTACK = 400,
	SFX_TR1_RAPTOR_ROAR = 401,
	SFX_TR1_RAPTOR_ATTACK = 402,
	SFX_TR1_RAPTOR_FEET = 403,
	SFX_TR1_CHAINDOOR_UP = 404,
	SFX_TR1_CHAINDOOR_DOWN = 405,
	SFX_TR1_COG_PUZZLE = 406,
	SFX_TR1_LION_HURT = 407,
	SFX_TR1_LION_ATTACK = 408,
	SFX_TR1_LION_ROAR = 409,
	SFX_TR1_LION_DEATH = 410,
	SFX_TR1_GORILLA_FEET = 411,
	SFX_TR1_GORILLA_PANT = 412,
	SFX_TR1_GORILLA_DEATH = 413,
	SFX_TR1_GORILLA_ROAR = 414,
	SFX_TR1_GORILLA_GRUNTS = 415,
	SFX_TR1_CROC_FEET = 416,
	SFX_TR1_CROC_ATTACK = 417,
	SFX_TR1_CROC_DEATH = 418,
	SFX_TR1_RAT_FEET = 419,
	SFX_TR1_RAT_CHIRP = 420,
	SFX_TR1_RAT_ATTACK = 421,
	SFX_TR1_RAT_DEATH = 422,
	SFX_TR1_ATLANTEAN_EXPLODE = 423,
	SFX_TR1_ATLANTEAN_SNEAK = 424,
	SFX_TR1_ATLANTEAN_ATTACK = 425,
	SFX_TR1_ATLANTEAN_JUMP_ATTACK = 426,
	SFX_TR1_ATLANTEAN_NEEDLE = 427,
	SFX_TR1_ATLANTEAN_BALL = 428,
	SFX_TR1_ATLANTEAN_WINGS = 429,
	SFX_TR1_ATLANTEAN_FEET = 430,
	SFX_TR1_ATLANTEAN_EGG_LOOP = 431,
	SFX_TR1_ATLANTEAN_EGG_HATCH = 432,
	SFX_TR1_ATLANTEAN_DEATH = 433,
	SFX_TR1_CENTAUR_HOOVES = 434,
	SFX_TR1_CENTAUR_ROAR = 435,
	SFX_TR1_GIANT_MUTANT_HIT_GROUND = 436,
	SFX_TR1_GIANT_MUTANT_ATTACK1 = 437,
	SFX_TR1_GIANT_MUTANT_ATTACK2 = 438,
	SFX_TR1_GIANT_MUTANT_DEATH = 439,
	SFX_TR1_GIANT_MUTANT_ARM_SWING = 440,
	SFX_TR1_GIANT_MUTANT_MOVE = 441,
	SFX_TR1_GIANT_MUTANT_HIT = 442,
	SFX_TR1_MUMMY_GROWL = 443,
	SFX_TR1_LARSON_FIRE = 444,
	SFX_TR1_LARSON_RICOCHET = 445,
	SFX_TR1_SKATEBOARDKID_MOVE = 446,
	SFX_TR1_SKATEBOARDKID_STOP = 447,
	SFX_TR1_SKATEBOARDKID_SHOOT = 448,
	SFX_TR1_SKATEBOARDKID_HIT = 449,
	SFX_TR1_SKATEBOARDKID_START = 450,
	SFX_TR1_SKATEBOARDKID_DEATH = 451,
	SFX_TR1_SKATEBOARDKID_HIT_GROUND = 452,
	SFX_TR1_THOR_ROOM_THUNDER = 453,
	SFX_TR1_EXPLOSION = 454,
	SFX_TR1_DAMOCLES_ROOM_SWORD = 455,
	SFX_TR1_RAISINGBLOCK_FX = 456,
	SFX_TR1_SAND_FX = 457,
	SFX_TR1_STAIRS2SLOPE_FX = 458,
	SFX_TR1_LAVA_LOOP = 459,
	SFX_TR1_LAVA_FOUNTAIN = 460,
	SFX_TR1_SCION_ALTAR_LOOP = 461,
	SFX_TR1_SCION_POWERUP_FX = 462,
	SFX_TR1_NM_DRILL_ENGINE_START = 463,
	SFX_TR1_NM_DRILL_ENGINE_LOOP = 464,
	SFX_TR1_NM_CONVEYOR_BELT = 465,
	SFX_TR1_NM_HUT_LOWERED = 466,
	SFX_TR1_NM_HUT_HIT_GROUND = 467,
	SFX_TR1_empty_1 = 468,
	SFX_TR1_METAL_DOOR_OPEN = 469,
	SFX_TR1_METAL_DOOR_CLOSE = 470,
	SFX_TR1_PIERRE_DEATH = 471,
	SFX_TR1_PIERRE_SPEECH = 472,
	SFX_TR1_PIERRE_MAGNUM_FIRE = 473,
		//Tomb Raider 2 Sounds
	SFX_TR2_LARA_M16_FIRE = 474,
	SFX_TR2_LARA_M16_STOP = 475,
	SFX_TR2_HENCHMAN_BELT_JINGLE = 476,
	SFX_TR2_HENCHMAN_CHUCKLE = 477,
	SFX_TR2_HENCHMAN_CLIMBDOWN = 478,
	SFX_TR2_HENCHMAN_AH_DYING = 479,
	SFX_TR2_HENCHMAN_CLIMBUP = 480,
	SFX_TR2_HENCHMAN_LOADING_SHOTGUN = 481,
	SFX_TR2_HENCHMAN_DEATH1 = 482,
	SFX_TR2_HENCHMAN_DEATH2 = 483,
	SFX_TR2_HENCHMAN_DEATH3 = 484,
	SFX_TR2_HENCHMAN_FEET = 485,
	SFX_TR2_HENCHMAN_FEET_SNOW = 486,
	SFX_TR2_HENCHMAN_FIRE_SEMIAUTO = 487,
	SFX_TR2_HENCHMAN_FIRE_SILENCER = 488,
	SFX_TR2_HENCHMAN_FIRE_TWIRL = 489,
	SFX_TR2_HENCHMAN_FIRE1 = 490,
	SFX_TR2_HENCHMAN_FIRE2 = 491,
	SFX_TR2_HENCHMAN_FLAMETHROWER_CLICK = 492,
	SFX_TR2_HENCHMAN_FLAMETHROWER_DEATH = 493,
	SFX_TR2_HENCHMAN_FLAMETHROWER_FIRE = 494,
	SFX_TR2_HENCHMAN_FLAMETHROWER_SCRAPE = 495,
	SFX_TR2_HENCHMAN_GRUNT = 496,
	SFX_TR2_HENCHMAN_GUN_COCKING = 497,
	SFX_TR2_HENCHMAN_HEAVY_BREATH = 498,
	SFX_TR2_HENCHMAN_HEELS = 499,
	SFX_TR2_HENCHMAN_HIT1 = 500,
	SFX_TR2_HENCHMAN_HIT2 = 501,
	SFX_TR2_HENCHMAN_HOLSTER = 502,
	SFX_TR2_HENCHMAN_JUMP = 503,
	SFX_TR2_HENCHMAN_OOH_DYING = 504,
	SFX_TR2_HENCHMAN_THUMP = 505,
	SFX_TR2_HENCHMAN_WRENCH = 506,
	SFX_TR2_BIG_SPIDER_FEET = 507,
	SFX_TR2_BIG_SPIDER_SNARL = 508,
	SFX_TR2_BIG_SPIDER_DEATH = 509,
	SFX_TR2_SPIDER_JUMP = 510,
	SFX_TR2_SPIDER_MOVING = 511,
	SFX_TR2_SPIDER_BITE = 512,
	SFX_TR2_SPIDER_EXPLODE = 513,
	SFX_TR2_LEOPARD_TIGER_FEET = 514,
	SFX_TR2_LEOPARD_ROAR = 515,
	SFX_TR2_LEOPARD_BITE = 516,
	SFX_TR2_LEOPARD_STRIKE = 517,
	SFX_TR2_LEOPARD_DEATH = 518,
	SFX_TR2_LEOPARD_GROWL = 519,
	SFX_TR2_TIGER_ROAR = 520,
	SFX_TR2_TIGER_BITE = 521,
	SFX_TR2_TIGER_STRIKE = 522,
	SFX_TR2_TIGER_DEATH = 523,
	SFX_TR2_TIGER_GROWL = 524,
	SFX_TR2_DOG_FEET = 525,
	SFX_TR2_DOG_BARK1 = 526,
	SFX_TR2_DOG_BARK2 = 527,
	SFX_TR2_DOG_PANT = 528,
	SFX_TR2_DOG_DEATH = 529,
	SFX_TR2_DOG_GROWL = 530,
	SFX_TR2_RAT_ATTACK = 531,
	SFX_TR2_RAT_DEATH = 532,
	SFX_TR2_EAGLE_SQUAWK = 533,
	SFX_TR2_EAGLE_WING_FLAP = 534,
	SFX_TR2_EAGLE_DEATH = 535,
	SFX_TR2_3_CROW_CAW = 536,
	SFX_TR2_3_CROW_WING_FLAP = 537,
	SFX_TR2_3_CROW_DEATH = 538,
	SFX_TR2_3_CROW_ATTACK = 539,
	SFX_TR2_YETI_CHEST_BEAT = 540,
	SFX_TR2_YETI_FEET = 541,
	SFX_TR2_YETI_GROWL = 542,
	SFX_TR2_YETI_GROWL1 = 543,
	SFX_TR2_YETI_GROWL2 = 544,
	SFX_TR2_YETI_GROWL3 = 545,
	SFX_TR2_YETI_GRUNT1 = 546,
	SFX_TR2_YETI_GRUNT2 = 547,
	SFX_TR2_YETI_SCREAM = 548,
	SFX_TR2_YETI_THUMP = 549,
	SFX_TR2_YETI_DEATH = 550,
	SFX_TR2_BIRD_MONSTER_SCREAM = 551,
	SFX_TR2_BIRD_MONSTER_GASP = 552,
	SFX_TR2_BIRD_MONSTER_BREATH = 553,
	SFX_TR2_BIRD_MONSTER_FEET = 554,
	SFX_TR2_BIRD_MONSTER_DEATH = 555,
	SFX_TR2_BIRD_MONSTER_SCRAPE = 556,
	SFX_TR2_DRAGON_FEET = 557,
	SFX_TR2_DRAGON_GROWL1 = 558,
	SFX_TR2_DRAGON_GROWL2 = 559,
	SFX_TR2_DRAGON_GROWL3 = 560,
	SFX_TR2_DRAGON_BREATH = 561,
	SFX_TR2_DRAGON_GRUNT = 562,
	SFX_TR2_DRAGON_FIRE = 563,
	SFX_TR2_DRAGON_LEG_LIFT = 564,
	SFX_TR2_DRAGON_LEG_HIT = 565,
	SFX_TR2_MONK_BREATH = 566,
	SFX_TR2_MONK_TAKEHIT = 567,
	SFX_TR2_MONK_DEATH = 568,
	SFX_TR2_MONK_FEET = 569,
	SFX_TR2_MONK_OYE = 570,
	SFX_TR2_MONK_POY = 571,
	SFX_TR2_MONK_SHOUT1 = 572,
	SFX_TR2_MONK_SHOUT2 = 573,
	SFX_TR2_MONK_SHOUT3 = 574,
	SFX_TR2_MONK_SHOUT4 = 575,
	SFX_TR2_MONK_SINGLE_SWORD_SWING = 576,
	SFX_TR2_MONK_MULTI_SWORD_SWING = 577,
	SFX_TR2_WARRIOR_BLADE_SWING_FAST = 578,
	SFX_TR2_WARRIOR_BLADE_SWING1 = 579,
	SFX_TR2_WARRIOR_BLADE_SWING2 = 580,
	SFX_TR2_WARRIOR_BREATH_ACTIVE = 581,
	SFX_TR2_WARRIOR_GROWL1 = 582,
	SFX_TR2_WARRIOR_GROWL2 = 583,
	SFX_TR2_WARRIOR_GROWL3 = 584,
	SFX_TR2_WARRIOR_HOVER = 585,
	SFX_TR2_WARRIOR_LANDING = 586,
	SFX_TR2_WARRIOR_SWORD_SLICE = 587,
	SFX_TR2_WARRIOR_WAKE = 588,
	SFX_TR2_WARRIOR_BURP = 589,
	SFX_TR2_3_SCUBA_DIVER_ARM = 590,
	SFX_TR2_3_SCUBA_DIVER_BREATH_LAND = 591,
	SFX_TR2_3_SCUBA_DIVER_BREATH_WATER = 592,
	SFX_TR2_3_SCUBA_DIVER_DEATH = 593,
	SFX_TR2_3_SCUBA_DIVER_DIVING = 594,
	SFX_TR2_3_SCUBA_DIVER_FLIPPER = 595,
	SFX_TR2_SHARK_BITE = 596,
	SFX_TR2_3_WINSTON_BRUSH_OFF = 597,
	SFX_TR2_3_WINSTON_BULLET_TRAY = 598,
	SFX_TR2_3_WINSTON_CUPS = 599,
	SFX_TR2_3_WINSTON_FLATULENCE = 600,
	SFX_TR2_3_WINSTON_FOOTSTEPS = 601,
	SFX_TR2_3_WINSTON_GET_UP = 602,
	SFX_TR2_3_WINSTON_SURPRISED = 603,
	SFX_TR2_3_WINSTON_TAKE_HIT = 604,
	SFX_TR2_3_WINSTON_GRUNT1 = 605,
	SFX_TR2_3_WINSTON_GRUNT2 = 606,
	SFX_TR2_3_WINSTON_GRUNT3 = 607,
	SFX_TR2_3_WINSTON_SHUFFLE = 608,
	SFX_TR2_SPEEDBOAT_ACCELERATE = 609,
	SFX_TR2_SPEEDBOAT_ENGINE = 610,
	SFX_TR2_SPEEDBOAT_HIT = 611,
	SFX_TR2_SPEEDBOAT_INTO_WATER = 612,
	SFX_TR2_SPEEDBOAT_IDLE = 613,
	SFX_TR2_SPEEDBOAT_MOVING = 614,
	SFX_TR2_SPEEDBOAT_SLOWDOWN = 615,
	SFX_TR2_SPEEDBOAT_START = 616,
	SFX_TR2_SPEEDBOAT_STOP = 617,
	SFX_TR2_SPEEDBOAT_ENGINE_HIT = 618,
	SFX_TR2_SNOWMOBILE_ACCELERATE = 619,
	SFX_TR2_SNOWMOBILE_IDLE = 620,
	SFX_TR2_SNOWMOBILE_MOVING = 621,
	SFX_TR2_SNOWMOBILE_START = 622,
	SFX_TR2_SNOWMOBILE_STOP = 623,
	SFX_TR2_ZIPLINE_GO = 624,
	SFX_TR2_ZIPLINE_GRAB = 625,
	SFX_TR2_ZIPLINE_STOP = 626,
	SFX_TR2_JUMP_PAD_UP = 627,
	SFX_TR2_JUMP_PAD_DOWN = 628,
	SFX_TR2_KNIFETHROWER_FEET = 629,
	SFX_TR2_KNIFETHROWER_HICCUP = 630,
	SFX_TR2_KNIFETHROWER_WARRIOR_FEET = 631,
	SFX_TR2_MINISUB_VEHICLE_IMPACT1 = 632,
	SFX_TR2_MINISUB_VEHICLE_IMPACT2 = 633,
	SFX_TR2_MINISUB_VEHICLE_IMPACT3 = 634,
	SFX_TR2_HAND_WHEEL_LOOSE = 635,
	SFX_TR2_HAND_WHEEL_TURN = 636,
	SFX_TR2_HAND_WHEEL_OPEN = 637,
	SFX_TR2_HAND_WHEEL_CREAK = 638,
	SFX_TR2_VEHICLE_IMPACT_1 = 639,
	SFX_TR2_VEHICLE_IMPACT_2 = 640,
	SFX_TR2_VEHICLE_IMPACT_3 = 641,
	SFX_TR2_CRUNCH1 = 642,
	SFX_TR2_CRUNCH2 = 643,
	SFX_TR2_CLICK = 644,
	SFX_TR2_GLASS_BREAK = 645,
	SFX_TR2_SWORD_STATUE_DROP = 646,
	SFX_TR2_SWORD_STATUE_LIFT = 647,
	SFX_TR2_SANDBAG_SNAP = 648,
	SFX_TR2_SANDBAG_HIT = 649,
	SFX_TR2_AIRPLANE_IDLE = 650,
	SFX_TR2_BIRDS_CHIRP = 651,
	SFX_TR2_BOWL_POUR = 652,
	SFX_TR2_BOWL_TIPPING = 653,
	SFX_TR2_BURGLAR_ALARM = 654,
	SFX_TR2_BURGLARS = 655,
	SFX_TR2_CHAIN_PULLEY = 656,
	SFX_TR2_CIRCLE_BLADE = 657,
	SFX_TR2_CIRCLE_BLADE_HIT = 658,
	SFX_TR2_DOORBELL = 659,
	SFX_TR2_LIFT_OPEN = 660,
	SFX_TR2_LIFT_CLOSE = 661,
	SFX_TR2_SPEEDBOAT_BODY_SLUMP = 662,
	SFX_TR2_CURTAIN = 663,
	SFX_TR2_DRIPS_REVERB = 664,
	SFX_TR2_3_HELICOPTER_LOOP = 665,
	SFX_TR2_ICILE_DETACH = 666,
	SFX_TR2_ICICLE_HIT = 667,
	SFX_TR2_MASSIVE_CRASH = 668,
	SFX_TR2_PLATFORM_ALARM = 669,
	SFX_TR2_PULLEY_CRANE = 670,
	SFX_TR2_ROLLING_BLADE = 671,
	SFX_TR2_SAW_STOP = 672,
	SFX_TR2_SMALL_FAN_ON = 673,
	SFX_TR2_SNOWBALL_ROLL = 674,
	SFX_TR2_SNOWBALL_STOP = 675,
	SFX_TR2_STAGE_BACKDROP = 676,
	SFX_TR2_ZIPPER = 677,
	SFX_TR2_SAW_REVVING = 678,
	SFX_TR2_SWING = 679,
	SFX_TR2_SLAM_DOOR_SLIDE = 680,
	SFX_TR2_SLAM_DOOR_CLOSE = 681,
	SFX_TR2_SWINGING = 682,
	SFX_TR2_SWINGING_TRAP = 683,
		//Tomb Raider 3 Sounds
	SFX_TR3_TONY_ATTACK = 684,
	SFX_TR3_TONY_LAUGH = 685,
	SFX_TR3_TONY_NORMAL_DEATH = 686,
	SFX_TR3_TONY_SHOOT_1 = 687,
	SFX_TR3_TONY_SHOOT_2 = 688,
	SFX_TR3_TONY_SHOOT_3 = 689,
	SFX_TR3_TONY_ARTEFACT_DEATH = 690,
	SFX_TR3_PUNA_BOSS_ATTACK = 691,
	SFX_TR3_PUNA_BOSS_CHAIR_2 = 692,
	SFX_TR3_PUNA_BOSS_DEATH = 693,
	SFX_TR3_PUNA_BOSS_SHOOT = 694,
	SFX_TR3_PUNA_BOSS_TAKE_HIT = 695,
	SFX_TR3_PUNA_BOSS_TURN_CHAIR = 696,
	SFX_TR3_SOFIALEE_DEATH_PART_1 = 697,
	SFX_TR3_SOFIALEE_DEATH_PART_2 = 698,
	SFX_TR3_SOFIALEE_FIRE = 699,
	SFX_TR3_SOFIALEE_LAUGH = 700,
	SFX_TR3_SOFIALEE_SHOOTER = 701,
	SFX_TR3_SOFIALEE_SUMMON = 702,
	SFX_TR3_SOFIALEE_SUMMON_FAIL = 703,
	SFX_TR3_SOFIALEE_TAKE_HIT = 704,
	SFX_TR3_SOFIALEE_VAULT = 705,
	SFX_TR3_WILLARD_ATTACK = 706,
	SFX_TR3_WILLARD_FIRE_CHARGE = 707,
	SFX_TR3_WILLARD_FIRE_SHOOT = 708,
	SFX_TR3_WILLARD_FOOT_STEPS = 709,
	SFX_TR3_WILLARD_LEGS_SHUFFLE = 710,
	SFX_TR3_WILLARD_ODD_NOISE = 711,
	SFX_TR3_WILLARD_STAB = 712,
	SFX_TR3_WILLARD_TAKE_HIT = 713,
	SFX_TR3_BLAST_CIRCLE = 714,
	SFX_TR3_QUADBIKE_ACCELERATE = 715,
	SFX_TR3_QUADBIKE_FRONT_IMPACT = 716,
	SFX_TR3_QUADBIKE_IDLE = 717,
	SFX_TR3_QUADBIKE_LAND = 718,
	SFX_TR3_QUADBIKE_MOVE = 719,
	SFX_TR3_QUADBIKE_SIDE_IMPACT = 720,
	SFX_TR3_QUADBIKE_START = 721,
	SFX_TR3_QUADBIKE_STOP = 722,
	SFX_TR3_RUBBERBOAT_ACCELERATE = 723,
	SFX_TR3_RUBBERBOAT_IDLE = 724,
	SFX_TR3_RUBBERBOAT_MOVING = 725,
	SFX_TR3_RUBBERBOAT_SCRAPE = 726,
	SFX_TR3_RUBBERBOAT_SLOW_DOWN = 727,
	SFX_TR3_RUBBERBOAT_START = 728,
	SFX_TR3_RUBBERBOAT_STOP = 729,
	SFX_TR3_UPV_LOOP = 730,
	SFX_TR3_UPV_START = 731,
	SFX_TR3_UPV_STOP = 732,
	SFX_TR3_MINE_CART_START = 733,
	SFX_TR3_MINE_CART_PULLY_LOOP = 734,
	SFX_TR3_MINE_CART_BRAKE = 735,
	SFX_TR3_MINE_CART_TRACK_LOOP = 736,
	SFX_TR3_AMERCAN_HOY = 737,
	SFX_TR3_ARMY_SMG_FOOTSTEPS = 738,
	SFX_TR3_ARMY_SMG_FIRE = 739,
	SFX_TR3_ARMY_SMG_DEATH = 740,
	SFX_TR3_BLOWPIPE_NATIVE_ATTACK = 741,
	SFX_TR3_BLOWPIPE_NATIVE_BLOW = 742,
	SFX_TR3_BLOWPIPE_NATIVE_DEATH = 743,
	SFX_TR3_BLOWPIPE_NATIVE_FEET = 744,
	SFX_TR3_BLOWPIPE_NATIVE_SWOOSH = 745,
	SFX_TR3_THEDAMNED_ATTACK = 746,
	SFX_TR3_THEDAMNED_CLIMB = 747,
	SFX_TR3_THEDAMNED_DEATH = 748,
	SFX_TR3_THEDAMNED_FEET = 749,
	SFX_TR3_THEDAMNED_GET_DOWN = 750,
	SFX_TR3_CLAW_MUTANT_ATTACK = 751,
	SFX_TR3_CLAW_MUTANT_BODY_THUD = 752,
	SFX_TR3_CLAW_MUTANT_CLAW = 753,
	SFX_TR3_CLAW_MUTANT_DEATH = 754,
	SFX_TR3_CLAW_MUTANT_FOOTSTEPS = 755,
	SFX_TR3_CLAW_MUTANT_LASER = 756,
	SFX_TR3_CLAW_MUTANT_SWOOSH = 757,
	SFX_TR3_COBRA_HISS = 758,
	SFX_TR3_COMPY_ATTACK = 759,
	SFX_TR3_COMPY_DEATH = 760,
	SFX_TR3_COMPY_JUMP = 761,
	SFX_TR3_COMPY_RUN_WALK = 762,
	SFX_TR3_COMPY_WAIT = 763,
	SFX_TR3_HYBRID_ATTACK = 764,
	SFX_TR3_HYBRID_DEATH = 765,
	SFX_TR3_HYBRID_FOOT = 766,
	SFX_TR3_HYBRID_HOOF = 767,
	SFX_TR3_HYBRID_SWOOSH = 768,
	SFX_TR3_METEOR = 769,
	SFX_TR3_SEAL_MUTE_BREATH_IN = 770,
	SFX_TR3_SEAL_MUTANT_BRUSH_TAIL = 771,
	SFX_TR3_SEAL_MUTANT_DEATH = 772,
	SFX_TR3_SEAL_MUTANT_FIRE = 773,
	SFX_TR3_SEAL_MUTANT_FIRE_2 = 774,
	SFX_TR3_SEAL_MUTANT_FOOT = 775,
	SFX_TR3_SEAL_MUTANT_HIT_FLOOR = 776,
	SFX_TR3_WING_MUTANT_ATTACK = 777,
	SFX_TR3_WING_MUTANT_DEATH = 778,
	SFX_TR3_WING_MUTANT_FLYING = 779,
	SFX_TR3_MUTANT_GASSED = 780,
	SFX_TR3_SHIVA_DEATH = 781,
	SFX_TR3_SHIVA_FOOTSTEP = 782,
	SFX_TR3_SHIVA_HIT_GROUND = 783,
	SFX_TR3_SHIVA_LAUGH = 784,
	SFX_TR3_SHIVA_SWORD_1 = 785,
	SFX_TR3_SHIVA_SWORD_2 = 786,
	SFX_TR3_SHIVA_WALK_MURMA = 787,
	SFX_TR3_ENGLISH_HOY = 788,
	SFX_TR3_LIZARD_MAN_ATTACK_1 = 789,
	SFX_TR3_LIZARD_MAN_ATTACK_2 = 790,
	SFX_TR3_LIZARD_MAN_CLIMB = 791,
	SFX_TR3_LIZARD_MAN_DEATH = 792,
	SFX_TR3_LIZARD_MAN_FIRE = 793,
	SFX_TR3_RATTLE_SNAKE = 794,
	SFX_TR3_TIGER_BITE = 795,
	SFX_TR3_TIGER_DEATH = 796,
	SFX_TR3_TIGER_GROWL = 797,
	SFX_TR3_TIGER_ROAR = 798,
	SFX_TR3_TIGER_STRIKE = 799,
	SFX_TR3_WHALE_CALL = 800,
	SFX_TR3_COMMANDER = 801,
	SFX_TR3_ALARM_1 = 802,
	SFX_TR3_AREA51_SWINGER_LOOP = 803,
	SFX_TR3_AREA51_SWINGER_START = 804,
	SFX_TR3_AREA51_SWINGER_STOP = 805,
	SFX_TR3_DART_GUN = 806,
	SFX_TR3_DART_SPITT = 807,
	SFX_TR3_LONDON_MERCENARY_DEATH = 808,
	SFX_TR3_MACAQUE_ATTACK_JUMP = 809,
	SFX_TR3_MACAQUE_ATTACK_LOW = 810,
	SFX_TR3_MACAQUE_CHATTER = 811,
	SFX_TR3_MACAQUE_DEATH = 812,
	SFX_TR3_MACAQUE_JUMP = 813,
	SFX_TR3_MACAQUE_ROLL = 814,
	SFX_TR3_MACAQUE_SAT_AND_WAIT = 815,
	SFX_TR3_PUNK_ATTACK = 816,
	SFX_TR3_PUNK_DEATH = 817,
	SFX_TR3_RAPTOR_ATTACK_1 = 818,
	SFX_TR3_RAPTOR_ATTACK_2 = 819,
	SFX_TR3_RAPTOR_ATTACK_3 = 820,
	SFX_TR3_RAPTOR_DIE_1 = 821,
	SFX_TR3_RAPTOR_DIE_2 = 822,
	SFX_TR3_RAPTOR_FEET = 823,
	SFX_TR3_RAPTOR_ROAR = 824,
	SFX_TR3_VULTURE_ATTACK = 825,
	SFX_TR3_VULTURE_DEATH = 826,
	SFX_TR3_VULTURE_GLIDE = 827,
	SFX_TR3_VULTURE_WING_FLAP = 828,
	SFX_TR3_SECURITY_GUARD_DEATH = 829,
	SFX_TR3_SECURITY_GUARD_FIRE = 830,
	SFX_TR3_1ST_LOOP_FOR_BIG_DRILL = 831,
	SFX_TR3_2ND_LOOP_FOR_BIG_DRILL = 832,
	SFX_TR3_BURGLAR_ALARM = 833,
	SFX_TR3_CITY_PORTCULLIS = 834,
	SFX_TR3_CLEANER_FUSEBOX = 835,
	SFX_TR3_CLEANER_LOOP = 836,
	SFX_TR3_CRICKET_LOOP = 837,
	SFX_TR3_USER_838 = 838,
	SFX_TR3_DOORBELL = 839,
	SFX_TR3_DRILL_BIT_1 = 840,
	SFX_TR3_GASMETER = 841,
	SFX_TR3_GATE_OPENING = 842,
	SFX_TR3_GENERATOR_LOOP = 843,
	SFX_TR3_GENERATOR_BREAKING = 844,
	SFX_TR3_GIANT_METAL_WHEELS = 845,
	SFX_TR3_JET_FLY_BY = 846,
	SFX_TR3_JET_ROOFS = 847,
	SFX_TR3_LASER_LOOP = 848,
	SFX_TR3_LOOP_FOR_GAS_HISS = 849,
	SFX_TR3_LONDON_AMBIENCE_LOOP = 850,
	SFX_TR3_METAL_SHUTTERS_SMASH = 851,
	SFX_TR3_OIL_RED_SMG_DEATH = 852,
	SFX_TR3_OIL_SMG_FIRE = 853,
	SFX_TR3_OILDRUM_ROLL = 854,
	SFX_TR3_PLUG_WINCH = 855,
	SFX_TR3_PORTCULLIS_DOWN = 856,
	SFX_TR3_PORTCULLIS_UP = 857,
	SFX_TR3_POWER_HUM_LOOP = 858,
	SFX_TR3_USER_859 = 859,
	SFX_TR3_RADAR_BLIP = 860,
	SFX_TR3_RADIO_LOOP = 861,
	SFX_TR3_RESERVOIR_FLUSH = 862,
	SFX_TR3_SAVE_CRYSTAL = 863,
	SFX_TR3_SIDE_BLADE_BACK = 864,
	SFX_TR3_SIDE_BLADE_SWING = 865,
	SFX_TR3_SKEL_TRAP_PART_1 = 866,
	SFX_TR3_SKEL_TRAP_PART_2 = 867,
	SFX_TR3_USER_868 = 868,
	SFX_TR3_USER_869 = 869,
	SFX_TR3_SMALL_DOOR_SUBWAY = 870,
	SFX_TR3_SMALL_DOOR_SUBWAY_CLOSE = 871,
	SFX_TR3_SMALL_FAN = 872,
	SFX_TR3_USER_873 = 873,
	SFX_TR3_SMALL_METAL_SHUTTERS = 874,
	SFX_TR3_SMALL_SWITCH = 875,
	SFX_TR3_SPANNER = 876,
	SFX_TR3_SPINING_HOOKS = 877,
	SFX_TR3_SQUEAK = 878,
	SFX_TR3_STALEGTITE = 879,
	SFX_TR3_SWAT_SMG_FIRE = 880,
	SFX_TR3_SWING_PUMP = 881,
	SFX_TR3_SWINGING_FLAMES = 882,
	SFX_TR3_USER_883 = 883,
	SFX_TR3_SWITCH_COVER = 884,
	SFX_TR3_TARGET_HITS = 885,
	SFX_TR3_TARGET_SMASH = 886,
	SFX_TR3_TICK_TOCK = 887,
	SFX_TR3_TICKET_BARRIER = 888,
	SFX_TR3_TONK = 889,
	SFX_TR3_TUBE_LOOP = 890,
	SFX_TR3_UNDERWATER_FAN_ON = 891,
	SFX_TR3_USER_892 = 892,
	SFX_TR3_VENDING_MACHINE_LOOP = 893,
	SFX_TR3_VENDING_SPIT = 894,
	SFX_TR3_VERY_SMALL_WINCH = 895,
	SFX_TR3_WALL_BLADES = 896,
	SFX_TR3_WATER_MILL = 897,
	//Tomb Raider Chronicles Sounds
	SFX_TR5_INSIDEMILL = 898,
	SFX_TR5_2GUNTEX_LASER_START = 899,
	SFX_TR5_2GUNTEX_STAIR_FALL = 900,
	SFX_TR5_AGENT_HITMAN_FEET = 901,
	SFX_TR5_AGENT_HITMAN_JUMP = 902,
	SFX_TR5_AGENT_HITMAN_LAND = 903,
	SFX_TR5_SCIENTIST_DONT_SHOOT = 904 ,
	SFX_TR5_VCI_BLUEMAN_GUNS = 905 ,
	SFX_TR5_VCI_BLUEMAN_CHAIRHIT = 906 ,
	SFX_TR5_VCI_BLUEMAN_GETUP = 907 ,
	SFX_TR5_VCI_BLUEMAN_HITDIE = 908 ,
	SFX_TR5_VCI_BLUEMAN_JUMP = 909 ,
	SFX_TR5_VCI_BLUEMAN_CHAIR = 910 ,
	SFX_TR5_VCI_BLUEMAN_CONSOLE = 911 ,
	SFX_TR5_CHEF_ATTACK = 912 ,
	SFX_TR5_HYDRA_ATTACK = 913 ,
	SFX_TR5_HYDRA_BREATHE = 914,
	SFX_TR5_HYDRA_BREATHEOUT = 915 ,
	SFX_TR5_HYDRA_FIRE = 916 ,
	SFX_TR5_xTOR_ATTACK = 917 ,
	SFX_TR5_GLADIATOR_DEATH = 918 ,
	SFX_TR5_GLADIATOR_FEET = 919 ,
	SFX_TR5_GLADIATOR_SHIELD1 = 920 ,
	SFX_TR5_GLADIATOR_SHIELD2 = 921 ,
	SFX_TR5_GLADIATOR_SWORD = 922 ,
	SFX_TR5_GOD_HEAD_BLAST = 923 ,
	SFX_TR5_GOD_HEAD_CHARGE = 924 ,
	SFX_TR5_GOD_HEAD_LASERLOOPS = 925 ,
	SFX_TR5_GOD_HEAD_TENTACLES = 926 ,
	SFX_TR5_HAMMER_GOD_HAMMER_HIT = 927 ,
	SFX_TR5_HAMMER_GOD_HAMMER_MED = 928 ,
	SFX_TR5_HAMMER_GOD_HAMMER= 929 ,
	SFX_TR5_HAMMER_GOD_PULSE = 930 ,
	SFX_TR5_HANGMAN_LAUGH_OFFCAM = 931 ,
	SFX_TR5_HITMAN_CHOKE = 932 ,
	SFX_TR5_HITMAN_ELECSHORT = 933 ,
	SFX_TR5_HITMAN_GUNSEND = 934 ,
	SFX_TR5_HITMAN_GUNSFIRE = 935 ,
	SFX_TR5_IMP_ATTACK = 936 ,
	SFX_TR5_IMP_BARRELDROP = 937 ,
	SFX_TR5_IMP_BARRELROLL = 938 ,
	SFX_TR5_IMP_DEATH = 939 ,
	SFX_TR5_IMP_FALL = 940 ,
	SFX_TR5_IMP_FEET = 941 ,
	SFX_TR5_IMP_LAUGH = 942 ,
	SFX_TR5_IMP_RUNAWAY = 943 ,
	SFX_TR5_IMP_STONEHIT = 944 ,
	SFX_TR5_DIVE_SUIT_ENGINE = 945 ,
	SFX_TR5_DIVE_SUIT_HIT = 946 ,
	SFX_TR5_DIVE_SUIT_BREATHE = 947 ,
	SFX_TR5_DIVE_SUIT_LOOP = 948 ,
	SFX_TR5_MAFIA_DEATH = 949 ,
	SFX_TR5_MAFIA_GETDOWN = 950 ,
	SFX_TR5_MAFIA_HIT = 951 ,
	SFX_TR5_MAFIA_JUMP = 952 ,
	SFX_TR5_SWORD_GOD_SWORDDEATH1 = 953 ,
	SFX_TR5_SWORD_GOD_FALL = 954 ,
	SFX_TR5_SWORD_GOD_FEET = 955 ,
	SFX_TR5_SWORD_GOD_FEETQUIET = 956 ,
	SFX_TR5_SWORD_GOD_FEETVERYQUIET = 957 ,
	SFX_TR5_SWORD_GOD_HITMETAL = 958 ,
	SFX_TR5_SWORD_GOD_LASER = 959 ,
	SFX_TR5_SWORD_GOD_SCREAM = 960 ,
	SFX_TR5_SWORD_GOD_SWORD = 961 ,
	SFX_TR5_SHOCKWAVE_RUMBLE = 962 ,
	SFX_TR5_SKELETON_GHOST_APPEAR = 963 ,
	SFX_TR5_SKELETONGHOST_ATTACK = 964 ,
	SFX_TR5_SNIPER_RIFLE = 965 ,
	SFX_TR5_WEREWOLF_ATTACK1 = 966 ,
	SFX_TR5_WEREWOLF_ATTACK2 = 967 ,
	SFX_TR5_WEREWOLF_DEATH = 968 ,
	SFX_TR5_WEREWOLF_DROP = 969 ,
	SFX_TR5_WEREWOLF_HIND_FEET = 970 ,
	SFX_TR5_WEREWOLF_FORE_FEET = 971 ,
	SFX_TR5_WEREWOLF_JUMP = 972 ,
	SFX_TR5_WILLOW_WISP_LOOP = 973 ,
	SFX_TR5_SWAMPY_ATTACK = 974 ,
	SFX_TR5_LION_ATTACK = 975 ,
	SFX_TR5_LION_DEATH = 976 ,
	SFX_TR5_LION_FEET = 977 ,
	SFX_TR5_LION_GROWL = 978 ,
	SFX_TR5_LION_HIT_FLOOR = 979 ,
	SFX_TR5_GUARD_SNORE_INHALE = 980 ,
	SFX_TR5_GUARD_SNORE_EXHALE = 981 ,
	SFX_TR5_INSECTS = 982 ,
	SFX_TR5_KEYPAD_HASH = 983 ,
	SFX_TR5_KEYPAD_ASTERISK = 984 ,
	SFX_TR5_KEYPAD_0 = 985 ,
	SFX_TR5_KEYPAD_1 = 986 ,
	SFX_TR5_KEYPAD_2 = 987 ,
	SFX_TR5_KEYPAD_3 = 988 ,
	SFX_TR5_KEYPAD_4 = 989 ,
	SFX_TR5_KEYPAD_5 = 990 ,
	SFX_TR5_KEYPAD_6=  991 ,
	SFX_TR5_KEYPAD_7 = 992 ,
	SFX_TR5_KEYPAD_8 = 993 ,
	SFX_TR5_KEYPAD_9 = 994 ,
	SFX_TR5_KEYPAD_ENTRY_NO = 995 ,
	SFX_TR5_KEYPAD_ENTRY_YES = 996 ,
	SFX_TR5_GRILL_1 = 997 ,
	SFX_TR5_GRILL_2 = 998 ,
	SFX_TR5_GRILL_3 = 999 ,
	SFX_TR5_COMPUTER_BEEPIES = 1000 ,
	SFX_TR5_ELECTRIC_LIGHT_CRACKLES = 1001 ,
	SFX_TR5_ELECTRIC_WIRES = 1002 ,
	SFX_TR5_WATER_WHEEL = 1003 ,
	SFX_TR5_AUTO_GUN_DOOR = 1004 ,
	SFX_TR5_AUTO_GUN_UNFOLD = 1005 ,
	SFX_TR5_BARN_DOOR_NORMAL = 1006 ,
	SFX_TR5_BARN_DOOR_SLAM = 1007 ,
	SFX_TR5_BATTERING_RAM_CREAK = 1008 ,
	SFX_TR5_BELL = 1009 ,
	SFX_TR5_BIO_BREATHE_OUT = 1010 ,
	SFX_TR5_BOATMILL_1 = 1011 ,
	SFX_TR5_BOATMILL_2 = 1012 ,
	SFX_TR5_CABINETCLOSEWOOD = 1013 ,
	SFX_TR5_CABINETOPENWOOD = 1014 ,
	SFX_TR5_CABINETSHUT = 1015 ,
	SFX_TR5_COG = 1016 ,
	SFX_TR5_COGS_ROME = 1017 ,
	SFX_TR5_CRANE = 1018 ,
	SFX_TR5_SCIENTIST_SUIT_DOOR_END = 1019 ,
	SFX_TR5_CAGE_OPEN = 1020 ,
	SFX_TR5_METAL_KICK_OPEN = 1021 ,
	SFX_TR5_METAL_CROWBAR_DOOR = 1022 ,
	SFX_TR5_METAL_CROWBAR_DOOR_OPEN = 1023 ,
	SFX_TR5_METAL_OPEN = 1024 ,
	SFX_TR5_SWIPECARD_SFX = 1025 ,
	SFX_TR5_DRAWERS_METALCLOSE = 1026 ,
	SFX_TR5_DRAWERS_METALOPEN = 1027 ,
	SFX_TR5_DRAWERS_WOOD_CLOSE = 1028 ,
	SFX_TR5_DRAWERS_WOOD_OPEN = 1029 ,
	SFX_TR5_FIRE_EXTINGUISHER_RICOCHET = 1030 ,
	SFX_TR5_FIREDOOR = 1031 ,
	SFX_TR5_FISH_TANK_WATER = 1032 ,
	SFX_TR5_FLOOR_DOOR_A = 1033 ,
	SFX_TR5_FLOOR_DOOR_B = 1034 ,
	SFX_TR5_GEN_STONE_DOOR_LOW=1035,
	SFX_TR5_GENSWORD_SWOOSH_LOW=1036,
	SFX_TR5_GENERATOR_HUML_LOOP=1037,
	SFX_TR5_GENERIC_BOOTSTEPS=1038,
	SFX_TR5_GENERIC_KNEES_FALL=1039,
	SFX_TR5_GENERIC_SQK_SLOW=1040,
	SFX_TR5_GENERIC_SQKS_NONRND=1041,
	SFX_TR5_BASE_CLAW_DROP=1042,
	SFX_TR5_BASE_CLAW_CRASH=1043,
	SFX_TR5_BASE_CLAW_MOTOR_A =1044,
	SFX_TR5_BASE_CLAW_MOTOR_B_LOOP =1045,
	SFX_TR5_BASE_CLAW_MOTOR_C =1046,
	SFX_TR5_BASE_CLAW_TOP_IMPACT=1047,
	SFX_TR5_BASE_CLAW_WINCH_LOOP=1048,
	SFX_TR5_BASE_CLAW_HANGAR_DOOR_OPEN=1049,
	SFX_TR5_HISS_LOOP_SMALL=1050,
	SFX_TR5_HOLOGRAM=1051,
	SFX_TR5_INDUSTRY_AMBIENCE_LOOP=1052,
	SFX_TR5_IRIS_ELEC=1053,
	SFX_TR5_CRANE_END=1054,
	SFX_TR5_KITCHEN_HOB_LOOP=1055,
	SFX_TR5_KLAXON=1056,
	SFX_TR5_LASER_LOOP=1057,
	SFX_TR5_LIFT_BRAKES_SQUEAL=1058,
	SFX_TR5_LIFT_DOORS=1059,
	SFX_TR5_LIFT_HIT_FLOOR1=1060,
	SFX_TR5_LIFT_HIT_FLOOR2=1061,
	SFX_TR5_LIFT_MOVE=1062,
	SFX_TR5_LIFT_MOVE_BROKEN=1063,
	SFX_TR5_LOCKER_CLOSE=1064,
	SFX_TR5_LOCKER_OPEN=1065,
	SFX_TR5_LOW_RUMBLE_LOWER=1066,
	SFX_TR5_METAL_CABINET_CLOSE_1=1067,
	SFX_TR5_METAL_CABINET_CLOSE_2=1068,
	SFX_TR5_METAL_DOOR_OPEN_1=1069,
	SFX_TR5_METAL_DOOR_OPEN_2=1070,
	SFX_TR5_MODEM_LOOP=1071,
	SFX_TR5_OFFICE_DOOR_CLOSE=1072,
	SFX_TR5_OFFICE_DOOR_OPEN=1073,
	SFX_TR5_OFFICE_DOOR_SQUEAK=1074,
	SFX_TR5_PORTAL_LOOP=1075,
	SFX_TR5_POUR_DUST=1076,
	SFX_TR5_HITECH_ROOM_LOOP=1077,
	SFX_TR5_XRAY_ROOM_LOOP=1078,
	SFX_TR5_RATS=1079,
	SFX_TR5_RATS_SPLASH=1080,
	SFX_TR5_RAVEN_SWITCH_EXPLODE=1081,
	SFX_TR5_DOOR_BEAM=1082,
	SFX_TR5_LOWER_BEAM=1083,
	SFX_TR5_ENERGY_BEAM=1084,
	SFX_TR5_TARGET_RISE=1085,
	SFX_TR5_SEARCHER=1086,
	SFX_TR5_SERVO_01=1087,
	SFX_TR5_SERVO_02=1088,
	SFX_TR5_SHOWER_LOOP=1089,
	SFX_TR5_SLIDE_DOOR=1090,
	SFX_TR5_SMALL_METAL_VENT_HINGE=1091,
	SFX_TR5_SMASH_GLASS=1092,
	SFX_TR5_SMASH_METAL=1093,
	SFX_TR5_SMASH_ROCK=1094,
	SFX_TR5_SMASH_WOOD=1095,
	SFX_TR5_PHILOSPHERS_STONE=1096,
	SFX_TR5_STAIR_BANISTER_BEND=1097,
	SFX_TR5_STAIR_SNAP=1098,
	SFX_TR5_SUBCONTROL_ROOM_LOOP=1099,
	SFX_TR5_SWITCH_ELEC_SWAP=1100,
	SFX_TR5_TELEPORT=1101,
	SFX_TR5_TELEPORT_CRACKLES=1102,
	SFX_TR5_TELEPORT_FLASH=1103,
	SFX_TR5_THUNDER_RUMBLE=1104,
	SFX_TR5_WHITE_NOISE_LOOP=1105,
	SFX_TR5_UNDERWATER_CHAFF=1106,
	SFX_TR5_UNDERWATER_DOOR=1107,
	SFX_TR5_UNDERWATER_EXPLOSION=1108,
	SFX_TR5_UNDERWATER_FAN=1109,
	SFX_TR5_UNDERWATER_TORPEDO=1110,
	SFX_TR5_VCI_DOOR_CLOSE=1111,
	SFX_TR5_VCI_DOOR_OPEN=1112,
	SFX_TR5_VENT_IMPACT=1113,
	SFX_TR5_VENT_STRESS=1114,
	SFX_TR5_WALL_SUIT_OUT=1115,
	SFX_TR5_WATER_LAPS_LOOP=1116,
	SFX_TR5_WELD_THRU_DOOR_LOOP=1117,
	SFX_TR5_XRAY_SCAN=1118,
	SFX_TR5_MANHOLE_METAL=1119,
	SFX_TR5_2GUNTEX_DEATH=1120,
	SFX_TR5_2GUNTEX_FALL_BIG=1121,
	SFX_TR5_2GUNTEX_FALL_END=1122,
	SFX_TR5_2GUNTEX_FEET_LOUD=1123,
	SFX_TR5_2GUNTEX_FEET_QUIET=1124,
	SFX_TR5_2GUNTEX_HIT_GUNS=1125,
	SFX_TR5_2GUNTEX_LASER_FIRE=1126,
	SFX_TR5_2GUNTEX_DOUBLE_LASER_FIRE=1127,
	SFX_TR5_2GUNTEX_LASER_MISFIRE=1128,
//Custom Footstep Sounds
	SFX_CUSTOM_FOOTSTEP_SOUNDS_1 = 1129,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_2 = 1130,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_3 = 1131,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_4 = 1132,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_5 = 1133,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_6 = 1134,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_7 = 1135,
	SFX_CUSTOM_FOOTSTEP_SOUNDS_8 = 1136,
//Custom Level Builder slots
	SFX_CUSTOM_1=1137,
	SFX_CUSTOM_2=1138,
	SFX_CUSTOM_3=1139,
	SFX_CUSTOM_4=1140,
	SFX_CUSTOM_5=1141,
	SFX_CUSTOM_6=1142,
	SFX_CUSTOM_7=1143,
	SFX_CUSTOM_8=1144,
	SFX_CUSTOM_9=1145,
	SFX_CUSTOM_10=1146,
	SFX_CUSTOM_11=1147,
	SFX_CUSTOM_12=1148,
	SFX_CUSTOM_13=1149,
	SFX_CUSTOM_14=1150,
	SFX_CUSTOM_15=1151,
	SFX_CUSTOM_16=1152,
	SFX_CUSTOM_17=1153,
	SFX_CUSTOM_18=1154,
	SFX_CUSTOM_19=1155,
	SFX_CUSTOM_20=1156,
	SFX_CUSTOM_21=1157,
	SFX_CUSTOM_22=1158,
	SFX_CUSTOM_23=1159,
	SFX_CUSTOM_24=1160,
	SFX_CUSTOM_25=1161,
	SFX_CUSTOM_26=1162,
	SFX_CUSTOM_27=1163,
	SFX_CUSTOM_28=1164,
	SFX_CUSTOM_29=1165,
	SFX_CUSTOM_30=1166,
	SFX_CUSTOM_31=1167,
	SFX_CUSTOM_32=1168,
	SFX_CUSTOM_33=1169,
	SFX_CUSTOM_34=1170,
	SFX_CUSTOM_35=1171,
	SFX_CUSTOM_36=1172,
	SFX_CUSTOM_37=1173,
	SFX_CUSTOM_38=1174,
	SFX_CUSTOM_39=1175,
	SFX_CUSTOM_40=1176,
	SFX_CUSTOM_41=1177,
	SFX_CUSTOM_42=1178,
	SFX_CUSTOM_43=1179,
	SFX_CUSTOM_44=1180,
	SFX_CUSTOM_45=1181,
	SFX_CUSTOM_46=1182,
	SFX_CUSTOM_47=1183,
	SFX_CUSTOM_48=1184,
	SFX_CUSTOM_49=1185,
	SFX_CUSTOM_50=1186,
	NUM_SFX,
};