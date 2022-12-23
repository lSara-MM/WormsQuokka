#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics.h"

enum class WormType
{
	RED,
	BLUE
};

class Object
{
	SDL_Texture* graphic;
	uint fx;

	Object() : graphic(NULL)
	{}

	Object(iPoint pos, int id_, WormType type_, bool render = true) : graphic(NULL)
	{
		position = pos;
		id = id_;
		type = type_;
		renderable = render;
	}

public:
	iPoint position;
	bool renderable = true;
	int id;
	WormType type;

	PhysBall* body;
};

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

};