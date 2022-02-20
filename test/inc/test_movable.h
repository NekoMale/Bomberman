#include "..\inc\clove.h"
#include "..\..\inc\bomberman.h"

CLOVE_TEST(MovableFixPositionX)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 4, 4, 8, map);
	movable_t movable = { 1, 0, 4, 4 };
	move_on_level(&level, &movable, -10, 0);
	CLOVE_FLOAT_EQ(movable.x, 0);
}

CLOVE_TEST(MovableFixPositionY)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 4, 4, 8, map);
	movable_t movable = { 1, 1, 4, 4 };
	move_on_level(&level, &movable, 0, -30);
	CLOVE_FLOAT_EQ(movable.y, 0);
}

CLOVE_TEST(MovableFixPositionWidthOnRight)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 4, 4, 8, map);
	movable_t movable = { 1, 0, 4, 4 };
	move_on_level(&level, &movable, 50, 0);
	CLOVE_FLOAT_EQ(movable.x, 28);
}

CLOVE_TEST(MovableFixPositionHeight)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 4, 4, 8, map);
	movable_t movable = { 1, 0, 4, 4 };
	move_on_level(&level, &movable, 0, 100);
	CLOVE_FLOAT_EQ(movable.y, 28);
}

CLOVE_TEST(MovableWallFeetHitOnRight)
{
	level_t level;
	int32_t map[] = { 
		0, 0, 0,
		0, 0, 0x101, 
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 65, 60, 32, 32 };
	move_on_level(&level, &movable, 80, 0);
	CLOVE_FLOAT_EQ(movable.x, 96);
}

CLOVE_TEST(MovableWallNeckHitOnRight)
{
	level_t level;
	int32_t map[] = {
		0, 0, 0,
		0, 0, 0x101,
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 65, 110, 32, 32 };
	move_on_level(&level, &movable, 80, 0);
	CLOVE_FLOAT_EQ(movable.x, 96);
}

CLOVE_TEST(MovableWallHitOnRight)
{
	level_t level;
	int32_t map[] = { 0, 0x101, 0, 0 };
	level_init(&level, 2, 2, 64, map);
	movable_t movable = { 1, 1, 32, 32 };
	move_on_level(&level, &movable, 80, 0);
	CLOVE_FLOAT_EQ(movable.x, 32);
}

CLOVE_SUITE(MovableSuite)
{
	CLOVE_SUITE_TESTS(
		MovableFixPositionX,
		MovableFixPositionY,
		MovableFixPositionWidthOnRight,
		MovableFixPositionHeight,
		MovableWallFeetHitOnRight,
		MovableWallNeckHitOnRight,
		MovableWallHitOnRight
	);
}