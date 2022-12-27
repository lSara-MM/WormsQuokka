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
	// Blue team
	posX = PIXEL_TO_METERS(100);
	posY = PIXEL_TO_METERS(200);
	radBody = 1.0f;

	int num = App->physics->CreateBall(posX, posY, radBody, WormType::BLUE, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f);
	movement = MovementType::APPLY_FORCE;
	//listPlayers.emplace_back(num);
	listBlueP.emplace_back(num);
	

	// Red team
	posX = PIXEL_TO_METERS(600);
	posY = PIXEL_TO_METERS(350);

	num = App->physics->CreateBall(posX, posY, radBody, WormType::RED, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f);
	movement = MovementType::APPLY_FORCE;
	//listPlayers.emplace_back(num);
	listRedP.emplace_back(num);


	LOG("Loading player");


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

	playerTurn = true;
	return true;
}

//Player control
update_status ModulePlayer::Update()
{
	if (playerTurn)	// Blue turn
	{
		controls(listBlueP.front(), movement);
	}
	else // Red turn
	{
		controls(listRedP.front(), movement);
	}

	if (listBLUE.empty()) 
	{ LOG("RED team wins") }

	if (listRED.empty()) 
	{ LOG("BLUE team wins") }

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

	return true;
}

void ModulePlayer::controls(int player, MovementType move)
{
	switch (move)
	{
	case MovementType::APPLY_FORCE:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{ App->physics->balls.at(player).ApplyForce(1500.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{ App->physics->balls.at(player).ApplyForce(-1500.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) 
		{ App->physics->balls.at(player).ApplyForce(0.0F, 0.0f); }

		break;

	case MovementType::APPLY_VELOCITY:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{ App->physics->balls.at(player).SetVelocity(15.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{ App->physics->balls.at(player).SetVelocity(-15.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) 
		{ App->physics->balls.at(player).SetVelocity(0.0F, 0.0f); }

		break;

	case MovementType::CHANGE_POSITION:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{ App->physics->balls.at(player).AddPosition(15.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{ App->physics->balls.at(player).AddPosition(-15.0f, 0.0f); }

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{ App->physics->balls.at(player).AddPosition(0.0F, 0.0f); }

		break;
	default:
		break;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		playerTurn = !playerTurn;
	}

}


//// Update: draw background
//update_status ModulePlayer::Update()
//{
//	p2List_item <Object*>* item = listPlayers->getFirst();
//	for (item; item != nullptr; item = item->next)
//	{
//		if (player)
//		{
//
//		}
//		else
//		{
//
//		}
//
//		(item->data->type == WormType::BLUE) ?
//			App->renderer->DrawCircle(item->data->posX, item->data->posY, item->data->body->radius, 0, 0, 255):
//			App->renderer->DrawCircle(item->data->posX, item->data->posY, item->data->body->radius, 255, 0, 0);
//	}
//
//	if (listBLUE->count() == 0) { LOG("RED team wins"); }
//	if (listRED->count() == 0) { LOG("BLUE team wins"); }
//
//
//	return UPDATE_CONTINUE;
//}
//
//void ModulePlayer::controls(Object player)
//{
//	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
//	{
//		if (player.body->x > SCREEN_WIDTH) { player.body->vx++; }
//		//player.body->vx++;
//	}
//
//	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
//	{
//		if (player.body->x < 0) { player.body->x--; }
//	}
//
//	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
//	{
//	
//	}
//
//	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
//	{
//	
//	}
//}


