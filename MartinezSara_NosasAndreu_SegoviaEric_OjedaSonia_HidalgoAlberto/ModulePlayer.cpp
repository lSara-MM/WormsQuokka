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
	posX = PIXEL_TO_METERS(100);
	posY = PIXEL_TO_METERS(200);
	radBody = 0.5f;

	playerBody = App->physics->CreateBall(posX, posY, radBody, 200.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.1f, 0.0, 0.0,true);

	movement = MovementType::APPLY_FORCE;

	LOG("Loading player");
	return true;
}

//Player control
update_status ModulePlayer::Update()
{
	

	switch (movement)
	{
	case MovementType::APPLY_FORCE:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			App->physics->balls.at(playerBody).ApplyForce(1500.0f, 0.0f);

		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			App->physics->balls.at(playerBody).ApplyForce(-1500.0f, 0.0f);

		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A)==KEY_UP) {

	
			App->physics->balls.at(playerBody).ApplyForce(0.0F, 0.0f);

		}

		break;
	case MovementType::APPLY_VELOCITY:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			App->physics->balls.at(playerBody).SetVelocity(15.0f, 0.0f);

		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			App->physics->balls.at(playerBody).SetVelocity(-15.0f, 0.0f);

		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {


			App->physics->balls.at(playerBody).SetVelocity(0.0F, 0.0f);

		}

		break;
	case MovementType::CHANGE_POSITION:

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			App->physics->balls.at(playerBody).AddPosition(15.0f, 0.0f);

		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			App->physics->balls.at(playerBody).AddPosition(-15.0f, 0.0f);

		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {


			App->physics->balls.at(playerBody).AddPosition(0.0F, 0.0f);

		}

		break;
	default:
		break;
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

	return true;
}


