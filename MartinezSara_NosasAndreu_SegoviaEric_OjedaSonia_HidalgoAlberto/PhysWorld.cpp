#include "PhysWorld.h"

pBody::pBody(int x, int y, int width_, int height_, BodyShape shape_, BodyType type_)
{
    pos.x = x;
    pos.y = y;

    width = width_;
    height = height_;

    shape = shape_;
    type = type_;
}

void pBody::SetMass(float m) { mass = m; }
void pBody::SetVel(Vec2D v) { velocity = v; }
void pBody::SetAcc(Vec2D a) { acc = a; }
void pBody::SetDen(float d) { density = d; }
void pBody::SetMassFric(float ff) { friction = ff; }
void pBody::SetRest(float rest) { restitution = rest; }

// apply force to the center
void pBody::ApplyForce(Vec2D f) { force += f; }