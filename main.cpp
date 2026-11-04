#include <glad/glad.h>   
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>  

#include <glm/glm.hpp>                  
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>         
#include <vector>
#include "Enemy.h"
#include "Player.h"
#include "Screens.h"
#include "Projectiles.h"
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include "Collectibles.h"
#include <string>

GLFWwindow* window;
int g_screenWidth = 800;
int g_screenHeight = 600;
unsigned int vao_square;
unsigned int vao_triangle;
unsigned int vao_circle;
int g_numCircleVertices;
glm::mat4 g_projectionMatrix;
glm::mat4 g_rotateMatrix = glm::mat4(1.0f);
glm::mat4 g_squashMatrix = glm::mat4(1.0f);
GLint locTransform;
GLint locColour;
float y_arr[] = {30.0f,120.0f, 210.0f, 300.0f};
float PLAYER_START_X = 180;
float ENEMY_START_X = 780;
float cutsceneStartTime;
float elapsed;
float t;
Screens currentscreen = Screens::mainmenu;
int hoveredlane = -1;
float spawntime;
bool selectedlanes[4] = {false, false, false, false};
std::vector<Enemy> enemies; 
std::vector<Player> players; 
std::vector<Projectiles> projectiles;
std::vector<Collectibles> collectibles;
int waves[] = {3,5,10};
int prices[] = {10,40,60};
bool unlocked_cards[] = {false,false,false};
int inwave = 0;
int currentwave = 0;
float g_hurt;
float g_grey;
int money = 30;
bool cursor_check;
bool card1;
bool card2;
bool card3;
bool start_hovered;
bool exit_hovered;

