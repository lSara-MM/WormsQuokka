#pragma once
#include <math.h>
#include "p2Point.h"
#include "p2List.h"

class Vec2D
{
public:
	Vec2D() {}

	Vec2D(float x_, float y_) { x = x_; y = y_; }

	~Vec2D() {}

	void VectorZero() { x = 0.0f; y = 0.0f; }
	void VectorSet(float x_, float y_) { x = x_; y = y_; }

    void operator += (const Vec2D& vector_) { x += vector_.x; y += vector_.y; }
    void operator + (const Vec2D& vector_) { x += vector_.x; y += vector_.y; }
  
    void operator -= (const Vec2D& vector_) { x -= vector_.x; y -= vector_.y; }
    void operator - (const Vec2D& vector_) { x -= vector_.x; y -= vector_.y; }
    
    void operator *= (float scalar) { x *= scalar; y *= scalar; }
    void operator * (float scalar) { x *= scalar; y *= scalar; }


    void operator /= (const Vec2D& vector_) { x / vector_.x, y / vector_.y; }
    void operator / (const Vec2D& vector_) { x / vector_.x, y / vector_.y; }

    bool operator == (const Vec2D& v2)
    {
        bool ret = false;
        if (x == v2.x && y == v2.y) { ret = true; }
        return ret;
    }
    void operator - () { x = -x; y = -y; }

    float DotProduct(const Vec2D& a, const Vec2D& b)
    {
        return ((a.x * b.x) + (a.y * b.y));
    }

    float CrossProduct(const Vec2D& a, const Vec2D& b)
    {
        return ((a.x * b.y) - (a.y * b.x));
    }

    float Length() const
    {
        return sqrtf(x * x + y * y);
    }

    Vec2D Normalize()
    {
        float l = Length();
        x /= l;
        y /= l;

        return Vec2D(x, y);
    }

public:
	float x, y;
};

enum BodyType
{
    STATIC,
    KINEMATIK,
    DYNAMIC,
};

enum BodyShape
{
    CIRCLE,
    RECTANGLE
};

class pBody
{
public:
    pBody() {};
    pBody(int x, int y, int width_, int height_, BodyShape shape_ = BodyShape::RECTANGLE, BodyType type_ = BodyType::STATIC);
    ~pBody() {};

    void SetMass(float m);
    void SetVel(Vec2D v);
    void SetAcc(Vec2D a);
    void SetDen(float d);
    void SetMassFric(float ff);
    void SetRest(float rest);

    // apply force to the center
    void ApplyForce(Vec2D f);



public:
    int width, height;
    //float radius;

    iPoint pos;
    Vec2D velocity = { 0, 0 };
    Vec2D acc = { 0, 0 };

    Vec2D force;

    float mass = 1.0f;
    float density = 0.0f;
    float friction = 0.20f;
    float restitution = 0.0f;

    BodyShape shape;
    BodyType type;
    
    p2List<pBody>* listBodies; //Lista con cuerpos estaticos
private:

};
