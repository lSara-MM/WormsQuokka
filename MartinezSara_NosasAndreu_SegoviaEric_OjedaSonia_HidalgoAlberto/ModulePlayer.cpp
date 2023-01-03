#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"

#include <iostream>
using namespace std;
#include <sstream>


ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	// Blue team
	//posX = PIXEL_TO_METERS(100);
	//posY = PIXEL_TO_METERS(200);
	//radBody = 0.7f;

	//int num = App->physics->CreateBall(posX, posY, radBody, ObjectType::BLUE, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f);
	//movement = MovementType::APPLY_FORCE;
	////listPlayers.emplace_back(num);
	//listBlueP.emplace_back(num);
	//
	//posX = PIXEL_TO_METERS(130);
	//posY = PIXEL_TO_METERS(200);
	//num = App->physics->CreateBall(posX, posY, radBody, ObjectType::BLUE, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f);
	//movement = MovementType::APPLY_FORCE;
	////listPlayers.emplace_back(num);
	//listBlueP.emplace_back(num);


	// Red team
	//posX = PIXEL_TO_METERS(600);
	//posY = PIXEL_TO_METERS(350);

	//num = App->physics->CreateBall(posX, posY, radBody, ObjectType::RED, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f);
	//movement = MovementType::APPLY_FORCE;
	////listPlayers.emplace_back(num);
	//listRedP.emplace_back(num);

	// Blue team
	CreatePlayer(100, 200, ObjectType::BLUE);
	CreatePlayer(200, 200, ObjectType::BLUE);

	setID = 0;

	// Red team
	CreatePlayer(600, 350, ObjectType::RED);
	CreatePlayer(700, 350, ObjectType::RED);

	playerTurn = true;
	currentBlue = 0;
	currentRed = 0;
	return true;
}

//Player control
update_status ModulePlayer::Update()
{
	// Select player
	if (playerTurn)	// Blue turn
	{
		currentBlue = selectPlayer(ObjectType::BLUE, currentBlue);
		controls(listBlueP.at(currentBlue), movement);
	}
	else // Red turn
	{
		currentRed = selectPlayer(ObjectType::RED, currentRed);
		controls(listRedP.at(currentRed), movement);
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		listBlueP.at(currentBlue)->hp = 0;
	}

	// Check hp
	for (auto& Worm : listBlueP)
	{
		Worm->posX = METERS_TO_PIXELS(App->physics->balls.at(Worm->body).x);
		Worm->posY = METERS_TO_PIXELS(App->physics->balls.at(Worm->body).y);

		if (listBlueP.at(Worm->id)->hp <= 0)
		{
			App->renderer->BlitText(listBlueP.at(Worm->id)->posX - METERS_TO_PIXELS(App->physics->balls.at(listBlueP.at(Worm->id)->body).radius) / 2, listBlueP.at(Worm->id)->posY - 40, App->renderer->blueFont, "DEAD");
			App->physics->balls.at(listBlueP.at(Worm->id)->body).physics_enabled = false;
			
			(currentBlue == 0) ? currentBlue = 1 : currentBlue = 0;
		}
		else
		{
			// strings to const char*
			string s_hp = std::to_string(Worm->hp);
			const char* ch_hp = s_hp.c_str();

			App->renderer->BlitText(Worm->posX - METERS_TO_PIXELS(App->physics->balls.at(Worm->body).radius) / 2, Worm->posY - 40, App->renderer->blueFont, ch_hp);
		}
	}


	for (auto& Worm : listRedP)
	{
		Worm->posX = METERS_TO_PIXELS(App->physics->balls.at(Worm->body).x);
		Worm->posY = METERS_TO_PIXELS(App->physics->balls.at(Worm->body).y);

		if (listRedP.at(Worm->id)->hp <= 0)
		{
			App->renderer->BlitText(listRedP.at(Worm->id)->posX - METERS_TO_PIXELS(App->physics->balls.at(listRedP.at(Worm->id)->body).radius) / 2, listRedP.at(Worm->id)->posY - 40, App->renderer->blueFont, "DEAD");
			App->physics->balls.at(listRedP.at(Worm->id)->body).physics_enabled = false;
			
			(currentRed == 0) ? currentRed = 1 : currentRed = 0;
		}
		else
		{
			// strings to const char*
			string s_hp = std::to_string(Worm->hp);
			const char* ch_hp = s_hp.c_str();

			App->renderer->BlitText(Worm->posX - METERS_TO_PIXELS(App->physics->balls.at(Worm->body).radius) / 2, Worm->posY - 40, App->renderer->blueFont, ch_hp);
		}
	}



	// Win/Lose conditions
	if (listBlueP.empty() && !listRedP.empty())
	{ LOG("RED team wins"); }

	if (listRedP.empty() && !listBlueP.empty())
	{ LOG("BLUE team wins"); }
	
	if (listRedP.empty() && listBlueP.empty()) 
	{ LOG("DRAW"); }

	timer++;

	if (App->physics->losehp == true) {
		LoseHPplayer(App->physics->bodyHP, App->physics->typeW, App->physics->typeP);
		App->physics->losehp = false; 
	}

	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModulePlayer::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	listBlueP.clear();
	listRedP.clear();

	return true;
}


