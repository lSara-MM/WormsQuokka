#pragma once
#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "ModuleInput.h"
#include "Application.h"

#include "ModulePhysics.h"

#define BOUNCER_TIME 200

enum lightTypes
{
	tiny,
	medium,
	big
};

class ModuleSceneIntro;

struct Light
{
	Light() : texture(NULL), on(false), fx(0)
	{}

	Light(ModuleSceneIntro* physics, int x, int y, lightTypes type);

	lightTypes type;
	SDL_Texture* texture;
	bool on;
	uint fx;
	int x, y;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void Debug();

public:

	SDL_Texture* graphics;

	SDL_Texture* tex_light_tiny;
	SDL_Texture* tex_light_medium;
	SDL_Texture* tex_light_big;
	
	uint fx_light_tiny;
	uint fx_light_medium;
	uint fx_light_big;

	p2DynArray<Light> lights;

	uint player_lose_fx;
 
	SDL_Rect rect; 

	int x = 500, y = 200, rad = 100;
};
