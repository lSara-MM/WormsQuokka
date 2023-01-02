#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"

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
	if (playerTurn)	// Blue turn
	{
		currentBlue = selectPlayer(currentBlue);
		controls(listBlueP.at(currentBlue), movement);
	}
	else // Red turn
	{
		currentRed = selectPlayer(currentRed);
		controls(listRedP.at(currentRed), movement);
	}

	if (listBlueP.at(currentBlue)->hp < 0)
	{
		listBlueP.at(currentBlue);
	}

	// Win/Lose conditions
	if (listBlueP.empty() && !listRedP.empty())
	{ LOG("RED team wins"); }

	if (listRedP.empty() && !listBlueP.empty())
	{ LOG("BLUE team wins"); }
	
	if (listRedP.empty() && listBlueP.empty()) 
	{ LOG("DRAW"); }

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

	new_worm->body = App->physics->CreateBall(PIXEL_TO_METERS(posX_), PIXEL_TO_METERS(posY_), 1.0f, type_, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f, 0.0f, 0.0f, a);

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
		vx = 30.0f * dirX;		vy = 20.0f;		mass = 10.0f;
	}
	if (type_ == ObjectType::GRENADE) 
	{ 
		new_gun->range = 20; 
		vx = 10.0f * dirX;		vy = 20.0f;		mass = 50.0f;
	}

	new_gun->body = App->physics->CreateBall(PIXEL_TO_METERS(posX_), PIXEL_TO_METERS(posY_), new_gun->range / 10, type_, mass, vx, vy, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f);

	return new_gun->body;
}

int ModulePlayer::selectPlayer(int p)
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

void ModulePlayer::controls(Worm* player, MovementType move)
{
	switch (move)
	{
	case MovementType::APPLY_FORCE:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{ 
			App->physics->balls.at(player->body).ApplyForce(1500.0f, 0.0f); 
			player->direction = true;
			listBlueP.at(currentBlue);
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).ApplyForce(-1500.0f, 0.0f);
			player->direction = false;
		}
		
		for (auto& ground : App->physics->grounds)
		{
			if (is_colliding_with_ground(App->physics->balls.at(player->body), ground))
			{
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
				{ 
					App->physics->balls.at(player->body).ApplyForce(0.0f, -3000.0f);
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		{ App->physics->balls.at(player->body).ApplyForce(0.0f, 300.0f); }

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP || App->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
		{ App->physics->balls.at(player->body).ApplyForce(0.0F, 0.0f); }		

		break;

	case MovementType::APPLY_VELOCITY:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{ App->physics->balls.at(player->body).SetVelocity(15.0f, App->physics->balls.at(player->body).vy); }

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{ App->physics->balls.at(player->body).SetVelocity(-15.0f, App->physics->balls.at(player->body).vy); }


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
		{ App->physics->balls.at(player->body).SetVelocity(0.0F, 0.0f); }

		break;

	case MovementType::CHANGE_POSITION:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{ App->physics->balls.at(player->body).AddPosition(15.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{ App->physics->balls.at(player->body).AddPosition(-15.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{ App->physics->balls.at(player->body).AddPosition(0.0F, 0.0f); }

		break;
	default:
		break;
	}

	player->posX = METERS_TO_PIXELS(App->physics->balls.at(player->body).x);
	player->posY = METERS_TO_PIXELS(App->physics->balls.at(player->body).y);

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
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

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		int a = METERS_TO_PIXELS(2.0f);
		ObjectType b;

		switch (player->weapon)
		{
		case ObjectType::GUN:
			b = ObjectType::GUN;
			break;
		case ObjectType::GRENADE:
			b = ObjectType::GRENADE;
			break;
		default:
			break;
		}

		(player->direction == true) ? CreateWeapon(player->posX + a, player->posY, 1, 1, b)
			: CreateWeapon(player->posX - a, player->posY, -1, 1, b);

		// hacer que espere a que colisione el proyectil 
		playerTurn = !playerTurn;
	}

}


