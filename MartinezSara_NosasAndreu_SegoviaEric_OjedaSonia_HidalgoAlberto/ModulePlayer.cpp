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

	setID = 0;

	// Blue team
	CreatePlayer(100, 200, ObjectType::BLUE);
	CreatePlayer(200, 200, ObjectType::BLUE);

	setID = 0;

	// Red team
	CreatePlayer(600, 350, ObjectType::RED);
	CreatePlayer(700, 350, ObjectType::RED);

	canPlay = true;
	playerTurn = true;
	currentBlue = 0;
	currentRed = 0;

	deadBlue = 0;
	deadRed = 0;
	App->scene_intro->endGame = false;
	App->scene_intro->winaudio = false;

	jumpTimer = 0;
	jump = false;

	turnTimer = 0;
	startTime = SDL_GetTicks();

	audiogun = App->audio->LoadFx("audio/gun.ogg");
	audiomisile = App->audio->LoadFx("audio/misile.ogg");
	audiogrenade = App->audio->LoadFx("audio/grenade.ogg");
	audiodrums = App->audio->LoadFx("audio/drums.ogg");
	audiowin = App->audio->LoadFx("audio/win.ogg");

	App->audio->PlayFx(audiodrums);
	return true;
}

//Player control
update_status ModulePlayer::Update()
{
	if (!App->scene_intro->fixCutre)
	{
		CleanUp();
		App->scene_intro->fixCutre = true;
	}

	if (!App->scene_intro->endGame)
	{
		if (canPlay)
		{
			// Select player
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

			// Turn control
			turnControl();

			jumpTimer++;
		}

		// Check hp
		currentBlue = checkHp(listBlueP, currentBlue, &deadBlue);
		currentRed = checkHp(listRedP, currentRed, &deadRed);

		// Win/Lose conditions
		if (deadBlue == listBlueP.size() && deadRed < listRedP.size())
		{
			App->scene_intro->endGame = true;
			App->scene_intro->result = 0;
			App->scene_intro->winaudio = true;
		}

		if (deadBlue < listBlueP.size() && deadRed == listRedP.size())
		{
			App->scene_intro->endGame = true;
			App->scene_intro->result = 1;
			App->scene_intro->winaudio = true;
		}

		if (deadBlue == listBlueP.size() && deadRed == listRedP.size())
		{
			App->scene_intro->endGame = true;
			App->scene_intro->result = -1;
			App->scene_intro->winaudio = true;
		}

		if (App->physics->losehp == true) {
			LoseHPplayer(App->physics->bodyHP, App->physics->typeW, App->physics->typeP);
			App->physics->losehp = false;
		}
	}

	//Events control
	eventTimer = SDL_GetTicks() - startTimeEvents;

	LOG("Turn %d - Time left: %d", playerTurn, turnTimer);
	if (eventTimer > 4000)
	{
		if (App->scene_intro->eventProbability <= 5) { App->scene_intro->eventProbability++; }
		startTimeEvents = SDL_GetTicks();
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

	App->physics->CleanUp();
	Start();
	return true;
}

int ModulePlayer::CreatePlayer(int posX_, int posY_, ObjectType type_, int hp_)
{
	Worm* new_worm = new Worm(posX_, posY_, type_, hp_);
	new_worm->id = setID++;
	new_worm->weapon = ObjectType::GUN; // gun by default 


	new_worm->body = App->physics->CreateBall(PIXEL_TO_METERS(posX_), PIXEL_TO_METERS(posY_), 1.0f, type_, 80.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f, 0.0f, 0.0f, true, true);
	LOG("newWorm: %d", new_worm->body);

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

int ModulePlayer::CreateWeapon(int posX_, int posY_, int dir, float angle, float force, ObjectType type_)
{
	Weapon* new_gun = new Weapon(posX_, posY_, type_);
	
	float vx, vy, mass, rest;

	float dirX = dir * cos(angle * DEG_TO_RAD);
	float dirY = -sin(angle * DEG_TO_RAD);

	//force actua como un impulso ya que es una fuerza instantnea

	if (type_ == ObjectType::GUN)
	{
		new_gun->range = 3;
		mass = 10.0f;
		vx = force / mass * dirX;		vy = force / mass * dirY;
		rest = 0.1f;
		//vx = force * dirX;		vy = -force * dirY;		mass = 10.0f;// no acaba d'anar bé
	}
	if (type_ == ObjectType::GRENADE)
	{
		new_gun->range = 10;
		mass = 20.0f;
		vx = force / mass * dirX;		vy = force / mass * dirY;
		rest = 0.9f;
		//vx = force * dirX;		vy = -force * dirY;		mass = 50.0f;
	}
	if (type_ == ObjectType::MISSILE)
	{
		new_gun->range = 5;
		mass = 30.0f;
		vx = force / mass * dirX;		vy = force / mass * dirY;
		rest = 0.0f;
		//vx = force * dirX;		vy = -force * dirY;		mass = 50.0f;
	}

	new_gun->body = App->physics->CreateBall(PIXEL_TO_METERS(posX_), //Position X
		PIXEL_TO_METERS(posY_), //Position y
		new_gun->range / 10, //Radius
		type_, //Tipo
		mass, //Massa
		vx, // Velocidad X
		vy, // Velocidad Y
		1.0f, //Superficie
		1.0f, // Coeficiente lift aerodinamico
		1.0f, //Coeficiente drag aerodinamico
		0.3, //Coeficiente buoyancy
		0.1f, //Coeficiente friccion
		rest //Coeficiente restitucion
	);

	return new_gun->body;
}


int ModulePlayer::selectPlayer(int p)
{
	// Depende del num de players, se puede hacer general pero nose, por ahora parece mas comodo asi
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			return 0;
		}
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			return 1;
		}
		/*if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) { return 2; }
		if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) { return 3; }
		if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) { return 4; }*/
	}

	return p;
}

