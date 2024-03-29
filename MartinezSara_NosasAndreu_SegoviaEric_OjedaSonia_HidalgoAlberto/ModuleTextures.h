#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL\include\SDL.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures(Application* app, bool start_enabled = true);
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	SDL_Texture* const Load(const char* path);
	void Unload(SDL_Texture* texture);

	// Retrieve size of a texture by modifying parameters with and height
	void GetTextureSize(const SDL_Texture* texture, uint& width, uint& height) const;

public:
	p2List<SDL_Texture*> textures;
};