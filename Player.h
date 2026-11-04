#pragma once
#include "Player_type.h"

class Player{
    public:
        int lane;
        float posx;
        Player_type player_type;
        int health;
        bool stopped;
        float attacktime;
        int max_health;
        float shooting_time;
        float jump_time;
        float duck_time;
};