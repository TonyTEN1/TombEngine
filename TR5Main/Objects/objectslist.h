#pragma once

enum GAME_OBJECT_ID : short
{
	ID_NO_OBJECT = -1,

	/* Lara Primary Slot */
	ID_LARA,
	ID_LARA_EXTRA_ANIMS,
	ID_PISTOLS_ANIM,
	ID_UZI_ANIM,
	ID_SHOTGUN_ANIM,
	ID_REVOLVER_ANIM,
	ID_CROSSBOW_ANIM,
	ID_HK_ANIM,
	ID_GRENADE_ANIM,
	ID_ROCKET_ANIM,
	ID_HARPOON_ANIM,
	ID_LARA_FLARE_ANIM,
	ID_LARA_SKIN,
	ID_LARA_SKIN_JOINTS,
	ID_LARA_SCREAM,
	ID_LARA_CROSSBOW_LASER,
	ID_LARA_REVOLVER_LASER,
	ID_LARA_HOLSTERS,
	ID_LARA_HOLSTERS_PISTOLS,
	ID_LARA_HOLSTERS_UZIS,
	ID_LARA_HOLSTERS_REVOLVER,
	ID_LARA_SPEECH_HEAD1,
	ID_LARA_SPEECH_HEAD2,
	ID_LARA_SPEECH_HEAD3,
	ID_LARA_SPEECH_HEAD4,
	ID_ACTOR1_SPEECH_HEAD1,
	ID_ACTOR1_SPEECH_HEAD2,
	ID_ACTOR2_SPEECH_HEAD1,
	ID_ACTOR2_SPEECH_HEAD2,
	ID_LARA_EXTRA_MESH1,
	ID_LARA_EXTRA_MESH2,
	ID_LARA_EXTRA_MESH3,
	ID_LARA_WATER_MESH,
	ID_LARA_PETROL_MESH,
	ID_LARA_DIRT_MESH,
	ID_LARA_CROWBAR_ANIM,
	ID_LARA_TORCH_ANIM,
	ID_LARA_HAIR,

	/* Lara Vehicles */
	ID_SNOWMOBILE_LARA_ANIMS = 50,
	ID_SNOWMOBILE,
	ID_QUAD_LARA_ANIMS,
	ID_QUAD,
	ID_SPEEDBOAT_LARA_ANIMS,
	ID_SPEEDBOAT,									// TR2
	ID_KAYAK_LARA_ANIMS,
	ID_KAYAK,
	ID_UPV_LARA_ANIMS,
	ID_UPV,											// TR3 - 90% (Need new render)
	ID_MINECART_LARA_ANIMS,
	ID_MINECART,
	ID_JEEP_LARA_ANIMS,
	ID_JEEP,										// TR4 - 95% Working
	ID_MOTORBIKE_LARA_ANIMS,
	ID_MOTORBIKE,
	ID_RUBBER_BOAT_LARA_ANIMS,
	ID_RUBBER_BOAT,									// TR3 / (TR4 TRNG)
	ID_BIGGUN_ANIMS,
	ID_BIGGUN,										// TR3
	

	ID_VEHICLE_SMASHABLE_FLOOR = 90, // NEW
	ID_VEHICLE_SMASHABLE_WALL, // NEW

	/* Enemy */

	/* Animals */
	ID_WOLF = 100,
	ID_BEAR,
	ID_APE,
	ID_SMALL_SPIDER,
	ID_BIG_SPIDER,
	ID_CROW,
	ID_TIGER,
	ID_EAGLE,
	ID_RAPTOR,
	ID_TYRANNOSAUR,
	ID_COBRA,
	ID_MONKEY,
	ID_WHALE,
	ID_SMALL_DINOSAUR,		// TR3 - 34
	ID_FISH_EMITTER,		// TR3 - 338 - OK - Needs testing and drawing
	ID_RAT,					// TR2 - TR3 - OK
	ID_BIG_RAT,				// TR1 - (Land And Water)
	ID_CROCODILE,			// TR4 - OK - We just need to fix land/water transition but it was bugged also in TR4
	ID_BAT,					// TR4 - OK
	ID_SPHINX,				// TR4 - OK
	ID_WILD_BOAR,			// TR4 - BUGGED TURNS!
	ID_HARPY,				// TR4 - OK
	ID_BIG_SCORPION,		// TR4 - OK
	ID_SMALL_SCORPION,		// TR4 - OK
	ID_BABOON_NORMAL,
	ID_BABOON_INV,
	ID_BABOON_SILENT,
	ID_LITTLE_BEETLE,		// TR4 - OK
	ID_LOCUSTS,				// TR4 - OK
	ID_SHARK,				// DEPRECATED?
	ID_HUSKIE,
	ID_DOG,					// TR4 - OK
	ID_BATS_EMITTER,
	ID_RATS_EMITTER,		// TR5 - OK
	ID_SPIDERS_EMITTER,
	ID_LION,
	ID_DOBERMAN,
	ID_HAMMERHEAD,			// TR4 - OK
	ID_CROCODILE_GOD,		// TR4 - OK (needs accurate testing)
	ID_LOCUSTS_EMITTER,		// TR4 - OK

