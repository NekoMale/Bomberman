#include "bomberman.h"

int32_t move_on_level(level_t* level, movable_t* movable, const float delta_x, const float delta_y)
{
	float new_x = movable->x + delta_x;
	float new_y = movable->y + delta_y;

	if (new_x < 0)
		new_x = 0;
	else if (new_x + movable->width >= level->cols * level->cell_size)
		new_x = (level->cols * level->cell_size) - movable->width;

	if (new_y < 0)
		new_y = 0;
	else if (new_y + movable->height >= level->rows * level->cell_size)
		new_y = (level->rows * level->cell_size) - movable->height;

	int32_t cell = -1;
	if (new_x > movable->x)
	{
		uint32_t cell_x = (new_x + movable->width - 1) / level->cell_size;
		uint32_t cell_y = (movable->y + movable->height - 1) / level->cell_size;
		cell = level_cell(level, cell_x, cell_y);
		if (cell & BLOCK_MASK_UNWALKABLE)
		{
			movable->x = cell_x * level->cell_size - movable->width;
		}
		else {
			cell_y = movable->y / level->cell_size;
			cell = level_cell(level, cell_x, cell_y);
			if (cell & BLOCK_MASK_UNWALKABLE)
			{
				movable->x = cell_x * level->cell_size - movable->width;
			}
			else {
				movable->x = new_x;
			}
		}
	}
	else if (new_x < movable->x)
	{
		uint32_t cell_x = (new_x) / level->cell_size;
		uint32_t cell_y = (movable->y + movable->height - 1) / level->cell_size;
		cell = level_cell(level, cell_x, cell_y);
		if (cell & BLOCK_MASK_UNWALKABLE)
		{
			movable->x = cell_x * level->cell_size;
		}
		else {
			cell_y = movable->y / level->cell_size;
			cell = level_cell(level, cell_x, cell_y);
			if (cell & BLOCK_MASK_UNWALKABLE)
			{
				movable->x = cell_x * level->cell_size;
			}
			else {
				movable->x = new_x;
			}
		}
	}

	/*movable->x = new_x;
	movable->y = new_y;*/



	return cell;
}