void rectangle_helper(float x,float y,float width,float height,glm::vec4 color){
    glm::mat4 g_transformMatrix = glm::mat4(1.0f);
    g_transformMatrix = glm::translate(g_transformMatrix, glm::vec3(x,y,0));
    g_transformMatrix = glm::scale(g_transformMatrix,glm::vec3(width,height,1));
    glm::mat4 full = glm::mat4(1.0f);
    full = g_projectionMatrix*g_squashMatrix*g_rotateMatrix*g_transformMatrix;
    glUniformMatrix4fv(locTransform, 1, GL_FALSE, glm::value_ptr(full));
    glm::vec4 c = glm::mix(color, glm::vec4(1.0f,0.0f,0.0f,color.a), g_hurt);
    float bright = (c.r+c.g+c.b)/3;
    c = glm::mix(c,glm::vec4(bright,bright,bright,c.a),g_grey);
    glUniform4f(locColour, c.r, c.g, c.b, c.a);
    glBindVertexArray(vao_square);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void circle_helper(float x,float y,float width,float height,glm::vec4 color){
    glm::mat4 g_transformMatrix = glm::mat4(1.0f);
    g_transformMatrix = glm::translate(g_transformMatrix, glm::vec3(x,y,0));
    g_transformMatrix = glm::scale(g_transformMatrix,glm::vec3(width,height,1));
    glm::mat4 full = glm::mat4(1.0f);
    full = g_projectionMatrix*g_squashMatrix*g_rotateMatrix*g_transformMatrix;
    glUniformMatrix4fv(locTransform, 1, GL_FALSE, glm::value_ptr(full));
    glm::vec4 c = glm::mix(color, glm::vec4(1.0f,0.0f,0.0f,color.a), g_hurt);
    float bright = (c.r+c.g+c.b)/3;
    c = glm::mix(c,glm::vec4(bright,bright,bright,c.a),g_grey);
    glUniform4f(locColour, c.r, c.g, c.b, c.a);
    glBindVertexArray(vao_circle);
    glDrawArrays(GL_TRIANGLE_FAN,0,g_numCircleVertices);
}

void triangle_helper(float x,float y,float width,float height,glm::vec4 color){
    glm::mat4 g_transformMatrix = glm::mat4(1.0f);
    g_transformMatrix = glm::translate(g_transformMatrix, glm::vec3(x,y,0));
    g_transformMatrix = glm::scale(g_transformMatrix,glm::vec3(width,height,1));
    glm::mat4 full = glm::mat4(1.0f);
    full = g_projectionMatrix*g_squashMatrix*g_rotateMatrix*g_transformMatrix;
    glUniformMatrix4fv(locTransform, 1, GL_FALSE, glm::value_ptr(full));
    glm::vec4 c = glm::mix(color, glm::vec4(1.0f,0.0f,0.0f,color.a), g_hurt);
    float bright = (c.r+c.g+c.b)/3;
    c = glm::mix(c,glm::vec4(bright,bright,bright,c.a),g_grey);
    glUniform4f(locColour, c.r, c.g, c.b, c.a);    
    glBindVertexArray(vao_triangle);
    glDrawArrays(GL_TRIANGLES,0,3);
} 

void drawsea(){
    glm::vec4 color = glm::vec4(0.039f, 0.196f, 0.259f, 1.0f);
    rectangle_helper(400,200,800,400,color);
}

void drawWaves(){
    glm::vec4 color = glm::vec4(0.15f, 0.35f, 0.45f, 1.0f);
    float pos[] = { 80, 385, 16,  250, 345, 20,  400, 360, 24,  550, 340, 18,  700, 355, 22,
                    160, 325, 14,  330, 375, 16,  480, 320, 20,  620, 380, 14,  760, 330, 18,
                    40, 240, 16,  300, 250, 20,  650, 245, 22,  180, 230, 14,  480, 258, 18,  760, 235, 16,
                    90, 150, 18,  450, 160, 20,  720, 155, 22,  270, 140, 14,  580, 168, 16,
                    20,  60, 16,  350,  65, 20,  600,  70, 22,  200,  52, 14,  480,  78, 18,  740,  48, 16};
    int size = sizeof(pos)/sizeof(pos[0]);
    for(int i = 0;i<size;i+=3){
        float x = fmod(pos[i] + t*10,800);
        rectangle_helper(x,pos[i+1],pos[i+2],2,color);
    }
}

void drawhorizon(){
    glm::vec4 color = glm::vec4(0.189f,0.346f,0.409f,1.0f);
    rectangle_helper(400,400,800,2,color);
}

void drawmoonwithcraters(){
    glm::vec4 moon_color = glm::vec4(0.95f, 0.93f, 0.75f,1.0f);
    circle_helper(650,520,60,60,moon_color);
    glm::vec4 craters_color = glm::vec4(0.80f, 0.78f, 0.60f,1.0f);
    circle_helper(640, 530,14,14,craters_color);
    circle_helper(662, 512,10,10,craters_color);
    circle_helper(646, 505,8,8,craters_color);
}

void drawpier(float y){
    for(float x = 200.0f;x<800.0f;x+=80.0f){
        glm::vec4 posts_color = glm::vec4(0.28f,0.18f,0.10f,1.0f);
        rectangle_helper(x,y-20,10.0f,24.0f,posts_color);
    }
    glm::vec4 deck_color = glm::vec4(0.45f,0.30f,0.18f,1.0f);
    glm::vec4 moonlight_color = glm::vec4(0.58f, 0.42f, 0.28f, 1.0f);
    rectangle_helper(480,y,640,16,deck_color);// deck
    rectangle_helper(480,y+6.5,640,3,moonlight_color);//moonlight
    for(float x = 180.0f;x<800.0f;x+=40.0f){
        glm::vec4 planklines_color = glm::vec4(0.32f,0.21f,0.12f,1.0f);
        rectangle_helper(x,y,2,16,planklines_color);
    }
}

void drawhoveredlane(float x,float y,glm::vec4 color){
    rectangle_helper(x,y,800,90,color);
}

void drawLightHouse(){
    //tower
    glm::vec4 tower_color = glm::vec4(0.92f,0.92f,0.88f,1.0f);
    rectangle_helper(80,220,70,440,tower_color);
    //red stripes
    glm::vec4 red_stripes_color = glm::vec4(0.75f, 0.15f, 0.15f,1.0f);
    for(float y = 80;y<400;y+=120){
        rectangle_helper(80,y,70,40,red_stripes_color);
    }
    //door
    glm::vec4 door_color = glm::vec4(0.25f, 0.15f, 0.08f, 1.0f);
    rectangle_helper(80,40,22,36,door_color);
    //small window
    glm::vec4 small_window_color = glm::vec4(0.10f, 0.12f, 0.20f, 1.0f);
    rectangle_helper(80,260,12,18,small_window_color);
    //balcony
    glm::vec4 balcony_color = glm::vec4(0.25f, 0.25f, 0.28f,1.0f);
    rectangle_helper(80, 445,90,10,balcony_color);
    //lamp room
    glm::vec4 lamp_room_color = glm::vec4(1.0f, 0.95f, 0.6f,1.0f);
    rectangle_helper(80,470,50,40,lamp_room_color);
    //lamp
    glm::vec4 lamp_color = glm::vec4(1.0f, 0.85f, 0.2f,1.0f);
    circle_helper(80,470,26,26,lamp_color);
    //window bars
    glm::vec4 window_bars_color = glm::vec4(0.2f, 0.2f, 0.22f,1.0f);
    rectangle_helper(68, 470,2,40,window_bars_color);
    rectangle_helper(92, 470,2,40,window_bars_color);
    //roof
    glm::vec4 roof_color = glm::vec4(0.7f, 0.12f, 0.12f,1.0f);
    triangle_helper(80,510,64,40,roof_color);
    
}

void drawrocks(){
    glm::vec4 left_bigrock_color = glm::vec4(0.26f, 0.27f, 0.30f, 1.0f);
    glm::vec4 left_smallrock_color = glm::vec4(0.34f, 0.35f, 0.38f, 1.0f);
    glm::vec4 right_bigrock_color = glm::vec4(0.26f, 0.27f, 0.30f, 1.0f);
    glm::vec4 right_smallrock_color = glm::vec4(0.34f, 0.35f, 0.38f, 1.0f);
    glm::vec4 stone_base_color = glm::vec4(0.40f, 0.40f, 0.43f, 1.0f);
    glm::vec4 linebetweenrocks_color = glm::vec4(0.30f, 0.30f, 0.33f, 1.0f);
    glm::vec4 highlight_rock = glm::vec4(0.48f, 0.49f, 0.52f, 1.0f);

    rectangle_helper(80.0f,10.0f,96.0f,20.0f,stone_base_color);
    rectangle_helper(80.0f,10.0f,96.0f,2.0f,linebetweenrocks_color);
    circle_helper(22.0f,14.0f,44.0f,34.0f,left_bigrock_color);
    circle_helper(40.0f,6.0f,40.0f,22.0f,left_smallrock_color);
    circle_helper(138.0f,14.0f,40.0f,32.0f,right_bigrock_color);
    circle_helper(122.0f,6.0f,40.0f,22.0f,right_smallrock_color);
    circle_helper(16, 24, 12, 6,highlight_rock);
    circle_helper(132, 24, 12, 6,highlight_rock);
}

void drawstars(){
    glm::vec4 stars_color = glm::vec4(0.9f, 0.9f, 1.0f, 1.0f);
    float pos[] = {60, 560,  150, 520,  230, 585,  300, 470,  370, 545,  450, 590,  520, 500,
                  580, 565,  720, 580,  760, 470,  690, 430,  420, 440,  260, 420};
    int size = sizeof(pos)/sizeof(pos[0]);
    for(int i = 0;i<size;i+=2){
        circle_helper(pos[i],pos[i+1],3,3,stars_color);
    }
}

void drawboats(float x,float y,float scale){
    //hull
    glm::vec4 hull_color = glm::vec4(0.30f, 0.20f, 0.12f, 1.0f);
    rectangle_helper(x,y,46.0f*scale,10.0f*scale,hull_color);
    //mast
    glm::vec4 mast_color = glm::vec4(0.25f, 0.18f, 0.10f, 1.0f);
    rectangle_helper(x, y+(23.0f*scale), 2.0f*scale, 36.0f*scale,mast_color);
    //sail
    glm::vec4 sail_color = glm::vec4(0.85f, 0.85f, 0.80f, 1.0f);
    triangle_helper(x+(11.0f*scale), y+(21.0f*scale), 18.0f*scale, 28.0f*scale,sail_color);
    //lantern
    glm::vec4 lantern_color = glm::vec4(1.0f, 0.85f, 0.3f, 1.0f);
    circle_helper(x,y+(42.0f*scale),5.0f*scale,5.0f*scale,lantern_color);
}


glm::mat4 sword_rotate(float x,float y,float angle){
    glm::mat4 m = glm::mat4(1.0f);
    glm::mat4 translate_back = glm::mat4(1.0f);
    translate_back = glm::translate(translate_back,glm::vec3(x,y,0.0f));
    glm::mat4 rotate = glm::mat4(1.0f);
    angle = glm::radians(angle);
    rotate  = glm::rotate(rotate,angle,glm::vec3(0,0,1));
    glm::mat4 translate_first = glm::mat4(1.0f);
    translate_first = glm::translate(translate_first,glm::vec3(-x,-y,0.0f));
    m = translate_back*rotate*translate_first;
    return m;
}

glm::mat4 duck_squash(float x,float y,float sy){
    glm::mat4 m = glm::mat4(1.0f);
    glm::mat4 translate_back = glm::mat4(1.0f);
    translate_back = glm::translate(translate_back,glm::vec3(x,y,0.0f));
    glm::mat4 squash = glm::mat4(1.0f);
    squash = glm::scale(squash,glm::vec3(1,sy,1));
    glm::mat4 translate_first = glm::mat4(1.0f);
    translate_first = glm::translate(translate_first,glm::vec3(-x,-y,0.0f));
    m = translate_back*squash*translate_first;
    return m;
}

void drawFisherman(float x,float y,float step,float angle){
    glm::vec4 boot_color = glm::vec4(0.15f, 0.12f, 0.10f, 1.0f);
    glm::vec4 pants_color = glm::vec4(0.20f, 0.25f, 0.40f, 1.0f);
    glm::vec4 coat_color = glm::vec4(0.95f, 0.80f, 0.15f, 1.0f);
    glm::vec4 coat__Dark_color = glm::vec4(0.78f, 0.62f, 0.10f, 1.0f);
    glm::vec4 coat__Light_color = glm::vec4(0.98f, 0.86f, 0.30f, 1.0f);
    glm::vec4 skin_color = glm::vec4(0.93f, 0.76f, 0.60f, 1.0f);
    glm::vec4 nose_color = glm::vec4(0.85f, 0.62f, 0.48f, 1.0f);
    glm::vec4 beard_color = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
    glm::vec4 dark_color = glm::vec4(0.10f, 0.10f, 0.12f, 1.0f);
    glm::vec4 wood_color = glm::vec4(0.55f, 0.38f, 0.20f, 1.0f);
    glm::vec4 blade_color =  glm::vec4(0.68f, 0.48f, 0.27f, 1.0f);

    float swing = 4*step;
    float by = y + fabs(step)*2;

    rectangle_helper(x-12,by+31,7,20,coat__Dark_color); // back arm (behind the body)
    rectangle_helper(x-6 - swing,y+14, 8, 16, pants_color); // back leg
    rectangle_helper(x+6 + swing,y+14, 8, 16, pants_color ); // front leg
    rectangle_helper(x-6 - swing,y+3, 10, 6, boot_color); // back boot
    rectangle_helper(x+7 + swing,y+3, 10, 6, boot_color); // front boot
    rectangle_helper(x,by+33, 22, 26, coat_color); // body (raincoat)
    circle_helper(x+3,by+38, 3, 3, dark_color); // top button
    circle_helper(x+3,by+30, 3, 3, dark_color); // bottom button
    circle_helper(x,by+54,18,18,skin_color); //head
    circle_helper(x+2,  by+49, 15, 9, beard_color); //beard
    circle_helper( x+5,  by+56, 2.5, 2.5, dark_color); //eye
    circle_helper( x+9,  by+53, 4, 4, nose_color); //nose
    circle_helper(x-1,  by+64, 18, 10, coat_color); //hat
    rectangle_helper(x, by+61, 28, 4, coat__Dark_color); //hat brim
    g_rotateMatrix = sword_rotate(x+17,by+25,angle);
    rectangle_helper(x+18, by+34, 3, 44, wood_color);// oar handle
    rectangle_helper(x+18, by+62, 9, 18, wood_color);//oar blade
    g_rotateMatrix = glm::mat4(1.0f); 
    rectangle_helper(x+13,by+34,7,16,coat__Light_color);// front arm
    rectangle_helper(x+13, by+27, 7, 3, coat__Dark_color);// sleeve cuff
    circle_helper(x+17, by+25, 7, 7, skin_color);// hand
}

void drawFirstMate(float x,float y,float step,float angle){
    glm::vec4 boot_color = glm::vec4(0.10f, 0.08f, 0.07f, 1.0f);
    glm::vec4 pants_color = glm::vec4(0.30f, 0.22f, 0.15f, 1.0f);
    glm::vec4 shirt_color = glm::vec4(0.85f, 0.83f, 0.78f, 1.0f);
    glm::vec4 shirt__Dark_color = glm::vec4(0.70f, 0.68f, 0.63f, 1.0f);
    glm::vec4 stripe_color = glm::vec4(0.65f, 0.12f, 0.12f, 1.0f);
    glm::vec4 vest_color = glm::vec4(0.18f, 0.18f, 0.22f, 1.0f);
    glm::vec4 belt_color = glm::vec4(0.12f, 0.10f, 0.08f, 1.0f);
    glm::vec4 gold_color = glm::vec4(0.85f, 0.70f, 0.20f, 1.0f);
    glm::vec4 skin_color = glm::vec4(0.78f, 0.56f, 0.40f, 1.0f);
    glm::vec4 skin_Dark_color =  glm::vec4(0.66f, 0.45f, 0.31f, 1.0f);
    glm::vec4 beard_color =  glm::vec4(0.12f, 0.10f, 0.10f, 1.0f);
    glm::vec4 patch_color = glm::vec4(0.04f, 0.04f, 0.05f, 1.0f);
    glm::vec4 bandana_color = glm::vec4(0.70f, 0.10f, 0.12f, 1.0f);
    glm::vec4 steel_color = glm::vec4(0.75f, 0.78f, 0.82f, 1.0f);
    glm::vec4 handle_color = glm::vec4(0.25f, 0.15f, 0.08f, 1.0f);

    float swing = 4*step;
    float by = y + fabs(step)*2;


    rectangle_helper(x+17, by+33, 9, 22, shirt__Dark_color);// back arm
    circle_helper(x+18, by+22, 8, 8,skin_Dark_color);// back hand
    rectangle_helper(x-8 + swing,y+13, 10, 16, pants_color );// front leg
    rectangle_helper(x+8 - swing,y+13, 10, 16, pants_color);// back leg
    rectangle_helper(x-10 + swing,y+3, 14, 6, boot_color);//front boot
    rectangle_helper(x+8 - swing,y+3, 12, 6, boot_color);//back boot
    rectangle_helper(x, by+34, 32, 28, shirt_color);//body
    for(float inc = 27;inc<42;inc+=7){
        rectangle_helper(x, by+inc,32,3,stripe_color); // stripe
    }
    rectangle_helper( x-12, by+34, 8, 28,vest_color);// vest front
    rectangle_helper( x+12, by+34, 8, 28, vest_color);// vest back
    rectangle_helper(x,by+21,34,5,belt_color);// belt
    rectangle_helper( x-3,  by+21, 6, 6, gold_color);// buckle
    circle_helper(x,by+56, 20, 20, skin_color);// head
    circle_helper(x-2,  by+49, 18, 11, beard_color);// beard
    circle_helper(x-11, by+55, 5, 5, skin_Dark_color);// nose
    rectangle_helper(x,by+59, 20, 1.5, patch_color);// eye patch strap
    circle_helper(x-5,  by+58, 6, 6, patch_color);//eye patch
    circle_helper(x+7,by+52, 3, 3, gold_color);//earring
    circle_helper(x+1,  by+64, 22, 10, bandana_color);//bandana
    triangle_helper(x+10, by+61, 8, 8, bandana_color);//bandana knot
    g_rotateMatrix = angle!=0?sword_rotate(x-20,by+23,angle):glm::mat4(1.0f);
    rectangle_helper(x-21, by+47, 4, 32, steel_color);// cutlass blade
    triangle_helper(x-21,by+67,4,8,steel_color);// blade tip
    rectangle_helper(x-21, by+30, 12, 3, gold_color);// sword guard
    rectangle_helper(x-21, by+18, 3, 8, handle_color);// sword handle
    g_rotateMatrix = glm::mat4(1.0f);
    rectangle_helper(x-15,by+33,9,20,shirt_color);// front arm
    circle_helper(x-20,by+23,8,8,skin_color);// front hand
}

void drawhealthbar(float x,float y,int health,int max_health){
    glm::vec4 gray_color = glm::vec4(0.75f,0.75f,0.75f,1.0f);
    glm::vec4 green_color = glm::vec4(0.13f,0.73f,0.38f,1.0f);
    rectangle_helper(x,y,40,5,gray_color);
    float width = ((float)health / max_health) * 40;    
    rectangle_helper(x - (40 - width)/2,y,width,5,green_color);
}

bool drawbutton(float x,float y,std::string s,glm::vec4 button_color,glm::vec4 text_color,float width,float height){
    double posx,posy;
    glfwGetCursorPos(window,&posx,&posy);
    double gamey = 600 - posy;
    bool check = (posx>=x-(width/2) && posx<x+(width/2)) && (gamey>=y-(height/2) && gamey<y+(height/2));
    glm::vec4 hover_color = glm::vec4(1.0, 0.92, 0.55,1.0);
    if(check){
        y+=6;
        rectangle_helper(x,y,width+10,height+10,hover_color);
    }
    rectangle_helper(x,y,width,height,button_color);
    return check;
    //draw the text that text could be digits or a normal string 
}

void drawcard(float x,float y,Player_type type){
    if(money>=10 && type == Player_type::Fisherman){
        unlocked_cards[(int) type] = true;
    }else if(money>= 40 && type == Player_type::Harpooner){
        unlocked_cards[(int) type] = true;
    }else if(money>= 60 && type == Player_type::Dockmaster){
        unlocked_cards[(int) type] = true;
    }else{
        unlocked_cards[(int) type] = false;
    }
    if(!unlocked_cards[(int) type])
        g_grey = 1;
    glm::vec4 hover_color = glm::vec4(1.0, 0.92, 0.55,1.0);
    glm::vec4 wood_frame_color = glm::vec4(0.40, 0.26, 0.13,1.0);
    glm::vec4 digits_color = glm::vec4(0.25, 0.16, 0.08,1.0);
    glm::vec4 parchment_color = glm::vec4(0.93, 0.87, 0.72,1.0);
    glm::vec4 parchment_color_hovered = glm::vec4(1.0, 0.96, 0.84,1.0);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double gamey = 600 - ypos;
    cursor_check = unlocked_cards[(int) type]&&(xpos>=x-32 && xpos<x+32) && (gamey>=y-56 && gamey<y+56);
    if(cursor_check && currentscreen==Screens::game){
        y+=6;
        rectangle_helper(x,y,72,120,hover_color);
    }
    rectangle_helper(x,y,64,112,wood_frame_color);
    rectangle_helper(x,y,56,104,(cursor_check && currentscreen==Screens::game)?parchment_color_hovered:parchment_color);
    rectangle_helper(x,y-32,56,2,wood_frame_color);
    if(type == Player_type::Fisherman){
        card1 = cursor_check;
        drawFisherman(x-3.5,y-28,0,0);
    }else if(type == Player_type::Harpooner){
        card2 = cursor_check;
        //drawharpooner untill we do it;
    }else{
        card3 = cursor_check;
        //drawdockmaster untill we do it;
    }
    g_grey = 0;
}

void drawcollectibles(float x,float y){
    glm::vec4 shade_color = glm::vec4(0, 0, 0, 0.25);
    glm::vec4 rim_color = glm::vec4(0.66, 0.45, 0.06, 1);
    glm::vec4 gold_color = glm::vec4(0.95, 0.76, 0.20, 1);
    glm::vec4 bar_color = glm::vec4(0.78, 0.57, 0.10, 1);
    glm::vec4 shine_color = glm::vec4(1.0, 0.96, 0.76, 1);

    circle_helper(x,y-2,16,4,shade_color);
    circle_helper(x,y + 13 + 3*sin(3*t),18,18,rim_color);
    circle_helper(x,y + 13 + 3*sin(3*t),14,14,gold_color);
    rectangle_helper(x,y + 13 + 3*sin(3*t),8,2,bar_color);
    rectangle_helper(x,y + 13 + 3*sin(3*t),2,8,bar_color);
    circle_helper(x-3,y + 16 + 3*sin(3*t),4,4,shine_color);
}

void drawBackground(){
    drawsea();
    drawhorizon();
    drawWaves();
    drawmoonwithcraters();
    for(float y:y_arr){
        drawpier(y);
    }
    drawLightHouse();
    drawrocks();
    drawstars();
    float boats_pos[] = {
        600.0f, 372.0f, 1.0f,
        430.0f, 385.0f, 0.7f,
        720.0f, 390.0f, 0.5f
    };
    int size = sizeof(boats_pos)/sizeof(boats_pos[0]);
    for(int i = 0;i<size;i+=3){
        drawboats(boats_pos[i],boats_pos[i+1] + 2*sin(boats_pos[i] + 2*t),boats_pos[i+2]);
    }
}

void drawCutscene(){
        drawBackground();
        float angle = fabs(60*sin(t));
        if(180 + elapsed*15 <= 400){
            float step = sin(4*t);
            drawFisherman(180 + elapsed*15,128,step,-angle);

        }
        else{
            drawFisherman(400,128,0,-angle);
        }

        if(778 - elapsed*15 >= 530){
            float step = sin(4*t);
            drawFirstMate(778 - elapsed*15,128,step,angle);
        }
        else{
            drawFirstMate(530,128,0,angle);
        }
        if(elapsed>=20){
            currentscreen = Screens::game;
        }
}



bool isprotected(Player p){
    for(Player& p1:players){
        if(p.player_type != Player_type::Dockmaster && p.posx < p1.posx && p.lane == p1.lane && p1.player_type == Player_type::Dockmaster){
            return true;
        }
    }
    return false;
}

void spawnprojectiles(int lane,float posx,Projectile_type proj_type){
    Projectiles p;
    p.lane = lane;
    p.posx = posx;
    p.height = proj_type == Projectile_type::Harpoon?30: proj_type==Projectile_type::Shot? 50 : 10;
    p.speed = proj_type == Projectile_type::Harpoon?200:-200;
    p.proj_type = proj_type;
    p.hit = false;
    projectiles.push_back(p);
}

void spawnplayers(Player_type type,int health,int lane){
    Player p;
    p.posx = PLAYER_START_X;
    p.player_type = type;
    p.health = health;
    p.max_health = health;
    p.lane = lane;
    p.stopped = false;
    p.attacktime = 0;
    p.jump_time = 0;
    p.duck_time = 0;
    if(p.player_type == Player_type::Harpooner){
        p.shooting_time = 1;
    }
    players.push_back(p);
}

void spawnenemies(Enemy_type type){
    Enemy e;
    int lane = rand()%4;
    e.posx = ENEMY_START_X;
    e.enemy_type = type;
    if(type == Enemy_type::FirstMate){
        e.health = 7;
        e.max_health = 7;
        e.speed = 30;
    }else{
        e.health = 5;
        e.max_health = 5;
        e.speed = 15;
    }
    e.lane = lane;
    e.stopped = false;
    e.attacktime = 0;
    if(e.enemy_type == Enemy_type::Deckhand || e.enemy_type == Enemy_type::PowderMonkey){
        e.shooting_time = 2;
    }
    enemies.push_back(e);
}

void spawnecollectibles(float x,int price,int lane){
    Collectibles c;
    c.posx = x;
    c.price = price;
    c.lane = lane;
    collectibles.push_back(c);
}

void updateshooting(float dt){
    for(Player& p:players){
        if(p.player_type == Player_type::Harpooner){
            if(p.shooting_time<1)
                p.shooting_time+=dt;
            else{   
                spawnprojectiles(p.lane,p.posx,Projectile_type::Harpoon);
                p.shooting_time = 0;
            }
        }
    }

    for(Enemy& e:enemies){
        if(e.enemy_type == Enemy_type::Deckhand || e.enemy_type == Enemy_type::PowderMonkey){
            if(e.shooting_time<2)
                e.shooting_time+=dt;
            else{
                if(e.enemy_type ==  Enemy_type::Deckhand)
                    spawnprojectiles(e.lane,e.posx,Projectile_type::Shot);
                else if(e.enemy_type == Enemy_type::PowderMonkey)
                    spawnprojectiles(e.lane,e.posx,Projectile_type::Barrel);
                e.shooting_time = 0;
            }
        }
    }
}

void updateprojectile(float dt){
    for(Projectiles& p:projectiles){
        p.posx+=p.speed*dt;
    }
}

void updateenemy(float dt){
    for(Enemy& e:enemies){
        if(e.posx<=PLAYER_START_X){
            e.stopped = true;
            currentscreen = Screens::losemenu;
        }
        if(!e.stopped)
        e.posx-=e.speed*dt;
    }
}

void updateplayer(float dt){
    for(Player& p:players){
        if(p.posx>=760)
            p.stopped = true;
        if(p.jump_time>0){
            p.jump_time-=dt;
        }else if(p.duck_time>0){
            p.duck_time-=dt;
        }
        if(!p.stopped && (p.player_type == Player_type::Fisherman || p.player_type == Player_type::Harpooner))
            p.posx+=15*dt;
    }
}

void updatecombat_fullrange(){
    for(Projectiles& proj:projectiles){
        if(proj.proj_type == Projectile_type::Harpoon){
            for(Enemy& e:enemies){
                if(proj.lane == e.lane && fabs(proj.posx - e.posx) <= 16){
                    e.health--;
                    proj.hit = true;
                    break;
                }
            }
        }else if(proj.proj_type == Projectile_type::Shot){
            for(Player& p:players){
                if(proj.lane == p.lane && fabs(proj.posx - p.posx) <= 16 && p.duck_time<=0){
                    p.health--;
                    proj.hit = true;
                    break;
                }
            }
        }else if(proj.proj_type == Projectile_type::Barrel){
            for(Player& p:players){
                if(proj.lane == p.lane && fabs(proj.posx - p.posx) <= 16 && p.jump_time<=0){
                    p.health--;
                    proj.hit = true;
                    break;
                }
            }
        }
    }
}

void updatecombat_closerange(float dt){
    for(Enemy& e:enemies){
        e.stopped = false;
    }
    for(Player& p:players){
        p.stopped = false;
        for(Enemy& e:enemies){
            if((p.lane == e.lane)&&(fabs(e.posx - p.posx)<=70)){
                p.stopped = true;
                e.stopped = true;
                if(p.attacktime>=1 && p.player_type != Player_type::Dockmaster && p.player_type != Player_type::Harpooner){
                    e.health--;
                    p.attacktime = 0;
                }
                if(e.attacktime>=1){
                    if(e.enemy_type==Enemy_type::FirstMate && !isprotected(p)){
                        p.health-=2;
                        e.attacktime = 0;
                    }
                }
            }
        }
    }
    for(Player& p:players){
        if(p.stopped)p.attacktime+=dt;
        else p.attacktime = 0;
    }

    for(Enemy& e:enemies){
        if(e.stopped)e.attacktime+=dt;
        else e.attacktime = 0;
    }
}

void spawnenemy_check(){
    spawntime = 0;
    if(inwave < waves[currentwave] ){
        int number = inwave % (currentwave + 1);
        Enemy_type type = static_cast<Enemy_type>(number);
        spawnenemies(type);
        inwave++;
    }
}

void removedead_projectiles_andcoins(){
    for(int i = 0;i<players.size();i++){
        if(players.at(i).health <= 0){
            players.erase(players.begin() + i);
            i--;
        }
    }
    for(int i = 0;i<enemies.size();i++){
        if(enemies.at(i).health <= 0){
            spawnecollectibles(enemies.at(i).posx,5,enemies.at(i).lane);
            enemies.erase(enemies.begin() + i);
            i--;
        }
    }

    for(int i = 0;i<projectiles.size();i++){
        if(projectiles.at(i).posx >= 800 || projectiles.at(i).posx <= 0 || projectiles.at(i).hit){
            projectiles.erase(projectiles.begin() + i);
            i--;
        }
    }

    for(int i = 0;i<collectibles.size();i++){
        for(int j = 0;j<players.size();j++){
            if(players.at(j).lane == collectibles.at(i).lane && fabs(players.at(j).posx - collectibles.at(i).posx)<=20){
                money+=collectibles.at(i).price;
                collectibles.erase(collectibles.begin() + i);
                i--;
                break;
            }
        }
    }
}

void resetgame(){
    cutsceneStartTime = glfwGetTime();
    currentscreen = Screens::cutscene;
    spawntime = 0;
    for(int i = 0;i<4;i++){
        selectedlanes[i] = false;
    }
    enemies.clear();
    players.clear();
    projectiles.clear();
    collectibles.clear();
    for(int i = 0;i<3;i++){
        unlocked_cards[i] = false;
    }
    inwave = 0;
    currentwave = 0;
    money = 30;
    
}
void drawgamescreen(){
    drawBackground();
    double mx;
    double my;
    glfwGetCursorPos(window,&mx,&my);
    double gamey = 600 - my;
    hoveredlane = -1;
    for(int i = 0;i<=3;i++){
        glm::vec4 color;
        if(selectedlanes[i]){
            color = glm::vec4(1.0f, 0.9f, 0.6f,0.6f);
            drawhoveredlane(400,y_arr[i]+37,color);
        }else{
            color = glm::vec4(1.0f, 0.9f, 0.6f,0.25f);
        }
        if(gamey>=y_arr[i] - 8 && gamey<y_arr[i]+82){
            hoveredlane = i;
            if(!selectedlanes[i] && currentscreen == Screens::game)
                drawhoveredlane(400,y_arr[i]+37,color);      
        }  
    }
    drawcard(200,534,Player_type::Fisherman);
    drawcard(280,534,Player_type::Harpooner);
    drawcard(360,534,Player_type::Dockmaster);
    for(Player& p : players){
        drawhealthbar(p.posx,y_arr[p.lane] + 88,p.health,p.max_health);
        g_hurt = 1 - ((float)p.health/p.max_health);
        if(p.player_type == Player_type::Fisherman){
            float step = !p.stopped?sin(4*t):0;
            float progress = (0.5 - p.jump_time)/0.5;
            float jump = 0;
            if(p.jump_time >= 0){
                jump = 30*sin(progress*3.14159f);
            }
            if(p.duck_time > 0){
                g_squashMatrix = duck_squash(p.posx,y_arr[p.lane]+8,0.6);
            }
            drawFisherman(p.posx,y_arr[p.lane] + 8 + jump,step,0);
        }else if(p.player_type == Player_type::Dockmaster){
            rectangle_helper(500,300,300,100,glm::vec4(1.0,1.0,1.0,1.0));
        }else if(p.player_type == Player_type::Harpooner){
            rectangle_helper(500,300,300,100,glm::vec4(0.7,0.5,1.0,1.0));
        }
        g_hurt = 0;
        g_squashMatrix = glm::mat4(1.0f);
    }

    for(Enemy& e : enemies){
        drawhealthbar(e.posx,y_arr[e.lane] + 88,e.health,e.max_health);
        g_hurt = 1 - ((float)e.health/e.max_health);
        if(e.enemy_type == Enemy_type::FirstMate){
            float step = !e.stopped?sin(((e.speed)/4)*t):0;
            drawFirstMate(e.posx,y_arr[e.lane] + 8,step,0);
        }else if(e.enemy_type == Enemy_type::Deckhand){
            float step = !e.stopped?sin(((e.speed)/4)*t):0;
            drawFirstMate(e.posx,y_arr[e.lane] + 8,step,0);
        }else if(e.enemy_type == Enemy_type::PowderMonkey){
            float step = !e.stopped?sin(((e.speed)/4)*t):0;
            drawFirstMate(e.posx,y_arr[e.lane] + 8,step,0);
        }
        g_hurt = 0;
    }

    for(Projectiles& p : projectiles){
        rectangle_helper(p.posx,y_arr[p.lane]+8+p.height,24,4,glm::vec4(1,1,1,1));
    }

    for(Collectibles& c:collectibles){
        float y_pos = y_arr[c.lane] + 8;
        drawcollectibles(c.posx,y_pos);
    }
}
void drawmainmenuscreen(){
    glm::vec4 start_color = glm::vec4(0.13f, 0.55f, 0.30f, 1.0f);
    glm::vec4 exit_color = glm::vec4(0.75f, 0.15f, 0.15f, 1.0f);
    glm::vec4 text_color = glm::vec4(0.93f, 0.87f, 0.72f, 1.0f);
    start_hovered = drawbutton(400,345,"START",start_color,text_color,200,50);
    exit_hovered = drawbutton(400,255,"EXIT",exit_color,text_color,200,50);
}
void drawpausemenu(){
    drawBackground();
    rectangle_helper(500,250,100,108,glm::vec4(0.4,0.2,0.5,1.0));
}
void drawwinmenu(){
    drawBackground();
    for(Player& p:players){
        p.stopped = true;
    }
    for(Enemy& e:enemies){
        e.stopped = true;
    }
    drawgamescreen();
    rectangle_helper(400,300,800,600,glm::vec4(0,0,0,0.5));
    rectangle_helper(400,300,300,200,glm:: vec4(0.0f, 0.659f, 0.349f, 1.0f));
}
void drawlosemenu(){
    for(Player& p:players){
        p.stopped = true;
    }
    for(Enemy& e:enemies){
        e.stopped = true;
    }
    drawgamescreen();
    rectangle_helper(400,300,800,600,glm::vec4(0,0,0,0.5));
    rectangle_helper(400,300,300,200,glm::vec4(1.0, 0.0, 1.0, 1.0));
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        if(currentscreen == Screens::game){
            currentscreen = Screens::pausemenu;
        }else if(currentscreen == Screens::pausemenu && action == GLFW_PRESS){
            currentscreen = Screens::game;
        }
    }

    if(key == GLFW_KEY_1 && action == GLFW_PRESS && currentscreen == Screens::game){
        int no_playersto_spawn = 0;
        for(int i = 0;i<4;i++){
            if(selectedlanes[i] && (money - no_playersto_spawn*prices[0]) >= prices[0]){
                spawnplayers(Player_type::Fisherman,5,i);
                no_playersto_spawn++;
            }
        }
        money-=no_playersto_spawn*prices[0];
    }

    if(key == GLFW_KEY_2 && action == GLFW_PRESS && currentscreen == Screens::game){
        int no_playersto_spawn = 0;
        for(int i = 0;i<4;i++){
            if(selectedlanes[i] &&  (money - no_playersto_spawn*prices[1]) >= prices[1]){
                spawnplayers(Player_type::Harpooner,5,i);
                no_playersto_spawn++;
            }
        }
        money-=no_playersto_spawn*prices[1];
    }

    if(key == GLFW_KEY_3 && action == GLFW_PRESS && currentscreen == Screens::game){
        int no_playersto_spawn = 0;
        for(int i = 0;i<4;i++){
            if(selectedlanes[i] &&  (money - no_playersto_spawn*prices[2]) >= prices[2]){
                spawnplayers(Player_type::Dockmaster,7,i);
                no_playersto_spawn++;
            }
        }
        money-=no_playersto_spawn*prices[2];
    }        

    if((key == GLFW_KEY_UP || key == GLFW_KEY_W) && action == GLFW_PRESS && currentscreen == Screens::game){
        for(int i = 0;i<4;i++){
            if(selectedlanes[i]){
               for(Player& p:players){
                    if(p.lane == i && p.player_type != Player_type::Dockmaster && p.jump_time <= 0 && p.duck_time <= 0)
                        p.jump_time = 0.5;
               }
            }
        }
    }


    if((key == GLFW_KEY_DOWN || key == GLFW_KEY_S) && action == GLFW_PRESS && currentscreen == Screens::game){
        for(int i = 0;i<4;i++){
            if(selectedlanes[i]){
               for(Player& p:players){
                    if(p.lane == i && p.player_type != Player_type::Dockmaster && p.jump_time <= 0 && p.duck_time <= 0)
                        p.duck_time = 0.5;
               }
            }
        }
    }
}