	/* Humans */
	ID_SCUBA_HARPOON = 150,
	ID_SCUBA_DIVER,
	ID_GOON_SILENCER1,
	ID_GOON_SILENCER2,
	ID_GOON_SILENCER3,
	ID_BARRACUDA,
	ID_WORKER_SHOTGUN,
	ID_WORKER_MACHINEGUN,
	ID_WORKER_DUAL_REVOLVER,
	ID_WORKER_FLAMETHROWER,
	ID_MONK1,
	ID_MONK2,
	ID_KNIFETHROWER,
	ID_KNIFETHROWER_KNIFE,
	ID_MERCENARY_UZI,
	ID_MERCENARY_AUTOPISTOLS1,
	ID_MERCENARY_AUTOPISTOLS2,
	ID_SNOWMOBILE_GUN,
	ID_SNOWMOBILE_DRIVER,
	ID_FLAMETHROWER_BADDY,
	ID_TRIBESMAN_WITH_AX,
	ID_TRIBESMAN_WITH_DARTS,
	ID_MP_WITH_STICK,
	ID_MP_WITH_GUN,
	ID_BADDY1,						// TR4 - OK
	ID_BADDY2,						// TR4 - OK
	ID_SAS_CAIRO,					// TR4 - OK
	ID_SAS_DYING,					// DEPRECATED?
	ID_SAS_CAPTAIN,					// DEPRECATED?
	ID_SAS_DRAG_BLOKE,				// TR4 - OK
	ID_GUIDE,
	ID_VON_CROY,
	ID_TROOPS,						// TR4 - OK
	ID_SAS,
	ID_SWAT,
	ID_SWAT_PLUS,
	ID_GUARD1,
	ID_GUARD_LASER,
	ID_LARSON,
	ID_PIERRE,
	ID_MAFIA,
	ID_MAFIA2,
	ID_GUARD2,
	ID_GUARD3,
	ID_GLADIATOR,
	ID_HITMAN,
	ID_SNIPER,
	ID_CHEF,

	/* Soprannatural, boss and others */
	ID_WINGED_MUMMY = 220,
	ID_CENTAUR_MUTANT,
	ID_LARA_DOPPELGANGER,
	ID_NATLA,
	ID_MUTANT,
	ID_GIANT_MUTANT,
	ID_PROJ_NATLA,
	ID_PROJ_SHARD,
	ID_PROJ_BOMB,
	ID_YETI,
	ID_BIRDMONSTER,
	ID_MARCO_BARTOLI,
	ID_DRAGON_FRONT,
	ID_DRAGON_BACK,
	ID_DRAGON_BONE_FRONT,
	ID_DRAGON_BONE_BACK,
	ID_SPHERE_OF_DOOM,      // Used by the dragon/boss to appear or death explosion.
	ID_SPHERE_OF_DOOM2,
	ID_SPHERE_OF_DOOM3,
	ID_CIVVIE,
	ID_SPEAR_GUARDIAN,
	ID_SPEAR_GUARDIAN_STATUE,
	ID_SWORD_GUARDIAN,
	ID_SWORD_GUARDIAN_STATUE,
	ID_SHIVA,
	ID_SHIVA_STATUE,
	ID_TRIBEBOSS,
	ID_CIVVY,
	ID_MUTANT2,
	ID_LIZARD,				// TR3 - 35
	ID_TONY_BOSS,			// TR3 - Not Finished
	ID_TONY_BOSS_FLAME,     // TR3 Tony Flame Controller
	ID_PUNA_BOSS,			// TR3 - 36
	ID_SOPHIA_LEE_BOSS,		// TR3 - 57
	ID_LASER_BOLT,
	ID_SKELETON,			// TR4 - OK
	ID_MUMMY,				// TR4 - OK
	ID_ENEMY_JEEP,
	ID_HORSE,				// TR4 - OK
	ID_HORSEMAN,			// TR4 - OK
	ID_DEMIGOD1,
	ID_DEMIGOD2,
	ID_DEMIGOD3,
	ID_SETHA,
	ID_KNIGHT_TEMPLAR,
	ID_AHMET,				// TR4 - OK
	ID_BIG_BEETLE,
	ID_JEAN_YVES,			// TR4 - OK - Needs testing
	ID_WRAITH1,
	ID_WRAITH2,
	ID_WRAITH3,
	ID_WRAITH4,
	ID_LARA_DOUBLE,			// TR4 - OK - Needs testing
	ID_COMPY,
	ID_HYDRA,
	ID_LASERHEAD,
	ID_SCIENTIST,
	ID_MERCENARY,
	ID_WILLOWISP,
	ID_INVISIBLE_GHOST,
	ID_REAPER,
	ID_BROWN_BEAST,
	ID_ATTACK_SUB,
	ID_IMP,
	ID_IMP_ROCK,			// maybe needed for imp so moved here !
	ID_GUNSHIP,
	ID_AUTOGUN,
	ID_ROMAN_GOD1,
	ID_ROMAN_GOD2,
	ID_LAGOON_WITCH,
	ID_BOSS_SHIELD,
	ID_BOSS_EXPLOSION_SHOCKWAVE,
	ID_BOSS_EXPLOSION_RING,

