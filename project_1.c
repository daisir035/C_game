#include "raylib.h"
#include "raymath.h"

#define G 400
#define PLAYER_JUMP_SPD 350.0f // 跳跃速度
#define PLAYER_HOR_SPD 200.0f // 地速
#define PLAYER_MAX_SHOOTS   10
typedef struct Player {
    Vector2 position; // 位置
    float speed;
    float speedh;
    int jumpt;
    int ajumpt;
    int boostt;
    bool canJump;
    bool boost;
    Color Color;
    int towards;  
} Player;
typedef struct Shoot {
    Vector2 position;
    Vector2 speed;
    float radius;
    int lifetime;
    bool out;
    Color color;
} Shoot;

typedef struct EnvItem { 
    Rectangle rect; // 矩形
    int blocking; // 是否阻挡
    Color color; // 颜色
} EnvItem;

const int screenWidth = 1300;
const int screenHeight = 800; //窗口尺寸
Shoot shoot[PLAYER_MAX_SHOOTS] = { 0 };
Shoot shoot2[PLAYER_MAX_SHOOTS] = { 0 };

void UpdatePlayer(Player *player, EnvItem *envItems, Shoot *shoot, Shoot *shoot2,int envItemsLength,int shootLength, float delt);
void UpdatePlayer2(Player *player2, EnvItem *envItems, Shoot *shoot, Shoot *shoot2, int envItemsLength,int shootLength2, float delt);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delt, int width, int height);

