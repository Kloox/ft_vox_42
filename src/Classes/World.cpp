#include "vox.hpp"

extern Engine *engine;


World::World(void)
{
	this->noiser_ptr = engine->noiser;
	this->position = glm::vec3(0, CY + 1, 0);
	this->angle = glm::vec3(0, -0.5, 0);
	this->lookat = glm::vec3(0, 0, 0);
	this->up = glm::vec3(0, 0, 0);
	this->right = glm::vec3(0, 0, 0);
	this->forward = glm::vec3(0, 0, 0);
	this->cam_dx = 0;
	this->cam_dz = 0;
	this->shiftX = 0;
	this->shiftY = 0;
	this->shiftZ = 0;
	this->mx = 0;
	this->my = 0;
	this->mz = 0;
	this->face = 0;
	this->buildtype = 1;

	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				c[x][y][z] = new Chunk(x - engine->scx / 2, y - engine->scy / 2, z - engine->scz / 2);
			}

	this->assign_chunk_ptrs();
}

World::~World(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				glDeleteBuffers(1, &(this->c[x][y][z]->vbo));
				delete this->c[x][y][z];
			}
}

// World::World(World & inst) {
//         *this = inst;
// }

void World::assign_chunk_ptrs(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				if (x > 0)
					c[x][y][z]->left = c[x - 1][y][z];
				else
					c[x][y][z]->left = NULL;
				if (x < engine->scx - 1)
					c[x][y][z]->right = c[x + 1][y][z];
				else
					c[x][y][z]->right = NULL;
				if (y > 0)
					c[x][y][z]->below = c[x][y - 1][z];
				else
					c[x][y][z]->below = NULL;
				if (y < engine->scy - 1)
					c[x][y][z]->above = c[x][y + 1][z];
				else
					c[x][y][z]->above = NULL;
				if (z > 0)
					c[x][y][z]->front = c[x][y][z - 1];
				else
					c[x][y][z]->front = NULL;
				if (z < engine->scz - 1)
					c[x][y][z]->back = c[x][y][z + 1];
				else
					c[x][y][z]->back = NULL;
			}
}

void World::precompute(World *src, int *new_center)
{
	int start = glutGet(GLUT_ELAPSED_TIME);
	int dx, dz;

	dx = new_center[0] - engine->scx / 2;
	dz = new_center[2] - engine->scz / 2;
	int start_copy = glutGet(GLUT_ELAPSED_TIME);
	this->copy_world(src);
	int end_copy = glutGet(GLUT_ELAPSED_TIME);
	this->cam_dx = dx;
	this->cam_dz = dz;
	Chunk *save[128][8][128];
	memcpy(save, this->c, sizeof(Chunk *) * 128 * 8 * 128);

	if (engine->state.exiting)
	{
		engine->computing = false;
		return;
	}

	int start_swap = glutGet(GLUT_ELAPSED_TIME);
	this->precompute_swap_chunks(dx, dz, save);
	int end_swap = glutGet(GLUT_ELAPSED_TIME);

	int start_assign = glutGet(GLUT_ELAPSED_TIME);
	this->assign_chunk_ptrs();
	int end_assign = glutGet(GLUT_ELAPSED_TIME);

	if (engine->state.exiting)
	{
		engine->computing = false;
		return;
	}

	int start_noise = glutGet(GLUT_ELAPSED_TIME);
	this->precompute_noise(dx,dz, engine->state.threads);
	int end_noise = glutGet(GLUT_ELAPSED_TIME);

	if (engine->state.exiting)
	{
		engine->computing = false;
		return;
	}

	int start_update = glutGet(GLUT_ELAPSED_TIME);
	this->update_all_multithread(false, engine->state.threads);
	int end_update = glutGet(GLUT_ELAPSED_TIME);
	int start_push = glutGet(GLUT_ELAPSED_TIME);
	this->set_all_to_not_changed(); // maybe useless (Force changed to false because all chunks with true will be updated again.)
	int end_push = glutGet(GLUT_ELAPSED_TIME);

	engine->state.precompute_timer = end_update - start;
	if (engine->state.verbose)
	{
		fprintf(stderr, "Precomputing Timers:\n");
		fprintf(stderr, "\tCopy World: %dms\n\tSwap Chunks: %dms\n\tAssign ptrs: %dms\n\tNoise new chunks: %dms\n\tUpdate chunks: %dms\n\tPush list: %dms\n",
				end_copy - start_copy, end_swap - start_swap, end_assign - start_assign, end_noise - start_noise, end_update - start_update, end_push - start_push);
		fprintf(stderr, "Global: %dms\n\n", end_update - start);
	}
	this->set_all_to_disabled();

	engine->state.doSwap = true;
}

