#pragma once
#include "Math/Math.h"

using namespace TEN::Math;
using std::pair;

class FloorInfo;
struct CollisionInfo;
struct ItemInfo;
struct MESH_INFO;

constexpr auto MAX_COLLIDED_OBJECTS = 1024;
constexpr auto ITEM_RADIUS_YMAX = SECTOR(3);

constexpr auto VEHICLE_COLLISION_TERMINAL_VELOCITY = 30.0f;

extern GameBoundingBox GlobalCollisionBounds;
extern ItemInfo* CollidedItems[MAX_COLLIDED_OBJECTS];
extern MESH_INFO* CollidedMeshes[MAX_COLLIDED_OBJECTS];

class InteractionBounds
{
public:
	// Components
	GameBoundingBox				   BoundingBox		= GameBoundingBox::Zero;
	pair<EulerAngles, EulerAngles> OrientConstraint = {};

	// Constructors
	InteractionBounds(const GameBoundingBox& box, const pair<EulerAngles, EulerAngles>& orientConstraint);

	// Utilities
	bool TestEntityInteraction(const ItemInfo& entity0, const ItemInfo& entity1) const;
	bool TestPoseInteraction(const Pose& pose0, const Pose& pose1) const;
};

// TODO: Refactor this family of functions into a more comprehensive position alignment system. -- Sezz 2022.10.30
bool TestPlayerEntityInteract(ItemInfo* item, ItemInfo* laraItem, const InteractionBounds& interactBounds);
bool AlignPlayerToEntity(ItemInfo* item, ItemInfo* laraItem, const Vector3i& posOffset = Vector3i::Zero, const EulerAngles& orientOffset = EulerAngles::Zero, bool doSnapAlign = false);
bool AlignPlayerToPose(ItemInfo* item, const Pose& toPose, float velocity, short turnRate);

void GenericSphereBoxCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll);
bool GetCollidedObjects(ItemInfo* collidingItem, int radius, bool onlyVisible, ItemInfo** collidedItems, MESH_INFO** collidedMeshes, bool ignoreLara);
bool TestWithGlobalCollisionBounds(ItemInfo* item, ItemInfo* laraItem, CollisionInfo* coll);
void TestForObjectOnLedge(ItemInfo* item, CollisionInfo* coll);

bool ItemNearLara(const Vector3i& origin, int radius);
bool ItemNearTarget(const Vector3i& origin, ItemInfo* targetEntity, int radius);

bool TestBoundsCollide(ItemInfo* item, ItemInfo* laraItem, int radius);
bool TestBoundsCollideStatic(ItemInfo* item, const MESH_INFO& mesh, int radius);
bool ItemPushItem(ItemInfo* item, ItemInfo* laraItem, CollisionInfo* coll, bool enableSpasm, char bigPush);
bool ItemPushStatic(ItemInfo* laraItem, const MESH_INFO& mesh, CollisionInfo* coll);

bool CollideSolidBounds(ItemInfo* item, const GameBoundingBox& box, const Pose& pose, CollisionInfo* coll);
void CollideSolidStatics(ItemInfo* item, CollisionInfo* coll);

void AIPickupCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll);
void ObjectCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll);
void CreatureCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll);
void TrapCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll);

void DoProjectileDynamics(short itemNumber, int x, int y, int z, int xv, int yv, int zv);
void DoObjectCollision(ItemInfo* item, CollisionInfo* coll);
