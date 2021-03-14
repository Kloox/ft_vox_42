#include "vox.hpp"
#include "Cpu.hpp"

Engine::Engine(void)
{
	this->world = NULL;
	this->next_world = NULL;
	this->noiser = NULL;

	// GL states
	this->ogl_state.program = 0;
	this->ogl_state.attribute_coord = 0;
	this->ogl_state.attribute_sky_coord = 0;
	this->ogl_state.t_vbo = 0;
	this->ogl_state.t_vao = 0;
	this->ogl_state.sky_vao = 0;
	this->ogl_state.sky_vbo = 0;
	this->ogl_state.uniform_mvp = 0;
	this->ogl_state.cursor_vbo = 0;
	this->ogl_state.uniform_texture = 0;
	this->ogl_state.attribute_sky_coord = 0;
	this->ogl_state.texture = 0;
	this->ogl_state.skybox_cubemap = 0;

	// Libs class
	this->sound = irrklang::createIrrKlangDevice();

	// Engine states
	this->state.volume = 0.4f;
	this->state.i_display = 0;
	this->state.anchor_cam = false;
	this->state.loading = true;
	this->state.threads = std::thread::hardware_concurrency();
	this->state.precompute_timer = 0;
	this->state.timebase = 0;
	this->state.doSwap = false;
	this->state.debug_show = 0;
	this->state.worker = false;
	this->state.verbose = false;
	this->state.focus_on_transparent = false;
	this->state.speedmult = 0;
	this->state.collision = true;
	this->state.warp = false;
	this->state.exiting = false;

	//world size
	this->scx = 64;
	this->scy = 8;
	this->scz = 64;
	this->scxf = 64.0f;
	this->scyf = 8.0f;
	this->sczf = 64.0f;

	// debug
	this->elements_world_one = 0;
	this->elements_world_two = 0;

	// Hook state
	this->shift = false;
	this->keys = 0;
	this->autowalk = false;
	this->infinite = true;
	this->mousespeed = 0.001;
	this->focus = 9999;

	// Precompute state
	this->computing = false;
	this->chunk_index = new int[3];
	this->new_center = new int[3];
	this->barrier = 0;
	this->edit_size = 0;

	// Engine graphics states
	this->g_state.window = 0;
	this->g_state.ww = RES_X;
	this->g_state.wh = RES_Y;
	this->g_state.aa = false;
	this->g_state.vsync = true;
	this->g_state.transparency = true;
	this->g_state.fov = glm::radians(80.0f);
	this->g_state.frame = 0;
	this->g_state.fullscreen = false;
	this->g_state.framerate = 10000;
	this->g_state.maxi = 2;
	this->g_state.renderDistance = 15;
	this->g_state.target_fps = 0;
	this->g_state.fpsToShow = 0;

	int k = 0;
	glutInit(&k, NULL);
}

Engine::~Engine(void)
{

	delete[] this->chunk_index;
	delete[] this->new_center;
	delete this->noiser;
	glDeleteTextures(1, &(this->ogl_state.texture));
	if (this->sound)
		this->sound->drop();
	glutDestroyWindow(this->g_state.window);
	glDeleteProgram(this->ogl_state.program);
}