	/* Traps / Doors */
	ID_SPRINGBOARD = 320,
	ID_ROLLING_SPINDLE,
	ID_DISK_SHOOTER,
	ID_DISK,
	ID_WALL_MOUNTED_BLADE,		// TR3 - 111
	ID_STATUE_WITH_BLADE,
	ID_COG,						// TR4 - OK
	ID_SPIKEBALL,				// TR4 - OK
	ID_FLOOR_4BLADES,			// TR4 - OK
	ID_CEILING_4BLADES,			// TR4 - OK
	ID_CATWALK_BLADE,			// TR4 - OK
	ID_SETH_BLADE,				// TR4 - OK
	ID_PLINTH_BLADE,			// TR4 - OK
	ID_SLICER_DICER,			// TR4 - OK
	ID_BIRD_BLADE,				// TR4 - OK
	ID_CHAIN,					// TR4 - OK
	ID_PLOUGH,					// TR4 - OK
	ID_STARGATE,				// TR4 - OK
	ID_SPIKY_FLOOR,
	ID_SPIKY_WALL,
	ID_SPIKY_CEILING,
	ID_TEETH_SPIKES,			// TR4 - TESTING
	ID_JOBY_SPIKES,				// TR4 - TESTING
	ID_SENTRY_GUN,				// TR4 - OK
	ID_MAPPER,					// TR4 - TESTING
	ID_MOVING_BLADE,			// TR4 - OK
	ID_ELEMENT_PUZZLE,			// TR4 - TESTING
	ID_LIGHTING_CONDUCTOR,		// LUA
	ID_HAMMER,					// TR4 - Hardcoded setup, not decompiled for now
	ID_OBELISK,					// TR4 - Hardcoded setup, not decompiled for now
	ID_WHEEL_OF_FORTUNE,		// TR4 - OK
	ID_GAME_PIECE1,				// TR4 - OK
	ID_GAME_PIECE2,				// TR4 - OK
	ID_GAME_PIECE3,				// TR4 - OK
	ID_RAISING_COG,
	ID_MINE,					// TR4 - TESTING	
	ID_SCALES,					// TR4 - OK			
	ID_ROME_HAMMER,
	ID_FLAME,					// TR4 - OK	
	ID_FLAME_EMITTER,			// TR4 - OK	
	ID_FLAME_EMITTER2,			// TR4 - OK	
	ID_FLAME_EMITTER3,			// TR4 - OK	
	ID_BURNING_ROOTS,
	ID_ROPE,					// TR4 - OK but needs rendering
	ID_FIREROPE,
	ID_POLEROPE,				// TR4 - OK
	ID_ELECTRICAL_CABLES,       // TR5 - OK
	ID_BURNING_FLOOR,
	ID_DARTS,					// TR4 - OK	but needs rendering
	ID_DART_EMITTER,			// TR4 - OK	
	ID_HOMING_DART_EMITTER,		// TR4 - OK	
	ID_FALLING_CEILING,
	ID_FALLING_BLOCK,			// TR5 - OK
	ID_FALLING_BLOCK2,
	ID_CRUMBLING_FLOOR,			// Should be same as falling block or different behaviour?
	ID_TRAPDOOR1,				// No floor/ceiling portal blockage applied
	ID_TRAPDOOR2,				// No floor/ceiling portal blockage applied
	ID_TRAPDOOR3,				// No floor/ceiling portal blockage applied
	ID_FLOOR_TRAPDOOR1,			// No floor/ceiling portal blockage applied
	ID_FLOOR_TRAPDOOR2,			// No floor/ceiling portal blockage applied
	ID_CEILING_TRAPDOOR1,		// No floor/ceiling portal blockage applied	
	ID_CEILING_TRAPDOOR2,		// No floor/ceiling portal blockage applied
	ID_SCALING_TRAPDOOR,		// No floor/ceiling portal blockage applied
	ID_ROLLINGBALL,				// TR4 - OK
	ID_ROLLINGBARREL,
	ID_PROPELLER_H,
	ID_PROPELLER_V,
	ID_ONEBLOCK_PLATFORM,		// TR4 - OK
	ID_TWOBLOCK_PLATFORM,		// TR4 - OK
	ID_RAISING_BLOCK1,			// TR4 - OK
	ID_RAISING_BLOCK2,			// TR4 - OK
	ID_RAISING_BLOCK3,			// TR4 - OK
	ID_RAISING_BLOCK4,			// TR4 - OK
	ID_PUSHABLE_OBJECT1,		// TR4 - OK
	ID_PUSHABLE_OBJECT2,		// TR4 - OK
	ID_PUSHABLE_OBJECT3,		// TR4 - OK
	ID_PUSHABLE_OBJECT4,		// TR4 - OK
	ID_PUSHABLE_OBJECT4,		// TR4 - OK
	ID_PUSHABLE_OBJECT5,		// TR4 - OK
	ID_PUSHABLE_OBJECT6,		// TR4 - OK
	ID_PUSHABLE_OBJECT7,		// TR4 - OK
	ID_PUSHABLE_OBJECT8,		// TR4 - OK
	ID_PUSHABLE_OBJECT9,		// TR4 - OK
	ID_PUSHABLE_OBJECT10,		// TR4 - OK
	ID_WRECKING_BALL,
	ID_ZIPLINE_HANDLE,
	ID_TORPEDO,
	ID_CHAFF,
	ID_ELECTRIC_FENCE,
	ID_LIFT,
	ID_TIGHT_ROPE,
	ID_PARALLEL_BARS,
	ID_XRAY_CONTROLLER,
	ID_PORTAL,
	ID_GEN_SLOT1,
	ID_GEN_SLOT2,
	ID_GEN_SLOT3,
	ID_GEN_SLOT4,

	/* Searchable Objects */
	ID_SEARCH_OBJECT1,
	ID_SEARCH_OBJECT2,
	ID_SEARCH_OBJECT3,
	ID_SEARCH_OBJECT4,
	ID_SARCOPHAGUS,				// TR4 - OK

	ID_ENEMY_PIECE,				// TR4 - OK
	ID_EXPANDING_PLATFORM,
	ID_SQUISHY_BLOCK1,
	ID_SQUISHY_BLOCK2,
	ID_TRIPWIRE,
	ID_MINE_DETECTOR,
	ID_MAP, 
	ID_SECRET_MAP,
	ID_SETH_DOOR,
	ID_HORUS_STATUE,
	ID_STATUE_PLINTH,
	ID_CLASSIC_ROLLING_BALL,
	ID_BIG_ROLLING_BALL,
	ID_PUSHABLE_OBJECT_CLIMBABLE1,
	ID_PUSHABLE_OBJECT_CLIMBABLE2,
	ID_PUSHABLE_OBJECT_CLIMBABLE3,
	ID_PUSHABLE_OBJECT_CLIMBABLE4,
	ID_PUSHABLE_OBJECT_CLIMBABLE5,
	ID_PUSHABLE_OBJECT_CLIMBABLE6,
	ID_PUSHABLE_OBJECT_CLIMBABLE7,
	ID_PUSHABLE_OBJECT_CLIMBABLE8,
	ID_PUSHABLE_OBJECT_CLIMBABLE9,
	ID_PUSHABLE_OBJECT_CLIMBABLE10,
	ID_TRAIN,
	ID_EXPLOSION,