void World::precompute_noise(int dx, int dz, int split)
{
	this->precompute_noise_and_update(dx, dz);

	std::vector<vec3int> noise_arr;
	int noise_arr_size = 0;
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
				if (!(this->c[x][y][z]->noised))
					noise_arr_size++;
	noise_arr.reserve(noise_arr_size);
	noise_arr.resize(noise_arr_size);
	int j = 0;
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
				if (!(this->c[x][y][z]->noised)) {
					noise_arr[j] = vec3int(x,y,z);
					j++;
				}

	this->noise_tab_multithread(noise_arr, split);

	noise_arr.clear();
}

void World::precompute_swap_chunks(int dx, int dz, Chunk *save[128][8][128])
{

	if (dx < 0 || dz < 0)
	{
		for (int x = engine->scx - 1; x >= 0; x--)
			for (int y = engine->scy - 1; y >= 0; y--)
				for (int z = engine->scz - 1; z >= 0; z--)
				{
					if (!CUT_FORMULA)
					{
						this->c[x][y][z] = save[x + dx][y][z + dz];
						this->c[x][y][z]->ax = x - engine->scx / 2;
						this->c[x][y][z]->ay = y - engine->scy / 2;
						this->c[x][y][z]->az = z - engine->scz / 2;
					}
					else
					{
						if (x + dx < 0 || z + dz < 0)
						{
							this->c[x][y][z] = save[(x + dx < 0) ? engine->scx + (dx + x) : (x + dx) > engine->scx - 1 ? (dx + x) - engine->scx : x + dx][y][(z + dz < 0) ? engine->scz + (dz + z) : (z + dz) > engine->scz - 1 ? (dz + z) - engine->scz : z + dz];
							this->c[x][y][z]->ax = x - engine->scx / 2;
							this->c[x][y][z]->ay = y - engine->scy / 2;
							this->c[x][y][z]->az = z - engine->scz / 2;
						}
						else
						{
							this->c[x][y][z] = save[(x + dx > engine->scx - 1) ? (dx + x) - engine->scx : x + dx][y][(z + dz > engine->scz - 1) ? (dz + z) - engine->scz : z + dz];
							this->c[x][y][z]->ax = x - engine->scx / 2;
							this->c[x][y][z]->ay = y - engine->scy / 2;
							this->c[x][y][z]->az = z - engine->scz / 2;
						}
					}
				}
	}
	else
	{

		for (int x = 0; x < engine->scx; x++)
			for (int y = 0; y < engine->scy; y++)
				for (int z = 0; z < engine->scz; z++)
				{
					if (!CUT_FORMULA)
					{
						this->c[x][y][z] = save[x + dx][y][z + dz];
						this->c[x][y][z]->ax = x - engine->scx / 2;
						this->c[x][y][z]->ay = y - engine->scy / 2;
						this->c[x][y][z]->az = z - engine->scz / 2;
					}
					else
					{
						if (x + dx < 0 || z + dz < 0)
						{
							this->c[x][y][z] = save[(x + dx < 0) ? engine->scx + (dx + x) : (x + dx) > engine->scx - 1 ? (dx + x) - engine->scx : x + dx][y][(z + dz < 0) ? engine->scz + (dz + z) : (z + dz) > engine->scz - 1 ? (dz + z) - engine->scz : z + dz];
							this->c[x][y][z]->ax = x - engine->scx / 2;
							this->c[x][y][z]->ay = y - engine->scy / 2;
							this->c[x][y][z]->az = z - engine->scz / 2;
						}
						else
						{
							this->c[x][y][z] = save[(x + dx > engine->scx - 1) ? (dx + x) - engine->scx : x + dx][y][(z + dz > engine->scz - 1) ? (dz + z) - engine->scz : z + dz];
							this->c[x][y][z]->ax = x - engine->scx / 2;
							this->c[x][y][z]->ay = y - engine->scy / 2;
							this->c[x][y][z]->az = z - engine->scz / 2;
						}
					}
				}
	}
}