int Engine::init_glew(void)
{
	if (this->g_state.vsync)
		putenv( (char *) "__GL_SYNC_TO_VBLANK=1" );
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(this->g_state.ww, this->g_state.wh);
	this->g_state.window = glutCreateWindow("ft_vox - jjaouen & drimo");
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();

	if (this->g_state.fullscreen)
		glutFullScreen();
	if (GLEW_OK != glew_status)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if (!GLEW_VERSION_4_6)
	{
		fprintf(stderr, "Warning: No support for OpenGL 4.6 found.");
		if (!GLEW_VERSION_4_0)
		{
			fprintf(stderr, "Warning: No support for OpenGL 4.0 found\n");
			if (!GLEW_VERSION_3_3)
			{
				fprintf(stderr, "Warning: No support for OpenGL 3.3 found\n");
				if (!GLEW_VERSION_3_0)
				{
					fprintf(stderr, "Warning: No support for OpenGL 3.0 found\n");
					if (!GLEW_VERSION_2_0)
					{
						fprintf(stderr, "Fatal Error: No support for OpenGL 2.0 found\n");
						return 1;
					}
				}
			}
		}
	}
	if (this->state.verbose)
	{
		int major;
		int minor;
		CPUInfo cpu;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		fprintf(stderr, "CPU: %s\nCPU Specs: %d Core(s) %d Thread(s)\n", cpu.model().c_str(), cpu.cores(), cpu.logicalCpus());
		fprintf(stderr, "Renderer: %s\nVendor : %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		fprintf(stderr, "OpenGL Version: %s\nShaders Version: %s\nGLEW Version: %d.%d\n", glGetString(GL_VERSION), this->state.shader_version, GLEW_VERSION_MAJOR, GLEW_VERSION_MAJOR);
	}
	return 0;
}

void Engine::load_world_texture(void)
{
	glGenTextures(1, &(this->ogl_state.texture));
	glBindTexture(GL_TEXTURE_2D, this->ogl_state.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures.width, textures.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures.pixel_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned int Engine::load_skybox(void)
{

	float skybox_vertices[] = {
		// positions
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f};

	// Create VAO/VBO for skybox

	glGenVertexArrays(1, &(this->ogl_state.sky_vao));
	glGenBuffers(1, &(this->ogl_state.sky_vbo));
	glBindVertexArray(this->ogl_state.sky_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl_state.sky_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUniform1i(glGetUniformLocation(this->ogl_state.program, "skybox"), 2);
	// create Skybox texture

	glGenTextures(1, &(this->ogl_state.skybox_cubemap));
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ogl_state.skybox_cubemap);

	// RIGHT
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0,
				 0, GL_RGB, skyboxRight.width, skyboxRight.height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxRight.pixel_data);
	// LEFT
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1,
				 0, GL_RGB, skyboxLeft.width, skyboxLeft.height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxLeft.pixel_data);
	// TOP
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2,
				 0, GL_RGB, skyboxTop.width, skyboxTop.height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxTop.pixel_data);
	// BOTTOM
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3,
				 0, GL_RGB, skyboxBottom.width, skyboxBottom.height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxBottom.pixel_data);
	// FRONT
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4,
				 0, GL_RGB, skyboxFront.width, skyboxFront.height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxFront.pixel_data);
	// BACK
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5,
				 0, GL_RGB, skyboxBack.width, skyboxBack.height, 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxBack.pixel_data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return this->ogl_state.skybox_cubemap;
}

void Engine::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	glm::mat4 mvp = glm::ortho(0.0f, static_cast<float>(this->g_state.ww), 0.0f, static_cast<float>(this->g_state.wh));
	glUniformMatrix4fv(this->ogl_state.uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	// activate corresponding render state
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform1i(glGetUniformLocation(this->ogl_state.program, "text_render"), 1);

	glUniform3f(glGetUniformLocation(this->ogl_state.program, "textColor"), color.x, color.y, color.z);
	glBindVertexArray(this->ogl_state.t_vao);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos, ypos, 0.0f, 1.0f},
			{xpos + w, ypos, 1.0f, 1.0f},

			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f}};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, this->ogl_state.t_vao);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glUniform1i(glGetUniformLocation(this->ogl_state.program, "text_render"), 0);
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, this->ogl_state.texture);
}