void ModulePlayer::selectWeapon(Worm* player)
{
	switch (player->weapon)
	{
	case ObjectType::GUN:
		(player->gAmmo > 0) ? player->weapon = ObjectType::GRENADE : player->weapon = ObjectType::MISSILE;
		break;
	case ObjectType::GRENADE:
		player->weapon = ObjectType::MISSILE;	// in case more weapons, put next weapon
		break;
	case ObjectType::MISSILE:
		player->weapon = ObjectType::GUN;
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
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).ApplyForce(-1500.0f, 0.0f);
			player->direction = -1;
		}

		if (jump) {
			if (jumpTimer >= 10) {
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
					jumpTimer = 0.0f;
					jump = true;
					App->physics->balls.at(player->body).ApplyForce(App->physics->balls.at(player->body).fx, -2000.0f);
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
			App->physics->balls.at(player->body).SetVelocity(10.0f, 0.0f);
			player->direction = 1;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).SetVelocity(-10.0f, 0.0f);
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
			App->physics->balls.at(player->body).AddPosition(5.0f, 0.0f);
			player->direction = 1;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			App->physics->balls.at(player->body).AddPosition(-5.0f, 0.0f);
			player->direction = -1;
		}


		for (auto& ground : App->physics->grounds)
		{
			if (is_colliding_with_ground(App->physics->balls.at(player->body), ground))
			{
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
				{
					App->physics->balls.at(player->body).AddPosition(App->physics->balls.at(player->body).x, -80.0f);
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

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN && !App->physics->debug)
	{
		player->direction = 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN && !App->physics->debug)
	{
		player->direction = -1;
	}

	if (player->angle < 90)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			player->angle += 15;
		}
	}
	if (player->angle > -90)
	{
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			player->angle -= 15;
		}
	}


	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		selectWeapon(player);
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && player->forceApplied <= 500.0f)
	{
		player->forceApplied += 10.0f;
	}

	if (player->gAmmo <= 0 && player->weapon == ObjectType::GRENADE) { player->weapon = ObjectType::GUN; }
	if (player->mAmmo <= 0 && player->weapon == ObjectType::MISSILE) { player->weapon = ObjectType::GUN; }

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		shoot(player);
		
		LOG("force %f", player->forceApplied);
		player->forceApplied = 0;
		if (player->weapon == ObjectType::GUN) {
			App->audio->PlayFx(audiogun);
		}

		if (player->weapon == ObjectType::MISSILE) {
			App->audio->PlayFx(audiomisile);
		}

		if (player->weapon == ObjectType::GRENADE) {
			App->audio->PlayFx(audiogrenade);
		}
	}

	renderStats(player);

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		CleanUp();
	}

	if (App->physics->debug)
	{
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
		{
			(player->type == ObjectType::BLUE) ? listBlueP.at(player->id)->hp = 0 : listRedP.at(player->id)->hp = 0;
		}
	}
}

