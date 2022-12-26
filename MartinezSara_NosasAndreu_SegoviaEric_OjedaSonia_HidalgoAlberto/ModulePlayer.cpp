#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading players");

	// Blue team
	//Object* worm = new Object( 10, 28, 1, WormType::BLUE);
	//listBLUE->add(worm);

	//// Red team
	//worm = new Object(100, 28, 1, WormType::RED);
	//listRED->add(worm);

	//p2List_item <Object*>* item = listBLUE->getFirst();
	//for (item; item != nullptr; item = item->next) { listPlayers->add(item->data); }
	//item = listRED->getFirst();
	//for (item; item != nullptr; item = item->next) { listPlayers->add(item->data); }

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	p2List_item <Object*>* item = listPlayers->getFirst();
	for (item; item != nullptr; item = item->next)
	{
		if (player)
		{

		}
		else
		{

		}

		(item->data->type == WormType::BLUE) ?
			App->renderer->DrawCircle(item->data->posX, item->data->posY, item->data->body->radius, 0, 0, 255):
			App->renderer->DrawCircle(item->data->posX, item->data->posY, item->data->body->radius, 255, 0, 0);
	}

	if (listBLUE->count() == 0) { LOG("RED team wins"); }
	if (listRED->count() == 0) { LOG("BLUE team wins"); }


	return UPDATE_CONTINUE;
}

void ModulePlayer::controls(Object player)
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		if (player.body->x > SCREEN_WIDTH) { player.body->vx++; }
		//player.body->vx++;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
	{
		if (player.body->x < 0) { player.body->x--; }
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
	{
	
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
	{
	
	}
}