void Engine::draw_loading_screen(std::string str, bool clear)
{
	static int c;
	static int t = glutGet(GLUT_ELAPSED_TIME);
	int x, y;
	x = this->g_state.ww / 2 - 48;
	y = this->g_state.wh / 2;
	int ct = glutGet(GLUT_ELAPSED_TIME);
	if (ct - t > 200)
	{
		c++;
		if (c > 2)
			c = 0;
		t = ct;
	}
	if (clear)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (c == 0)
		RenderText(str + ".", x, y, 1.0, glm::vec3(0.0f, 0.0f, 0.0f));
	else if (c == 1)
		RenderText(str + "..", x, y, 1.0, glm::vec3(0.0f, 0.0f, 0.0f));
	else if (c == 2)
	{
		RenderText(str + "...", x, y, 1.0, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	if (clear)
		glutSwapBuffers();
}

int Engine::get_framerate(void)
{
	this->g_state.frame++;
	int time;
	time = glutGet(GLUT_ELAPSED_TIME);

	int fps = 0;
	if (time - this->state.timebase > 1000)
	{
		fps = this->g_state.frame * 1000.0 / (time - this->state.timebase);
		this->state.timebase = time;
		this->g_state.frame = 0;
	}
	return fps;
}

void Engine::apply_collision(glm::vec3 lastP)
{
	int x = floorf(this->world->position.x);
	int y = floorf(this->world->position.y);
	int z = floorf(this->world->position.z);

	uint8_t block = this->world->getChunk(x, y, z);

	if (block != 0 && block != 8)
	{

		int mx = floorf(lastP.x);
		int my = floorf(lastP.y);
		int mz = floorf(lastP.z);

		if (y - my == 1 || y - my == -1)
		{
			this->world->position.y = lastP.y;
			this->apply_collision(lastP);
			return;
		}
		if (x - mx == 1 || x - mx == -1)
		{
			this->world->position.x = lastP.x;
			this->apply_collision(lastP);
			return;
		}
		if (z - mz == 1 || z - mz == -1)
		{
			this->world->position.z = lastP.z;
			this->apply_collision(lastP);
			return;
		}
	}
}

void Engine::draw_hud(glm::mat4 &mvp, bool draw)
{
	/* Very naive ray casting algorithm to find out which block we are looking at */

	glm::vec3 testpos = this->world->position;
	glm::vec3 prevpos = this->world->position;

	for (int i = 0; i < RAYCAST_LENGTH; i++)
	{
		/* Advance from our currect position to the direction we are looking at, in small steps */

		prevpos = testpos;
		testpos += this->world->lookat * 0.1f;

		this->world->mx = floorf(testpos.x);
		this->world->my = floorf(testpos.y);
		this->world->mz = floorf(testpos.z);

		uint8_t block = this->world->getChunk(this->world->mx, this->world->my, this->world->mz);

		if (!(this->state.focus_on_transparent) && (block == 8 || block == 9))
			continue;

		/* If we find a block that is not air or unrange, we are done */

		if (this->world->mx > (this->scx * CX) / 2 ||
			this->world->mx < ((this->scx * CX) / 2) * -1 ||
			this->world->mz > (this->scz * CZ) / 2 ||
			this->world->mz < ((this->scz * CZ) / 2) * -1 ||
			this->world->my > (this->scy * CY) / 2 ||
			this->world->my < ((this->scy * CY) / 2) * -1)
			continue;

		if (block)
			break;
	}

	/* Find out which face of the block we are looking at */

	int px = floorf(prevpos.x);
	int py = floorf(prevpos.y);
	int pz = floorf(prevpos.z);

	if (px > this->world->mx)
		this->world->face = 0;
	else if (px < this->world->mx)
		this->world->face = 3;
	else if (py > this->world->my)
		this->world->face = 1;
	else if (py < this->world->my)
		this->world->face = 4;
	else if (pz > this->world->mz)
		this->world->face = 2;
	else if (pz < this->world->mz)
		this->world->face = 5;

	/* If we are looking at air, move the cursor out of sight */

	if (!this->world->getChunk(this->world->mx, this->world->my, this->world->mz))
		this->world->mx = this->world->my = this->world->mz = 99999;

	float bx = this->world->mx;
	float by = this->world->my;
	float bz = this->world->mz;

	float distance = glm::length(testpos - this->world->position);
	if (distance > 100)
		distance = 100;

	/* Change the eye's focus distance smoothly */

	if (this->focus / 1.01 > distance)
		this->focus /= 1.01;
	else if (this->focus * 1.01 < distance)
		this->focus *= 1.01;

	/* Render a box around the block we are pointing at */

	float box[24][4] = {
		{bx + 0, by + 0, bz + 0, 14},
		{bx + 1, by + 0, bz + 0, 14},
		{bx + 0, by + 1, bz + 0, 14},
		{bx + 1, by + 1, bz + 0, 14},
		{bx + 0, by + 0, bz + 1, 14},
		{bx + 1, by + 0, bz + 1, 14},
		{bx + 0, by + 1, bz + 1, 14},
		{bx + 1, by + 1, bz + 1, 14},

		{bx + 0, by + 0, bz + 0, 14},
		{bx + 0, by + 1, bz + 0, 14},
		{bx + 1, by + 0, bz + 0, 14},
		{bx + 1, by + 1, bz + 0, 14},
		{bx + 0, by + 0, bz + 1, 14},
		{bx + 0, by + 1, bz + 1, 14},
		{bx + 1, by + 0, bz + 1, 14},
		{bx + 1, by + 1, bz + 1, 14},

		{bx + 0, by + 0, bz + 0, 14},
		{bx + 0, by + 0, bz + 1, 14},
		{bx + 1, by + 0, bz + 0, 14},
		{bx + 1, by + 0, bz + 1, 14},
		{bx + 0, by + 1, bz + 0, 14},
		{bx + 0, by + 1, bz + 1, 14},
		{bx + 1, by + 1, bz + 0, 14},
		{bx + 1, by + 1, bz + 1, 14},
	};

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	glUniformMatrix4fv(this->ogl_state.uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl_state.cursor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->ogl_state.attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 36);

	/* Draw a cross in the center of the screen */

	float cross[4][4] = {
		{-0.05, 0, 0, 13},
		{+0.05, 0, 0, 13},
		{0, -0.05, 0, 13},
		{0, +0.05, 0, 13},
	};

	glDisable(GL_DEPTH_TEST);
	glm::mat4 one(1);
	glUniformMatrix4fv(this->ogl_state.uniform_mvp, 1, GL_FALSE, glm::value_ptr(one));
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl_state.cursor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof cross, cross, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->ogl_state.attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 36);

	if (this->state.debug_show)
		this->draw_debug(draw);
}

debug_value Engine::update_debug_values(bool draw, int init)
{
	static debug_value vles;

	// Const str
	if (!init)
	{
		vles.color = glm::vec3(1.0f, 1.0f, 1.0f);
		vles.wp = std::string("World Position:");
		vles.wcp = std::string("World Chunk Position");
		vles.lp = std::string("Local Position");
		vles.lcp = std::string("Local Chunk Position:");
		vles.vs = this->g_state.vsync ? vles.vs = std::string("Vsync: true") : vles.vs = std::string("Vsync: false");
	}

	if (!draw && init)
		return vles;

	// Fps counter
	this->set_debug_world_size();
	vles.fps = std::string("FPS: " + std::to_string(this->g_state.fpsToShow) + " | Memory usage: " + std::to_string(get_mem_usage()) + "M");

	if (this->state.debug_show > 1) // Display info
	{
		vles.rd = std::string("Render Distance: " + std::to_string(this->g_state.renderDistance));
		vles.ft = this->g_state.target_fps ? std::string("Framerate target: " + std::to_string(this->g_state.target_fps) + " (" + std::to_string(this->g_state.framerate) + ")") : std::string("Framerate target: No limit");
		vles.sampling = std::string("Sampling: " + std::to_string(this->g_state.maxi));
		vles.aa = this->g_state.aa ? std::string("Antialiasing: true") : std::string("Antialiasing: false");
	}

	if (this->state.debug_show > 2) // Precomputing info
	{
		vles.infinite = this->infinite ? std::string("Infinite world (" + std::to_string(this->scx) + "x" + std::to_string(this->scy) + "x" + std::to_string(this->scz) + "): true") : std::string("Infinite world (" + std::to_string(this->scx) + "x" + std::to_string(this->scy) + "x" + std::to_string(this->scz) + "): false");
		vles.barrierl = std::string("Barrier limit: " + std::to_string(this->barrier) + " chunks");
		vles.lpt = std::string("Last precompute timer (" + std::to_string(this->state.threads) + " threads): " + std::to_string(this->state.precompute_timer) + "ms");
		vles.cxn = this->computing ? std::string("Computing next world: true") : std::string("Computing next world: false");
	}

	if (this->state.debug_show > 3) // Positions & Cam info
	{

		vles.wpp = std::string("X: " + std::to_string(this->world->position.x + ((this->world->shiftX) * 16.0f)) + " Y: " + std::to_string(this->world->position.y) + " Z: " + std::to_string(this->world->position.z + ((this->world->shiftZ) * 16.0f)));
		glm::vec3 pos_chunk = this->get_chunk_position(this->world->position);
		vles.wcpp = std::string("X: " + std::to_string(int(pos_chunk.x) + this->world->shiftX) + " Y: " + std::to_string(int(pos_chunk.y)) + " Z: " + std::to_string(int(pos_chunk.z) + this->world->shiftZ));
		vles.lpp = std::string("X: " + std::to_string(this->world->position.x) + " Y: " + std::to_string(this->world->position.y) + " Z: " + std::to_string(this->world->position.z));
		vles.lcpp = std::string("X: " + std::to_string(int(pos_chunk.x)) + " Y: " + std::to_string(int(pos_chunk.y)) + " Z: " + std::to_string(int(pos_chunk.z))),
		vles.forward = std::string("Forward: X: " + std::to_string(this->world->forward.x) + " Y: " + std::to_string(this->world->forward.y) + " Z: " + std::to_string(this->world->forward.z));
		vles.right = std::string("Right: X: " + std::to_string(this->world->right.x) + " Y: " + std::to_string(this->world->right.y) + " Z: " + std::to_string(this->world->right.z));
		vles.up = std::string("Up: X: " + std::to_string(this->world->up.x) + " Y: " + std::to_string(this->world->up.y) + " Z: " + std::to_string(this->world->up.z));
		vles.lookat = std::string("LookAt: X: " + std::to_string(this->world->lookat.x) + " Y: " + std::to_string(this->world->lookat.y) + " Z: " + std::to_string(this->world->lookat.z));
		vles.angle = std::string("Angle: X: " + std::to_string(this->world->angle.x) + " Y: " + std::to_string(this->world->angle.y) + " Z: " + std::to_string(this->world->angle.z));
		if (this->world->angle.x < 0.75 && this->world->angle.x > -0.75)
			vles.facing = std::string("Facing: North");
		else if (this->world->angle.x > 0.75 && this->world->angle.x < 2.25)
			vles.facing = std::string("Facing: West");
		else if (this->world->angle.x < -0.75 && this->world->angle.x > -2.25)
			vles.facing = std::string("Facing: East");
		else
			vles.facing = std::string("Facing: South");
		vles.velocity = std::string("Max velocity: " + std::to_string(int((this->state.speedmult + PLAYER_SPEED) * PLAYER_SPRINT_SPEED_MULT)) + " ((" + std::to_string(PLAYER_SPEED) + " + " + std::to_string(this->state.speedmult)) + ") * " + std::to_string(int(PLAYER_SPRINT_SPEED_MULT)) + ")";
	}

	return vles;
}

void Engine::draw_debug(bool draw)
{
	debug_value vles;
	static int init = 0;

	vles = this->update_debug_values(draw, init);

	if (!init)
		init++;

	// Fps counter
	this->RenderText(vles.fps, 5, this->g_state.wh - 24, 1.0, vles.color);

	if (this->state.debug_show == 1)
		return;

	if (this->state.debug_show > 1) // Display info
	{
		// Vsync
		this->RenderText(vles.vs, 5, 5, 1.0, vles.color);
		// Anti aliasing
		this->RenderText(vles.aa, 5, 24, 1.0, vles.color);
		// Sampling
		this->RenderText(vles.sampling, 5, 48, 1.0, vles.color);
		// Fps Target
		this->RenderText(vles.ft, 5, 72, 1.0, vles.color);
		// Render Distance
		this->RenderText(vles.rd, 5, 96, 1.0, vles.color);
	}

	if (this->state.debug_show > 2) // Precomputing info
	{
		// Compute next world bool
		this->RenderText(vles.cxn, 5, 144, 1.0, vles.color);
		// Last precompute timer
		this->RenderText(vles.lpt, 5, 168, 1.0, vles.color);
		// Barrier info
		this->RenderText(vles.barrierl, 5, 192, 1.0, vles.color);
		// infinite bool
		this->RenderText(vles.infinite, 5, 216, 1.0, vles.color);
	}

	if (this->state.debug_show > 3) // Positions & cam info
	{
		// World Position str
		this->RenderText(vles.wp, 5, this->g_state.wh - 48, 1.0, vles.color);
		// World Positions
		this->RenderText(vles.wpp, 5, this->g_state.wh - 72, 1.0, vles.color);
		// World Chunk pos str
		this->RenderText(vles.wcp, 5, this->g_state.wh - 96, 1.0, vles.color);
		// World Chunk positions
		this->RenderText(vles.wcpp, 5, this->g_state.wh - 120, 1.0, vles.color);
		// Local pos str
		this->RenderText(vles.lp, 5, this->g_state.wh - 144, 1.0, vles.color);
		// Local Positions
		this->RenderText(vles.lpp, 5, this->g_state.wh - 168, 1.0, vles.color);
		// Local Chunk pos str
		this->RenderText(vles.lcp, 5, this->g_state.wh - 192, 1.0, vles.color);
		// Local Chunk Positions
		this->RenderText(vles.lcpp, 5, this->g_state.wh - 214, 1.0, vles.color);
		// Forward
		this->RenderText(vles.forward, 5, this->g_state.wh - 238, 1.0, vles.color);
		// Right
		this->RenderText(vles.right, 5, this->g_state.wh - 262, 1.0, vles.color);
		// Up
		this->RenderText(vles.up, 5, this->g_state.wh - 286, 1.0, vles.color);
		// LookAt
		this->RenderText(vles.lookat, 5, this->g_state.wh - 310, 1.0, vles.color);
		// Angle
		this->RenderText(vles.angle, 5, this->g_state.wh - 334, 1.0, vles.color);
		// Facing
		this->RenderText(vles.facing, 5, this->g_state.wh - 358, 1.0, vles.color);
		// Max velocity
		this->RenderText(vles.velocity, 5, this->g_state.wh - 382, 1.0, vles.color);
	}
}

void Engine::draw_scene(glm::mat4 &mvp, glm::mat4 &view, glm::mat4 &projection, bool draw)
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_CULL_FACE);

	this->render_skybox(view, projection);

	this->world->render(mvp, this->g_state.renderDistance, this->ogl_state.uniform_mvp);

	/* Then draw HUD */

	this->draw_hud(mvp, draw);

	if (this->state.exiting)
		this->draw_loading_screen(std::string("Exiting"), false);
}

