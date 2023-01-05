#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"
#include "p2List.h"
#include "ModuleInput.h"
#include "Application.h"

#include "SString.h"

#include <iostream>
using namespace std;
#include <sstream>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");

	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :" };
	App->renderer->blueFont = App->renderer->LoadFont("Fonts/FuenteAzulClaro.png", lookupTable, 1, 38); // 1 = rows 39 = columns
	App->renderer->greenFont = App->renderer->LoadFont("Fonts/FuenteVerde.png", lookupTable, 1, 38); // 1 = rows 39 = columns
	endGame = false;
	frames = 60;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	App->renderer->UnLoadFont(App->renderer->blueFont);
	App->renderer->UnLoadFont(App->renderer->greenFont);
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	SString title("%s | Current FPS: %f DeltaTime: %f  Expected FPS: %i, DeltaTime: %i", TITLE, 1000 / App->dt, App->dt, 1000 / App->time, App->time);
	App->window->SetTitle(title.GetString());

	Debug();

	if (endGame)
	{
		switch (result)
		{
		case -1:
			LOG("DRAW");
			App->renderer->BlitText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, App->renderer->greenFont, "DRAW");
			break;

		case 0:
			LOG("RED team wins");
			App->renderer->BlitText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, App->renderer->greenFont, "RED TEAM WINS");
			break;

		case 1:
			LOG("BLUE team wins");
			App->renderer->BlitText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, App->renderer->greenFont, "BLUE TEAM WINS");
			break;
		default:
			break;
		}
		
		App->renderer->BlitText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, App->renderer->blueFont, "PRESS ENTER TO PLAY AGAIN");
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			App->player->CleanUp();
		}
	}
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Debug() {

	//"Nomenclatura": Bools con 1,2,3...,0 , multiple opcion con F1,F2...,F12

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && App->physics->debug == true)
	{
		App->physics->method = integrationMethods::BACKWARDS_EULER;
		LOG("Backwards Euler");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && App->physics->debug == true)
	{
		App->physics->method = integrationMethods::FORWARDS_EULER;
		LOG("Forwards Euler");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && App->physics->debug == true)
	{
		App->physics->method = integrationMethods::VERLET;
		LOG("Verlet");
	}

	else if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->physics->options[0] = !App->physics->options[0];
	}

	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->physics->options[1] = !App->physics->options[1]; //Gravity enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->physics->options[2] = !App->physics->options[2]; //Buoyancy enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->physics->options[3] = !App->physics->options[3]; //Hidro enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		App->physics->options[4] = !App->physics->options[4]; //Aero enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->player->movement = MovementType::APPLY_FORCE;
		LOG("Movement applying force");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{

		App->player->movement = MovementType::APPLY_VELOCITY;
		LOG("Movement applying velocity");

	}

	else if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{

		App->player->movement = MovementType::CHANGE_POSITION;
		LOG("Movement changing position");

	}

	else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{

		App->timeControl = DeltaTimeControl::FIXED_DELTATIME;
		LOG("Fixed delta time");

	}

	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && App->physics->debug == true)
	{

		App->timeControl = DeltaTimeControl::FIXED_DELTATIME_DELAY;
		LOG("Fixed delta time with delay");

	}

	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && App->physics->debug == true)
	{

		App->timeControl = DeltaTimeControl::VARIABLE_DELTATIME;
		LOG("Variable delta time");

	}
	else if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN && App->physics->debug == true)
	{
		App->physics->options[5] = !App->physics->options[5]; //Allow wind changes
		if (App->physics->options[6]) { App->physics->options[6] = !App->physics->options[6]; }
	}
	else if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN && App->physics->debug == true)
	{
		App->physics->options[6] = !App->physics->options[6]; //Allow current changes
		if (App->physics->options[5]) { App->physics->options[5] = !App->physics->options[5]; }
	}

	// FPS Change

	else if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) {
		App->physics->options[7] = !App->physics->options[7];//allow fps changes
		LOG("fps change enables");
	}

	if (!App->physics->options[7]) { App->renderer->BlitText(26, 90, App->renderer->blueFont, "8: CHANGE FPS VALUE USING KEYS OFF"); }

	if (App->physics->options[7])
	{
		App->renderer->BlitText(26, 90, App->renderer->blueFont, "8: CHANGE FPS VALUE USING KEYS ON");
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN && frames < 120)
		{
			frames += 5;
			App->time = 1000 / frames;
			LOG("frames %d", frames);
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN && frames > 5)
		{
			frames -= 5;
			App->time = 1000 / frames;
			LOG("frames %d", frames);
		}

	}

	App->renderer->BlitText(10, 110, App->renderer->blueFont, "STATUS:");//Valores fps, valores bala/player, etc...
	App->renderer->BlitText(26, 120, App->renderer->blueFont, "FPS:");
	string f_num = std::to_string(frames);
	const char* f = f_num.c_str();

	App->renderer->BlitText(60, 120, App->renderer->blueFont, f);


	App->renderer->BlitText(26, 130, App->renderer->blueFont, "PosX: ");

	App->renderer->BlitText(26, 140, App->renderer->blueFont, "PosY: ");

	App->renderer->BlitText(26, 150, App->renderer->blueFont, "VelX: ");
	
	App->renderer->BlitText(26, 160, App->renderer->blueFont, "VelY: ");

	App->renderer->BlitText(26, 170, App->renderer->blueFont, "AccX: ");

	App->renderer->BlitText(26, 180, App->renderer->blueFont, "AccY: ");


	if (!App->player->playerTurn)	
	{
		if (App->player->listBlueP.at(App->player->currentBlue)->playerWeapon != 0) {

			string posx = std::to_string(App->physics->balls.at(App->player->listBlueP.at(App->player->currentBlue)->playerWeapon).x);
			const char* x = posx.c_str();
			App->renderer->BlitText(60, 130, App->renderer->blueFont, x);

			string posy = std::to_string(App->physics->balls.at(App->player->listBlueP.at(App->player->currentBlue)->playerWeapon).y);
			const char* y = posy.c_str();
			App->renderer->BlitText(60, 140, App->renderer->blueFont, y);

			string velx = std::to_string(App->physics->balls.at(App->player->listBlueP.at(App->player->currentBlue)->playerWeapon).vx);
			const char* vx = velx.c_str();
			App->renderer->BlitText(60, 150, App->renderer->blueFont, vx);

			string vely = std::to_string(App->physics->balls.at(App->player->listBlueP.at(App->player->currentBlue)->playerWeapon).vy);
			const char* vy = vely.c_str();
			App->renderer->BlitText(60, 160, App->renderer->blueFont, vy);

			string accx = std::to_string(App->physics->balls.at(App->player->listBlueP.at(App->player->currentBlue)->playerWeapon).ax);
			const char* ax = accx.c_str();
			App->renderer->BlitText(60, 170, App->renderer->blueFont, ax);

			string accy = std::to_string(App->physics->balls.at(App->player->listBlueP.at(App->player->currentBlue)->playerWeapon).ay);
			const char* ay = accy.c_str();
			App->renderer->BlitText(60, 180, App->renderer->blueFont, ay);

		}

	}

	else if (App->player->playerTurn) 
	{
		if (App->player->listRedP.at(App->player->currentRed)->playerWeapon != 0) {

			string posx = std::to_string(App->physics->balls.at(App->player->listRedP.at(App->player->currentRed)->playerWeapon).x);
			const char* x = posx.c_str();
			App->renderer->BlitText(60, 130, App->renderer->blueFont, x);

			string posy = std::to_string(App->physics->balls.at(App->player->listRedP.at(App->player->currentRed)->playerWeapon).y);
			const char* y = posy.c_str();
			App->renderer->BlitText(60, 140, App->renderer->blueFont, y);

			string velx = std::to_string(App->physics->balls.at(App->player->listRedP.at(App->player->currentRed)->playerWeapon).vx);
			const char* vx = velx.c_str();
			App->renderer->BlitText(60, 150, App->renderer->blueFont, vx);

			string vely = std::to_string(App->physics->balls.at(App->player->listRedP.at(App->player->currentRed)->playerWeapon).vy);
			const char* vy = vely.c_str();
			App->renderer->BlitText(60, 160, App->renderer->blueFont, vy);

			string accx = std::to_string(App->physics->balls.at(App->player->listRedP.at(App->player->currentRed)->playerWeapon).ax);
			const char* ax = accx.c_str();
			App->renderer->BlitText(60, 170, App->renderer->blueFont, ax);

			string accy = std::to_string(App->physics->balls.at(App->player->listRedP.at(App->player->currentRed)->playerWeapon).ay);
			const char* ay = accy.c_str();
			App->renderer->BlitText(60, 180, App->renderer->blueFont, ay);

		}
	}
}