void ModulePlayer::renderStats(Worm* player)
{
	float cosinus = cos(player->angle * DEG_TO_RAD);
	float Sinus = sin(player->angle * DEG_TO_RAD);

	App->renderer->DrawLine(player->posX, player->posY, player->posX + (player->forceApplied / 3 + 40) * cosinus * player->direction, player->posY - (player->forceApplied / 3 + 40) * Sinus, 100, 255, 100);

	//Actual weapon
	const char* weaponName = "NONE";
	int margin = 1;
	int ammo;

	switch (player->weapon)
	{
	case ObjectType::GUN:
		weaponName = "GUN";
		margin = 2;
		ammo = 99; //(random num per si peta)
		break;
	case ObjectType::GRENADE:
		weaponName = "GRENADE";
		margin = 1;
		ammo = player->gAmmo;
		break;
	case ObjectType::MISSILE:
		weaponName = "MISSILE";
		margin = 1;
		ammo = player->mAmmo;
		break;
	case ObjectType::OTHER:
		break;
	default:
		weaponName = "EROR 404";
		break;
	}

	//Show actual weapon
	App->renderer->BlitText(player->posX - METERS_TO_PIXELS(App->physics->balls.at(player->body).radius) / margin, player->posY - METERS_TO_PIXELS(App->physics->balls.at(player->body).radius) - 15, App->renderer->blueFont, weaponName);

	// strings to const char*
	string s_ammo = std::to_string(ammo);
	const char* ch_ammo = s_ammo.c_str();
	App->renderer->BlitText(player->posX - METERS_TO_PIXELS(App->physics->balls.at(player->body).radius) - 60, player->posY - METERS_TO_PIXELS(App->physics->balls.at(player->body).radius) - 15, App->renderer->blueFont, "AMMO-");
	App->renderer->BlitText(player->posX - METERS_TO_PIXELS(App->physics->balls.at(player->body).radius) - 20, player->posY - METERS_TO_PIXELS(App->physics->balls.at(player->body).radius) - 15, App->renderer->blueFont, ch_ammo);
}

