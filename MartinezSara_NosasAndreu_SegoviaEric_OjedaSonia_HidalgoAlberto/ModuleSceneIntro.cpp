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

	//De momento lo dejo aqui
	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :" };
	blueFont = App->renderer->LoadFont("WormsQuokka/MartinezSara_NosasAndreu_SegoviaEric_OjedaSonia_HidalgoAlberto/Game/Fonts/FuenteAzulClaro.png", lookupTable, 1, 39); // 1 = rows 39 = columns
	

	//Object* worm = new Object(10, 28, 1, WormType::BLUE);
	//App->player->listBLUE->add(worm);

	//pBody* b1 = new pBody(500, 200, 100, 100);

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

	App->renderer->BlitText(100, 100, blueFont, "A");

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Debug() {

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->physics->method = integrationMethods::BACKWARDS_EULER;
		LOG("Backwards Euler");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->physics->method = integrationMethods::FORWARDS_EULER;
		LOG("Forwards Euler");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->physics->method = integrationMethods::VERLET;
		LOG("Verlet");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->physics->gravityIsEnabled = !App->physics->gravityIsEnabled;
	}

	else if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->physics->buoyancyIsEnabled = !App->physics->buoyancyIsEnabled;
	}

	else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->physics->hidroIsEnabled = !App->physics->hidroIsEnabled;
	}

	else if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		App->physics->aeroIsEnabled = !App->physics->aeroIsEnabled;
	}

	else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		App->player->movement = MovementType::APPLY_FORCE;
		LOG("Movement applying force");
	}

	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{

		App->player->movement = MovementType::APPLY_VELOCITY;
		LOG("Movement applying velocity");

	}

	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{

		App->player->movement = MovementType::CHANGE_POSITION;
		LOG("Movement changing position");

	}

	else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{

		App->timeControl = DeltaTimeControl::FIXED_DELTATIME;
		LOG("Fixed delta time");

	}

	else if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)//m'he quedat sense fs
	{

		App->timeControl = DeltaTimeControl::FIXED_DELTATIME_DELAY;
		LOG("Fixed delta time with delay");

	}

	else if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{

		App->timeControl = DeltaTimeControl::VARIABLE_DELTATIME;
		LOG("Variable delta time");

	}
}