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
    pBody();

    pBody(int x, int y, int width_, int height_, BodyShape shape = BodyShape::RECTANGLE, BodyType type_ = BodyType::STATIC);
    ~pBody();

    void SetMass(float m) { mass = m; }
    void SetVel(Vec2D v) { velocity = v; }
    void SetAcc(Vec2D a) { acc = a; }
    void SetDen(float d) { density = d; }
    void SetMassFric(float ff) { friction = ff; }
    void SetRest(float rest) { restitution = rest; }

    // apply force to the center
    void ApplyForce(Vec2D f) { force += f; }

public:
    int width, height;
    //float radius;

    iPoint pos;
    Vec2D velocity;
    Vec2D acc;

    Vec2D force;

    float mass;
    float density;
    float friction;
    float restitution;

    BodyType type;
    
    p2List<pBody>* allBodies; //Lista con cuerpos estaticos
private:

};

pBody::pBody(int x, int y, int width_, int height_, BodyShape shape, BodyType type_)
{
    pos.x = x;
    pos.y = y;
    type = type_;
}