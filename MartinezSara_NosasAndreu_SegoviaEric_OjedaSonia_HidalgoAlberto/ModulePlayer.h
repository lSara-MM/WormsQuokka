#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"
#include "ModulePhysics.h"

enum class WormType
{
	RED,
	BLUE
};

class Object
{
public:
	SDL_Texture* graphic;
	uint fx;

	Object() : graphic(NULL) {}

	/*Object(iPoint pos, int id_, WormType type_, bool render = true) : graphic(NULL)
	{
		position = pos;
		id = id_;
		type = type_;
		renderable = render;
	}*/

	Object(int posX_, int posY_, int id_, WormType type_, bool render = true) : graphic(NULL)
	{
		posX = posX_;
		posY = posY_;
		id = id_;
		type = type_;
		renderable = render;
		
		body = new PhysBall(PIXEL_TO_METERS(posX), PIXEL_TO_METERS(posY), 1.0f, 10.0f, 0.0f, 20.0f, 1.0f, 1.2f, 0.4f, 10.0f, 0.9f, 0.8f);
	}

public:
	//iPoint position;	// idk perque me dona problemes? tf 
	int posX, posY;

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

	void controls(Object player);

public:
	p2List <Object*>* listPlayers;
	p2List <Object*>* listBLUE;
	p2List <Object*>* listRED;

	bool player;	// blue = true, red = false

	Object currentPlayer;
};