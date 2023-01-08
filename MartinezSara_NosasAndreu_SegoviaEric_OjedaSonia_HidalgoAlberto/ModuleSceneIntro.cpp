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

	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :.-" };
	App->renderer->blueFont = App->renderer->LoadFont("Fonts/FuenteAzulClaro.png", lookupTable, 1, 40); // 1 = rows 39 = columns
	App->renderer->greenFont = App->renderer->LoadFont("Fonts/FuenteVerde.png", lookupTable, 1, 40); // 1 = rows 39 = columns
	endGame = false;
	frames = 60;

	//Set all events to 0
	for (size_t i = 0; i < NUMEVENTS; i++)
	{
		eventCounter[i] = 0;
	}

	fixCutre = false;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	
	//Set all events to 0
	for (size_t i = 0; i < NUMEVENTS; i++)
	{
		eventCounter[i] = 0;
	}

	App->renderer->UnLoadFont(App->renderer->blueFont);
	App->renderer->UnLoadFont(App->renderer->greenFont);
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	SString title("%s | Current FPS: %f DeltaTime: %f  Expected FPS: %i, DeltaTime: %i", TITLE, 1000 / App->dt, App->dt, 1000 / App->time, App->time);
	App->window->SetTitle(title.GetString());

	(App->physics->debug) ? Debug() : App->renderer->BlitText(10, 10, App->renderer->blueFont, "PRESS F1 TO ACTIVATE DEBUG");

	//RANDOM EVENT GENERATOR
	int random = rand() % 11+1;

	if (random<=eventProbability* eventProbability)
	{
		RandomEvent();
	}
	

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
			if (winaudio == true) {
				App->audio->PlayFx(App->player->audiowin);
				winaudio = false; 
			}
			break;

		case 1:
			LOG("BLUE team wins");
			App->renderer->BlitText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, App->renderer->greenFont, "BLUE TEAM WINS");
			if (winaudio == true) {
				App->audio->PlayFx(App->player->audiowin);
				winaudio = false;
			}
			break;
		default:
			break;
		}
		
		App->renderer->BlitText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, App->renderer->blueFont, "PRESS ENTER TO PLAY AGAIN");
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) 
		{ App->player->CleanUp(); }
	}
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Debug() {

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		//Reset water
		App->physics->water.x = 0; // [m]
		App->physics->water.y = PIXEL_TO_METERS(SCREEN_HEIGHT); // [m]
		App->physics->water.w = PIXEL_TO_METERS(SCREEN_WIDTH); // [m]
		App->physics->water.h = 5.0f; // [m]
		App->physics->water.density = 50.0f; // [kg/m^3]
		App->physics->water.vx = -1.0f; // [m/s]
		App->physics->water.vy = 0.0f; // [m/s]

		//Reset wind
		App->physics->atmosphere.windx = 5.0f; // [m/s]
		App->physics->atmosphere.windy = 0.0f; // [m/s]
		App->physics->atmosphere.density = 1.0f; // [kg/m^3]
	}

	float posX = App->input->GetMouseX();
	float posY = App->input->GetMouseY();

	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) 
	{ App->player->CreateWeapon(posX, posY, 1, 1, 1, ObjectType::MISSILE); }

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) 
	{ App->player->CreateWeapon(posX, posY, 1, 1, 1, ObjectType::GUN); }

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) 
	{ App->player->CreateWeapon(posX, posY, 1, 1, 1, ObjectType::GRENADE); }



	// "Nomenclatura": Bools con 1,2,3...,0 , multiple opcion con F1,F2...,F12
	
	// Integration methods
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->physics->method = integrationMethods::BACKWARDS_EULER;
		LOG("Backwards Euler");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->physics->method = integrationMethods::FORWARDS_EULER;
		LOG("Forwards Euler");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->physics->method = integrationMethods::VERLET;
		LOG("Verlet");
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

	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		App->timeControl = DeltaTimeControl::FIXED_DELTATIME_DELAY;
		LOG("Fixed delta time with delay");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		App->timeControl = DeltaTimeControl::VARIABLE_DELTATIME;
		LOG("Variable delta time");
	}

	// Options []
	else if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[0] = !App->physics->options[0];
	}

	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[1] = !App->physics->options[1]; // Gravity enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[2] = !App->physics->options[2]; // Buoyancy enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[3] = !App->physics->options[3]; // Hidro enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[4] = !App->physics->options[4]; // Aero enabler
	}

	else if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[5] = !App->physics->options[5]; // Allow wind changes
		if (App->physics->options[6]) { App->physics->options[6] = !App->physics->options[6]; }
	}
	else if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[6] = !App->physics->options[6]; // Allow current changes
		if (App->physics->options[5]) { App->physics->options[5] = !App->physics->options[5]; }
	}

	// FPS Change
	else if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE)
	{
		App->physics->options[7] = !App->physics->options[7];//allow fps changes
		LOG("fps change enables");
	}

	if (App->physics->options[5])
	{
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) { App->physics->atmosphere.windx += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) { App->physics->atmosphere.windx -= 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) { App->physics->atmosphere.windy += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) { App->physics->atmosphere.windy -= 0.1F; }
	}

	if (App->physics->options[6])
	{
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) { App->physics->water.vx += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) { App->physics->water.vx -= 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) { App->physics->water.h += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) { App->physics->water.h -= 0.1F; }
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

	//renderParameters();
	(!App->player->playerTurn) ? renderParameters(App->player->listBlueP, App->player->currentBlue) : renderParameters(App->player->listRedP, App->player->currentRed);
}


