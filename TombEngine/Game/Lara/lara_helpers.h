#pragma once
#include "Game/collision/collide_room.h"

enum class JumpDirection;
struct ItemInfo;
struct CollisionInfo;
struct LaraInfo;
struct VaultTestResult;

// -----------------------------
// HELPER FUNCTIONS
// For State Control & Collision
// -----------------------------

void HandleLaraMovementParameters(ItemInfo* item, CollisionInfo* coll);
bool HandleLaraVehicle(ItemInfo* item, CollisionInfo* coll);
void HandlePlayerLean(ItemInfo* item, CollisionInfo* coll, short baseRate, short maxAngle);
void HandlePlayerCrawlFlex(ItemInfo& item);
void HandlePlayerWetnessDrips(ItemInfo& item);
void HandlePlayerDiveBubbles(ItemInfo& item);
void HandlePlayerAirBubbles(ItemInfo* item);

void EaseOutLaraHeight(ItemInfo* item, int height);
void DoLaraStep(ItemInfo* item, CollisionInfo* coll);
void DoLaraMonkeyStep(ItemInfo* item, CollisionInfo* coll);
void DoLaraCrawlToHangSnap(ItemInfo* item, CollisionInfo* coll);
void DoLaraTightropeBalance(ItemInfo* item);
void DoLaraTightropeLean(ItemInfo* item);
void DoLaraTightropeBalanceRegen(ItemInfo* item);
void DoLaraFallDamage(ItemInfo* item);

LaraInfo& GetLaraInfo(ItemInfo& item);
const LaraInfo& GetLaraInfo(const ItemInfo& item);
LaraInfo*& GetLaraInfo(ItemInfo* item);
JumpDirection GetPlayerJumpDirection(const ItemInfo& item, const CollisionInfo& coll);

short GetLaraSlideDirection(ItemInfo* item, CollisionInfo* coll);

short ModulateLaraTurnRate(short turnRate, short accelRate, short minTurnRate, short maxTurnRate, float axisCoeff, bool invert);
void ModulateLaraTurnRateX(ItemInfo* item, short accelRate, short minTurnRate, short maxTurnRate, bool invert = true);
void ModulateLaraTurnRateY(ItemInfo* item, short accelRate, short minTurnRate, short maxTurnRate, bool invert = false);
void ResetPlayerTurnRateX(ItemInfo& item, short decelRate = SHRT_MAX);
void ResetPlayerTurnRateY(ItemInfo& item, short decelRate = SHRT_MAX);
void ModulateLaraSwimTurnRates(ItemInfo* item, CollisionInfo* coll);
void ModulateLaraSubsuitSwimTurnRates(ItemInfo* item);
void UpdateLaraSubsuitAngles(ItemInfo* item);
void ModulateLaraSlideVelocity(ItemInfo* item, CollisionInfo* coll);
void AlignLaraToSurface(ItemInfo* item, float alpha = 0.15f);

void SetLaraRunJumpQueue(ItemInfo* item, CollisionInfo* coll);
void SetLaraVault(ItemInfo* item, CollisionInfo* coll, VaultTestResult vaultResult);
void SetLaraLand(ItemInfo* item, CollisionInfo* coll);
void SetLaraFallAnimation(ItemInfo* item);
void SetLaraFallBackAnimation(ItemInfo* item);
void SetLaraMonkeyFallAnimation(ItemInfo* item);
void SetLaraMonkeyRelease(ItemInfo* item);
void SetLaraSlideAnimation(ItemInfo* item, CollisionInfo* coll);
void SetLaraCornerAnimation(ItemInfo* item, CollisionInfo* coll, bool flip);
void SetLaraSwimDiveAnimation(ItemInfo* item);
void SetLaraVehicle(ItemInfo* item, ItemInfo* vehicle = nullptr);

void ResetPlayerLean(ItemInfo* item, float alpha = 1.0f, bool resetRoll = true, bool resetPitch = true);
void ResetPlayerFlex(ItemInfo* item, float alpha = 1.0f);

void RumbleLaraHealthCondition(ItemInfo* item);