int ModulePlayer::shoot(Worm* player)
{
	int a;	// projectile radius
	
	switch (player->weapon)
	{
	case ObjectType::GUN:
		a = METERS_TO_PIXELS(0.5f);
		break;
	case ObjectType::GRENADE:
		a = METERS_TO_PIXELS(2.0f);
		player->gAmmo--;
		break;
	case ObjectType::MISSILE:
		a = METERS_TO_PIXELS(3.0f);
		player->mAmmo--;
		break;
	default:
		break;
	}

	float ang = player->angle * DEG_TO_RAD;
	float co = cos(ang);
	float si = sin(ang);

	//LOG("Player angle: %f", player->angle);
	//LOG("Rad bullet: %d", a);
	//LOG("Angle: %f", ang);
	//LOG("Cos: %f", co);
	//LOG("Sin: %f", si);

	if (player->angle == 180 || player->angle == 0) {
		(player->direction == 1) ? player->playerWeapon = CreateWeapon((player->posX + a) + METERS_TO_PIXELS(co), (player->posY) - METERS_TO_PIXELS(si), 1, player->angle, player->forceApplied, player->weapon)
			: player->playerWeapon = CreateWeapon((player->posX - a) - METERS_TO_PIXELS(co), (player->posY - a) - METERS_TO_PIXELS(si), -1, player->angle, player->forceApplied, player->weapon);
	}
	else if (player->angle < 180 && player->angle > 0) {
		(player->direction == 1) ? player->playerWeapon = CreateWeapon((player->posX + a) + METERS_TO_PIXELS(co), (player->posY - a) - METERS_TO_PIXELS(si), 1, player->angle, player->forceApplied, player->weapon)
			: player->playerWeapon = CreateWeapon((player->posX - a) - METERS_TO_PIXELS(co), (player->posY - a) - METERS_TO_PIXELS(si), -1, player->angle, player->forceApplied, player->weapon);
	}
	else {
		(player->direction == 1) ? player->playerWeapon = CreateWeapon((player->posX + a) + METERS_TO_PIXELS(co), (player->posY + a) - METERS_TO_PIXELS(si), 1, player->angle, player->forceApplied, player->weapon)
			: player->playerWeapon = CreateWeapon((player->posX - a) - METERS_TO_PIXELS(co), (player->posY + a) - METERS_TO_PIXELS(si), -1, player->angle, player->forceApplied, player->weapon);
	}

	// Wait for projectile hit to continue playing
	canPlay = false;
	playerTurn = !playerTurn;
	startTime = SDL_GetTicks();
	return 0;
}

void ModulePlayer::LoseHPplayer(int body, ObjectType type_W, ObjectType type_P) {

	int rest = 0;

	if (type_W == ObjectType::GUN) {
		rest = 20;
	}

	if (type_W == ObjectType::GRENADE) {
		rest = 50;
	}

	if (type_W == ObjectType::MISSILE) {
		rest = 35;
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
			}
		}
	}
}

int ModulePlayer::checkHp(std::vector<Worm*> list, int current, int *deadCount)
{
	int dead = 0;
	for (auto& Worm : list)
	{
		Worm->posX = METERS_TO_PIXELS(App->physics->balls.at(Worm->body).x);
		Worm->posY = METERS_TO_PIXELS(App->physics->balls.at(Worm->body).y);

		if (list.at(Worm->id)->hp <= 0)
		{
			App->renderer->BlitText(list.at(Worm->id)->posX - METERS_TO_PIXELS(App->physics->balls.at(list.at(Worm->id)->body).radius) / 2, list.at(Worm->id)->posY - 35, App->renderer->blueFont, "DEAD");
			App->physics->balls.at(list.at(Worm->id)->body).physics_enabled = false;

			int j = 0;
			for (int i = current + 1; j < list.size(); i++)
			{
				if (i >= list.size()) { i = 0; }
				if (list.at(i)->hp > 0)
				{
					current = i;
					break;
				}
				j++;
			}
			dead++;
			*deadCount = dead;
		}
		else
		{
			// strings to const char*
			string s_hp = std::to_string(Worm->hp);
			const char* ch_hp = s_hp.c_str();

			App->renderer->BlitText(Worm->posX - METERS_TO_PIXELS(App->physics->balls.at(Worm->body).radius) / 2 - 15, Worm->posY - 50, App->renderer->blueFont, "HP-");
			App->renderer->BlitText(Worm->posX - METERS_TO_PIXELS(App->physics->balls.at(Worm->body).radius) / 2 + 10, Worm->posY - 50, App->renderer->blueFont, ch_hp);
		}
	}

	return current;
}

void ModulePlayer::turnControl()
{
	turnTimer = SDL_GetTicks() - startTime;

	LOG("Turn %d - Time left: %d", playerTurn, turnTimer);
	if (turnTimer > 10800)
	{
		playerTurn = !playerTurn;
		startTime = SDL_GetTicks();
	}

	int timeLeft = (10800 - turnTimer) / 600;

	string s_timer = std::to_string(timeLeft);
	const char* ch_timer = s_timer.c_str();
	App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 300, App->renderer->greenFont, "TIME LEFT-");
	App->renderer->BlitText(SCREEN_WIDTH - 15 * 8, 300, App->renderer->greenFont, ch_timer);
}