void World::precompute_noise_and_update(int dx, int dz)
{

	if (dx < 0 || dz < 0)
	{
		for (int x = engine->scx - 1; x >= 0; x--)
			for (int y = engine->scy - 1; y >= 0; y--)
				for (int z = engine->scz - 1; z >= 0; z--)
				{
					if (CUT_FORMULA)
					{
						this->c[x][y][z]->sx = ((x + dx) - (engine->scx / 2)) + this->shiftX;
						this->c[x][y][z]->sz = ((z + dz) - (engine->scz / 2)) + this->shiftZ;
						this->c[x][y][z]->clear_blk();
						this->c[x][y][z]->noised = false;
					}
				}
	}
	else
	{

		for (int x = 0; x < engine->scx; x++)
			for (int y = 0; y < engine->scy; y++)
				for (int z = 0; z < engine->scz; z++)
				{
					if (CUT_FORMULA)
					{
						this->c[x][y][z]->sx = ((x + dx) - (engine->scx / 2)) + this->shiftX;
						this->c[x][y][z]->sz = ((z + dz) - (engine->scz / 2)) + this->shiftZ;
						this->c[x][y][z]->clear_blk();
						this->c[x][y][z]->noised = false;
					}
				}
	}
}

void World::adjust_camera(World *src)
{
	this->position = glm::vec3(src->position.x + (-(this->cam_dx) * 16.0f), src->position.y, src->position.z + (-(this->cam_dz) * 16.0f));
	this->shiftX += this->cam_dx;
	this->cam_dx = 0;
	this->shiftZ += this->cam_dz;
	this->cam_dz = 0;
	this->forward = src->forward;
	this->right = src->right;
	this->up = src->up;
	this->lookat = src->lookat;
	this->angle = src->angle;
}

void World::set_all_to_disabled(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				this->c[x][y][z]->enable = false;
			}
}

void World::set_all_to_not_changed(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				this->c[x][y][z]->changed = false;
			}
}

void World::set_all_to_not_noised(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				this->c[x][y][z]->noised = false;
			}
}

void World::set_all_cleared_blk(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				this->c[x][y][z]->clear_blk();
			}
}

void World::copy_world(World *ptr)
{
	this->position = ptr->position;
	this->forward = ptr->forward;
	this->right = ptr->right;
	this->up = ptr->up;
	this->lookat = ptr->lookat;
	this->angle = ptr->angle;

	this->mz = ptr->mz;
	this->my = ptr->my;
	this->mz = ptr->mz;

	this->cam_dx = ptr->cam_dx;
	this->cam_dz = ptr->cam_dz;

	this->face = ptr->face;
	this->buildtype = ptr->buildtype;
	this->shiftX = ptr->shiftX;
	this->shiftZ = ptr->shiftZ;
	this->shiftY = ptr->shiftY;

	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
				this->c[x][y][z]->copy_chunk(ptr->c[x][y][z]);
	this->assign_chunk_ptrs();
}

void World::bind_all_buffers(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				this->c[x][y][z]->enable = true;
				if (this->c[x][y][z]->elements > 0)
				{
					glBindBuffer(GL_ARRAY_BUFFER, this->c[x][y][z]->vbo);
					glBufferData(GL_ARRAY_BUFFER, this->c[x][y][z]->elements * sizeof(byte4), this->c[x][y][z]->v, GL_DYNAMIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
			}
}

void World::create_all_buffers(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				glGenBuffers(1, &(this->c[x][y][z]->vbo));
			}
}

void World::noise_all(void)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				c[x][y][z]->noise(this->noiser_ptr);
			}
}

void World::noise_worker_range(int xstart, int xend, int ystart, int yend, int zstart, int zend)
{
	for (int x = xstart; x < xend; x++)
	{
		for (int y = ystart; y < yend; y++)
		{
			for (int z = zstart; z < zend; z++)
			{
				this->c[x][y][z]->noise(this->noiser_ptr);
			}
		}
	}
}

void World::update_worker_range(int xstart, int xend, int ystart, int yend, int zstart, int zend, bool doBind)
{
	for (int x = xstart; x < xend; x++)
	{
		for (int y = ystart; y < yend; y++)
		{
			for (int z = zstart; z < zend; z++)
			{
				this->c[x][y][z]->update(doBind);
				this->c[x][y][z]->initialized = true;
			}
		}
	}
}