	/* Items */
	ID_PUZZLE_ITEM1 = 500,
	ID_PUZZLE_ITEM2,
	ID_PUZZLE_ITEM3,
	ID_PUZZLE_ITEM4,
	ID_PUZZLE_ITEM5,
	ID_PUZZLE_ITEM6,
	ID_PUZZLE_ITEM7,
	ID_PUZZLE_ITEM8,
	ID_PUZZLE_ITEM9,
	ID_PUZZLE_ITEM10,
	ID_PUZZLE_ITEM11,
	ID_PUZZLE_ITEM12,
	ID_PUZZLE_ITEM13,
	ID_PUZZLE_ITEM14,
	ID_PUZZLE_ITEM15,
	ID_PUZZLE_ITEM16,
	ID_PUZZLE_ITEM1_COMBO1,
	ID_PUZZLE_ITEM1_COMBO2,
	ID_PUZZLE_ITEM2_COMBO1,
	ID_PUZZLE_ITEM2_COMBO2,
	ID_PUZZLE_ITEM3_COMBO1,
	ID_PUZZLE_ITEM3_COMBO2,
	ID_PUZZLE_ITEM4_COMBO1,
	ID_PUZZLE_ITEM4_COMBO2,
	ID_PUZZLE_ITEM5_COMBO1,
	ID_PUZZLE_ITEM5_COMBO2,
	ID_PUZZLE_ITEM6_COMBO1,
	ID_PUZZLE_ITEM6_COMBO2,
	ID_PUZZLE_ITEM7_COMBO1,
	ID_PUZZLE_ITEM7_COMBO2,
	ID_PUZZLE_ITEM8_COMBO1,
	ID_PUZZLE_ITEM8_COMBO2,
	ID_PUZZLE_ITEM9_COMBO1,
	ID_PUZZLE_ITEM9_COMBO2,
	ID_PUZZLE_ITEM10_COMBO1,
	ID_PUZZLE_ITEM10_COMBO2,
	ID_PUZZLE_ITEM11_COMBO1,
	ID_PUZZLE_ITEM11_COMBO2,
	ID_PUZZLE_ITEM12_COMBO1,
	ID_PUZZLE_ITEM12_COMBO2,
	ID_PUZZLE_ITEM13_COMBO1,
	ID_PUZZLE_ITEM13_COMBO2,
	ID_PUZZLE_ITEM14_COMBO1,
	ID_PUZZLE_ITEM14_COMBO2,
	ID_PUZZLE_ITEM15_COMBO1,
	ID_PUZZLE_ITEM15_COMBO2,
	ID_PUZZLE_ITEM16_COMBO1,
	ID_PUZZLE_ITEM16_COMBO2,
	ID_KEY_ITEM1,
	ID_KEY_ITEM2,
	ID_KEY_ITEM3,
	ID_KEY_ITEM4,
	ID_KEY_ITEM5,
	ID_KEY_ITEM6,
	ID_KEY_ITEM7,
	ID_KEY_ITEM8,
	ID_KEY_ITEM9,
	ID_KEY_ITEM10,
	ID_KEY_ITEM11,
	ID_KEY_ITEM12,
	ID_KEY_ITEM13,
	ID_KEY_ITEM14,
	ID_KEY_ITEM15,
	ID_KEY_ITEM16,
	ID_KEY_ITEM1_COMBO1,
	ID_KEY_ITEM1_COMBO2,
	ID_KEY_ITEM2_COMBO1,
	ID_KEY_ITEM2_COMBO2,
	ID_KEY_ITEM3_COMBO1,
	ID_KEY_ITEM3_COMBO2,
	ID_KEY_ITEM4_COMBO1,
	ID_KEY_ITEM4_COMBO2,
	ID_KEY_ITEM5_COMBO1,
	ID_KEY_ITEM5_COMBO2,
	ID_KEY_ITEM6_COMBO1,
	ID_KEY_ITEM6_COMBO2,
	ID_KEY_ITEM7_COMBO1,
	ID_KEY_ITEM7_COMBO2,
	ID_KEY_ITEM8_COMBO1,
	ID_KEY_ITEM8_COMBO2,
	ID_KEY_ITEM9_COMBO1,
	ID_KEY_ITEM9_COMBO2,
	ID_KEY_ITEM10_COMBO1,
	ID_KEY_ITEM10_COMBO2,
	ID_KEY_ITEM11_COMBO1,
	ID_KEY_ITEM11_COMBO2,
	ID_KEY_ITEM12_COMBO1,
	ID_KEY_ITEM12_COMBO2,
	ID_KEY_ITEM13_COMBO1,
	ID_KEY_ITEM13_COMBO2,
	ID_KEY_ITEM14_COMBO1,
	ID_KEY_ITEM14_COMBO2,
	ID_KEY_ITEM15_COMBO1,
	ID_KEY_ITEM15_COMBO2,
	ID_KEY_ITEM16_COMBO1,
	ID_KEY_ITEM16_COMBO2,
	ID_PICKUP_ITEM1,
	ID_PICKUP_ITEM2,
	ID_PICKUP_ITEM3,
	ID_PICKUP_ITEM4,
	ID_PICKUP_ITEM5,
	ID_PICKUP_ITEM6,
	ID_PICKUP_ITEM7,
	ID_PICKUP_ITEM8,
	ID_PICKUP_ITEM9,
	ID_PICKUP_ITEM10,
	ID_PICKUP_ITEM11,
	ID_PICKUP_ITEM12,
	ID_PICKUP_ITEM13,
	ID_PICKUP_ITEM14,
	ID_PICKUP_ITEM15,
	ID_PICKUP_ITEM16,
	ID_PICKUP_ITEM1_COMBO1,
	ID_PICKUP_ITEM1_COMBO2,
	ID_PICKUP_ITEM2_COMBO1,
	ID_PICKUP_ITEM2_COMBO2,
	ID_PICKUP_ITEM3_COMBO1,
	ID_PICKUP_ITEM3_COMBO2,
	ID_PICKUP_ITEM4_COMBO1,
	ID_PICKUP_ITEM4_COMBO2,
	ID_PICKUP_ITEM5_COMBO1,
	ID_PICKUP_ITEM5_COMBO2,
	ID_PICKUP_ITEM6_COMBO1,
	ID_PICKUP_ITEM6_COMBO2,
	ID_PICKUP_ITEM7_COMBO1,
	ID_PICKUP_ITEM7_COMBO2,
	ID_PICKUP_ITEM8_COMBO1,
	ID_PICKUP_ITEM8_COMBO2,
	ID_PICKUP_ITEM9_COMBO1,
	ID_PICKUP_ITEM9_COMBO2,
	ID_PICKUP_ITEM10_COMBO1,
	ID_PICKUP_ITEM10_COMBO2,
	ID_PICKUP_ITEM11_COMBO1,
	ID_PICKUP_ITEM11_COMBO2,
	ID_PICKUP_ITEM12_COMBO1,
	ID_PICKUP_ITEM12_COMBO2,
	ID_PICKUP_ITEM13_COMBO1,
	ID_PICKUP_ITEM13_COMBO2,
	ID_PICKUP_ITEM14_COMBO1,
	ID_PICKUP_ITEM14_COMBO2,
	ID_PICKUP_ITEM15_COMBO1,
	ID_PICKUP_ITEM15_COMBO2,
	ID_PICKUP_ITEM16_COMBO1,
	ID_PICKUP_ITEM16_COMBO2,
	ID_EXAMINE1,
	ID_EXAMINE2,
	ID_EXAMINE3,
	ID_EXAMINE4,
	ID_EXAMINE5,
	ID_EXAMINE6,
	ID_EXAMINE7,
	ID_EXAMINE8,
	ID_EXAMINE1_COMBO1,
	ID_EXAMINE1_COMBO2,
	ID_EXAMINE2_COMBO1,
	ID_EXAMINE2_COMBO2,
	ID_EXAMINE3_COMBO1,
	ID_EXAMINE3_COMBO2,
	ID_EXAMINE4_COMBO1,
	ID_EXAMINE4_COMBO2,
	ID_EXAMINE5_COMBO1,
	ID_EXAMINE5_COMBO2,
	ID_EXAMINE6_COMBO1,
	ID_EXAMINE6_COMBO2,
	ID_EXAMINE7_COMBO1,
	ID_EXAMINE7_COMBO2,
	ID_EXAMINE8_COMBO1,
	ID_EXAMINE8_COMBO2,
	ID_PUZZLE_HOLE1,
	ID_PUZZLE_HOLE2,
	ID_PUZZLE_HOLE3,
	ID_PUZZLE_HOLE4,
	ID_PUZZLE_HOLE5,
	ID_PUZZLE_HOLE6,
	ID_PUZZLE_HOLE7,
	ID_PUZZLE_HOLE8,
	ID_PUZZLE_HOLE9,
	ID_PUZZLE_HOLE10,
	ID_PUZZLE_HOLE11,
	ID_PUZZLE_HOLE12,
	ID_PUZZLE_HOLE13,
	ID_PUZZLE_HOLE14,
	ID_PUZZLE_HOLE15,
	ID_PUZZLE_HOLE16,
	ID_PUZZLE_DONE1,
	ID_PUZZLE_DONE2,
	ID_PUZZLE_DONE3,
	ID_PUZZLE_DONE4,
	ID_PUZZLE_DONE5,
	ID_PUZZLE_DONE6,
	ID_PUZZLE_DONE7,
	ID_PUZZLE_DONE8,
	ID_PUZZLE_DONE9,
	ID_PUZZLE_DONE10,
	ID_PUZZLE_DONE11,
	ID_PUZZLE_DONE12,
	ID_PUZZLE_DONE13,
	ID_PUZZLE_DONE14,
	ID_PUZZLE_DONE15,
	ID_PUZZLE_DONE16,
	ID_KEY_HOLE1,
	ID_KEY_HOLE2,
	ID_KEY_HOLE3,
	ID_KEY_HOLE4,
	ID_KEY_HOLE5,
	ID_KEY_HOLE6,
	ID_KEY_HOLE7,
	ID_KEY_HOLE8,
	ID_KEY_HOLE9,
	ID_KEY_HOLE10,
	ID_KEY_HOLE11,
	ID_KEY_HOLE12,
	ID_KEY_HOLE13,
	ID_KEY_HOLE14,
	ID_KEY_HOLE15,
	ID_KEY_HOLE16,
	ID_WATERSKIN1_EMPTY,			// TR4 Small waterskin - OK
	ID_WATERSKIN1_1,				// TR4 Small waterskin 1L - OK
	ID_WATERSKIN1_2,				// TR4 Small waterskin 2L - OK
	ID_WATERSKIN1_3,				// TR4 Small waterskin 3L - OK
	ID_WATERSKIN2_EMPTY,			// TR4 Big waterskin - OK
	ID_WATERSKIN2_1,				// TR4 Big waterskin 1L - OK
	ID_WATERSKIN2_2,				// TR4 Big waterskin 2L - OK
	ID_WATERSKIN2_3,				// TR4 Big waterskin 3L - OK
	ID_WATERSKIN2_4,				// TR4 Big waterskin 4L - OK
	ID_WATERSKIN2_5,				// TR4 Big waterskin 5L - OK