void ModuleSceneIntro::renderParameters(std::vector<Worm*> list, int current)
{
	// LADO IZQUIERDO
	App->renderer->BlitText(10, 10, App->renderer->blueFont, "DEBUG MODE ACTIVE:");

	if (App->physics->options[0]) { App->renderer->BlitText(26, 20, App->renderer->blueFont, "1: SHOW OR HIDE WIND AND CURRENT ON "); }
	if (!App->physics->options[0]) { App->renderer->BlitText(26, 20, App->renderer->blueFont, "1: SHOW OR HIDE WIND AND CURRENT OFF"); }
	if (App->physics->options[1]) { App->renderer->BlitText(26, 30, App->renderer->blueFont, "2: ENABLE OR DISABLE GRAVITY ON"); }
	if (!App->physics->options[1]) { App->renderer->BlitText(26, 30, App->renderer->blueFont, "2: ENABLE OR DISABLE GRAVITY OFF"); }
	if (App->physics->options[2]) { App->renderer->BlitText(26, 40, App->renderer->blueFont, "3: ENABLE OR DISABLE BUOYANCY ON"); }
	if (!App->physics->options[2]) { App->renderer->BlitText(26, 40, App->renderer->blueFont, "3: ENABLE OR DISABLE BUOYANCY OFF"); }
	if (App->physics->options[3]) { App->renderer->BlitText(26, 50, App->renderer->blueFont, "4: ENABLE OR DISABLE HYDRO DRAG ON"); }
	if (!App->physics->options[3]) { App->renderer->BlitText(26, 50, App->renderer->blueFont, "4: ENABLE OR DISABLE HYDRO DRAG OFF"); }
	if (App->physics->options[4]) { App->renderer->BlitText(26, 60, App->renderer->blueFont, "5: ENABLE OR DISABLE AERO DRAG ON"); }
	if (!App->physics->options[4]) { App->renderer->BlitText(26, 60, App->renderer->blueFont, "5: ENABLE OR DISABLE AERO DRAG OFF"); }
	if (App->physics->options[5]) { App->renderer->BlitText(26, 70, App->renderer->blueFont, "6: CHANGE WIND VELOCITY USING KEYS ON"); }
	if (!App->physics->options[5]) { App->renderer->BlitText(26, 70, App->renderer->blueFont, "6: CHANGE WIND VELOCITY USING KEYS OFF"); }
	if (App->physics->options[6]) { App->renderer->BlitText(26, 80, App->renderer->blueFont, "7: CHANGE CURRENT VELOCITY USING KEYS ON"); }
	if (!App->physics->options[6]) { App->renderer->BlitText(26, 80, App->renderer->blueFont, "7: CHANGE CURRENT VELOCITY USING KEYS OFF"); }

	// LADO CENTRAL 
	App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 20, App->renderer->blueFont, "B:CREATE BALL ");
	App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 30, App->renderer->blueFont, "V:CREATE BALL ");
	App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 40, App->renderer->blueFont, "C:CREATE BALL ");
	App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 50, App->renderer->blueFont, "M:INCREASE ALL BALLS RADIUS ");
	App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 60, App->renderer->blueFont, "N:REDUCE ALL BALLS RADIUS ");


	// LADO DERECHO PANTALLA

	// Integration method
	App->renderer->BlitText(SCREEN_WIDTH - 38 * 8, 20, App->renderer->blueFont, "INTEGRATION METHOD: ");
	switch (App->physics->method)
	{
	case integrationMethods::BACKWARDS_EULER:
		App->renderer->BlitText(SCREEN_WIDTH - 19 * 8, 16, App->renderer->greenFont, "BACKWARDS");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 40, App->renderer->blueFont, "F3: CHANGE TO FOWARDS EULER ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 50, App->renderer->blueFont, "F4: CHANGE TO VERLET ");
		break;
	case integrationMethods::FORWARDS_EULER:
		App->renderer->BlitText(SCREEN_WIDTH - 19 * 8, 16, App->renderer->greenFont, "FOWARDS");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 40, App->renderer->blueFont, "F2: CHANGE TO BACWARDS EULER ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 50, App->renderer->blueFont, "F4: CHANGE TO VERLET ");
		break;
	case integrationMethods::VERLET:
		App->renderer->BlitText(SCREEN_WIDTH - 19 * 8, 16, App->renderer->greenFont, "VERLET");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 40, App->renderer->blueFont, "F2: CHANGE TO BACWARDS EULER ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 50, App->renderer->blueFont, "F3: CHANGE TO FOWARDS EULER ");
		break;
	default:
		break;
	}

	// Movement type
	App->renderer->BlitText(SCREEN_WIDTH - 38 * 8, 70, App->renderer->blueFont, "PLAYER MOVEMENT MODE: ");
	switch (App->player->movement)
	{
	case MovementType::APPLY_FORCE:
		App->renderer->BlitText(SCREEN_WIDTH - 17 * 8, 66, App->renderer->greenFont, "FORCE");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 90, App->renderer->blueFont, "F6: CHANGE TO VELOCITY MOVEMENT ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 100, App->renderer->blueFont, "F7: CHANGE TO POSITION MOVEMENT ");
		break;
	case MovementType::APPLY_VELOCITY:
		App->renderer->BlitText(SCREEN_WIDTH - 17 * 8, 66, App->renderer->greenFont, "VELOCITY");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 90, App->renderer->blueFont, "F5: CHANGE TO FORCE MOVEMENT ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 100, App->renderer->blueFont, "F7: CHANGE TO POSITION MOVEMENT ");
		break;
	case MovementType::CHANGE_POSITION:
		App->renderer->BlitText(SCREEN_WIDTH - 17 * 8, 66, App->renderer->greenFont, "POSITION");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 90, App->renderer->blueFont, "F5: CHANGE TO FORCE MOVEMENT ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 100, App->renderer->blueFont, "F6: CHANGE TO VELOCITY MOVEMENT ");
		break;
	default:
		break;
	}

	// Delta time
	App->renderer->BlitText(SCREEN_WIDTH - 38 * 8, 120, App->renderer->blueFont, "DELTA TIME MODE: ");
	switch (App->timeControl)
	{
	case DeltaTimeControl::FIXED_DELTATIME:
		App->renderer->BlitText(SCREEN_WIDTH - 22 * 8, 116, App->renderer->greenFont, "FIXED");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 140, App->renderer->blueFont, "F9: CHANGE TO FIXED DELAY ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 150, App->renderer->blueFont, "F10: CHANGE TO VARIABLE ");
		break;
	case DeltaTimeControl::FIXED_DELTATIME_DELAY:
		App->renderer->BlitText(SCREEN_WIDTH - 22 * 8, 116, App->renderer->greenFont, "FIXED DELAY");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 140, App->renderer->blueFont, "F8: CHANGE TO FIXED ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 150, App->renderer->blueFont, "F10: CHANGE TO VARIABLE ");
		break;
	case DeltaTimeControl::VARIABLE_DELTATIME:
		App->renderer->BlitText(SCREEN_WIDTH - 22 * 8, 116, App->renderer->greenFont, "VARIABLE");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 140, App->renderer->blueFont, "F8: CHANGE TO FIXED  ");
		App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 150, App->renderer->blueFont, "F9: CHANGE TO FIXED DELAY ");
		break;
	default:
		break;
	}


	App->renderer->BlitText(10, 110, App->renderer->blueFont, "STATUS:"); // Valores fps, valores bala/player, etc...
	App->renderer->BlitText(26, 120, App->renderer->blueFont, "FPS:");
	string f_num = std::to_string(frames);
	const char* f = f_num.c_str();

	App->renderer->BlitText(60, 120, App->renderer->blueFont, f);


	App->renderer->BlitText(26, 130, App->renderer->blueFont, "POSITION X: ");
	App->renderer->BlitText(26, 140, App->renderer->blueFont, "POSITION Y: ");
	App->renderer->BlitText(26, 150, App->renderer->blueFont, "VELOCITY X: ");
	App->renderer->BlitText(26, 160, App->renderer->blueFont, "VELOCITY Y: ");
	App->renderer->BlitText(26, 170, App->renderer->blueFont, "ACCELERATION X: ");
	App->renderer->BlitText(26, 180, App->renderer->blueFont, "ACCELERATION Y: ");


	int t = App->physics->balls.size();


	if (list.at(current)->playerWeapon < t && list.at(current)->playerWeapon != -1)
	{
		string posx = std::to_string(App->physics->balls.at(list.at(current)->playerWeapon).x);
		const char* x = posx.c_str();
		App->renderer->BlitText(120, 130, App->renderer->blueFont, x);

		string posy = std::to_string(App->physics->balls.at(list.at(current)->playerWeapon).y);
		const char* y = posy.c_str();
		App->renderer->BlitText(120, 140, App->renderer->blueFont, y);

		string velx = std::to_string(App->physics->balls.at(list.at(current)->playerWeapon).vx);
		const char* vx = velx.c_str();
		App->renderer->BlitText(120, 150, App->renderer->blueFont, vx);

		string vely = std::to_string(App->physics->balls.at(list.at(current)->playerWeapon).vy);
		const char* vy = vely.c_str();
		App->renderer->BlitText(120, 160, App->renderer->blueFont, vy);

		string accx = std::to_string(App->physics->balls.at(list.at(current)->playerWeapon).ax);
		const char* ax = accx.c_str();
		App->renderer->BlitText(150, 170, App->renderer->blueFont, ax);

		string accy = std::to_string(App->physics->balls.at(list.at(current)->playerWeapon).ay);
		const char* ay = accy.c_str();
		App->renderer->BlitText(150, 180, App->renderer->blueFont, ay);
	}
}