void mousebutton_callback(GLFWwindow* window, int button,int action, int mods){
        if(currentscreen == Screens::game && hoveredlane>=0 && hoveredlane<=3 && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            selectedlanes[hoveredlane] = !selectedlanes[hoveredlane];
        }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentscreen == Screens::game && card1){
        int no_playersto_spawn = 0;
        for(int i = 0;i<4;i++){
            if(selectedlanes[i] && (money - no_playersto_spawn*prices[0]) >= prices[0]){
                spawnplayers(Player_type::Fisherman,5,i);
                no_playersto_spawn++;
            }
        }
        money-=no_playersto_spawn*prices[0];
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentscreen == Screens::game && card2){
        int no_playersto_spawn = 0;
        for(int i = 0;i<4;i++){
            if(selectedlanes[i] &&  (money - no_playersto_spawn*prices[1]) >= prices[1]){
                spawnplayers(Player_type::Harpooner,5,i);
                no_playersto_spawn++;
            }
        }
        money-=no_playersto_spawn*prices[1];
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentscreen == Screens::game && card3){
        int no_playersto_spawn = 0;
        for(int i = 0;i<4;i++){
            if(selectedlanes[i] &&  (money - no_playersto_spawn*prices[2]) >= prices[2]){
                spawnplayers(Player_type::Dockmaster,7,i);
                no_playersto_spawn++;
            }
        }
        money-=no_playersto_spawn*prices[2];
    }   

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentscreen == Screens::mainmenu && start_hovered){
        cutsceneStartTime = glfwGetTime();
        elapsed = 0;
        resetgame();
        currentscreen = Screens::cutscene;
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentscreen == Screens::mainmenu && exit_hovered){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;


uniform mat4 u_Transform;

void main()
{
    gl_Position = u_Transform * vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

uniform vec4 u_colour;

void main()
{
    FragColor = u_colour;
}
)";

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error ("
            << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "):\n" << infoLog << std::endl;
    }
    return shader;
}


