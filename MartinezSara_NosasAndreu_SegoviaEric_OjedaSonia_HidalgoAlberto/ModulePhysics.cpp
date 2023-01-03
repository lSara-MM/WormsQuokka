#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"
#include <cmath>

#define PI 3.14159265f

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	// Create ground
	ground = Ground();
	ground.x = 0.0f; // [m]
	ground.y = PIXEL_TO_METERS(SCREEN_HEIGHT); // [m]
	ground.w = 13.0f; // [m]
	ground.h = 20.0f; // [m]

	grounds.emplace_back(ground);

	ground1 = Ground();
	ground1.x = 28.0f; // [m]
	ground1.y = PIXEL_TO_METERS(SCREEN_HEIGHT); // [m]
	ground1.w = 10.0f; // [m]
	ground1.h = 6.0f; // [m]

	grounds.emplace_back(ground1);

	ground2 = Ground();
	ground2.x = 35.0f; // [m]
	ground2.y = PIXEL_TO_METERS(SCREEN_HEIGHT); // [m]
	ground2.w = 30.0f; // [m]
	ground2.h = 10.0f; // [m]

	grounds.emplace_back(ground2);

	// Create Water
	water = Water();
	water.x = ground.x + ground.w; // Start where ground ends [m]
	water.y = PIXEL_TO_METERS(SCREEN_HEIGHT); // [m]
	water.w = 30.0f; // [m]
	water.h = 5.0f; // [m]
	water.density = 50.0f; // [kg/m^3]
	water.vx = -1.0f; // [m/s]
	water.vy = 0.0f; // [m/s]

	// Create atmosphere
	atmosphere = Atmosphere();
	atmosphere.windx = 5.0f; // [m/s]
	atmosphere.windy = 0.0f; // [m/s]
	atmosphere.density = 1.0f; // [kg/m^3]

	PhysBall ball = PhysBall();
	// Set static properties of the ball
	ball.mass = 10.0f; // [kg]
	ball.surface = 1.0f; // [m^2]
	ball.radius = 0.5f; // [m]
	ball.cd = 0.4f; // [-]
	ball.cl = 1.2f; // [-]
	ball.b = 10.0f; // [...]
	ball.coef_friction = 0.9f; // [-]
	ball.coef_restitution = 0.8f; // [-]

	// Set initial position and velocity of the ball
	ball.x = 2.0f ;
	ball.y = (ground.y - ground.h) + 2.0f;
	ball.vx = 5.0f;
	ball.vy = 10.0f;

	// Add ball to the collection
	//balls.emplace_back(ball);


	//debug
	options[0] = true; //Show wind
	options[1] = true; //Gravity
	options[2] = true; //Bouyancy
	options[3] = true; //Hidro Drag
	options[4] = true; //Aero Drag
	options[5] = false; //Allow change wind with keys
	options[6] = false; //Allow change current with keys
	options[7] = false;
	
	method = integrationMethods::VERLET;

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	if(options[5])
	{ 
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) { atmosphere.windx += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) { atmosphere.windx -= 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) { atmosphere.windy += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) { atmosphere.windy -= 0.1F; }


	}

	if (options[6])
	{
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) { water.vx += 0.1F; }
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) { water.vx -= 0.1F; }
		//if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) { water.vy += 0.1F; }
		//if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) { water.vy -= 0.1F; }
	}

	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	{
		float posX = PIXEL_TO_METERS(App->input->GetMouseX());
		float posY = PIXEL_TO_METERS(App->input->GetMouseY());
		PhysBall ball2 = PhysBall(posX, posY, 0.5f, ObjectType::OTHER, 10.0f, 0.0f, 20.0f, 1.0f, 1.2f, 0.4f, 10.0f, 0.9f, 0.8f);

		// Add ball to the collection
		balls.emplace_back(ball2);
	}

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		float posX = App->input->GetMouseX();
		float posY = App->input->GetMouseY();
		App->player->CreateWeapon(posX, posY, 1, 1,1, ObjectType::GUN);//change force
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		float posX = App->input->GetMouseX();
		float posY = App->input->GetMouseY();
		App->player->CreateWeapon(posX, posY, 1, 1,1, ObjectType::GRENADE);
	}

	// Process all balls in the scenario
	for (auto& ball : balls)
	{
		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) { ball.radius += 0.1f; }
		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) { ball.radius -= 0.1f; }

		for (auto& ground : grounds) 
		{
			// Skip ball if physics not enabled
			if (!ball.physics_enabled)
			{
				continue;
			}

			// Step #0: Clear old values
			// ----------------------------------------------------------------------------------------

			// Reset total acceleration and total accumulated force of the ball
			ball.fx = ball.fy = 0.0f;
			ball.ax = ball.ay = 0.0f;

			// Step #1: Compute forces
			// ----------------------------------------------------------------------------------------

			// Gravity force
			if (options[1] = true)
			{
				float fgx = ball.mass * 0.0f;
				float fgy = ball.mass * 10.0f; // Let's assume gravity is constant and downwards

				ball.fx += fgx; ball.fy += fgy; // Add this force to ball's total force
			}

			//for(int i = 0; i < balls.size(); i++){	
			//	for (int j = 0; j < balls.size(); j++) {
			//		if (balls.at(j).x == balls.at(i).x)
			//		{
			//			balls.at(j).x += balls.at(j).radius;

			//			balls.at(j).vx = balls.at(i).vx;
			//			balls.at(i).vx = balls.at(j).vx;

			//			// FUYM non-elasticity
			//			balls.at(j).vx *= balls.at(j).coef_friction;
			//		}
			//		if (((balls.at(j).x + ball.radius) == balls.at(i).x)) {
			//			balls.at(j).x -= balls.at(j).radius;

			//			balls.at(j).vx = balls.at(i).vx;
			//			balls.at(i).vx = balls.at(j).vx;
			//		}
			//	}
			//}

			// Aerodynamic Drag force (only when not in water)
			if (!is_colliding_with_water(ball, water) && options[4])
			{
				float fdx = 0.0f; float fdy = 0.0f;
				compute_aerodynamic_drag(fdx, fdy, ball, atmosphere);
				ball.fx += fdx; ball.fy += fdy; // Add this force to ball's total force
			}

			// Hydrodynamic forces (only when in water)
			if (is_colliding_with_water(ball, water))
			{
				// Hydrodynamic Drag force
				if (options[3])
				{
					float fhdx = 0.0f; float fhdy = 0.0f;
					compute_hydrodynamic_drag(fhdx, fhdy, ball, water);
					ball.fx += fhdx; ball.fy += fhdy; // Add this force to ball's total force
				}

				// Hydrodynamic Buoyancy force
				if (options[2])
				{
					float fhbx = 0.0f; float fhby = 0.0f;
					compute_hydrodynamic_buoyancy(fhbx, fhby, ball, water);

					ball.fx += fhbx; ball.fy += fhby; // Add this force to ball's total force
				}
			
			}

			int t = balls.size(); 

			for (int i = 0; i < t; i++) {
				for (int j = 0; j < t; j++) {

					/*if(balls.at(i).x + balls.at(i).radius >= balls.at(j).x + balls.at(j).radius && balls.at(i).x + balls.at(i).radius >= balls.at(j).x + balls.at(j).radius*/
					if (!balls.at(i).physics_enabled || !balls.at(j).physics_enabled)
					{
						continue;
					}
					if (check_collision_circle_circle(balls.at(i).x, balls.at(i).y, balls.at(i).radius, balls.at(j).x, balls.at(j).y, balls.at(j).radius) == true && i != j) {

						if (balls.at(j).player != balls.at(i).player) {
							losehp = true; 
							 
							if (balls.at(j).type == ObjectType::GUN && balls.at(i).type == ObjectType::RED) {

								bodyHP = i; 
								typeW = ObjectType::GUN;
								typeP =ObjectType::RED;
							}

							if (balls.at(j).type == ObjectType::GRENADE && balls.at(i).type == ObjectType::RED) {

								bodyHP = i;
								typeW = ObjectType::GRENADE;
								typeP = ObjectType::RED;
							}

							if (balls.at(i).type == ObjectType::GUN && balls.at(j).type == ObjectType::RED) {

								bodyHP = j;
								typeW = ObjectType::GUN;
								typeP = ObjectType::RED;
							}

							if (balls.at(i).type == ObjectType::GRENADE && balls.at(j).type == ObjectType::RED) {

								bodyHP = j;
								typeW = ObjectType::GRENADE;
								typeP = ObjectType::RED;
							}


							if (balls.at(i).type == ObjectType::GUN && balls.at(j).type == ObjectType::BLUE) {
								LOG("bodyHP %d", bodyHP); 
								bodyHP = j;
								typeW = ObjectType::GUN;
								typeP = ObjectType::BLUE;
							}

							if (balls.at(i).type == ObjectType::GRENADE && balls.at(j).type == ObjectType::BLUE) {

								bodyHP = j;
								typeW = ObjectType::GRENADE;
								typeP = ObjectType::BLUE;
							}

							if (balls.at(j).type == ObjectType::GUN && balls.at(i).type == ObjectType::BLUE) {
								LOG("bodyHP %d", bodyHP);
								bodyHP = i;
								typeW = ObjectType::GUN;
								typeP = ObjectType::BLUE;
							}

							if (balls.at(j).type == ObjectType::GRENADE && balls.at(i).type == ObjectType::BLUE) {

								bodyHP = i;
								typeW = ObjectType::GRENADE;
								typeP = ObjectType::BLUE;
							}
						}
				

						// Sacamos el vector desde el centro de i hacia j
						float preXj = balls.at(j).x - balls.at(i).x;
						float preYj = balls.at(j).y - balls.at(i).y;

						//Modulo del vector
						float module = modulus(preXj, preYj);

						//Hacemos el vector unitario
						preXj /= module;
						preYj /= module;

						float dist = balls.at(i).radius + balls.at(j).radius;

						balls.at(j).x = balls.at(i).x + preXj * dist;
						balls.at(j).y = balls.at(i).y + preYj * dist;

						//Hacemos lo mismo de j a i
						float preXi = balls.at(i).x - balls.at(j).x;
						float preYi = balls.at(i).y - balls.at(j).y;

						//Modulo del vector
						module = modulus(preXi, preYi);

						//Hacemos el vector unitario
						preXi /= module;
						preYi /= module;

						balls.at(i).x = balls.at(j).x + preXi * dist;
						balls.at(i).y = balls.at(j).y + preYi * dist;

						/*if (App->input->GetKey(SDL_SCANCODE_5) == KEY_REPEAT)
						{
							balls.at(i).vx = -balls.at(j).vx;
							balls.at(i).vy = -balls.at(j).vy;
							balls.at(j).vy = -balls.at(i).vy;
							balls.at(j).vx = -balls.at(i).vx;
						}*/


						//if (App->input->GetKey(SDL_SCANCODE_P) == KEY_IDLE)
						{
							//Velocidad en X
							float auxX = 0;
							if (abs(balls.at(j).vx) <= 0.2f) { balls.at(j).vx = 0; }
							if (abs(balls.at(i).vx) <= 0.2f) { balls.at(i).vx = 0; }
							auxX = balls.at(j).vx;
							balls.at(j).vx = balls.at(i).vx;
							balls.at(i).vx = auxX;


							//Velocidad en Y
							if (abs(balls.at(j).vy) <= 0.2f) { balls.at(j).vy = 0; }
							if (abs(balls.at(i).vy) <= 0.2f) { balls.at(i).vy = 0; }
							float auxY = 0;
							auxY = balls.at(j).vy;
							balls.at(j).vy = balls.at(i).vy;
							balls.at(i).vy = auxY;
						}
					}
				}
			}
			/*if (check_collision_circle_circle(ball.x, ball.y, ball.radius, ball.x, ball.y, ball.radius) == true) {

				ball.vx = -ball.vx; 
				ball.vy = -ball.vy; 
			}*/

			// Other forces
			// ...
			//Force added with function 
			ball.fx += ball.addedForceX;
			ball.fy += ball.addedForceY;

			// Step #2: 2nd Newton's Law
			// ----------------------------------------------------------------------------------------

			// SUM_Forces = mass * accel --> accel = SUM_Forces / mass
			ball.ax = ball.fx / ball.mass;
			ball.ay = ball.fy / ball.mass;

			// Step #3: Integrate --> from accel to new velocity & new position
			// ----------------------------------------------------------------------------------------
			//Update dt 
			dt_updated = App->dt / 1000;
			
			// Switch to determine which method of integration to use.(posar text en pantalla de quin mètode s'utilitza)
			switch (method)
			{
			case integrationMethods::BACKWARDS_EULER:

				integrator_velocity_Backwards_Euler(ball, dt_updated);

				break;
			case integrationMethods::FORWARDS_EULER:

				integrator_velocity_Forwards_Euler(ball, dt_updated);

				break;
			case integrationMethods::VERLET:
			
				integrator_velocity_verlet(ball, dt_updated);

				break;
			default:
				break;
			}
			
			// Step #4: solve collisions
			// ----------------------------------------------------------------------------------------

			if ((ball.x - ball.radius) <= 0)
			{
				// TP ball to ground surface
				ball.x += ball.radius;

				ball.vx = -ball.vx;

				// FUYM non-elasticity
				ball.vx *= ball.coef_friction;
				ball.vy *= ball.coef_restitution;

			}

			if(ball.y  >= PIXEL_TO_METERS(SCREEN_HEIGHT))
			{
				// TP ball to ground surface
				ball.y -= ball.radius;

				ball.vy = -ball.vy;

				// FUYM non-elasticity
				ball.vx *= ball.coef_friction;
				ball.vy *= ball.coef_restitution;

			}

			if ((ball.x + ball.radius) >= PIXEL_TO_METERS(SCREEN_WIDTH))
			{
				//In case the ball gets out of the screen
				if ((ball.x) >= PIXEL_TO_METERS(SCREEN_WIDTH)) { ball.x = PIXEL_TO_METERS(SCREEN_WIDTH)-ball.radius; }

				ball.vx = -ball.vx;

				// FUYM non-elasticity
				ball.vx *= ball.coef_friction;
				ball.vy *= ball.coef_restitution;

			}

			// Solve collision between ball and ground
			if (is_colliding_with_ground(ball, ground))
			{
				if (ball.x >= (ground.x + ground.w))
				{
					ball.x = ground.x + ground.w + ball.radius;
					
					if (abs(ball.vx)<=0.2f){ball.vx = 0;} //FUYM, if speed is to slow stop moving (this is to stop the ball continuos bounce do to travesing the floor)

					ball.vx = -ball.vx;
					ball.vx *= ball.coef_friction;
					ball.vy *= ball.coef_restitution;
				}


				else if ((ball.x + (ball.radius + ball.radius)) <= ground.x)
				{
					ball.x = ground.x - ball.radius;
					if (abs(ball.vx) <= 0.2f) { ball.vx = 0; } //FUYM, if speed is to slow stop moving (this is to stop the ball continuos bounce do to travesing the floor)
					ball.vx = -ball.vx*1.2f;
					ball.vx *= ball.coef_friction;
					ball.vy *= ball.coef_restitution;
				}
				else
				{
					if (ball.y >= (ground.y - ground.h * 2))
					{
						// TP ball to ground surface
						ball.y = ground.y - ground.h - ball.radius;

						// Elastic bounce with ground
						if (abs(ball.vy) <= 0.4f) { ball.vy = 0; } //FUYM, if speed is to slow stop moving (this is to stop the ball continuos bounce do to travesing the floor)
						ball.vy = -ball.vy;

						// FUYM non-elasticity
						ball.vx *= ball.coef_friction;
						ball.vy *= ball.coef_restitution;
					}
				}
			}
		}
	}
	// Continue game
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	//ACTIVAR O DESACTIVAR 
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debug = !debug;
	}


	// Colors
	int color_r, color_g, color_b;

	// Draw water
	color_r = 0; color_g = 0; color_b = 255;
	App->renderer->DrawQuad(water.pixels(), color_r, color_g, color_b);

	//Draw wind
	color_r = 0; color_g = 255; color_b = 125;
	
	++iwind;
	if (options[0])
	{
		for(float i=-SCREEN_HEIGHT;i<=2*SCREEN_HEIGHT- water.pixels().y;i+= SCREEN_HEIGHT /10) //Pos y
		{
			for (float j = -SCREEN_WIDTH; j <= 2*SCREEN_WIDTH; j += SCREEN_WIDTH/10) //Pos x
			{		
				App->renderer->DrawLine(
					j+ iwind* atmosphere.windx * 4, //X0
					i + iwind * atmosphere.windy*4, //Y0
					j + (iwind +1) *atmosphere.windx*4 , //X1
					i +  (iwind +1) * atmosphere.windy*4, //Y1
					color_r, color_g, color_b, 100);
			}		
		}

		//Draw water currents
		//LOG("El agua es %d y su w es %d",water.pixels().x, water.pixels().w)
		for (int k = water.pixels().y+  water.pixels().h; k <= water.pixels().y; k -= water.pixels().h /4 ) //Pos y
		{
			for (int m = water.pixels().x; m <= water.pixels().x+ water.pixels().w; m += water.pixels().w / 5) //Pos x
			{
				App->renderer->DrawLine(
					m + iwind * water.vx*4 , //X0
					k + iwind * water.vy*4 , //Y0
					m + (iwind + 1) * water.vx*4 , //X1
					k + (iwind + 1) * water.vy*4 , //Y1
					125, 125, 255);
			}
		}
	}

	if (iwind >= 25) { iwind = 0; }
	

	// Draw ground
	for (auto& ground : grounds)
	{
		color_r = 0; color_g = 255; color_b = 0;
		App->renderer->DrawQuad(ground.pixels(), color_r, color_g, color_b);
	}

	if (!debug)
	{
		App->renderer->BlitText(10, 10, App->renderer->blueFont, "PRESS F1 TO ACTIVATE DEBUG");
	}

	if (debug)
	{
		//LADO IZQUIERDO
		App->renderer->BlitText(10, 10, App->renderer->blueFont, "DEBUG MODE ACTIVE:");
		
		if (options[0]){App->renderer->BlitText(26, 20, App->renderer->blueFont, "1: SHOW OR HIDE WIND AND CURRENT ON ");}
		if (!options[0]) { App->renderer->BlitText(26, 20, App->renderer->blueFont, "1: SHOW OR HIDE WIND AND CURRENT OFF"); }
		if (options[1]) { App->renderer->BlitText(26, 30, App->renderer->blueFont, "2: ENABLE OR DISABLE GRAVITY ON"); }
		if (!options[1]) { App->renderer->BlitText(26, 30, App->renderer->blueFont, "2: ENABLE OR DISABLE GRAVITY OFF"); }
		if (options[2]) { App->renderer->BlitText(26, 40, App->renderer->blueFont, "3: ENABLE OR DISABLE BUOYANCY ON"); }
		if (!options[2]) { App->renderer->BlitText(26, 40, App->renderer->blueFont, "3: ENABLE OR DISABLE BUOYANCY OFF"); }
		if (options[3]) { App->renderer->BlitText(26, 50, App->renderer->blueFont, "4: ENABLE OR DISABLE HYDRO DRAG ON"); }
		if (!options[3]) { App->renderer->BlitText(26, 50, App->renderer->blueFont, "4: ENABLE OR DISABLE HYDRO DRAG OFF"); }
		if (options[4]) { App->renderer->BlitText(26, 60, App->renderer->blueFont, "5: ENABLE OR DISABLE AERO DRAG ON"); }
		if (!options[4]) { App->renderer->BlitText(26, 60, App->renderer->blueFont, "5: ENABLE OR DISABLE AERO DRAG OFF"); }
		if (options[5]) { App->renderer->BlitText(26, 70, App->renderer->blueFont, "6: CHANGE WIND VELOCITY USING KEYS ON"); }
		if (!options[5]) { App->renderer->BlitText(26, 70, App->renderer->blueFont, "6: CHANGE WIND VELOCITY USING KEYS OFF"); }
		if (options[6]) { App->renderer->BlitText(26, 80, App->renderer->blueFont, "7: CHANGE CURRENT VELOCITY USING KEYS ON"); }
		if (!options[6]) { App->renderer->BlitText(26, 80, App->renderer->blueFont, "7: CHANGE CURRENT VELOCITY USING KEYS OFF"); }
		
		//LADO CENTRAL 
		App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 20, App->renderer->blueFont, "B:CREATE BALL ");
		App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 30, App->renderer->blueFont, "V:CREATE BALL ");
		App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 40, App->renderer->blueFont, "C:CREATE BALL ");
		App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 50, App->renderer->blueFont, "M:INCREASE ALL BALLS RADIUS ");
		App->renderer->BlitText(SCREEN_WIDTH - 80 * 8, 60, App->renderer->blueFont, "N:REDUCE ALL BALLS RADIUS ");


		//LADO DERECHO PANTALLA

		//Integration method
		App->renderer->BlitText(SCREEN_WIDTH - 38 * 8, 20, App->renderer->blueFont, "INTEGRATION METHOD: ");
		switch (method)
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

		//Movement type
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

		//Delta time
		App->renderer->BlitText(SCREEN_WIDTH - 38 * 8, 120, App->renderer->blueFont, "DELTA TIME MODE: ");
		switch (App->timeControl)
		{
		case DeltaTimeControl::FIXED_DELTATIME:
			App->renderer->BlitText(SCREEN_WIDTH - 22 * 8, 116, App->renderer->greenFont, "FIXED");
			App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 140, App->renderer->blueFont, "F9: CHANGE TO FIXED DELAY ");
			App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 150, App->renderer->blueFont, "F10: CHANGE TO POSITION MOVEMENT ");
			break;
		case DeltaTimeControl::FIXED_DELTATIME_DELAY:
			App->renderer->BlitText(SCREEN_WIDTH - 22 * 8, 116, App->renderer->greenFont, "FIXED DELAY");
			App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 140, App->renderer->blueFont, "F8: CHANGE TO FIXED ");
			App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 150, App->renderer->blueFont, "F10: CHANGE TO POSITION MOVEMENT ");
			break;
		case DeltaTimeControl::VARIABLE_DELTATIME:
			App->renderer->BlitText(SCREEN_WIDTH - 22 * 8, 116, App->renderer->greenFont, "VARIABLE");
			App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 140, App->renderer->blueFont, "F8: CHANGE TO FIXED  ");
			App->renderer->BlitText(SCREEN_WIDTH - 36 * 8, 150, App->renderer->blueFont, "F9: CHANGE TO FIXED DELAY ");
			break;
		default:
			break;
		}

	}
	

	// Draw all balls in the scenario
	for (auto& ball : balls)
	{
		// Convert from physical magnitudes to geometrical pixels
		int pos_x = METERS_TO_PIXELS(ball.x);
		int pos_y = METERS_TO_PIXELS(ball.y);
		int size_r = METERS_TO_PIXELS(ball.radius);


		// Select color
		if (ball.physics_enabled)
		{
			switch (ball.type)
			{
			case ObjectType::RED:
				color_r = 255; color_g = 0; color_b = 0;
				break;
			case ObjectType::BLUE:
				color_r = 0; color_g = 130; color_b = 255;
				break;

			case ObjectType::GUN:
				color_r = 255; color_g = 100; color_b = 100;
				break;
			case ObjectType::GRENADE:
				color_r = 0; color_g = 255; color_b = 0;
				break;
			case ObjectType::OTHER:
				color_r = 255; color_g = 255; color_b = 255;
				break;
			default:
				break;
			}		
		}
		else
		{
			color_r = 100; color_g = 100; color_b = 100;
		}

		// Draw ball
		App->renderer->DrawCircle(pos_x, pos_y, size_r, color_r, color_g, color_b);
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	balls.clear();
	return true;
}

