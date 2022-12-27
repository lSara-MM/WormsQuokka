#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Module.h"
#include "Dummy.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"

enum class DeltaTimeControl
{

	FIXED_DELTATIME,
	FIXED_DELTATIME_DELAY,
	VARIABLE_DELTATIME

};

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModulePlayer* player;
	ModuleSceneIntro* scene_intro;
	ModulePhysics* physics;

	// time
	uint time = 1000 / 60;
	float dt;
	//int frame = 0;

private:

	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	DeltaTimeControl timeControl = DeltaTimeControl::VARIABLE_DELTATIME;

private:

	void AddModule(Module* mod);
};