void World::update_all_multithread(bool doBind, int split)
{
	if (split == 0 || split == 1)
	{
		this->update_all(doBind);
		return;
	}
	if (split >= 2 && split < 4)
	{
		std::thread t_one(&World::update_worker_range, this, 0, engine->scx, 0, engine->scy / 4, 0, engine->scz, doBind);
		std::thread t_two(&World::update_worker_range, this, 0, engine->scx, engine->scy / 4, engine->scy / 2, 0, engine->scz, doBind);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->update_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz, doBind);
		t_one.join();
		t_two.join();
	}
	else if (split >= 4 && split < 8)
	{
		std::thread t_one(&World::update_worker_range, this, 0, engine->scx, 0, engine->scy / 8, 0, engine->scz, doBind);
		std::thread t_two(&World::update_worker_range, this, 0, engine->scx, engine->scy / 8, engine->scy / 4, 0, engine->scz, doBind);

		std::thread t_three(&World::update_worker_range, this, 0, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz, doBind);
		std::thread t_four(&World::update_worker_range, this, 0, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz, doBind);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->update_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz, doBind);
		t_one.join();
		t_two.join();
		t_three.join();
		t_four.join();
	}
	else if (split >= 8 && split < 16)
	{
		std::thread t_one(&World::update_worker_range, this, 0, engine->scx / 2, 0, engine->scy / 8, 0, engine->scz, doBind);
		std::thread t_oneX(&World::update_worker_range, this, engine->scx / 2, engine->scx, 0, engine->scy / 8, 0, engine->scz, doBind);
		std::thread t_two(&World::update_worker_range, this, 0, engine->scx / 2, engine->scy / 8, engine->scy / 4, 0, engine->scz, doBind);
		std::thread t_twoX(&World::update_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 8, engine->scy / 4, 0, engine->scz, doBind);

		std::thread t_three(&World::update_worker_range, this, 0, engine->scx / 2, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz, doBind);
		std::thread t_threeX(&World::update_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz, doBind);

		std::thread t_four(&World::update_worker_range, this, 0, engine->scx / 2, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz, doBind);
		std::thread t_fourX(&World::update_worker_range, this, engine->scx / 2, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz, doBind);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->update_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz, doBind);

		t_one.join();
		t_oneX.join();
		t_two.join();
		t_twoX.join();
		t_three.join();
		t_threeX.join();
		t_four.join();
		t_fourX.join();
	}
	else
	{
		std::thread update_one(&World::update_worker_range, this, 0, engine->scx / 2, 0, engine->scy / 8, 0, engine->scz / 2, doBind);
		std::thread update_oneZ(&World::update_worker_range, this, 0, engine->scx / 2, 0, engine->scy / 8, engine->scz / 2, engine->scz, doBind);

		std::thread update_oneX(&World::update_worker_range, this, engine->scx / 2, engine->scx, 0, engine->scy / 8, 0, engine->scz / 2, doBind);
		std::thread update_oneXZ(&World::update_worker_range, this, engine->scx / 2, engine->scx, 0, engine->scy / 8, engine->scz / 2, engine->scz, doBind);

		std::thread update_two(&World::update_worker_range, this, 0, engine->scx / 2, engine->scy / 8, engine->scy / 4, 0, engine->scz / 2, doBind);
		std::thread update_twoZ(&World::update_worker_range, this, 0, engine->scx / 2, engine->scy / 8, engine->scy / 4, engine->scz / 2, engine->scz, doBind);

		std::thread update_twoX(&World::update_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 8, engine->scy / 4, 0, engine->scz / 2, doBind);
		std::thread update_twoXZ(&World::update_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 8, engine->scy / 4, engine->scz / 2, engine->scz, doBind);

		std::thread update_three(&World::update_worker_range, this, 0, engine->scx / 2, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz / 2, doBind);
		std::thread update_threeZ(&World::update_worker_range, this, 0, engine->scx / 2, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), engine->scz / 2, engine->scz, doBind);

		std::thread update_threeX(&World::update_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz / 2, doBind);
		std::thread update_threeXZ(&World::update_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), engine->scz / 2, engine->scz, doBind);

		std::thread update_four(&World::update_worker_range, this, 0, engine->scx / 2, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz / 2, doBind);
		std::thread update_fourZ(&World::update_worker_range, this, 0, engine->scx / 2, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, engine->scz / 2, engine->scz, doBind);

		std::thread update_fourX(&World::update_worker_range, this, engine->scx / 2, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz / 2, doBind);
		std::thread update_fourXZ(&World::update_worker_range, this, engine->scx / 2, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, engine->scz / 2, engine->scz, doBind);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->update_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz, doBind);
		update_one.join();
		update_oneZ.join();

		update_oneX.join();
		update_oneXZ.join();

		update_two.join();
		update_twoZ.join();

		update_twoX.join();
		update_twoXZ.join();

		update_three.join();
		update_threeZ.join();

		update_threeX.join();
		update_threeXZ.join();

		update_four.join();
		update_fourZ.join();

		update_fourX.join();
		update_fourXZ.join();
	}
	return ;
}