	/* Misc inventory objects */
	ID_HAMMER_ITEM = 750,
	ID_CROWBAR_ITEM,				// TR4 - OK
	ID_BURNING_TORCH_ITEM,
	ID_CLOCKWORK_BEETLE,			// TR4 - OK
	ID_CLOCKWORK_BEETLE_COMBO1,		// TR4 - OK
	ID_CLOCKWORK_BEETLE_COMBO2,		// TR4 - OK

	/* Switches */
	ID_SWITCH_TYPE1 = 800,			// OK
	ID_SWITCH_TYPE2,				// OK
	ID_SWITCH_TYPE3,				// OK
	ID_SWITCH_TYPE4,				// OK
	ID_SWITCH_TYPE5,				// OK
	ID_SWITCH_TYPE6,				// OK
	ID_SWITCH_TYPE7,				// OK
	ID_SWITCH_TYPE8,				// OK
	ID_SWITCH_TYPE9,				// OK
	ID_SWITCH_TYPE10,				// OK
	ID_SWITCH_TYPE11,				// OK
	ID_SWITCH_TYPE12,				// OK
	ID_SWITCH_TYPE13,				// OK
	ID_SWITCH_TYPE14,				// OK
	ID_SWITCH_TYPE15,				// OK
	ID_SWITCH_TYPE16,				// OK
	ID_SHOOT_SWITCH1,
	ID_SHOOT_SWITCH2,
	ID_SHOOT_SWITCH3,
	ID_SHOOT_SWITCH4,
	ID_AIRLOCK_SWITCH,
	ID_UNDERWATER_SWITCH1,			// OK
	ID_UNDERWATER_SWITCH2,			// OK
	ID_UNDERWATER_SWITCH3,			// OK
	ID_UNDERWATER_SWITCH4,			// OK
	ID_TURN_SWITCH,					// TR4 - OK, but will be scripted
	ID_COG_SWITCH,					// TR4 - OK
	ID_LEVER_SWITCH,				// TR4 - OK
	ID_JUMP_SWITCH,					// TR4 - OK
	ID_CROWBAR_SWITCH,				// TR4 - OK
	ID_PULLEY,						// TR4 - OK
	ID_CROWDOVE_SWITCH,