int main(void)
{
    // Initialization
 //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "game");              //初始化窗口
    SetTargetFPS(60);                                           //设置帧率

    Image bgImage = LoadImage("33.png");                        // 背景纹理
    Texture2D bgTexture = LoadTextureFromImage(bgImage);  
    UnloadImage(bgImage);

    //Vector2 aimPosition = { 0, 0 };                                       // 准星位置
    //int currentGesture = GESTURE_NONE;                                    // 当前手势
    Player player = { 0 };
    player.position = (Vector2){ 350, 280 };
    player.speed = 0;
    player.canJump = 2;
    player.towards = 1;

    Player player2 = { 0 };
    player2.position = (Vector2){ 700, 280 };
    player2.speed = 0;
    player2.canJump = 2;
    player2.towards = 1;

    // Initialization shoot 子弹

    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = (Vector2){0, 0};
        shoot[i].speed = (Vector2){0, 0};
        shoot[i].radius = 5;
        shoot[i].out = false;
        shoot[i].lifetime = 0;
        shoot[i].color = GREEN;
    }

    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot2[i].position = (Vector2){0, 0};
        shoot2[i].speed = (Vector2){0, 0};
        shoot2[i].radius = 5;
        shoot2[i].out = false;
        shoot2[i].lifetime = 0;
        shoot2[i].color = GREEN;
    }
    //

    EnvItem envItems[] = {
        //{{ 0, 0, 1000, 400 }, 0, LIGHTGRAY }, // x起点 y起点 长 宽 碰撞   地图数据
        {{ 0, 400, 1000, 10 }, 1, GRAY },     //地面
        {{ 300, 200, 400, 10 }, 1, GRAY },    // 上
        {{ 250, 300, 100, 10 }, 1, GRAY },    // 左
        {{ 650, 300, 100, 10 }, 1, GRAY },
        {{ 1050, 300, 100, 10 }, 1, GRAY },
        {{ 1450, 300, 100, 10 }, 1, GRAY },   // 右
    };                                      

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]); //sizeo获取数据在内存中占用的字节数 有多少个元素
    int shootLength = sizeof(shoot)/sizeof(shoot[0]);

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

 //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();                                   // Get time in seconds for last frame drawn (delta time) 一帧时间

        UpdatePlayer(&player, envItems, shoot, shoot2, envItemsLength, shootLength, deltaTime);
        UpdatePlayer2(&player2, envItems, shoot, shoot2, envItemsLength, shootLength, deltaTime);

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);                  //滚轮缩放视角

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;                   //缩放极限

        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            player.position = (Vector2){ 400, 280 };                        // 重置位置
        }

        UpdateCameraCenterSmoothFollow(&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY); //清屏

            BeginMode2D(camera);

                DrawTexture(bgTexture, -5000, -2000, WHITE);           // 背景
                for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color); // 画地图

                Rectangle playerRect = { player.position.x - 40, player.position.y - 40, 40, 40 }; //画方块
                DrawRectangleRec(playerRect, RED);

                Rectangle player2Rect = { player2.position.x - 40, player2.position.y - 40, 40, 40 };
                DrawRectangleRec(player2Rect, GREEN);

            
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (shoot[i].out) DrawCircleV(shoot[i].position, shoot[i].radius, YELLOW);
                }

                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (shoot2[i].out) DrawCircleV(shoot2[i].position, shoot2[i].radius, BLUE);
                }

            EndMode2D();

            DrawText("Good luck", 20, 20, 20, BLACK);                                   //ui文字 坐标(左上原点),字号, 颜色,ui文字
            DrawText(TextFormat("x = %2.3f", player.position.x), 40 , 200, 20, BLACK);
            DrawText(TextFormat("y = %2.3f", player.position.y), 40 , 240, 20, BLACK);
            DrawText(TextFormat("x speed = %f", player.speedh), 40 , 280, 20, BLACK);
            DrawText(TextFormat("can jump = %d", player.canJump), 40 , 320, 20, BLACK);
            DrawText(TextFormat("jumpt = %d", player.jumpt), 40 , 360, 20, BLACK);
            DrawText(TextFormat("BOOST = %d", player.boostt),40, 380,20, BLACK);
            DrawText(TextFormat("ajumpt = %d",player.ajumpt),40, 400,20, BLACK);

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


            DrawRectangleLines(550,75, 80, 20,BLACK);                               
            if (IsKeyDown(KEY_LEFT_SHIFT)) DrawRectangle(550,75, 80, 20,DARKGRAY);

            DrawRectangle(380,630, 190, 160,LIGHTGRAY);
            DrawRectangle(510,650, 40,40,RED);
            DrawText("PLAYER", 400, 650, 20, BLACK);
            DrawText("overheat", 400, 750, 20, BLACK); 
            DrawRectangleLines(500,750, 20, 20,BLACK);            
            if (shoot[9].out == true && shoot[0].out == true) 
            DrawRectangle(500,750, 19, 19,ORANGE);                                  //枪口过热 
            DrawText("flash", 400, 700, 20, BLACK);   
            DrawRectangleLines(460,700, 20, 20,BLACK);
            if (player.boost) DrawRectangle(460,700, 19, 19,YELLOW);                      // 闪现指示器   
            
            DrawRectangle(680,630, 190, 160,LIGHTGRAY);
            DrawRectangle(810,650, 40,40,GREEN);
            DrawText("PLAYER2", 700, 650, 20, BLACK);
            DrawText("overheat", 700, 750, 20, BLACK); 
            DrawRectangleLines(800,750, 20, 20,BLACK);            
            if (shoot2[9].out == true && shoot2[0].out == true) 
            DrawRectangle(800,750, 19, 19,ORANGE);                                  //枪口过热 
            DrawText("flash", 700, 700, 20, BLACK);   
            DrawRectangleLines(760,700, 20, 20,BLACK);
            if (player2.boost) DrawRectangle(760,700, 19, 19,YELLOW);                      // 闪现指示器 
               
        

            

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    UnloadTexture(bgTexture); // Unload background texture
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, Shoot *shoot, Shoot *shoot2, int envItemsLength, int shootLength, float delt)
{   
    player->position.x += player->speedh*delt; // 水平移动

    if (IsKeyDown(KEY_A))                         
    {
        //if(player->speedh>0)
        //player->speedh = 0;                                                // 急停
        player->speedh -= G*delt; 
        player->towards = -1;                                               // 向左
    }

    if (IsKeyDown(KEY_D)) 
    {
        //if(player->speedh<0)
        //player->speedh = 0;                                                // 急停
        player->speedh += G*delt; 
        player->towards = 1;                                                // 向右
    }
    
    if (IsKeyPressed(KEY_SPACE) && player->jumpt>0 ) // 跳 空跳 二段跳 
    {
        if (player->jumpt==2)
        {
           if (player->ajumpt<10) 
            {
                player->speed = -PLAYER_JUMP_SPD;
                player->jumpt =1;
                // player->canJump = false;
            }

            else if(player->ajumpt>10)
            {
                player->speed = -PLAYER_JUMP_SPD;
                player->jumpt -=2;
            }
        }

        else if (player->jumpt==1)
        {
            player->speed = -0.8*PLAYER_JUMP_SPD;
            player->jumpt =0;
        }
    }

    player->boostt +=1;     // 闪现计时器
    if (player->boostt>50) player->boost  = true;// 闪现阈值

    if (IsKeyPressed(KEY_LEFT_SHIFT) && player->boost)  //闪现
    {
        if (IsKeyDown(KEY_D))
        {
            player->position.x += 200;
            player->boostt = 0;     //重置闪现cd
            player->boost  = false; //关闭闪现开关
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

    for (int i = 0; i < shootLength; i++)
    {
        Shoot *ei = shoot2 + i;                   
        Vector2 *p = &(player->position);

        if (ei->position.x > p->x-40 &&
            ei->position.x < p->x-20 &&    // 速度太快打不到
            ei->position.y > p->y-40 &&
            ei->position.y < p->y &&
            ei->speed.x > 0 &&
            shoot2[i].out )                 
        {
            p->x += 50;                     // 子弹击退
            player->speedh += 300;                  
              shoot2[i].out = false;      
        }

        if (ei->position.x > p->x-20 &&
            ei->position.x < p->x &&    // 速度太快打不到
            ei->position.y > p->y-40 &&
            ei->position.y < p->y &&
            ei->speed.x < 0 &&
            shoot2[i].out )                 
        {
            p->x -= 50;                     // 子弹击退
            player->speedh -= 300;          // 速度       
              shoot2[i].out = false;        // 子弹消失
        }        

    }

        // 射击
            if (IsKeyPressed(KEY_J))
            {
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (!shoot[i].out)                                                                  // 有子弹没射出
                    {
                        if (player->towards == 1)                                                       //朝向
                        {
                            shoot[i].position = (Vector2){ player->position.x+2 , player->position.y-20 };// 枪口位置
                        }
                        
                        else if (player->towards == -1)                                                  //朝向
                        {
                            shoot[i].position = (Vector2){ player->position.x-42 , player->position.y-20 };// 枪口位置
                        }
                        shoot[i].out = true;                                            
                        shoot[i].speed.x = player->towards *10;                                         // 子弹初速度
                        break;
                    }
                }
            }

            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].out) shoot[i].lifetime++;         // 子弹生命计数器
            }

            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].out)
                {
                    shoot[i].position.x += shoot[i].speed.x;    // 子弹移动
                }

                if (shoot[i].lifetime >= 200)                   // 超过射程 小时
                {
                    shoot[i].position = (Vector2){0, 0};
                    shoot[i].speed = (Vector2){0, 0};           // 实际仅有x速度
                    shoot[i].lifetime = 0;
                    shoot[i].out = false;
                }
                
            }
        //--------------------------------------------------------------------------------------------    
    if (player->position.y>1000 || player->position.x<-500  || player->position.x>15000)   // 重生
    {    
        player->position.y = -100;
        player->position.x = 500;
        player->speed = 0;
        player->speedh = 0;
        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) shoot[i].out = false;
    }            

}