bool Engine::display_frame()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Calculate the translation matrix used for anti-aliasing */

	glm::mat4 aamat(1.0f);

	if (this->g_state.aa)
		aamat = glm::translate(aamat, glm::vec3((float)(this->state.i_display % 4) / (4 * this->g_state.ww), (float)(this->state.i_display / 4) / (4 * this->g_state.wh), 0.0f));

	/* If transparency is enabled, we use a different alpha cutoff for every frame */

	float alpha = 0.5;

	if (this->g_state.transparency)
		alpha = this->state.cutoff[this->state.i_display];

	glUniform1f(this->ogl_state.attribute_sky_coord, alpha);

	/* Calculate the MVP matrix */

	glm::mat4 view = glm::lookAt(this->world->position, this->world->position + this->world->lookat * this->focus, this->world->up);
	glm::mat4 projection = glm::perspective(this->g_state.fov, 1.0f * this->g_state.ww / this->g_state.wh, 0.01f, 1000.0f);

	glm::mat4 mvp = aamat * projection * view;

	glUniformMatrix4fv(this->ogl_state.uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	this->draw_scene(mvp, view, projection, (this->state.i_display + 1) >= this->g_state.maxi ? true : false);

	glAccum(this->state.i_display ? GL_ACCUM : GL_LOAD, 1.0 / this->g_state.maxi);

	/* And we are done */

	this->state.i_display++;
	if (this->state.i_display >= this->g_state.maxi)
	{
		int f = this->get_framerate();
		if (f)
		{
			this->g_state.fpsToShow = f;
		}

		glAccum(GL_RETURN, 1);
		glutSwapBuffers();
		this->state.i_display = 0;
	}
	return this->state.i_display;
}

