#pragma once
#include "Projectile_type.h"

class Projectiles{
    public:
        int lane;
        float posx;
        float height;
        float speed;
        Projectile_type proj_type;
        bool hit;
};