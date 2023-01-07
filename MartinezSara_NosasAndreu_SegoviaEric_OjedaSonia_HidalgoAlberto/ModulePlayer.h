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

class Worm
{
public:
	SDL_Texture* graphic;
	uint fx;

	Worm() : graphic(NULL) {}

	Worm(int posX_, int posY_, ObjectType type_, int hp = 100, bool render = true) : graphic(NULL)
	{
		posX = posX_;
		posY = posY_;
		type = type_;
		renderable = render;
		gAmmo = 2;
		mAmmo = 3;
	}

public:
	//iPoint position;	// idk perque me dona problemes? tf 
	int posX, posY;

	bool renderable = true;
	int id;	// Position in vector 
	int hp = 100;
	int direction = 1;	// 1 - derecha, -1 - izquierda
	ObjectType type;

	int body;	// Position in list Balls

	int playerWeapon;
	ObjectType weapon;
	float angle = 0;

	int gAmmo;
	int mAmmo;

	float forceApplied = 0;//force applied to projectile when launched
};

class Weapon
{
public:
	SDL_Texture* graphic;
	uint fx;

	Weapon() : graphic(NULL) {}

	Weapon(int posX_, int posY_, ObjectType type_, bool render = true) : graphic(NULL)
	{
		posX = posX_;
		posY = posY_;
		type = type_;

		renderable = render;
	}
	
public:
	//iPoint position;	// idk perque me dona problemes? tf 
	int posX, posY;

	bool renderable = true;
	float range;	// range of damage
	ObjectType type;

	int body;
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

	int CreatePlayer(int posX_, int posY_, ObjectType type_, int hp_ = 100, bool render = true);
	int CreateWeapon(int posX_, int posY_, int dirX, float angle, float force,ObjectType type_, bool render = true);
	void LoseHPplayer(int body, ObjectType type_W, ObjectType type_P);

	int selectPlayer(int p);
	void controls(Worm* player, MovementType move);
	void renderStats(Worm* player);

	void selectWeapon(Worm* player);
	int shoot(Worm* player);

public:
	 
	std::vector<int> listPlayers{};
	std::vector<Worm*> listBlueP{};
	std::vector<Worm*> listRedP{};

	MovementType movement;

	int setID;
	bool playerTurn;	// blue = true, red = false

	int currentBlue;
	int currentRed;
	int deadBlue;
	int deadRed;

	float timer;
	bool jump;

};