void World::noise_all_multithread(int split)
{
	if (split == 0 || split == 1)
	{
		this->noise_all();
	}
	else if (split >= 2 && split < 4)
	{
		std::thread noise_one(&World::noise_worker_range, this, 0, engine->scx, 0, engine->scy / 4, 0, engine->scz);
		std::thread noise_two(&World::noise_worker_range, this, 0, engine->scx, engine->scy / 4, engine->scy / 2, 0, engine->scz);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->noise_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz);
		noise_one.join();
		noise_two.join();
	}
	else if (split >= 4 && split < 8)
	{
		std::thread noise_one(&World::noise_worker_range, this, 0, engine->scx, 0, engine->scy / 8, 0, engine->scz);
		std::thread noise_two(&World::noise_worker_range, this, 0, engine->scx, engine->scy / 8, engine->scy / 4, 0, engine->scz);

		std::thread noise_three(&World::noise_worker_range, this, 0, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz);
		std::thread noise_four(&World::noise_worker_range, this, 0, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->noise_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz);
		noise_one.join();
		noise_two.join();
		noise_three.join();
		noise_four.join();
	}
	else if (split >= 8 && split < 16)
	{
		std::thread noise_one(&World::noise_worker_range, this, 0, engine->scx / 2, 0, engine->scy / 8, 0, engine->scz);
		std::thread noise_oneX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, 0, engine->scy / 8, 0, engine->scz);
		std::thread noise_two(&World::noise_worker_range, this, 0, engine->scx / 2, engine->scy / 8, engine->scy / 4, 0, engine->scz);
		std::thread noise_twoX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 8, engine->scy / 4, 0, engine->scz);

		std::thread noise_three(&World::noise_worker_range, this, 0, engine->scx / 2, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz);
		std::thread noise_threeX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz);

		std::thread noise_four(&World::noise_worker_range, this, 0, engine->scx / 2, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz);
		std::thread noise_fourX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->noise_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz);

		noise_one.join();
		noise_oneX.join();
		noise_two.join();
		noise_twoX.join();
		noise_three.join();
		noise_threeX.join();
		noise_four.join();
		noise_fourX.join();
	}
	else
	{
		std::thread noise_one(&World::noise_worker_range, this, 0, engine->scx / 2, 0, engine->scy / 8, 0, engine->scz / 2);
		std::thread noise_oneZ(&World::noise_worker_range, this, 0, engine->scx / 2, 0, engine->scy / 8, engine->scz / 2, engine->scz);

		std::thread noise_oneX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, 0, engine->scy / 8, 0, engine->scz / 2);
		std::thread noise_oneXZ(&World::noise_worker_range, this, engine->scx / 2, engine->scx, 0, engine->scy / 8, engine->scz / 2, engine->scz);

		std::thread noise_two(&World::noise_worker_range, this, 0, engine->scx / 2, engine->scy / 8, engine->scy / 4, 0, engine->scz / 2);
		std::thread noise_twoZ(&World::noise_worker_range, this, 0, engine->scx / 2, engine->scy / 8, engine->scy / 4, engine->scz / 2, engine->scz);

		std::thread noise_twoX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 8, engine->scy / 4, 0, engine->scz / 2);
		std::thread noise_twoXZ(&World::noise_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 8, engine->scy / 4, engine->scz / 2, engine->scz);

		std::thread noise_three(&World::noise_worker_range, this, 0, engine->scx / 2, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz / 2);
		std::thread noise_threeZ(&World::noise_worker_range, this, 0, engine->scx / 2, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), engine->scz / 2, engine->scz);

		std::thread noise_threeX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), 0, engine->scz / 2);
		std::thread noise_threeXZ(&World::noise_worker_range, this, engine->scx / 2, engine->scx, engine->scy / 4, (engine->scy / 4) + (engine->scy / 8), engine->scz / 2, engine->scz);

		std::thread noise_four(&World::noise_worker_range, this, 0, engine->scx / 2, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz / 2);
		std::thread noise_fourZ(&World::noise_worker_range, this, 0, engine->scx / 2, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, engine->scz / 2, engine->scz);

		std::thread noise_fourX(&World::noise_worker_range, this, engine->scx / 2, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, 0, engine->scz / 2);
		std::thread noise_fourXZ(&World::noise_worker_range, this, engine->scx / 2, engine->scx, (engine->scy / 4) + (engine->scy / 8), engine->scy / 2, engine->scz / 2, engine->scz);
		// |                 |
		// V ABOVE SEA LEVEL V
		this->noise_worker_range(0, engine->scx, engine->scy / 2, engine->scy, 0, engine->scz);
		noise_one.join();
		noise_oneZ.join();

		noise_oneX.join();
		noise_oneXZ.join();

		noise_two.join();
		noise_twoZ.join();

		noise_twoX.join();
		noise_twoXZ.join();

		noise_three.join();
		noise_threeZ.join();

		noise_threeX.join();
		noise_threeXZ.join();

		noise_four.join();
		noise_fourZ.join();

		noise_fourX.join();
		noise_fourXZ.join();
	}
	return ;
}

