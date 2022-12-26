#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"
#include "ModulePhysics.h"


enum class MovementType
{
	APPLY_FORCE,
	APPLY_VELOCITY,
	CHANGE_POSITION

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

	/*Object(int posX_, int posY_, int id_, WormType type_, bool render = true) : graphic(NULL)
	{
		posX = posX_;
		posY = posY_;
		id = id_;
		type = type_;
		renderable = render;
		
		body = new PhysBall(PIXEL_TO_METERS(posX), PIXEL_TO_METERS(posY), 1.0f, 10.0f, 0.0f, 20.0f, 1.0f, 1.2f, 0.4f, 10.0f, 0.9f, 0.8f);
	}*/

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
	update_status PostUpdate();
	bool CleanUp();

	void controls(int player, MovementType move);


public:

	std::vector<PhysBall> listBLUE{};
	std::vector<PhysBall> listRED{};

	//std::vector<int> listPlayers{};
	std::vector<int> listBlueP{};
	std::vector<int> listRedP{};

	float posX, posY, radBody;
	int playerBody;
	MovementType movement;

	int setID;
	bool playerTurn;	// blue = true, red = false

	int currentPlayer;
};