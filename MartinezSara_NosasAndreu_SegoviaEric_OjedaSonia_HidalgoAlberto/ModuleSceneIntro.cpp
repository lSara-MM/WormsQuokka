#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"
#include "p2List.h"
#include "ModuleInput.h"
#include "Application.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	app = new Application();

	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	rect.x = 200;
	rect.y = 200;
	rect.w = 100;
	rect.h = 100;

	pbody* b1 = new pbody(500, 200, 100);

	//allBodies->add(*b1);

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

	App->renderer->DrawQuad(rect, 0, 255, 255, 255); 

	App->renderer->DrawCircle(x, y, 100, 0, 255, 255, 255); 
		
	//p2List_item<pbody>* bodies ;
	
	//for(bodies=allBodies->getFirst();bodies=bodies->next;bodies->next!=nullptr)
	//{
		//App->renderer->DrawCircle(bodies->data.pos.x,bodies->data.pos.y,bodies->data.radious, 255, 255, 255);
	//}
	

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		x += 10;
		LOG("X");
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) {
		x -= 10;
		LOG("-X");
	}


	return UPDATE_CONTINUE;
}