// Compute modulus of a vector
float modulus(float vx, float vy)
{
	return std::sqrt(vx * vx + vy * vy);
}

// Compute Aerodynamic Drag force
void compute_aerodynamic_drag(float &fx, float& fy, const PhysBall &ball, const Atmosphere &atmosphere)
{
	float rel_vel[2] = { ball.vx - atmosphere.windx, ball.vy - atmosphere.windy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = 0.5f * atmosphere.density * speed * speed * ball.surface * ball.cd; // Drag force (modulus)
	fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
}

// Compute Hydrodynamic Drag force
void compute_hydrodynamic_drag(float& fx, float& fy, const PhysBall& ball, const Water& water)
{
	float rel_vel[2] = { ball.vx - water.vx, ball.vy - water.vy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = ball.b * speed; // Drag force (modulus)
	fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
}

// Compute Hydrodynamic Buoyancy force
void compute_hydrodynamic_buoyancy(float& fx, float& fy, const PhysBall& ball, const Water& water)
{
	// Compute submerged area (assume ball is a rectangle, for simplicity)
	float water_top_level = water.y - water.h; // Water top level y
	float h = 2.0f * ball.radius; // Ball "hitbox" height
	float surf = h * (water_top_level - ball.y-ball.radius); // Submerged surface
	
	if ((ball.y) > water_top_level && (ball.y + ball.radius) < water_top_level) { surf = ((ball.radius) * (ball.radius) * PI) - (ball.radius - (water_top_level - ball.y)) * (ball.radius - (water_top_level - ball.y) * PI / 2); } //Half of more the sphere submerged [Total area-non sumerged area]
	
	if ((ball.y) <= water_top_level) { surf = ((ball.y - water_top_level + ball.radius) * (ball.y - water_top_level + ball.radius) * PI/2); } //Half of less the sphere submerged
	
	if ((ball.y + ball.radius) <= water_top_level) surf = ((ball.radius) * (ball.radius) * PI); // If ball completely submerged, use just all ball area
	//surf *= 0.4; // FUYM to adjust values (should compute the area of circle segment correctly instead; I'm too lazy for that)

	// Compute Buoyancy force
	double fbuoyancy_modulus = water.density * 10.0 * surf * 0.5; // Buoyancy force (modulus) [Density*gravity*volume(surface realmente*FUYM coeficient)]
	fx = 0.0; // Buoyancy is parallel to pressure gradient
	fy = fbuoyancy_modulus; // Buoyancy is parallel to pressure gradient
}


// Integration scheme: Velocity Backwards Euler
void integrator_velocity_Backwards_Euler(PhysBall& ball, float dt)
{
	ball.x += ball.vx * dt;
	ball.y += ball.vy * dt;
	ball.vx += ball.ax * dt;
	ball.vy += ball.ay * dt;
}

// Integration scheme: Velocity Forwards Euler
void integrator_velocity_Forwards_Euler(PhysBall& ball, float dt)
{
	ball.vx += ball.ax * dt;
	ball.vy += ball.ay * dt;
	ball.x += ball.vx * dt;
	ball.y += ball.vy * dt;
}

// Integration scheme: Velocity Verlet
void integrator_velocity_verlet(PhysBall& ball, float dt)
{
	ball.x += ball.vx * dt + 0.5f * ball.ax * dt * dt;
	ball.y += ball.vy * dt + 0.5f * ball.ay * dt * dt;
	ball.vx += ball.ax * dt;
	ball.vy += ball.ay * dt;
}


// Detect collision with ground
bool is_colliding_with_ground(const PhysBall& ball, const Ground& ground)
{
	float rect_x = (ground.x + ground.w / 2.0f); // Center of rectangle
	float rect_y = (ground.y - ground.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, ground.w, ground.h);
}

// Detect collision with water
bool is_colliding_with_water(const PhysBall& ball, const Water& water)
{
	float rect_x = (water.x + water.w / 2.0f); // Center of rectangle
	float rect_y = (water.y - water.h / 2.0f); // Center of rectangle

	return check_collision_circle_rectangle(ball.x, ball.y, ball.radius, rect_x, rect_y, water.w, water.h); //El codigo ya tienen en cuenta que x,y es desde el centro.
}

// Detect collision between circle and rectange
bool check_collision_circle_rectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh)
{
	// Algorithm taken from https://stackoverflow.com/a/402010

	// Distance from center of circle to center of rectangle
	float dist_x = std::abs(cx - rx);
	float dist_y = std::abs(cy - ry);

	// If circle is further than half-rectangle, not intersecting
	if (dist_x > (rw / 2.0f + cr)) { return false; }
	if (dist_y > (rh / 2.0f + cr)) { return false; }

	// If circle is closer than half-rectangle, is intersecting
	if (dist_x <= (rw / 2.0f)) { return true; }
	if (dist_y <= (rh / 2.0f)) { return true; }

	// If all of above fails, check corners
	float a = dist_x - rw / 2.0f;
	float b = dist_y + rh / 2.0f;
	float cornerDistance_sq = a * a + b * b;
	return (cornerDistance_sq <= (cr * cr));
}

bool check_collision_circle_circle(float cx1, float cy1, float cr1, float cx2, float cy2, float cr2)
{
	// Distance from center of circle to center of rectangle
	float dist_x = std::abs(cx1 - cx2);
	float dist_y = std::abs(cy1 - cy2);

	// If circle is further than both angles, not interceting
	if ((sqrt(dist_x * dist_x + dist_y* dist_y)) > (cr1+cr2)) { return false; }
	

	// If circle is closer than radius together, is intersecting
	if ((sqrt(dist_x * dist_x + dist_y * dist_y)) <= (cr1 + cr2)) { return true; }
}

// Convert from meters to pixels (for SDL drawing)
SDL_Rect Ground::pixels()
{
	SDL_Rect pos_px{};
	pos_px.x = METERS_TO_PIXELS(x);
	pos_px.y = METERS_TO_PIXELS(y);
	pos_px.w = METERS_TO_PIXELS(w);
	pos_px.h = METERS_TO_PIXELS(-h); // Can I do this? LOL
	return pos_px;
}

//Setters ball
void PhysBall::ApplyForce(float forX, float forY)
{
	addedForceX = forX;
	addedForceY = forY;
}

void PhysBall::SetVelocity(float vx_, float vy_)
{
	vx = vx_;
	vy = vy_;
}


void PhysBall::AddPosition(float x_, float y_)
{
	x += PIXEL_TO_METERS(x_);
	y += PIXEL_TO_METERS(y_);
}

//Create ball
//És el mateix que el constructor, però retorna la posició de l'element a la llista, necessari per fer coses amb player
//Inspirat en un projecte de l'any passat
//on està listItem :(

int ModulePhysics::CreateBall(float x_, float y_, float rad_, ObjectType type_, float mass_, float vx_, float vy_, float surface_, float cl_, float cd_, float b_, float cFriction_, float cRest_, float ax_, float ay_, bool enabled_, bool play)
{
	PhysBall* new_ball = new PhysBall(x_, y_, rad_, type_,  mass_, vx_ , vy_, surface_, cl_,  cd_ , b_, cFriction_, cRest_, ax_, ay_, enabled_, play);
	balls.push_back(*new_ball);

	return balls.size() - 1;
}


