#pragma once
#include <math.h>
#include <vector>

class Vec2D
{
public:
	Vec2D() {}

	Vec2D(float x_, float y_) { x = x_; y = y_; }

	~Vec2D() {}

	void VectorZero() { x = 0.0f; y = 0.0f; }
	void VectorSet(float x_, float y_) { x = x_; y = y_; }

    void operator += (const Vec2D& vector_)
    {
        x += vector_.x;
        y += vector_.y;
    }

    void operator -= (const Vec2D& vector_)
    {
        x -= vector_.x; 
        y -= vector_.y;
    }

    void operator *= (float a)
    {
        x *= a; y *= a;
    }

    bool operator == (const Vec2D& v2)
    {
        bool ret = false;
        if (x == v2.x && y == v2.y) { ret = true; }
        return ret;
    }

public:
	float x, y;

};