	ID_DOOR_TYPE1 = 850,			// OK
	ID_DOOR_TYPE2,					// OK
	ID_DOOR_TYPE3,					// OK
	ID_DOOR_TYPE4,					// OK
	ID_DOOR_TYPE5,					// OK
	ID_DOOR_TYPE6,					// OK
	ID_DOOR_TYPE7,					// OK
	ID_DOOR_TYPE8,					// OK
	ID_DOOR_TYPE9,					// OK
	ID_DOOR_TYPE10,					// OK
	ID_DOOR_TYPE11,					// OK
	ID_DOOR_TYPE12,					// OK
	ID_DOOR_TYPE13,					// OK
	ID_DOOR_TYPE14,					// OK
	ID_DOOR_TYPE15,					// OK
	ID_DOOR_TYPE16,					// OK
	ID_DOOR_TYPE17,					// OK
	ID_DOOR_TYPE18,					// OK
	ID_DOOR_TYPE19,					// OK
	ID_DOOR_TYPE20,					// OK
	ID_DOOR_TYPE21,					// OK
	ID_DOOR_TYPE22,					// OK
	ID_DOOR_TYPE23,					// OK
	ID_DOOR_TYPE24,					// OK
	ID_DOOR_TYPE25,					// OK
	ID_DOOR_TYPE26,					// OK
	ID_DOOR_TYPE27,					// OK
	ID_DOOR_TYPE28,					// OK
	ID_DOOR_TYPE29,					// OK
	ID_DOOR_TYPE30,					// OK
	ID_LIFT_DOORS1,
	ID_LIFT_DOORS2,
	ID_PUSHPULL_DOOR1,				// TR4 - OK
	ID_PUSHPULL_DOOR2,				// TR4 - OK
	ID_PUSHPULL_DOOR3,				// TR4 - OK
	ID_PUSHPULL_DOOR4,				// TR4 - OK
	ID_KICK_DOOR1,					// TR4 - OK
	ID_KICK_DOOR2,					// TR4 - OK
	ID_KICK_DOOR3,					// TR4 - OK
	ID_KICK_DOOR4,					// TR4 - OK
	ID_UNDERWATER_DOOR1,			// OK
	ID_UNDERWATER_DOOR2,			// OK
	ID_UNDERWATER_DOOR3,			// OK
	ID_UNDERWATER_DOOR4,			// OK
	ID_DOUBLE_DOORS1,				// TR4 - OK
	ID_DOUBLE_DOORS2,				// TR4 - OK
	ID_DOUBLE_DOORS3,				// TR4 - OK
	ID_DOUBLE_DOORS4,				// TR4 - OK
	ID_SEQUENCE_DOOR1,				// TR4 - OK
	ID_SEQUENCE_SWITCH1,			// TR4 - OK
	ID_SEQUENCE_SWITCH2,			// TR4 - OK
	ID_SEQUENCE_SWITCH3,			// TR4 - OK
	ID_STEEL_DOOR,
	ID_GOD_HEAD,

	/* Lara Items */
	ID_PISTOLS_ITEM = 950,
	ID_PISTOLS_AMMO_ITEM,
	ID_UZI_ITEM,
	ID_UZI_AMMO_ITEM,
	ID_SHOTGUN_ITEM,
	ID_SHOTGUN_AMMO1_ITEM,
	ID_SHOTGUN_AMMO2_ITEM,
	ID_REVOLVER_ITEM,
	ID_REVOLVER_AMMO_ITEM,
	ID_CROSSBOW_ITEM,
	ID_CROSSBOW_AMMO1_ITEM,
	ID_CROSSBOW_AMMO2_ITEM,
	ID_CROSSBOW_AMMO3_ITEM,
	ID_CROSSBOW_BOLT,
	ID_HK_ITEM,
	ID_HK_AMMO_ITEM,
	ID_GRENADE_GUN_ITEM,
	ID_GRENADE_AMMO1_ITEM,
	ID_GRENADE_AMMO2_ITEM,
	ID_GRENADE_AMMO3_ITEM,
	ID_GRENADE,
	ID_ROCKET_LAUNCHER_ITEM,
	ID_ROCKET_LAUNCHER_AMMO_ITEM,
	ID_ROCKET,
	ID_HARPOON_ITEM,
	ID_HARPOON_AMMO_ITEM,
	ID_HARPOON,
	ID_GOLDROSE_ITEM,
	ID_BIGMEDI_ITEM,
	ID_SMALLMEDI_ITEM,
	ID_LASERSIGHT_ITEM,
	ID_BINOCULARS_ITEM,
	ID_SILENCER_ITEM,
	ID_FLARE_ITEM,
	ID_FLARE_INV_ITEM,
	ID_COMPASS_ITEM,
	ID_DIARY_ITEM,
	ID_TIMEX_ITEM,
	ID_MEMCARD_LOAD_INV_ITEM,
	ID_MEMCARD_SAVE_INV_ITEM,
	ID_PC_LOAD_INV_ITEM,
	ID_PC_LOAD_SAVE_ITEM,
	ID_OPEN_DIARY_ITEM,

