#pragma once
#include "Enemy_type.h"

class Enemy{
    public:
        int lane;
        float posx;
        int health;
        bool stopped;
        float speed;
        Enemy_type enemy_type;
        float attacktime;
        int max_health;
        float shooting_time;
};