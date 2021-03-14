#include "vox.hpp"

/*
** globals
*/

Engine *engine = NULL;


/*
** callbacks
*/

static void reshape(int w, int h)
{
	engine->g_state.ww = w;
	engine->g_state.wh = h;
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(w), 0.0f, static_cast<GLfloat>(h));
	glUniformMatrix4fv(glGetUniformLocation(engine->ogl_state.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glViewport(0, 0, w, h);
}

static void idle()
{
	if (engine->state.exiting && !(engine->state.loading) && !(engine->computing))
		do_exit(engine);
	else if (!engine->state.exiting)
		engine->move(PLAYER_SPEED + engine->state.speedmult);

	if (engine->infinite)
	{
		if (!(engine->computing) && engine->must_recompute_world())
		{
			engine->computing = true;
			std::thread precomputing(&World::precompute, engine->next_world, engine->world, engine->new_center);
			precomputing.detach();
		}
	}

	// swap worlds
	if (engine->state.doSwap)
	{
		engine->swap_worlds();
		engine->state.doSwap = false;
		if (engine->computing)
			engine->computing = false;
	}

	glutPostRedisplay();
}

static void specialup(int key, int x, int y)
{
	(void)key;
	(void)(x);
	(void)(y);
	engine->shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
}

static void special(int key, int x, int y)
{
	(void)x;
	(void)y;
	switch (key)
	{
	// Set position to start
	case GLUT_KEY_PAGE_UP:
		if (engine->state.volume < 1.0f)
			engine->state.volume += 0.1f;
		fprintf(stderr, "Set volume to %f\n", engine->state.volume);
		engine->sound->setSoundVolume(engine->state.volume);
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (engine->state.volume > 0.05f)
			engine->state.volume -= 0.1f;
		fprintf(stderr, "Set volume to %f\n", engine->state.volume);
		engine->sound->setSoundVolume(engine->state.volume);
		break;
	case GLUT_KEY_HOME:
		engine->world->position = glm::vec3(0, CY + 1, 0);
		engine->world->angle = glm::vec3(0, 0, 0);
		engine->world->update_vectors();
		break;
	case GLUT_KEY_INSERT:
		engine->state.anchor_cam = !(engine->state.anchor_cam);
		fprintf(stderr, "Anchor cam: %s\n", BoolToString(engine->state.anchor_cam));
		break;
	// Set position to sky
	case GLUT_KEY_END:
		engine->world->position = glm::vec3(0, CX * engine->scx, 0);
		engine->world->angle = glm::vec3(0, -M_PI * 0.49, 0);
		engine->world->update_vectors();
		break;
	// Toggle motion blur
	case GLUT_KEY_F1:
		engine->g_state.maxi *= 2;
		if (engine->g_state.maxi >= 32)
			engine->g_state.maxi = 1;
		fprintf(stderr, "Frame sampling is now %d\n", engine->g_state.maxi);
		break;
	// Toggle anti-aliasing
	case GLUT_KEY_F2:
		engine->g_state.aa = !(engine->g_state.aa);
		fprintf(stderr, "Anti-aliasing is now %s\n", engine->g_state.aa ? "on" : "off");
		break;
	// active/disable transparency
	case GLUT_KEY_F3:
		engine->g_state.transparency = !(engine->g_state.transparency);
		fprintf(stderr, "Transparency is now %s\n", engine->g_state.transparency ? "on" : "off");
		break;
	// Toggle transparency
	case GLUT_KEY_F4:
		engine->state.collision = !(engine->state.collision);
		fprintf(stderr, "Collision is now %s\n", engine->state.collision ? "on" : "off");
		break;
	// Toggle focus-on-glass
	case GLUT_KEY_F5:
		engine->state.focus_on_transparent = !(engine->state.focus_on_transparent);
		fprintf(stderr, "Focussing on transparent blocks is now %s\n", engine->state.focus_on_transparent ? "on" : "off");
		break;
	// Framerate limitation
	case GLUT_KEY_F6:
		if (engine->g_state.target_fps == 60) {
			engine->g_state.target_fps = 0;
			engine->g_state.framerate = 10000;
		}
		else if (engine->g_state.target_fps == 0) {
			engine->g_state.target_fps = 30;
			engine->g_state.framerate = 30 * (engine->g_state.maxi / 4 > 0 ? engine->g_state.maxi / 4 : 2);
		}
		else if (engine->g_state.target_fps == 30) {
			engine->g_state.target_fps = 60;
			engine->g_state.framerate = 60 * (engine->g_state.maxi / 4 > 0 ? engine->g_state.maxi / 4 : 2);
		}
		if (engine->g_state.target_fps)
			fprintf(stderr, "Framerate limit is now approximately %d Hz\n", engine->g_state.target_fps);
		else
			fprintf(stderr, "No framerate limit\n");
		break;
	// Render distance
	case GLUT_KEY_F7:
		engine->g_state.renderDistance++;
		if (engine->g_state.renderDistance > engine->scx / 2)
			engine->g_state.renderDistance = 2;
		fprintf(stderr, "Set render distance to %d chunks\n", engine->g_state.renderDistance);
		break;
	// Force swap
	case GLUT_KEY_F8:
		engine->state.doSwap = true;
		break;
	// Desactivate infinite world
	case GLUT_KEY_F10:
		engine->infinite = !(engine->infinite);
		fprintf(stderr, "Infinite World: %s\n", engine->infinite ? "on" : "off");
		break;
	// Autowalk
	case GLUT_KEY_F11:
		engine->autowalk = !(engine->autowalk);
		fprintf(stderr, "Autowalk: %s\n", BoolToString(engine->autowalk));
		break;
	// "Console dev" with gdb
	case GLUT_KEY_F12:
	{
		World *w = engine->world;
		World *nw = engine->next_world;
		Engine *e = engine;
		Noiser *n = engine->noiser;
		(void)w;
		(void)nw;
		(void)e;
		(void)n;
		fprintf(stderr, "GDB breakpoint launch (need compil with \"make gdb\"\n");
		// BP HOOK
		break;
	}
	// Position
	case GLUT_KEY_F9:
		engine->state.debug_show += 1;
		if (engine->state.debug_show > 4)
			engine->state.debug_show = 0;
		break;
	}

	engine->shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
}

static void keyboard(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	if (engine->autowalk)
		engine->keys |= 4;
	switch (key)
	{
	case 'q':
	case 'Q':
		engine->keys |= 1;
		break;
	case 'd':
	case 'D':
		engine->keys |= 2;
		break;
	case 'z':
	case 'Z':
		if (!(engine->autowalk))
		{
			engine->keys |= 4;
		}
		break;
	case 's':
	case 'S':
		engine->keys |= 8;
		break;
	case 'e':
	case 'E':
		engine->keys |= 16;
		break;
	case 'a':
	case 'A':
		engine->keys |= 32;
		break;
	case '+':
		engine->state.speedmult += 1.0f;
		break;
	case '-':
		engine->state.speedmult -= 1.0f;
		break;
	case 27:
		if (!(engine->state.loading) && !(engine->computing))
			do_exit(engine);
		else
			engine->state.exiting = true;
	}

	engine->shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
}

static void keyboardup(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	switch (key)
	{
	case 'q':
	case 'Q':
		engine->keys &= ~1;
		break;
	case 'd':
	case 'D':
		engine->keys &= ~2;
		break;
	case 'z':
	case 'Z':
		engine->keys &= ~4;
		break;
	case 's':
	case 'S':
		engine->keys &= ~8;
		break;
	case 'E':
	case 'e':
		engine->keys &= ~16;
		break;
	case 'a':
	case 'A':
		engine->keys &= ~32;
		break;
	}

	engine->shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
}

static void display()
{
	static bool binded = false;
	struct timeval tv = {0, 1000000 / engine->g_state.framerate};

	if (!(engine->state.loading)) {
		if (!binded) {
			engine->world->bind_all_buffers();
			engine->next_world->bind_all_buffers();
			binded = true;
		}
		while (engine->display_frame())
			;
	} else {
		if (!engine->state.exiting)
			engine->draw_loading_screen(std::string("Loading"), true);
		else
			engine->draw_loading_screen(std::string("Exiting"), true);
	}

	if (engine->g_state.target_fps > 0)
		select(0, NULL, NULL, NULL, &tv);
}

static void motion(int x, int y)
{
	if (!(engine->state.anchor_cam) && !(engine->state.exiting))
	{
		if (!(engine->state.warp))
		{
			engine->world->angle.x -= (x - engine->g_state.ww / 2) * engine->mousespeed;
			engine->world->angle.y -= (y - engine->g_state.wh / 2) * engine->mousespeed;
			if (engine->world->angle.x < -M_PI)
				engine->world->angle.x += M_PI * 2;
			if (engine->world->angle.x > M_PI)
				engine->world->angle.x -= M_PI * 2;
			if (engine->world->angle.y < -M_PI * 0.49)
				engine->world->angle.y = -M_PI * 0.49;
			if (engine->world->angle.y > M_PI * 0.49)
			{
				engine->world->angle.y = M_PI * 0.49;
			}

			engine->world->update_vectors();

			engine->state.warp = true;
			glutWarpPointer(engine->g_state.ww / 2, engine->g_state.wh / 2);
		}
		else
		{
			engine->state.warp = false;
		}
	}
}

static void mouse(int button, int state, int x, int y)
{

	(void)x;
	(void)y;
	if (state != GLUT_DOWN || engine->state.exiting)
		return;

	// Scrollwheel
	if (button == 3 || button == 4)
	{
		if (button == 3)
			engine->world->buildtype--;
		else
			engine->world->buildtype++;

		engine->world->buildtype &= 0xf;
		fprintf(stderr, "Building blocks of type %u (%s)\n", engine->world->buildtype, engine->state.blocknames[engine->world->buildtype]);
		return;
	}

	if (engine->state.verbose)
		fprintf(stderr, "Clicked on %d, %d, %d, face %d, button %d\n", engine->world->mx, engine->world->my, engine->world->mz, engine->world->face, button);

	Chunk *tmp = engine->world->getChunkPtr(engine->world->mx, engine->world->my, engine->world->mz);

	if (tmp != NULL)
	{
		if (engine->state.verbose)
			tmp->print_chunk();
		engine->sound->play2D(AUDIO_BLOCK_PATH, false);
	}

	if (button == 0)
	{
		if (engine->world->face == 0)
			engine->world->mx++;
		if (engine->world->face == 3)
			engine->world->mx--;
		if (engine->world->face == 1)
			engine->world->my++;
		if (engine->world->face == 4)
			engine->world->my--;
		if (engine->world->face == 2)
			engine->world->mz++;
		if (engine->world->face == 5)
			engine->world->mz--;
		
		if (engine->computing)
		{
			engine->edit_to_apply[engine->edit_size] = player_edit(engine->world->mx, engine->world->my, engine->world->mz, engine->world->buildtype);
			engine->edit_size++;
		}
		engine->world->setChunk(engine->world->mx, engine->world->my, engine->world->mz, engine->world->buildtype);
	}
	else
	{
		if (tmp && tmp->ay == -4) {
			uint8_t c_blk = engine->world->getChunk(engine->world->mx, engine->world->my, engine->world->mz);
			if (c_blk != 15)
				engine->world->setChunk(engine->world->mx, engine->world->my, engine->world->mz, 0);
		} else
			engine->world->setChunk(engine->world->mx, engine->world->my, engine->world->mz, 0);
	}
}


static void render_loop(void)
{
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(320, 240);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialup);
	glutIdleFunc(idle);
	glutPassiveMotionFunc(motion);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutMainLoop();
}

