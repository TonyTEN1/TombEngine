#pragma once

namespace TEN::Entities::TR4
{
	enum SkeletonState
	{
		SKELETON_STATE_BENEATH_FLOOR = 0,
		SKELETON_STATE_IDLE = 1,
		SKELETON_STATE_AVOID_ATTACK_1 = 5,
		SKELETON_STATE_AVOID_ATTACK_2 = 6,
		SKELETON_STATE_USE_SHIELD = 7,
		SKELETON_STATE_ATTACK_1 = 8,
		SKELETON_STATE_ATTACK_2 = 9,
		SKELETON_STATE_ATTACK_3 = 10,
		SKELETON_STATE_HURT_BY_SHOTGUN_1 = 12,
		SKELETON_STATE_HURT_BY_SHOTGUN_2 = 13,
		SKELETON_STATE_JUMP_LEFT = 19,
		SKELETON_STATE_JUMP_RIGHT = 20,
		SKELETON_STATE_JUMP_1_BLOCK = 21,
		SKELETON_STATE_JUMP_2_BLOCKS = 22,
		SKELETON_STATE_JUMP_LIE_DOWN = 25
	};

	void InitialiseSkeleton(short itemNumber);
	void SkeletonControl(short itemNumber);
}