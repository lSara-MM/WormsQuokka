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

	void renderParameters(std::vector<Worm*> list, int current);
	void renderParameters();


	void RandomEvent();
	

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
 
	bool endGame;	// true when a team has lost or draw
	int result;		//	-1 draw / 0 Blue / 1 red 
	bool winaudio; 

	int eventProbability=0;
	int actualEvent = -1;
	
	enum events
	{
		WATERRISE=0, //INCREASE WATER BY 5 (Max 2 times)
		WATERDESCEND, //REDUCE WATER BY 5 (Max 1 time)
		NOWIND, // Set wind to +- 0.1
		CHANGEWIND, //Wind *-1
		MOREWIND, //Add 5 of wind 
		LESSFWIND, // Substracts 5 of wind
		NUMEVENTS

	};
	int eventCounter[NUMEVENTS];

	int frames;

	bool fixCutre;

};