void Engine::render_skybox(glm::mat4 &view, glm::mat4 &projection)
{
	glUniform1i(glGetUniformLocation(this->ogl_state.program, "text_render"), 2);
	glDepthMask(GL_FALSE);

	glm::mat4 skybox_mvp = projection * glm::mat4(glm::mat3(view));
	glUniformMatrix4fv(glGetUniformLocation(this->ogl_state.program, "sky_mvp"), 1, GL_FALSE, glm::value_ptr(skybox_mvp));

	// skybox cube
	glBindVertexArray(this->ogl_state.sky_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl_state.sky_vbo);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ogl_state.skybox_cubemap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform1i(glGetUniformLocation(this->ogl_state.program, "text_render"), 0);

	// RESET GL states
	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->ogl_state.texture);
}

void Engine::move(float movespeed)
{
	static int pt = 0;

	if (this->shift)
		movespeed *= PLAYER_SPRINT_SPEED_MULT;

	int t = glutGet(GLUT_ELAPSED_TIME);
	float dt = (t - pt) * 1.0e-3;
	pt = t;

	glm::vec3 lastP = this->world->position;

	if (this->keys & 1)
		this->world->position -= this->world->right * movespeed * dt;
	if (this->keys & 2)
		this->world->position += this->world->right * movespeed * dt;
	if (this->keys & 4)
	{
		this->world->position += (this->world->forward) * movespeed * dt;
		this->world->position.y += (this->world->angle.y) * movespeed * dt;
	}
	if (this->keys & 8)
	{
		this->world->position -= (this->world->forward) * movespeed * dt;
		this->world->position.y -= (this->world->angle.y) * movespeed * dt;
	}
	if (this->keys & 16)
	{
		this->world->position.y += movespeed * dt;
	}
	if (this->keys & 32)
	{
		this->world->position.y -= movespeed * dt;
	}

	if (this->state.collision && lastP != this->world->position)
		this->apply_collision(lastP);
}