int ModulePlayer::CreatePlayer(int posX_, int posY_, ObjectType type_, int hp_, bool render)
{
	Worm* new_worm = new Worm(posX_, posY_, type_, hp_, render);
	new_worm->id = setID++;
	new_worm->weapon = ObjectType::GUN; // gun by default 

	bool a;
	(render) ? a = true : a = false;	// if render true, physics true 

	new_worm->body = App->physics->CreateBall(PIXEL_TO_METERS(posX_), PIXEL_TO_METERS(posY_), 1.0f, type_, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f, 0.0f, 0.0f, a, true);
	LOG("newWorm: %d", new_worm->body);
	// nose si aixo servira per a algo, potser si despues 
	switch (type_)
	{
	case ObjectType::RED:
		listRedP.emplace_back(new_worm);
		break;
	case ObjectType::BLUE:
		listBlueP.emplace_back(new_worm);
		break;
	case ObjectType::OTHER:
		break;
	default:
		break;
	}

	return new_worm->body;
}

int ModulePlayer::CreateWeapon(int posX_, int posY_, int dirX, float dirY, ObjectType type_, bool render)
{
	Weapon* new_gun = new Weapon(posX_, posY_, type_, render);
	//new_gun->id = setID++;
	
	int vx, vy, mass;

	if (type_ == ObjectType::GUN) 
	{ 
		new_gun->range = 2; 
		vx = 30.0f * dirX;		vy = -1 * dirY;		mass = 10.0f;
	}
	if (type_ == ObjectType::GRENADE) 
	{ 
		new_gun->range = 20; 
		vx = 10.0f * dirX;		vy = -5 * dirY;		mass = 50.0f;
	}

	new_gun->body = App->physics->CreateBall(PIXEL_TO_METERS(posX_), PIXEL_TO_METERS(posY_), new_gun->range / 10, type_, mass, vx, vy, 1.0f, 1.0f, 1.0f, 0.3f, 0.5f, 0.1f);

	return new_gun->body;
}


int ModulePlayer::selectPlayer(ObjectType t, int p)
{
	// Depende del num de players, se puede hacer general pero nose, por ahora parece mas comodo asi
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{ return 0; }
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{ return 1; }
	/*if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) { return 2; }
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) { return 3; }
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) { return 4; }*/

	return p;
}

void ModulePlayer::selectWeapon(Worm* player)
{
	switch (player->weapon)
	{
	case ObjectType::GUN:
		player->weapon = ObjectType::GRENADE;
		break;
	case ObjectType::GRENADE:
		player->weapon = ObjectType::GUN;	// in case more weapons, put next weapon
		break;
	default:
		break;
	}
}