int manage_params_entry(int ac, char **av)
{
	int i = 1;

	bool customF = false;
	int seenV = 0;
	int customV = 0;
	int seenH = 0;
	int customH = 0;
	int customW = 0;
	int seenW = 0;
	int	seenWz = 0;
	int	customWz = 0;
	int customS = 0;
	int seenS = 0;
	int seenSync = 0;
	int seenFs = 0;
	int	seenD = 0;

	if (ac == 1)
		return 0;
	while (i < ac + 1)
	{
		if (av[i] == NULL)
			break;
		if (!strcmp(av[i], "-w") || !strcmp(av[i], "--width"))
		{
			if (!seenW && av[i + 1] && atoi(av[i + 1]))
			{
				customW = atoi(av[i + 1]);
				if (customW < WIDTH_MIN || customW > WIDTH_MAX)
					return 1;
				seenW++;
				i++;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "-h") || !strcmp(av[i], "--height"))
		{
			if (!seenH && av[i + 1] && atoi(av[i + 1]))
			{
				customH = atoi(av[i + 1]);
				if (customH < HEIGHT_MIN || customH > HEIGHT_MAX)
					return 1;
				seenH++;
				i++;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "-s") || !strcmp(av[i], "--seed"))
		{
			if (!seenS && av[i+1] && atoi(av[i+1])) {
				customS = atoi(av[i+1]);
				i++;
				seenS++;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "-ws") || !strcmp(av[i], "--world-size"))
		{
			if (!seenWz && av[i + 1] && atoi(av[i + 1]))
			{
				customWz = atoi(av[i + 1]);
				if (customWz < WORLD_MIN || customWz > WORLD_MAX || customWz % 8 != 0)
					return 1;
				seenWz++;
				i++;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "--sync")) {
			if (!seenSync) {
				seenSync++;
				engine->g_state.vsync = false;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "-f") || !strcmp(av[i], "--fullscreen"))
		{
			if (!seenFs) {
				seenFs++;
				customF = true;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "-d") || !strcmp(av[i], "--debug"))
		{
			if (!seenD) {
				seenD++;
				engine->state.verbose = true;
			}
			else
				return 1;
		}
		else if (!strcmp(av[i], "-v") || !strcmp(av[i], "--version"))
		{
			if (!seenV && av[i + 1] && atoi(av[i + 1]))
			{
				customV = atoi(av[i + 1]);

				if (customV >= 100 && customV <= 999)
				{
					sprintf(engine->state.shader_version, "#version %d\n", customV);
					seenV++;
					i++;
				}
				else
					return 1;
				i++;
			}
			else
				return 1;
		}
		else
		{
			fprintf(stderr, "Parameter %s unknow.\n", av[i]);
			return 1;
		}
		i++;
	}
	if (customW != 0)
		engine->g_state.ww = customW;
	if (customH != 0)
		engine->g_state.wh = customH;
	if (customWz != 0) {
		engine->scx = customWz;
		engine->scz = customWz;
		engine->scxf = static_cast<float>(customWz);
		engine->sczf = static_cast<float>(customWz);
	}
	if (customS != 0) {
		engine->noiser = new Noiser();
		engine->noiser->set_seed(customS);
		engine->noiser->create_seed();
	}
	engine->g_state.fullscreen = customF;
	return 0;
}

/*
** main
*/

int main(int argc, char *argv[])
{
	engine = new Engine();
	if (manage_params_entry(argc, argv))
	{
		engine->write_usage();
		return 1;
	}
	if (engine->init_glew())
		return 1;
	if (engine->init_resources())
	{
		engine->sound->setSoundVolume(engine->state.volume);
		engine->sound->play2D(AUDIO_AMBIENCE_PATH, true);
		engine->write_keys();
		render_loop();
	}
	else
	{
		fprintf(stderr, "OpenGl KO\n");
		engine->write_usage();
	}
	return 0;
}