void Engine::init_freetype_chars(void)
{
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->g_state.ww), 0.0f, static_cast<GLfloat>(this->g_state.wh));
	glUseProgram(this->ogl_state.program);
	glUniformMatrix4fv(glGetUniformLocation(this->ogl_state.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	if (FT_Init_FreeType(&freetypeLib))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	if (FT_New_Face(freetypeLib, FONT_PATH, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 24);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, face->glyph->bitmap.width,
					 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character character = {
			tex,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(freetypeLib);
	glGenVertexArrays(1, &(this->ogl_state.t_vao));
	glGenBuffers(1, &(this->ogl_state.t_vbo));
	glBindVertexArray(this->ogl_state.t_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl_state.t_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Engine::noise_and_update_all(World *world, bool doBind)
{
	int start = glutGet(GLUT_ELAPSED_TIME);
	if (this->state.threads == 0)
		world->noise_all();
	else
		world->noise_all_multithread(this->state.threads);
	int noise_ok = glutGet(GLUT_ELAPSED_TIME);
	int create_ok = glutGet(GLUT_ELAPSED_TIME);
	world->update_all_multithread(false, this->state.threads);
	int start_bind = glutGet(GLUT_ELAPSED_TIME);
	if (doBind)
		world->bind_all_buffers();
	int end_bind = glutGet(GLUT_ELAPSED_TIME);
	int final = glutGet(GLUT_ELAPSED_TIME);
	if (this->state.verbose)
		fprintf(stderr, "Loading time (%dx%dx%d - %d threads - Infinite world: %s) : %dms\nNoise: %dms\nCreate buffers: %dms\nUpdate: %dms\nBind: %dms\n", this->scx, this->scy, this->scz, this->state.threads, this->infinite ? "True" : "False", final - start, noise_ok - start, create_ok - noise_ok, final - noise_ok, end_bind - start_bind);
	else
		fprintf(stderr, "Loading time (%dx%dx%d): %dms\n", this->scx, this->scy, this->scz, final - start);

	if (this->barrier == 0)
		this->set_limit_by_time(final - start);
}

void Engine::load_ressources(void)
{
	this->world->update_vectors();
	this->noise_and_update_all(this->world, false);

	if (this->state.exiting)
	{
		this->state.loading = false;
		return;
	}

	this->next_world->update_vectors();
	this->noise_and_update_all(this->next_world, false);
	this->next_world->copy_world(this->world);
	this->state.loading = false;
}

int Engine::init_resources(void)
{
	this->ogl_state.program = create_program("shaders/ft_vox.v.glsl", "shaders/ft_vox.f.glsl");
	if (this->ogl_state.program == 0)
		return 0;

	this->init_freetype_chars();

	this->ogl_state.attribute_coord = get_attrib(this->ogl_state.program, "coord");
	this->ogl_state.uniform_mvp = get_uniform(this->ogl_state.program, "mvp");
	this->ogl_state.attribute_sky_coord = get_uniform(this->ogl_state.program, "alpha");
	this->ogl_state.uniform_texture = get_uniform(this->ogl_state.program, "texture_2d");

	if (this->ogl_state.attribute_coord == -1 || this->ogl_state.attribute_sky_coord == -1 || this->ogl_state.uniform_mvp == -1 || this->ogl_state.attribute_sky_coord == -1 || this->ogl_state.uniform_texture == -1)
		return 0;

	/* Upload the texture */

	this->load_world_texture();
	this->load_skybox();

	/* Create worlds */
	if (this->noiser == NULL)
	{
		this->noiser = new Noiser;
		this->noiser->set_seed(SEED);
		this->noiser->create_seed();
	}

	this->world = new World;
	this->world->create_all_buffers();

	this->next_world = new World;
	this->next_world->create_all_buffers();

	std::thread loader(&Engine::load_ressources, this);
	loader.detach();

	glGenBuffers(1, &(this->ogl_state.cursor_vbo));
	glUseProgram(this->ogl_state.program);
	glEnableVertexAttribArray(this->ogl_state.attribute_coord);
	glPolygonOffset(1, 1);
	glClearColor(0.6, 0.8, 1.0, 0.0);

	return 1;
}

glm::vec3 Engine::get_chunk_position(glm::vec3 position)
{
	return glm::vec3(
		(((this->scxf * CXF) / 2.0f + position.x) / CX),
		(((this->scyf * CYF) / 2.0f + position.y) / CY),
		(((this->sczf * CZF) / 2.0f + position.z) / CZ));
}

void Engine::set_chunk_index_by_pos(glm::vec3 pos)
{
	glm::vec3 index = this->get_chunk_position(pos);
	this->chunk_index[0] = static_cast<int>(index.x);
	this->chunk_index[1] = static_cast<int>(index.y);
	this->chunk_index[2] = static_cast<int>(index.z);
}

void Engine::print_false_enable(void)
{
	for (int x = 0; x < this->scx; x++)
		for (int y = 0; y < this->scy; y++)
			for (int z = 0; z < this->scz; z++)
			{
				if (this->world->c[x][y][z]->enable == false)
					fprintf(stderr, "Chunk (%d %d %d) enable == false\n", x, y, z);
			}
	for (int x = 0; x < this->scx; x++)
		for (int y = 0; y < this->scy; y++)
			for (int z = 0; z < this->scz; z++)
			{
				if (this->next_world->c[x][y][z]->enable == false)
					fprintf(stderr, "Next Chunk (%d %d %d) enable == false\n", x, y, z);
			}
}

void Engine::set_debug_world_size(void)
{
	int ctr_one, ctr_two;
	ctr_one = 0;
	ctr_two = 0;
	for (int x = 0; x < this->scx; x++)
		for (int y = 0; y < this->scy; y++)
			for (int z = 0; z < this->scz; z++)
			{
				ctr_one += this->world->c[x][y][z]->elements;
				ctr_two += this->world->c[x][y][z]->elements;
			}
	this->elements_world_one = ctr_one;
	this->elements_world_two = ctr_two;
}

bool Engine::must_recompute_world(void)
{
	this->set_chunk_index_by_pos(this->world->position);
	if (this->chunk_index[0] >= this->scx - this->barrier ||
		this->chunk_index[0] <= this->barrier ||
		this->chunk_index[2] >= this->scz - this->barrier ||
		this->chunk_index[2] <= this->barrier)
	{

		this->new_center[0] = this->chunk_index[0];
		this->new_center[1] = this->chunk_index[1];
		this->new_center[2] = this->chunk_index[2];
		if (this->new_center[0] < 0)
			this->new_center[0] = 0;
		if (this->new_center[2] < 0)
			this->new_center[2] = 0;

		if (this->new_center[0] > this->scx - 1)
			this->new_center[0] = this->scx - 1;
		if (this->new_center[2] > this->scz - 1)
			this->new_center[2] = this->scz - 1;

		if (this->state.verbose)
			fprintf(stderr, "Cross barrier (%d): New center = %d %d %d\n", this->barrier, this->new_center[0], this->new_center[1], this->new_center[2]);
		return true;
	}
	return false;
}

void Engine::set_limit_by_time(int time)
{
	this->barrier = ((((time / 2) / 1000) * 20) / 16) + (this->g_state.renderDistance) + 4;
	if (this->barrier > this->scx / 2)
		this->barrier = this->scx / 2 - 2;
	if (this->barrier > 45)
		this->barrier = 45;
}

void Engine::swap_worlds(void)
{
	World *tmpW;
	tmpW = this->world;
	this->world = this->next_world;
	this->next_world = tmpW;

	this->world->adjust_camera(this->next_world);

	if (this->edit_size > 0)
		this->apply_player_edits();
}

void Engine::apply_player_edits(void)
{
	for (int i = 0; i < this->edit_size; i++)
	{
		this->world->setChunk(this->edit_to_apply[i].x + -((this->world->shiftX - this->next_world->shiftX) * CX), this->edit_to_apply[i].y,
							  this->edit_to_apply[i].z + -((this->world->shiftZ - this->next_world->shiftZ) * CZ), this->edit_to_apply[i].blk);
	}
	this->edit_size = 0;
}

void Engine::write_keys(void)
{
	printf("Use ZQSD/WASD to move around, A to go up, E to go down and shift to go fast.\n");
	printf("Use home and end to go to two predetermined positions.\n");
	printf("Use PageUp And PageDown to up/down the sound volume.\n");
	printf("Press the left mouse button to build a block.\n");
	printf("Press the right mouse button to remove a block.\n");
	printf("Use the scrollwheel to select different types of blocks.\n");
	printf("Press +/- to change player speed\n");
	printf("Press insert to toggle anchor camera.\n");
	printf("Press F1 to change sampling value.\n");
	printf("Press F2 to toggle anti-aliasing.\n");
	printf("Press F3 to toggle transparency.\n");
	printf("Press F4 to toggle collision.\n");
	printf("Press F5 to toggle focussing on transparent blocks.\n");
	printf("Press F6 to change the framerate limit.\n");
	printf("Press F7 to change the render distance limit.\n");
	printf("Press F8 to swap the both worlds.\n");
	printf("Press F9 to show debug info\n");
	printf("Press F10 to desactivate infinite world.\n");
	printf("Press F11 to activate autowalk.\n");
	printf("Press F12 to use breakpoint with gdb\n");
}

void Engine::write_usage(void)
{
	fprintf(stderr, "usage : ./ft_vox [options]\n");
	fprintf(stderr, "optional parameters :\n");
	fprintf(stderr, "\t<-w> or <--width> to specify the window width\n");
	fprintf(stderr, "\t\tmin: %d, max: %d\n", WIDTH_MIN, WIDTH_MAX);
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<-h> or <--height> to specify the window height\n");
	fprintf(stderr, "\t\tmin: %d, max: %d\n", HEIGHT_MIN, HEIGHT_MAX);
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<-s> or <--seed> to speciy the seed of the map\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<-ws> or <--world-size> to specify size of the saved world\n");
	fprintf(stderr, "\t\tDefault value is 128\n");
	fprintf(stderr, "\t\tmin: %d, max: %d, and must be a multiple of 8\n", WORLD_MIN, WORLD_MAX);
	fprintf(stderr, "\t\tThe lower the value is, the lower the RAM usage and loading time will be\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<--sync> to disable the VSync\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<-f> or <--fullscreen> to activate fullscreen\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<-d> or <--debug> to display debug mode\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t<-v> or <--version> to specify shader version\n");
	fprintf(stderr, "\t\tmin: 100, max: 999\n");
	fprintf(stderr, "\t\tThe right version to give is at your discretion\n");
	fprintf(stderr, "\t\tDefault value is 460\n");
}