void World::noise_tab_range(std::vector<vec3int> noise_arr, int start, long unsigned int end)
{
	for (long unsigned int x = start; x < end; x++)
		this->c[noise_arr[x].x][noise_arr[x].y][noise_arr[x].z]->noise(this->noiser_ptr);
}

void World::noise_tab_multithread(std::vector<vec3int> noise_arr, int split) 
{
	if (split == 0 || split == 1) {
		for (long unsigned int x = 0; x < noise_arr.size(); x++)
			this->c[noise_arr[x].x][noise_arr[x].y][noise_arr[x].z]->noise(this->noiser_ptr);
	} else {
		int adder = noise_arr.size() / split;
		int start = 0;
		int end = 0;
		std::vector<std::thread> pool;
		pool.reserve(split);
		pool.resize(split);
		int j = 0;
		for (int spliter = split; spliter > 0; spliter--) {
			end = spliter * adder;
			start = spliter * adder - adder;
			pool[j] = std::thread(&World::noise_tab_range, this, noise_arr, start, end);
			j++;
		}
		for (long unsigned int x = 0; x < pool.size(); x++)
			pool[x].join();
		pool.clear();
	}
}

void World::update_all(bool doBind)
{
	for (int x = 0; x < engine->scx; x++)
		for (int y = 0; y < engine->scy; y++)
			for (int z = 0; z < engine->scz; z++)
			{
				c[x][y][z]->update(doBind);
				c[x][y][z]->initialized = true;
			}
}

uint8_t World::getChunk(int x, int y, int z)
{
	int cx = (x + CX * (engine->scx / 2)) / CX;
	int cy = (y + CY * (engine->scy / 2)) / CY;
	int cz = (z + CZ * (engine->scz / 2)) / CZ;

	if (cx < 0 || cx >= engine->scx || cy < 0 || cy >= engine->scy || cz < 0 || cz >= engine->scz)
		return 0;

	return this->c[cx][cy][cz]->get(x & (CX - 1), y & (CY - 1), z & (CZ - 1));
}

void World::setChunk(int x, int y, int z, uint8_t type)
{
	int cx = (x + CX * (engine->scx / 2)) / CX;
	int cy = (y + CY * (engine->scy / 2)) / CY;
	int cz = (z + CZ * (engine->scz / 2)) / CZ;

	if (cx < 0 || cx >= engine->scx || cy < 0 || cy >= engine->scy || cz < 0 || cz >= engine->scz)
		return;

	this->c[cx][cy][cz]->set(x & (CX - 1), y & (CY - 1), z & (CZ - 1), type);
}