	/* Inventory main objects */
	ID_INVENTORY_PASSPORT = 1000,
	ID_INVENTORY_SUNGLASSES,
	ID_INVENTORY_KEYS,
	ID_INVENTORY_HEADPHONES,
	ID_INVENTORY_POLAROID,

	/* other effect in-game */
	ID_SMOKE_EMITTER_WHITE = 1020,
	ID_SMOKE_EMITTER_BLACK,
	ID_SMOKE_EMITTER,
	ID_LOCUST_EMITTER,
	ID_EARTHQUAKE,
	ID_BUBBLES,						// TR4 - OK
	ID_WATERFALLMIST,			
	ID_GUNSHELL,					// OK
	ID_SHOTGUNSHELL,				// OK
	ID_GUN_FLASH,					// OK
	ID_GUN_FLASH2,					// used for M16/MP5/HK
	ID_COLOR_LIGHT,
	ID_BLINKING_LIGHT,
	ID_PULSE_LIGHT,
	ID_STROBE_LIGHT,
	ID_ELECTRICAL_LIGHT,
	ID_LENS_FLARE,
	ID_ENERGY_BUBBLES,
	ID_PLANET_EFFECT,
	ID_BUTTERFLY,
	ID_AI_GUARD,					// OK
	ID_AI_AMBUSH,					// OK
	ID_AI_PATROL1,					// OK
	ID_AI_MODIFY,					// OK
	ID_AI_FOLLOW,					// OK
	ID_AI_PATROL2,					// OK
	ID_AI_X1,						// OK
	ID_AI_X2,						// OK
	ID_LARA_START_POS,
	ID_TELEPORTER,
	ID_LIFT_TELEPORTER,
	ID_LASERS,
	ID_STEAM_LASERS,
	ID_FLOOR_LASERS,
	ID_KILL_ALL_TRIGGERS,			// OK
	ID_TRIGGER_TRIGGERER,			// OK
	ID_HIGH_OBJECT1,
	ID_HIGH_OBJECT2,
	ID_SMASH_OBJECT1,
	ID_SMASH_OBJECT2,
	ID_SMASH_OBJECT3,
	ID_SMASH_OBJECT4,
	ID_SMASH_OBJECT5,
	ID_SMASH_OBJECT6,
	ID_SMASH_OBJECT7,
	ID_SMASH_OBJECT8,
	ID_SMASH_OBJECT9,
	ID_SMASH_OBJECT10,
	ID_SMASH_OBJECT11,
	ID_SMASH_OBJECT12,
	ID_SMASH_OBJECT13,
	ID_SMASH_OBJECT14,
	ID_SMASH_OBJECT15,
	ID_SMASH_OBJECT16,
	ID_BODY_PART,
	ID_CAMERA_TARGET,
	ID_WATERFALL1,
	ID_WATERFALL2,
	ID_WATERFALL3,
	ID_WATERFALL4,
	ID_WATERFALL5,
	ID_WATERFALL6,
	ID_WATERFALLSS1,
	ID_WATERFALLSS2,
	ID_FISHTANK,
	ID_BACON_REFERENCE,

	ID_MESHSWAP1 = 1100,
	ID_MESHSWAP2,
	ID_MESHSWAP3,
	ID_MESHSWAP4,
	ID_MESHSWAP5,
	ID_MESHSWAP6,
	ID_MESHSWAP7,
	ID_MESHSWAP8,
	ID_MESHSWAP9,
	ID_MESHSWAP10,
	ID_MESHSWAP_BADDY1,
	ID_MESHSWAP_BADDY2,
	ID_MESHSWAP_MAFIA2 = 1113,
	ID_MESHSWAP_IMP,
	ID_MESHSWAP_HITMAN,
	ID_MESHSWAP_ROMAN_GOD1,
	ID_MESHSWAP_ROMAN_GOD2,

	ID_ANIMATING1 = 1200,
	ID_ANIMATING2,
	ID_ANIMATING3,
	ID_ANIMATING4,
	ID_ANIMATING5,
	ID_ANIMATING6,
	ID_ANIMATING7,
	ID_ANIMATING8,
	ID_ANIMATING9,
	ID_ANIMATING10,
	ID_ANIMATING11,
	ID_ANIMATING12,
	ID_ANIMATING13,
	ID_ANIMATING14,
	ID_ANIMATING15,
	ID_ANIMATING16,
	ID_ANIMATING17,
	ID_ANIMATING18,
	ID_ANIMATING19,
	ID_ANIMATING20,
	ID_ANIMATING21,
	ID_ANIMATING22,
	ID_ANIMATING23,
	ID_ANIMATING24,
	ID_ANIMATING25,
	ID_ANIMATING26,
	ID_ANIMATING27,
	ID_ANIMATING28,
	ID_ANIMATING29,
	ID_ANIMATING30,
	ID_ANIMATING31,
	ID_ANIMATING32,
	ID_ANIMATING33,
	ID_ANIMATING34,
	ID_ANIMATING35,
	ID_ANIMATING36,
	ID_ANIMATING37,
	ID_ANIMATING38,
	ID_ANIMATING39,
	ID_ANIMATING40,
	ID_ANIMATING41,
	ID_ANIMATING42,
	ID_ANIMATING43,
	ID_ANIMATING44,
	ID_ANIMATING45,
	ID_ANIMATING46,
	ID_ANIMATING47,
	ID_ANIMATING48,
	ID_ANIMATING49,
	ID_ANIMATING50,
	ID_ANIMATING51,
	ID_ANIMATING52,
	ID_ANIMATING53,
	ID_ANIMATING54,
	ID_ANIMATING55,
	ID_ANIMATING56,
	ID_ANIMATING57,
	ID_ANIMATING58,
	ID_ANIMATING59,
	ID_ANIMATING60,
	ID_ANIMATING61,
	ID_ANIMATING62,
	ID_ANIMATING63,
	ID_ANIMATING64,
	ID_ANIMATING65,
	ID_ANIMATING66,
	ID_ANIMATING67,
	ID_ANIMATING68,
	ID_ANIMATING69,
	ID_ANIMATING70,
	ID_ANIMATING71,
	ID_ANIMATING72,
	ID_ANIMATING73,
	ID_ANIMATING74,
	ID_ANIMATING75,
	ID_ANIMATING76,
	ID_ANIMATING77,
	ID_ANIMATING78,
	ID_ANIMATING79,
	ID_ANIMATING80,
	ID_ANIMATING81,
	ID_ANIMATING82,
	ID_ANIMATING83,
	ID_ANIMATING84,
	ID_ANIMATING85,
	ID_ANIMATING86,
	ID_ANIMATING87,
	ID_ANIMATING88,
	ID_ANIMATING89,
	ID_ANIMATING90,
	ID_ANIMATING91,
	ID_ANIMATING92,
	ID_ANIMATING93,
	ID_ANIMATING94,
	ID_ANIMATING95,
	ID_ANIMATING96,
	ID_ANIMATING97,
	ID_ANIMATING98,
	ID_ANIMATING99,
	ID_ANIMATING100,
	ID_ANIMATING101,
	ID_ANIMATING102,
	ID_ANIMATING103,
	ID_ANIMATING104,
	ID_ANIMATING105,
	ID_ANIMATING106,
	ID_ANIMATING107,
	ID_ANIMATING108,
	ID_ANIMATING109,
	ID_ANIMATING110,
	ID_ANIMATING111,
	ID_ANIMATING112,
	ID_ANIMATING113,
	ID_ANIMATING114,
	ID_ANIMATING115,
	ID_ANIMATING116,
	ID_ANIMATING117,
	ID_ANIMATING118,
	ID_ANIMATING119,
	ID_ANIMATING120,
	ID_ANIMATING121,
	ID_ANIMATING122,
	ID_ANIMATING123,
	ID_ANIMATING124,
	ID_ANIMATING125,
	ID_ANIMATING126,
	ID_ANIMATING127,
	ID_ANIMATING128,
	ID_LASERHEAD_BASE,
	ID_LASERHEAD_TENTACLE,

