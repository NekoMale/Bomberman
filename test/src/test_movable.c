#define CLOVE_SUITE_NAME MovableSuite
#include "..\inc\clove.h"
#include "..\..\inc\bomberman.h"

CLOVE_TEST(MovableFixPositionWidthOnRight)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 2, 2, 8, map);
	movable_t movable = { 1, 0, 4, 4 };
	move_on_level(&level, &movable, 50, 0);
	CLOVE_FLOAT_EQ(12, movable.x);
}

CLOVE_TEST(MovableFixPositionWidthOnLeft)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 2, 2, 8, map);
	movable_t movable = { 1, 0, 4, 4 };
	move_on_level(&level, &movable, -50, 0);
	CLOVE_FLOAT_EQ(0, movable.x);
}

CLOVE_TEST(MovableFixPositionHeightOnDown)
{
	level_t level;
	int32_t map[] = { 0, 0, 0, 0 };
	level_init(&level, 2, 2, 8, map);
	movable_t movable = { 1, 1, 4, 4 };
	move_on_level(&level, &movable, 0, -30);
	CLOVE_FLOAT_EQ(0, movable.y);
}

CLOVE_TEST(MovableFixPositionHeightOnUp)
{
	level_t level;
	int32_t map[] = { 0, 0, 
					  0, 0 };
	level_init(&level, 2, 2, 8, map);
	movable_t movable = { 1, 0, 4, 4 };
	move_on_level(&level, &movable, 0, 100);
	CLOVE_FLOAT_EQ(12, movable.y);
}

CLOVE_TEST(MovableWallNeckHitOnRight)
{
	level_t level;
	int32_t map[] = { 
		0, 0, 0,
		0, 0, 0x101, 
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 65, 60, 32, 32 };
	move_on_level(&level, &movable, 80, 0);
	CLOVE_FLOAT_EQ(96, movable.x);
}

CLOVE_TEST(MovableWallFeetHitOnRight)
{
	level_t level;
	int32_t map[] = {
		0, 0, 0,
		0, 0, 0x101,
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 65, 110, 32, 32 };
	move_on_level(&level, &movable, 80, 0);
	CLOVE_FLOAT_EQ(96, movable.x);
}

CLOVE_TEST(MovableWallNeckHitOnLeft)
{
	level_t level;
	int32_t map[] = {
		0, 0, 0,
		0x101, 0, 0,
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 65, 60, 32, 32 };
	move_on_level(&level, &movable, -80, 0);
	CLOVE_FLOAT_EQ(64, movable.x);
}

CLOVE_TEST(MovableWallFeetHitOnLeft)
{
	level_t level;
	int32_t map[] = {
		0, 0, 0,
		0x101, 0, 0,
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 65, 110, 32, 32 };
	move_on_level(&level, &movable, -80, 0);
	CLOVE_FLOAT_EQ(64, movable.x);
}

CLOVE_TEST(MovableWallHitOnRight)
{
	level_t level;
	int32_t map[] = { 0, 0x101, 
					  0, 0 };
	level_init(&level, 2, 2, 64, map);
	movable_t movable = { 1, 1, 32, 32 };
	move_on_level(&level, &movable, 80, 0);
	CLOVE_FLOAT_EQ(32, movable.x);
}

CLOVE_TEST(MovableWallHitOnDownFromLeft)
{
	level_t level;
	int32_t map[] = {
		0, 0, 0,
		0, 0, 0,
		0, 0x101, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 55, 64, 32, 32 };
	move_on_level(&level, &movable, 0, 80);
	CLOVE_FLOAT_EQ(96, movable.y);
}

CLOVE_TEST(MovableWallHitOnDownFromRight)
{
	level_t level;
	int32_t map[] = {
		0, 0, 0,
		0, 0, 0,
		0, 0x101, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 110, 64, 32, 32 };
	move_on_level(&level, &movable, 0, 80);
	CLOVE_FLOAT_EQ(96, movable.y);
}

CLOVE_TEST(MovableWallHitOnUpFromLeft)
{
	level_t level;
	int32_t map[] = {
		0, 0x101, 0,
		0, 0, 0,
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 55, 64, 32, 32 };
	move_on_level(&level, &movable, 0, -80);
	CLOVE_FLOAT_EQ(64, movable.y);
}

CLOVE_TEST(MovableWallHitOnUpFromRight)
{
	level_t level;
	int32_t map[] = {
		0, 0x101, 0,
		0, 0, 0,
		0, 0, 0 };
	level_init(&level, 3, 3, 64, map);
	movable_t movable = { 110, 64, 32, 32 };
	move_on_level(&level, &movable, 0, -80);
	CLOVE_FLOAT_EQ(64, movable.y);
}
