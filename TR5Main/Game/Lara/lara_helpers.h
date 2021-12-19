#pragma once

struct ITEM_INFO;
struct COLL_INFO;

// -----------------------------
// HELPER FUNCTIONS
// For State Control & Collision
// -----------------------------

void DoLaraStep(ITEM_INFO* item, COLL_INFO* coll);
void DoLaraCrawlVault(ITEM_INFO* item, COLL_INFO* coll);
void DoLaraCrawlToHangSnap(ITEM_INFO* item, COLL_INFO* coll);
void DoLaraLean(ITEM_INFO* item, COLL_INFO* coll, int maxAngle, short rate);
void DoLaraCrawlFlex(ITEM_INFO* item, COLL_INFO* coll, short maxAngle, short rate);
void SetLaraJumpDirection(ITEM_INFO* item, COLL_INFO* coll);
void SetLaraJumpQueue(ITEM_INFO* item, COLL_INFO* coll);
void SetLaraFallState(ITEM_INFO* item);
void SetLaraFallBackState(ITEM_INFO* item);
void SetLaraSlideState(ITEM_INFO* item, COLL_INFO* coll);
short GetLaraSlideDirection(COLL_INFO* coll);
void ResetLaraFlex(ITEM_INFO* item, float rate = 1.0f);
void HandleLaraMovementParameters(ITEM_INFO* item, COLL_INFO* coll);