Chunk *World::getChunkPtr(int x, int y, int z)
{
	int cx = (x + CX * (engine->scx / 2)) / CX;
	int cy = (y + CY * (engine->scy / 2)) / CY;
	int cz = (z + CZ * (engine->scz / 2)) / CZ;

	if (cx < 0 || cx >= engine->scx || cy < 0 || cy >= engine->scy || cz < 0 || cz >= engine->scz)
		return NULL;

	return this->c[cx][cy][cz];
}

void World::update_vectors()
{
	this->forward.x = sinf(this->angle.x);
	this->forward.y = 0;
	this->forward.z = cosf(this->angle.x);

	this->right.x = -cosf(this->angle.x);
	this->right.y = 0;
	this->right.z = sinf(this->angle.x);

	this->lookat.x = sinf(this->angle.x) * cosf(this->angle.y);
	this->lookat.y = sinf(this->angle.y);
	this->lookat.z = cosf(this->angle.x) * cosf(this->angle.y);

	this->up = glm::cross(this->right, this->lookat);
}

void World::render(const glm::mat4 &pv, int renderDistance, GLint uniform_mvp)
{
	float ud = 1.0 / 0.0;
	int ux = -1;
	int uy = -1;
	int uz = -1;

	glm::vec3 index = engine->get_chunk_position(this->position);
	int kx = static_cast<int>(index.x);
	int ky = static_cast<int>(index.y);
	int kz = static_cast<int>(index.z);

	int kxm = kx - renderDistance;
	kxm = kxm < 1 ? 0 : kxm;
	int kzm = kz - renderDistance;
	kzm = kzm < 1 ? 0 : kzm;
	int kym = ky - renderDistance;
	kym = kym < 1 ? 0 : kym;
	int kxp = kx + renderDistance;
	kxp = kxp > engine->scx ? engine->scx : kxp;
	int kzp = kz + renderDistance;
	kzp = kzp > engine->scz ? engine->scz : kzp;
	int kyp = ky + renderDistance;
	kyp = kyp > engine->scy ? engine->scy : kyp;
	for (int x = kxm; x < (kxp); x++)
	{
		for (int y = kym; y < (kyp); y++)
		{
			for (int z = kzm; z < (kzp); z++)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(this->c[x][y][z]->ax * CX, this->c[x][y][z]->ay * CY, this->c[x][y][z]->az * CZ));
				glm::mat4 mvp = pv * model;

				glm::vec4 center = mvp * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);

				float d = glm::length(center);
				center.x /= center.w;
				center.y /= center.w;


				// If it is behind the camera, don't bother drawing it
				if (center.z < -CY / 2)
					continue;

				// If it is outside the screen, don't bother drawing it
				if (fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
					continue;

				if (!(this->c[x][y][z]->initialized))
				{
					// But if it is the closest to the camera, mark it for initialization
					if (ux < 0 || d < ud)
					{
						ud = d;
						ux = x;
						uy = y;
						uz = z;
					}
					continue;
				}

				glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
				if (this->c[x][y][z]->enable)
					this->c[x][y][z]->render();
				else
				{
					this->c[x][y][z]->enable = true;
					if (this->c[x][y][z]->elements > 0)
					{
						glBindBuffer(GL_ARRAY_BUFFER, this->c[x][y][z]->vbo);
						glBufferData(GL_ARRAY_BUFFER, this->c[x][y][z]->elements * sizeof(byte4), this->c[x][y][z]->v, GL_DYNAMIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}
					this->c[x][y][z]->render();
				}
			}
		}
	}

	if (ux >= 0)
	{
		this->c[ux][uy][uz]->noise(this->noiser_ptr);
		if (c[ux][uy][uz]->left)
			this->c[ux][uy][uz]->left->noise(this->noiser_ptr);
		if (this->c[ux][uy][uz]->right)
			this->c[ux][uy][uz]->right->noise(this->noiser_ptr);
		if (this->c[ux][uy][uz]->below)
			this->c[ux][uy][uz]->below->noise(this->noiser_ptr);
		if (this->c[ux][uy][uz]->above)
			this->c[ux][uy][uz]->above->noise(this->noiser_ptr);
		if (this->c[ux][uy][uz]->front)
			this->c[ux][uy][uz]->front->noise(this->noiser_ptr);
		if (this->c[ux][uy][uz]->back)
			this->c[ux][uy][uz]->back->noise(this->noiser_ptr);
		this->c[ux][uy][uz]->initialized = true;
	}
}