void UpdatePlayer2(Player *player2, EnvItem *envItems,Shoot *shoot, Shoot *shoot2, int envItemsLength, int shootLength, float delt)
{   
    player2->position.x += player2->speedh*delt; // 水平移动

    if (IsKeyDown(KEY_LEFT))                         
    {
        //if(player2->speedh>0)
        //player2->speedh = 0;                                                 // 急停
        player2->speedh -= G*delt; 
        player2->towards = -1;                                               // 向左跑
    }

    if (IsKeyDown(KEY_RIGHT)) 
    {
        // player->position.x += player->speed*delt; 
        //if(player2->speedh<0)
        //player2->speedh = 0;                                                 // 急停
        player2->speedh += G*delt; 
        player2->towards = 1;                                                // 向右跑
    }
    
    if (IsKeyPressed(KEY_UP) && player2->jumpt>0 ) // 跳 空跳 二段跳 
    {
        if (player2->jumpt==2)
        {
           if (player2->ajumpt<10) 
            {
                player2->speed = -PLAYER_JUMP_SPD;
                player2->jumpt =1;
                // player->canJump = false;
            }

            else if(player2->ajumpt>10)
            {
                player2->speed = -PLAYER_JUMP_SPD;
                player2->jumpt -=2;
            }
        }

        else if (player2->jumpt==1)
        {
            player2->speed = -0.8*PLAYER_JUMP_SPD;
            player2->jumpt =0;
        }
    }

    if (player2->speed < 600 && player2->speed >-600)                             // 闪现cd
    {
        player2->boostt +=1;                                                     // 充能数值
    }
    if (player2->boostt>50)                                                      // 充能阈值
    {
        player2->boost  = true;
    }

    if (IsKeyPressed(KEY_KP_2) && player2->boost)                          //闪现
    {
        if (IsKeyDown(KEY_RIGHT))
        {
            player2->position.x += 200;
            player2->boostt = 0;
            player2->boost  = false;
        }

        else if(IsKeyDown(KEY_LEFT))
        {
            player2->position.x -= 200;
            player2->boostt = 0;
            player2->boost  = false;
        }
    }


    if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && player2->canJump) 
    {
        if(player2->speedh>0)                        // 摩擦力
            player2->speedh -= 0.9*G*delt;
        else if(player2->speedh<0)
            player2->speedh += 0.9*G*delt; 
        if(player2->speedh<20 && player2->speedh>-20) // 消除卡顿
        player2->speedh = 0;
    } 

    if (player2->speedh > 350  ) player2->speedh = 350;
    if (player2->speedh < -350 ) player2->speedh = -350;  // 限速

 // 碰撞检测-----------------------------------------------------------------------------
    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;                                 // 指针指向第i个元素
        Vector2 *p = &(player2->position);

        if (ei->blocking && 
            ei->rect.x <= p->x &&                                   // 左边缘
            ei->rect.x + ei->rect.width >= p->x-40 &&               // 右边缘
            ei->rect.y >= p->y &&                                   // 玩家在上方
            ei->rect.y < p->y + player2->speed*delt)                 // 玩家下一帧在下方
        {
            hitObstacle = 1;                                        // 碰撞
            player2->speed = 0.0f;                                   // 如果碰撞，竖直速度为0
            p->y = ei->rect.y; 
            player2->jumpt=2;                                        // 将玩家的y坐标设置为碰撞物体的y坐标
            player2->ajumpt = 0;                                     // 重置 空跳计时器
                                     
            if (IsKeyDown(KEY_DOWN))                                   // 下台阶
            {player2->speedh = 0;
            p->y += 1;}
        }
    }

    if (!hitObstacle)                                               // 如果没有碰撞到环境物体 
    {
        player2->position.y += player2->speed*delt;                   // 竖直位移
        player2->ajumpt ++; 

        if (player2->speed>=0)                                       // 快速下落
        {
            player2->speed += 2.5*G*delt;  
        }

        else if(player2->speed<0)                                    // 
        {
            if (IsKeyDown(KEY_UP)&& player2->jumpt==1)           // 长按检测 跳得更高
            {
                player2->speed += G*delt;                                 // 不可以跳跃
            }

            else
            {
                player2->speed += 2.2*G*delt;                        // 正常G                        // 不可以跳跃
            }
        }                    
    }

    for (int i = 0; i < shootLength; i++)
    {
        Shoot *ei = shoot + i;                                 // 指针指向第i个元素
        Vector2 *p = &(player2->position);

        if (ei->position.x > p->x-40 &&
            ei->position.x < p->x-20 &&    // 速度太快打不到
            ei->position.y > p->y-40 &&
            ei->position.y < p->y &&
            ei->speed.x > 0 &&
            shoot[i].out )                 
        {
            p->x += 50;                     // 子弹击退
            player2->speedh += 300;                  
              shoot[i].out = false;      
        }

        if (ei->position.x > p->x-20 &&
            ei->position.x < p->x &&    // 速度太快打不到
            ei->position.y > p->y-40 &&
            ei->position.y < p->y &&
            ei->speed.x < 0 &&
            shoot[i].out )                 // 玩家下一帧在下方
        {
            p->x -= 50;                     // 子弹击退
            player2->speedh -=300;                  
              shoot[i].out = false;      
        }        
    }
    if (player2->position.y>1000 || player2->position.x<-500  || player2->position.x>15000)   // 重生
    {    
        player2->position.y = -100;
        player2->position.x = 500;
        player2->speed = 0;
        player2->speedh = 0;
        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) shoot[i].out = false;
    }

        // Player shoot logic
            if (IsKeyPressed(KEY_KP_1))
            {
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (!shoot2[i].out)                                                                  // 有子弹没射出
                    {
                        if (player2->towards == 1)
                        {
                            shoot2[i].position = (Vector2){ player2->position.x+2 , player2->position.y-20 };
                        }
                        
                        else if (player2->towards == -1)
                        {
                            shoot2[i].position = (Vector2){ player2->position.x-42 , player2->position.y-20 };
                        }
                        shoot2[i].out = true;                                            
                        shoot2[i].speed.x = player2->towards *10;                                         // 子弹初速度
                        break;
                    }
                }
            }

            // Shoot life timer
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot2[i].out) shoot2[i].lifetime++;
            }

            // Shot logic
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot2[i].out)
                {
                    // Movement
                    shoot2[i].position.x += shoot2[i].speed.x;
                }

                // Life of shoot
                if (shoot2[i].lifetime >= 200)                   // 射程 换弹 200足够大
                {
                    shoot2[i].position = (Vector2){0, 0};
                    shoot2[i].speed = (Vector2){0, 0};
                    shoot2[i].lifetime = 0;
                    shoot2[i].out = false;
                }
                
            }
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delt, int width, int height)
{
    static float minSpeed = 50;
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