void ModuleSceneIntro::RandomEvent()
{
	int max = NUMEVENTS;
	int num = rand() % max;


	switch (num)
	{
	case ModuleSceneIntro::WATERRISE:
		if (eventCounter[WATERRISE] <= (eventCounter[WATERDESCEND] + 4))
		{
			eventCounter[WATERRISE]++;
			actualEvent = WATERRISE;
			App->physics->water.h += 2.0f;
			eventProbability = 0;
			
		}
		break;
	case ModuleSceneIntro::WATERDESCEND:
		if (eventCounter[WATERRISE] + 3 >= eventCounter[WATERDESCEND])
		{
			eventCounter[WATERDESCEND]++;
			eventProbability = 0;
			App->physics->water.h -= 2.0f;
 			
		}
		break;
	case ModuleSceneIntro::NOWIND:
		if (abs(App->physics->atmosphere.windx)>1.0f)
		{
			eventCounter[NOWIND]++;
			eventProbability = 0;
			App->physics->atmosphere.windx = 0.1f*(App->physics->atmosphere.windx/abs(App->physics->atmosphere.windx)); //Asiganr un signo al viento
		}
		//RandomEvent();
		break;
	case ModuleSceneIntro::CHANGEWIND:
		if ((eventCounter[MOREWIND]+ eventCounter[LESSFWIND])/2 >= eventCounter[CHANGEWIND])
		{
		
		eventCounter[CHANGEWIND]++;
		eventProbability = 0;
		App->physics->atmosphere.windx *= -1.0f; //Asiganr un signo al viento
		}
		break;
	case ModuleSceneIntro::MOREWIND:
		if (eventCounter[MOREWIND] <= (eventCounter[LESSFWIND] + 3))
		{
			eventCounter[NOWIND]++;
			eventProbability = 0;
			App->physics->atmosphere.windx += 5.0f * (App->physics->atmosphere.windx / abs(App->physics->atmosphere.windx)); //Asiganr un signo al viento
		}
		//RandomEvent();
		break;
	case ModuleSceneIntro::LESSFWIND:
		if (eventCounter[MOREWIND]+3 >= (eventCounter[LESSFWIND]))
		{
			eventCounter[NOWIND]++;
			eventProbability = 0;
			App->physics->atmosphere.windx -= 5.0f * (App->physics->atmosphere.windx / abs(App->physics->atmosphere.windx)); //Asiganr un signo al viento
		}
		//RandomEvent();
		break;
	case ModuleSceneIntro::NUMEVENTS:
		//RandomEvent();
		break;
	default:
		break;
	}
}