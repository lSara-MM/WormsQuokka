#pragma once
#include "Module.h"
#include "Globals.h"

#include <vector>


#define PIXELS_PER_METER (20.0f) // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER) // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) (PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

//Integer method
enum class integrationMethods
{
	BACKWARDS_EULER,
	FORWARDS_EULER,
	VERLET
	
};

enum class ObjectType
{
	PLAYER,
	RED,
	BLUE,
	GUN,
	GRENADE,
	OTHER
};



// Class: Ball object (a simple stoopid physics object)
class PhysBall
{
public:

	PhysBall() {};

	// x, y, rad, mass, vx, vy, surface, cl, cd, b, friction, restitution, ax, ay, enabled;
	PhysBall(float x_, float y_, float rad, ObjectType type_ = ObjectType::OTHER, float mass_ = 1.0f, float vx_ = 0.0f, float vy_ = 0.0f, float surface_ = 1.0f, float cl_ = 1.0f, float cd_ = 1.0f, float b_ = 1.0f, float cFriction = 0.5f, float cRest = 1.0f, float ax_ = 0.0f, float ay_ = 0.0f, bool enabled = true, bool play = false)
	{
		x = x_;
		y = y_;
		radius = rad;
		mass = mass_;

		vx = vx_;
		vy = vy_;
		ax = ax_;
		ay = ay_;

		surface = surface_;
		cl = cl_;
		cd = cd_;
		b = b_;
		coef_friction = cFriction;
		coef_restitution = cRest;
			
		physics_enabled = enabled;
		type = type_;
		player = play; 
	}


	void ApplyForce(float forX, float forY);
	void SetVelocity(float vx_, float vy_);
	void AddPosition(float x_, float y_);

public:
	// Position
	// You could also use an array/vector
	float x;
	float y;

	// Velocity
	float vx;
	float vy;

	// Acceleration
	float ax;
	float ay;

	// Force (total) applied to the ball
	float fx;
	float fy;

	//Extra Force applied
	float addedForceX = 0;
	float addedForceY = 0;

	// Mass
	float mass;

	// Aerodynamics stuff
	float surface; // Effective wet surface
	float cl; // Aerodynamic Lift coefficient
	float cd; // Aerodynamic Drag coefficient
	float b; // Hydrodynamic Drag coefficient

	// Coefficients of friction & restitution (for bounces)
	float coef_friction;
	float coef_restitution;

	// Shape
	float radius;

	// Has physics enabled?
	bool physics_enabled = true;
	int id;
	ObjectType type;

	//variable used to draw wind
	int iwind = 0;

	bool player; 
};


// Class: Ground
class Ground : public SDL_Rect
{
public:
	float x,y,w,h; // Geometry (x,y,w,h)
	SDL_Rect pixels(); // Convert geometry to pixels to draw w/ SDL
};

// Class: Water
class Water : public Ground
{
public:
	float density; // Density of fluid
	float vx; // Velocity x
	float vy; // Velocity y
};

// Class: Atmosphere
class Atmosphere
{
public:
	float density; // Density of air
	float windx; // Wind x
	float windy; // Wind y
};


class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	// Physics objects
	std::vector<PhysBall> balls{};
	std::vector<Ground> grounds{};
	// Create a ball
	Atmosphere atmosphere{};
	Ground ground{};
	Ground ground1{};
	Ground ground2{};
	Water water{};

	//Debugs
	int integrator = 0;

	// Misc
	//float dt = 1.0 / 60.0;
	float dt_updated;

	//debug
	bool debug=true;
	// 0 Print wind & currents,
	// 1 Gravity enabled
	// 2 buoyancy Enabled
	// 3 hidro resistance enabled
	// 4 Aero is enabled
	// 5 Modify wind XOR 6 Modify current
	// 7 Modify fps value
	bool options[8];
	
	integrationMethods method;

	//problemes amb llista
	int CreateBall(float x_, float y_, float rad_, ObjectType type_ = ObjectType::OTHER, float mass_ = 1.0f, float vx_ = 0.0f, float vy_ = 0.0f, float surface_ = 1.0f, float cl_ = 1.0f, float cd_ = 1.0f, float b_ = 1.0f, float cFriction = 0.5f, float cRest = 1.0f, float ax_ = 0.0f, float ay_ = 0.0f, bool enabled = true, bool player = false);
	//int LoseHP(int body, ObjectType type_W, ObjectType type_P);

	//variable used to draw wind
	int iwind = 0;

	int bodyHP;
	ObjectType typeW; 
	ObjectType typeP; 
	bool losehp = false;

private:
};

// Compute modulus of a vector
float modulus(float vx, float vy);

// Compute Aerodynamic Drag force
void compute_aerodynamic_drag(float& fx, float& fy, const PhysBall& ball, const Atmosphere& atmosphere);

// Compute Hydrodynamic Drag force
void compute_hydrodynamic_drag(float& fx, float& fy, const PhysBall& ball, const Water& water);

// Compute Hydrodynamic Buoyancy force
void compute_hydrodynamic_buoyancy(float& fx, float& fy, const PhysBall& ball, const Water& water);

// Integration scheme: Velocity Backwards Euler
void integrator_velocity_Backwards_Euler(PhysBall& ball, float dt);

// Integration scheme: Velocity Forwards Euler
void integrator_velocity_Forwards_Euler(PhysBall& ball, float dt);

// Integration scheme: Velocity Verlet
void integrator_velocity_verlet(PhysBall& ball, float dt);

// Detect collision with ground
bool is_colliding_with_ground(const PhysBall& ball, const Ground& ground);

// Detect collision with water
bool is_colliding_with_water(const PhysBall& ball, const Water& water);

// Detect collision between circle and rectange
bool check_collision_circle_rectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh);

bool check_collision_circle_circle(float cx1, float cy1, float cr1, float cx2, float cy2, float cr2);


