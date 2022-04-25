/*******************************************************************************************
*
*   raylib [core] example - 2d camera platformer
*
*   This example has been created using raylib 2.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Example contributed by arvyy (@arvyy) and reviewed by Ramon Santamaria (@raysan5)
*
*   Copyright (c) 2019 arvyy (@arvyy)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <time.h>

#define G 400
#define PLAYER_JUMP_SPD 350.0f // 跳跃速度
#define PLAYER_HOR_SPD 200.0f // 地速
#define PLAYER_MAX_SHOOTS   1000000


typedef struct Player {
    Vector2 position; // 位置
    float speed;
    float speedh;
    int jumpt;
    int jumpt2;
    int ajumpt;
    int boostt; // 是
    bool canJump;
    bool boost;
    Color Color;
    int towards;  
} Player;
typedef struct Shoot {
    Vector2 position;
    Vector2 speed;
    float radius;
    //float rotation;
    int lifeSpawn;
    bool active;
    Color color;
} Shoot;

typedef struct EnvItem { 
    Rectangle rect; // 矩形
    int blocking; // 是否阻挡
    Color color; // 颜色
} EnvItem;

const int screenWidth = 1300;
const int screenHeight = 600; //窗口尺寸
Shoot shoot[PLAYER_MAX_SHOOTS] = { 0 };



void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delt);

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delt, int width, int height);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "game"); //初始化窗口
    SetTargetFPS(60); //设置帧率



    Image bgImage = LoadImage("0"); 
    Texture2D bgTexture = LoadTextureFromImage(bgImage);  
    UnloadImage(bgImage);

    //Vector2 aimPosition = { 0, 0 }; // 准星位置
    //int currentGesture = GESTURE_NONE; // 当前手势
    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    player.speed = 0;
    player.canJump = 2;
    player.towards = 1;


    // Initialization shoot 子弹
    
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = (Vector2){0, 0};
        shoot[i].speed = (Vector2){0, 0};
        shoot[i].radius = 5;
        shoot[i].active = false;
        shoot[i].lifeSpawn = 0;
        shoot[i].color = GREEN;
    }
    //

    EnvItem envItems[] = {
        //{{ 0, 0, 1000, 400 }, 0, LIGHTGRAY }, // x起点 y起点 长 宽 阻挡   地图数据
        {{ 0, 400, 1000, 10 }, 1, GRAY }, //地面
        {{ 300, 200, 400, 10 }, 1, GRAY }, // 上
        {{ 250, 300, 100, 10 }, 1, GRAY }, // 左
        {{ 650, 300, 100, 10 }, 1, GRAY },
        {{ 1050, 300, 100, 10 }, 1, GRAY },
        {{ 1450, 300, 100, 10 }, 1, GRAY }, // 右
    };                                      

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]); //sizeo获取数据在内存中占用的字节数 有多少个元素


    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Store pointers to the multiple update camera functions
    void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
        UpdateCameraCenterSmoothFollow,
    };

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

    char *cameraDescriptions[] = {                             
        "Follow player center; smoothed",
    };

    //--------------------------------------------------------------------------------------




  
    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();                                   // Get time in seconds for last frame drawn (delta time) 一帧时间

        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);                  //滚轮缩放视角

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;                   //缩放极限

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            player.position = (Vector2){ 400, 280 };                        // 重置位置
        }

        // Call update camera function by its pointer
        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY); //清屏

            
            BeginMode2D(camera);
                DrawTexture(bgTexture, -500, -200, WHITE);                                                         // 背景
                for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color); // 画地图

                Rectangle playerRect = { player.position.x - 40, player.position.y - 40, 40, 40 };
                DrawRectangleRec(playerRect, RED);


                if (player.jumpt==0)
                {
                    DrawCircle(player.position.x-20, player.position.y+10, 10, WHITE);                          //二段跳云雾
                }


                                                                                                                // Draw shoot
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (shoot[i].active) DrawCircleV(shoot[i].position, shoot[i].radius, YELLOW);
                }


            EndMode2D();

            DrawText("Controls:", 20, 20, 20, BLACK);                                   //ui文字 坐标(左上原点),字号, 颜色,ui文字
            DrawText("- Right/Left to move", 40, 60, 20, DARKGRAY);
            DrawText("- Space to jump", 40, 100, 20, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 0, DARKGRAY);
            DrawText("- C to change camera mode", 40, 140, 20, DARKGRAY);
            DrawText("Current camera mode:", 20, 160, 20, BLACK);
            DrawText(TextFormat("x = %2.3f", player.position.x), 40 , 200, 20, BLACK);
            DrawText(TextFormat("y = %2.3f", player.position.y), 40 , 240, 20, BLACK);
            DrawText(TextFormat("x speed = %f", player.speedh), 40 , 280, 20, BLACK);
            DrawText(TextFormat("can jump = %d", player.canJump), 40 , 320, 20, BLACK);
            DrawText(TextFormat("jumpt = %d", player.jumpt), 40 , 360, 20, BLACK);
            DrawText(TextFormat("BOOST = %d", player.boostt),40, 380,20, BLACK);
            DrawText(TextFormat("ajumpt = %d",player.ajumpt),40, 400,20, BLACK);
            DrawText(TextFormat("Follow player center; smoothed"), 250, 165, 10, DARKGRAY);

            DrawRectangleLines(650, 50, 20, 20,BLACK);
            if (IsKeyDown(KEY_A)) DrawRectangle(650, 50, 20, 20,DARKGRAY);

                DrawRectangleLines(675, 50, 20, 20,BLACK);
            if (IsKeyDown(KEY_S)) DrawRectangle(675, 50, 20, 20,DARKGRAY);

            DrawRectangleLines(700, 50, 20, 20,BLACK);
            if (IsKeyDown(KEY_D)) DrawRectangle(700, 50, 20, 20,DARKGRAY);

                DrawRectangleLines(675, 25, 20, 20,BLACK);
            if (IsKeyDown(KEY_W)) DrawRectangle(675, 25, 20, 20,DARKGRAY);

                DrawRectangleLines(725,50, 20, 20,BLACK); 
            if (IsKeyDown(KEY_J)) DrawRectangle(725, 50, 20, 20,DARKGRAY);

                DrawRectangleLines(650,75, 100, 20,BLACK);                
            if (IsKeyDown(KEY_SPACE)) DrawRectangle(650,75, 100, 20,DARKGRAY);


            DrawRectangleLines(550,75, 80, 20,BLACK);                               // 闪现槽
            if (IsKeyDown(KEY_LEFT_SHIFT)) DrawRectangle(550,75, 80, 20,DARKGRAY);
                DrawRectangleLines(40,440, 20, 20,BLACK);
            if (player.boost)                                                       // 闪现指示
                DrawRectangle(40,440, 20, 20,GREEN);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    UnloadTexture(bgTexture); // Unload background texture
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(bgTexture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delt)
{   
    player->position.x += player->speedh*delt; // 水平移动

    if (IsKeyDown(KEY_A))                         
    {
        // player->position.x += player->speed*delt; 
        if(player->speedh>0)
        player->speedh = 0;                                                 // 急停
        player->speedh -= G*delt; 
        player->towards = -1;                                               // 向左跑
    }

    if (IsKeyDown(KEY_D)) 
    {
        // player->position.x += player->speed*delt; 
        if(player->speedh<0)
        player->speedh = 0;                                                 // 急停
        player->speedh += G*delt; 
        player->towards = 1;                                                // 向右跑
    }
    
    if (IsKeyPressed(KEY_SPACE) && player->jumpt>0 ) // 跳 空跳 二段跳 2d跳跃想要好的跳跃手感最好是用非物理的方式，手动模拟重力，计算跳跃
    {
        if (player->jumpt==2)
        {
           if (player->ajumpt<10) 
            {
                player->speed = -PLAYER_JUMP_SPD;
                player->jumpt =1;
                player->canJump = false;
            }

            else if(player->ajumpt>10)
            {
                player->speed = -PLAYER_JUMP_SPD;
                player->jumpt -=3;
            }
        }

        else if (player->jumpt==1)
        {
            player->speed = -0.8*PLAYER_JUMP_SPD;
            player->jumpt =0;
        }
    }

    if (player->speed < 600 && player->speed >-600)                             // 闪现cd
    {
        player->boostt +=1;                                                     // 充能数值
    }
    if (player->boostt>50)                                                      // 充能阈值
    {
        player->boost  = true;
    }

    if (IsKeyPressed(KEY_LEFT_SHIFT) && player->boost)                          //闪现
    {
        if (IsKeyDown(KEY_D))
        {
            player->position.x += 200;
            player->boostt = 0;
            player->boost  = false;
        }

        else if(IsKeyDown(KEY_A))
        {
            player->position.x -= 200;
            player->boostt = 0;
            player->boost  = false;
        }
    }


    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && player->canJump) 
    {
        if(player->speedh>0)                        // 摩擦力
            player->speedh -= 0.9*G*delt;
        else if(player->speedh<0)
            player->speedh += 0.9*G*delt; 
        if(player->speedh<20 && player->speedh>-20) // 消除卡顿
        player->speedh = 0;
    } 

    if (player->speedh > 350  ) player->speedh = 350;
    if (player->speedh < -350 ) player->speedh = -350;  // 限速

 // 碰撞检测-----------------------------------------------------------------------------
    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;                                 // 指针指向第i个元素
        Vector2 *p = &(player->position);

        if (ei->blocking && 
            ei->rect.x <= p->x &&                                   // 左边缘
            ei->rect.x + ei->rect.width >= p->x-40 &&               // 右边缘
            ei->rect.y >= p->y &&                                   // 玩家在上方
            ei->rect.y < p->y + player->speed*delt)                 // 玩家下一帧在下方
        {
            hitObstacle = 1;                                        // 碰撞
            player->speed = 0.0f;                                   // 如果碰撞，竖直速度为0
            p->y = ei->rect.y; 
            player->canJump =true; 
            player->jumpt=2;                                        // 将玩家的y坐标设置为碰撞物体的y坐标
            player->ajumpt = 0;                                     // 重置 空跳计时器
                                     
            if (IsKeyDown(KEY_S))                                   // 下台阶
            {player->speedh = 0;
            p->y += 1;}
        }
    }

    if (!hitObstacle)                                               // 如果没有碰撞到环境物体 
    {
        player->position.y += player->speed*delt;                   // 竖直位移
        
        if (player->speed>=0)                                       // 快速下落
        {
            player->speed += 2.5*G*delt;  
            player->canJump = false; 
            player->ajumpt ++; 
        }

        else if(player->speed<0)                                    // 
        {
            if (IsKeyDown(KEY_SPACE) && player->jumpt==1)           // 长按检测 跳得更高
            {
                player->speed += G*delt;
                player->canJump = false;
                player->ajumpt ++;                                   // 不可以跳跃
            }

            else
            {
                player->speed += 2.2*G*delt;                        // 正常G
                player->canJump = false;                            // 不可以跳跃
                player->ajumpt ++; 
            }
        }                    
    }

    if (player->position.y>1000 || player->position.x<-500  || player->position.x>15000)   // 重生
    {    
        player->position.y = -100;
        player->position.x = 500;
        player->speed = 0;
        player->speedh = 0;
    }

        // Player shoot logic
            if (IsKeyPressed(KEY_J))
            {
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (!shoot[i].active)
                    {
                        shoot[i].position = (Vector2){ player->position.x-20 , player->position.y-20 };
                        shoot[i].active = true;
                        shoot[i].speed.x = player->towards *10;
                        break;
                    }
                }
            }

            // Shoot life timer
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active) shoot[i].lifeSpawn++;
            }

            // Shot logic
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    // Movement
                    shoot[i].position.x += shoot[i].speed.x;
                }
            }






                    
//                    // Collision logic: shoot vs walls
//                    if  (shoot[i].position.x > screenWidth + shoot[i].radius)
//                    {
//                        shoot[i].active = false;
//                        shoot[i].lifeSpawn = 0;
//                    }
//                    else if (shoot[i].position.x < 0 - shoot[i].radius)
//                    {
//                        shoot[i].active = false;
//                        shoot[i].lifeSpawn = 0;
//                    }
//                    if (shoot[i].position.y > screenHeight + shoot[i].radius)
//                    {
//                        shoot[i].active = false;
//                        shoot[i].lifeSpawn = 0;
//                    }
//                    else if (shoot[i].position.y < 0 - shoot[i].radius)
//                    {
//                        shoot[i].active = false;
//                        shoot[i].lifeSpawn = 0;
//                    }
//
//                    // Life of shoot
//                    if (shoot[i].lifeSpawn >= 60)
//                    {
//                        shoot[i].position = (Vector2){0, 0};
//                        shoot[i].speed = (Vector2){0, 0};
//                        shoot[i].lifeSpawn = 0;
//                        shoot[i].active = false;
//                    }
            //     }
            // }



}
//--------------------------------------------------------------------------------------






void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delt, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;                              

    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delt/length));
    }
}