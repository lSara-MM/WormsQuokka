#pragma once
#include "Module.h"
#include "Globals.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	class pbody
	{
	public:
		pbody();
		pbody(int x, int y, int rad, bool dynamic = true) 
		{
			
		}
		~pbody();
		
		bool isDynamic;
		iPoint pos;
		int radious;

	private:

	};

	

private:

	bool debug;
};