int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(g_screenWidth, g_screenHeight, "Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader link error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    float squareVertices[] = {
        -0.5f,-0.5f,
        0.5f,-0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
    };

    unsigned int squareindices[] = {
        0, 1, 2,
        2, 3, 0
    };

    float triangleVertices[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.0f, 0.5f
    };

    GLuint square_VBO;
    unsigned int ibo;

    glGenVertexArrays(1, &vao_square); // VAO remembers HOW to read the buffer
    glGenBuffers(1, &square_VBO);      // VBO is the raw vertex bytes stored on the GPU
    glGenBuffers(1, &ibo);    // IBO is the raw index bytes stored on the GPU

    glBindVertexArray(vao_square);
    glBindBuffer(GL_ARRAY_BUFFER, square_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareindices), squareindices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);   // vertex attributes are DISABLED by default

    // // Attribute 1 = colour: 3 floats, starting 3 floats (12 bytes) into each vertex, same stride.
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    unsigned int triangle_VBO;

    glGenVertexArrays(1, &vao_triangle);
    glGenBuffers(1, &triangle_VBO);

    glBindVertexArray(vao_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*) 0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);   

    std::vector <float> circ_vert;

    circ_vert.push_back(0.0);
    circ_vert.push_back(0.0);
    float x;
    float y;
    int slices = 50;
    for(int i = 0;i<=slices;i++){
        float angle = (2*3.14159*i)/slices;
        x=(0.5 *cos(angle));
        y=(0.5*sin(angle));
        circ_vert.push_back(x);
        circ_vert.push_back(y);
    }
    g_numCircleVertices = slices + 2;

    unsigned int circle_VBO;
    glGenVertexArrays(1, &vao_circle);
    glGenBuffers(1, &circle_VBO);

    glBindVertexArray(vao_circle);
    glBindBuffer(GL_ARRAY_BUFFER, circle_VBO);

    glBufferData(GL_ARRAY_BUFFER, circ_vert.size()*sizeof(float), circ_vert.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    locTransform = glGetUniformLocation(shaderProgram, "u_Transform");
    locColour = glGetUniformLocation(shaderProgram, "u_colour");
    g_projectionMatrix = glm::ortho(0.0f, static_cast<float>(g_screenWidth),0.0f, static_cast<float>(g_screenHeight),-1.0f, 1.0f);
    glfwSetKeyCallback(window,key_callback);
    glfwSetMouseButtonCallback(window,mousebutton_callback);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float angle = 0.0f;

    cutsceneStartTime = (float) glfwGetTime();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    srand(time(0));

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        elapsed = currentFrame - cutsceneStartTime;

        if(currentscreen != Screens::losemenu && currentscreen!=Screens::winmenu)
            t = currentFrame;
        float offsetX = sin(t) * 0.5f;
        float pulse = 0.75f + 0.25f * sin(t * 2.0f);


        glClearColor(0.05f, 0.07f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);




        float r = sin(t) * 0.5f + 0.5f;
        float g = sin(t + 2.094f) * 0.5f + 0.5f;
        float b = sin(t + 4.188f) * 0.5f + 0.5f;

        switch (currentscreen){
            case Screens::mainmenu:
                drawmainmenuscreen();
                break;
            case Screens::cutscene:
                drawCutscene();
                break;
            case Screens::game:
                updateshooting(deltaTime);
                updateprojectile(deltaTime);
                updatecombat_fullrange();
                updatecombat_closerange(deltaTime);
                removedead_projectiles_andcoins();
                if(inwave >= waves[currentwave] && enemies.empty()){
                    if(currentwave<2){
                        currentwave++;
                        inwave = 0;
                    }else{
                        currentscreen = Screens::winmenu;
                    }
                }
                updateplayer(deltaTime);
                spawntime+=deltaTime;
                if(spawntime>=3){
                    spawnenemy_check();
                }
                updateenemy(deltaTime);
                drawgamescreen();
                break;
            case Screens::pausemenu:
                drawpausemenu();
                break;
            case Screens::winmenu:
                drawwinmenu();
                break;
            case Screens::losemenu:
                drawlosemenu();
                break;      
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao_square);
    glDeleteBuffers(1, &square_VBO);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao_triangle);
    glDeleteBuffers(1, &triangle_VBO);
    glDeleteVertexArrays(1, &vao_circle);
    glDeleteBuffers(1, &circle_VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}