#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"
#include "p2List.h"
#include "ModuleInput.h"
#include "Application.h"

#include "SString.h"

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

	rect.x = 200;
	rect.y = 200;
	rect.w = 100;
	rect.h = 100;

	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :" };
	App->renderer->blueFont = App->renderer->LoadFont("Fonts/FuenteAzulClaro.png", lookupTable, 1, 38); // 1 = rows 39 = columns
	App->renderer->greenFont = App->renderer->LoadFont("Fonts/FuenteVerde.png", lookupTable, 1, 38); // 1 = rows 39 = columns
	
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	SString title("%s | Current FPS: %f DeltaTime: %f  Expected FPS: %i, DeltaTime: %i", TITLE, 1000 / App->dt, App->dt, 1000 / App->time, App->time);
	App->window->SetTitle(title.GetString());

	Debug();

	App->renderer->DrawQuad(rect, 0, 255, 255, 255); 
	App->renderer->DrawCircle(x, y, rad, 0, 255, 255, 255); 

	App->renderer->BlitText(100, 100, App->renderer->blueFont, "A");

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Debug() {


	//"Nomenclatura": Bools con 1,2,3...,0 , multiple opcion con F1,F2...,F12

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && App->physics->debug==true)
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
}