void ModulePlayer::controls(Worm* player, MovementType move)
{
	switch (move)
	{
	case MovementType::APPLY_FORCE:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).ApplyForce(1500.0f, 0.0f);
			player->direction = 1;
			listBlueP.at(currentBlue);
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).ApplyForce(-1500.0f, 0.0f);
			player->direction = -1;
		}

		if (jump) {
			if (timer >= 40) {
				App->physics->balls.at(player->body).ApplyForce(0.0F, 0.0f);
				jump = false;
			}
		}

		for (auto& ground : App->physics->grounds)
		{
			if (is_colliding_with_ground(App->physics->balls.at(player->body), ground))
			{
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
				{
					timer = 0.0f;
					jump = true;
					App->physics->balls.at(player->body).ApplyForce(App->physics->balls.at(player->body).fx, -2700.0f);
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{
			App->physics->balls.at(player->body).ApplyForce(App->physics->balls.at(player->body).fx, 300.0f);
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP || App->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
		{
			App->physics->balls.at(player->body).ApplyForce(0.0F, 0.0f);
		}

		break;

	case MovementType::APPLY_VELOCITY:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).SetVelocity(15.0f, 0.0f);
			player->direction = 1;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).SetVelocity(-15.0f, 0.0f);
			player->direction = -1;
		}


		for (auto& ground : App->physics->grounds)
		{
			if (is_colliding_with_ground(App->physics->balls.at(player->body), ground))
			{
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
				{
					App->physics->balls.at(player->body).SetVelocity(App->physics->balls.at(player->body).vx, -10.0f);
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{
			App->physics->balls.at(player->body).SetVelocity(0.0F, 0.0f);
		}

		break;

	case MovementType::CHANGE_POSITION:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).AddPosition(15.0f, 0.0f);
			player->direction = 1;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).AddPosition(-15.0f, 0.0f);
			player->direction = -1;
		}


		for (auto& ground : App->physics->grounds)
		{
			if (is_colliding_with_ground(App->physics->balls.at(player->body), ground))
			{
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
				{
					App->physics->balls.at(player->body).AddPosition(App->physics->balls.at(player->body).x, -100.0f);
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{
			App->physics->balls.at(player->body).AddPosition(0.0F, 0.0f);
		}

		break;
	default:
		break;
	}

	if (player->angle < 90)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			player->angle += 15;
		}
	}

	if (player->angle > 0)
	{
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			player->angle -= 15;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) 
	{ selectWeapon(player); }
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) 
	{ shoot(player); }

	// App->renderer->DrawLine(player->posX, player->posY, player->posX + cos(3.14 * player->angle / 180) * 50 * player->direction, player->posY - sin(3.14 * player->angle / 180) * 50, 0, 255, 100);
	// en vdd este no es acurate perque no se perque no dispara recte cap amunt totalment :/

	App->renderer->DrawLine(player->posX, player->posY, player->posX + 50 * player->direction, player->posY - player->angle, 0, 255, 100);
}

int ModulePlayer::shoot(Worm* player)
{
	int a;	// projectile radius
	switch (player->weapon)
	{
	case ObjectType::GUN:
		a = METERS_TO_PIXELS(2.0f);
		break;
	case ObjectType::GRENADE:
		a = METERS_TO_PIXELS(20.0f);
		break;
	default:
		break;
	}

	(player->direction == 1) ? player->playerWeapon = CreateWeapon(player->posX + a, player->posY, 1, player->angle, player->weapon)
		: player->playerWeapon = CreateWeapon(player->posX - a, player->posY, -1, player->angle, player->weapon);

	// hacer que espere a que colisione el proyectil 
	playerTurn = !playerTurn;

	return 0;
}

void ModulePlayer::LoseHPplayer(int body, ObjectType type_W, ObjectType type_P) {

	int rest = 1; 

	if (type_W == ObjectType::GUN) {

		rest = 20;
	}

	if (type_W == ObjectType::GRENADE) {

	    rest = 50;
	}

	int t = listRedP.size(); 

	if (type_P == ObjectType::RED) {

		for (int i = 0; i < t; i++) {

			if (listRedP.at(i)->body == body) {

				listRedP.at(i)->hp = listRedP.at(i)->hp - rest;
			}
		}

	}

	if (type_P == ObjectType::BLUE) {

		for (int i = 0; i < t; i++) {

			if (listBlueP.at(i)->body == body) {

				listBlueP.at(i)->hp = listBlueP.at(i)->hp - rest;
				LOG("BlueHP: %d", listBlueP.at(i)->hp - rest); 
			}
		}

	}
	

}