	ID_BRIDGE_FLAT = 1340,		// OK
	ID_BRIDGE_TILT1,			// OK
	ID_BRIDGE_TILT2,			// OK
	ID_BRIDGE_TILT3,
	ID_BRIDGE_TILT4,

	ID_HORIZON = 1350,
	ID_BINOCULAR_GRAPHICS,
	ID_TARGET_GRAPHICS,
	ID_SKY_GRAPHICS,
	ID_DEFAULT_SPRITES,
	ID_MISC_SPRITES,
	ID_CUSTOM_SPRITES,
	ID_FIRE_SPRITES = 1360,
	ID_SMOKE_SPRITES,
	ID_SPARK_SPRITE,
	ID_DRIP_SPRITE,
	ID_EXPLOSION_SPRITES,
	ID_MOTOR_BOAT_FOAM_SPRITES,
	ID_RUBBER_BOAT_WAVE_SPRITES,
	ID_SKIDOO_SNOW_TRAIL_SPRITES,
	ID_KAYAK_PADDLE_TRAIL_SPRITE,
	ID_KAYAK_WAKE_SPRTIES,
	ID_BINOCULAR_GRAPHIC = 1370,
	ID_LASER_SIGHT_GRAPHIC,
	ID_CAUSTICS_TEXTURES,
	ID_BAR_BORDER_GRAPHIC,
	ID_HEALTH_BAR_TEXTURE,
	ID_AIR_BAR_TEXTURE,
	ID_DASH_BAR_TEXTURE,
	ID_SFX_BAR_TEXTURE,

	ID_PANEL_BORDER = 1400,
	ID_PANEL_MIDDLE,
	ID_PANEL_CORNER,
	ID_PANEL_DIAGONAL,
	ID_PANEL_STRIP,
	ID_PANEL_HALF_BORDER1,
	ID_PANEL_HALF_BORDER2,
	ID_PANEL_MIDDLE_CORNER,

	ID_NUMBER_OBJECTS,		// NEED TO BE AT THE END !!!!
};

template <typename T, typename Enum> constexpr bool is_underlying = std::is_same_v<T, std::underlying_type_t<Enum>>;
template <typename T, typename Enum> constexpr bool same_or_underlying = std::is_same_v<T, Enum> || is_underlying<T, Enum>;

// += operator that only allows addition of shorts or GAME_OBJECT_IDs. This is to
// allow us to see and manually review any places where we're adding something that
// might be the wrong type.
template <typename T> std::enable_if_t<same_or_underlying<T, GAME_OBJECT_ID>, GAME_OBJECT_ID> & operator+=(GAME_OBJECT_ID & lhs, T const & rhs)
{
	lhs = GAME_OBJECT_ID{ lhs + GAME_OBJECT_ID{ rhs } };
	return lhs;
}

template <typename T> std::enable_if_t<std::is_same_v<T, std::underlying_type_t<GAME_OBJECT_ID>>, GAME_OBJECT_ID>
	from_underlying (T rhs)
{
	return GAME_OBJECT_ID{ rhs };
}

enum SPRITE_TYPES
{
	SPR_FIRE0,
	SPR_FIRE1,
	SPR_FIRE2,
	SPR_FIRE3,
	SPR_SPLASH1,
	SPR_SPLASH2,
	SPR_SPLASH3,
	SPR_SPLASH4,
	SPR_SPLASH,
	SPR_RIPPLES,
	SPR_LENSFLARE,
	SPR_LENSFLARE_LIGHT,
	SPR_BULLETIMPACT,
	SPR_BUBBLES,
	SPR_UNDERWATERDUST,
	SPR_BLOOD,
	SPR_EMPTY1,
	SPR_UNKNOWN1,
	SPR_EMPTY2,
	SPR_BACKGROUND,
	SPR_GUI_UPLEFT,
	SPR_GUI_UPRIGHT,
	SPR_GUI_DOWNLEFT,
	SPR_GUI_DOWNRIGHT,
	SPR_GUI_DOWN,
	SPR_GUI_UP,
	SPR_GUI_LEFT,
	SPR_GUI_RIGHT,
	SPR_LIGHTHING
};
