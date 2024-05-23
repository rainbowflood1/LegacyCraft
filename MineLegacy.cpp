#include "raylib.h"
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <vector>
#include "rlgl.h"
#include <map>
#include <iostream>
#include <string>


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

static int SEED = 0;

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y, int SEED = SEED)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

float noise2d(float x, float y, int seed = SEED)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int, seed);
    int t = noise2(x_int+1, y_int, seed);
    int u = noise2(x_int, y_int+1, seed);
    int v = noise2(x_int+1, y_int+1, seed);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth, int seed = SEED)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya, seed) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}

bool isInteger(double N)
{
 
    // Convert float value
    // of N to integer
    int X = N;
 
    double temp2 = N - X;
 
    // If N is not equivalent
    // to any integer
    if (temp2 > 0) {
        return false;
    }
    return true;
}

float lerp(float a, float b, float f)
{
    return a + (b - a) * f;
}






double randomGradientX[100][100];
double randomGradientY[100][100];

void initGradients(int gridSize, unsigned int seed) {
    srand(seed); // Initialize random number generator with seed
    int i, j;
    for (i = 0; i < gridSize; i++) {
        for (j = 0; j < gridSize; j++) {
            randomGradientX[i][j] = (double)rand() / RAND_MAX * 2 - 1;
            randomGradientY[i][j] = (double)rand() / RAND_MAX * 2 - 1;
        }
    }
}

double dotProduct(double x1, double y1, double x2, double y2) {
    return x1 * x2 + y1 * y2;
}

double smoothStep(double t) {
    return t * t * (3 - 2 * t);
}

double noise(int x, int y, int gridSize) {
    int x0 = x % gridSize;
    int y0 = y % gridSize;
    int x1 = (x0 + 1) % gridSize;
    int y1 = (y0 + 1) % gridSize;

    double sx = (double)(x % gridSize) / gridSize;
    double sy = (double)(y % gridSize) / gridSize;

    double n0, n1, ix0, ix1, value;
    n0 = dotProduct(randomGradientX[x0][y0], randomGradientY[x0][y0], sx, sy);
    n1 = dotProduct(randomGradientX[x1][y0], randomGradientY[x1][y0], sx - 1, sy);
    ix0 = smoothStep(sx);
    ix1 = smoothStep(sx - 1);
    value = n0 * (1 - ix0) + n1 * ix0;

    n0 = dotProduct(randomGradientX[x0][y1], randomGradientY[x0][y1], sx, sy - 1);
    n1 = dotProduct(randomGradientX[x1][y1], randomGradientY[x1][y1], sx - 1, sy - 1);
    n0 = n0 * (1 - ix0) + n1 * ix0;

    return value;
}

double fractalNoise(double x, double y, int gridSize, int octaves, double persistence, unsigned int seed) {
    initGradients(gridSize, seed); // Initialize random gradients with seed

    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxNoiseValue = 0;

    int i;
    for (i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency, gridSize) * amplitude;
        maxNoiseValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxNoiseValue;
}






float TPS = 20;

int take_damage(int fall_time) {
    return fall_time/20 * 2.0;
}

int main(int argc, char *argv[]) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    int world_size = 862;

    

    InitWindow(screenWidth, screenHeight, "MineLegacy");
    InitAudioDevice();
    
    

    Camera3D camera = { 0 };
    
    camera.target = (Vector3){ 0.0f, camera.position.y, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    int cameraMode = CAMERA_FIRST_PERSON;

    DisableCursor();
 
    SetTargetFPS(60);
	srand(time(0)); 
	int song_chosen = rand() % 4;
    SEED = rand();
    float bar_transparent = 1;
    Music battle1 = LoadMusicStream("./Pack_default/audio/battle_mode_1.mp3");
    Music battle2 = LoadMusicStream("./Pack_default/audio/battle_mode_2.mp3");
    Music battle3 = LoadMusicStream("./Pack_default/audio/battle_mode_3.mp3");
    Music battle4 = LoadMusicStream("./Pack_default/audio/battle_mode_4.mp3");
    Music nimbly_does_it = LoadMusicStream("./Pack_default/audio/nimbly_does_it.mp3");
    Music double_time = LoadMusicStream("./Pack_default/audio/double_time.mp3");
    
    Sound water_splash = LoadSound("Pack_default/audio/heavy_splash.mp3");
    Sound death = LoadSound("Pack_default/audio/death.mp3");
    Sound respawn = LoadSound("Pack_default/audio/respawn.mp3");
    Sound hit = LoadSound("Pack_default/audio/hit.wav");
    Sound fallsmall = LoadSound("Pack_default/audio/fallsmall.mp3");
    Sound water_swim1 = LoadSound("Pack_default/audio/swim1.mp3");
    
    // Footsteps
    
    Sound grass1 = LoadSound("Pack_default/audio/grass1.mp3");
    Sound grass2 = LoadSound("Pack_default/audio/grass2.mp3");
    Sound grass3 = LoadSound("Pack_default/audio/grass3.mp3");
    Sound grass4 = LoadSound("Pack_default/audio/grass4.mp3");
    Sound grass5 = LoadSound("Pack_default/audio/grass5.mp3");
    Sound grass6 = LoadSound("Pack_default/audio/grass6.mp3");
    
	Music song[] = {battle1, battle2, battle3, battle4};
    
    Font minfont = LoadFontEx("Pack_default/fonts/minecraft_font.ttf", 16, 0, 250);
    
    Model block = LoadModel("Pack_default/Models/cube.obj");
    Model plane = LoadModel("Pack_default/Models/plane.obj");
    Model UVmapped_cube = LoadModel("Pack_default/Models/UVmapped_cube.obj");
    
    
    // Material textures
    Texture2D brick = LoadTexture("Pack_default/texture/brick.png");
    Texture2D sand = LoadTexture("Pack_default/texture/sand.png");
    Texture2D stone = LoadTexture("Pack_default/texture/stone.png");
    Texture2D iron_ore = LoadTexture("Pack_default/texture/iron_ore.png");
    Texture2D snow = LoadTexture("Pack_default/texture/snow.png");
    Texture2D grass_blockUV = LoadTexture("Pack_default/texture/grass_blockUV.png");
    Texture2D no_texture = LoadTexture("Pack_default/texture/white.png");
    
    
    
    Texture2D crosshair = LoadTexture("Pack_default/texture/crosshairs.png");
    
    
    
    

    
    
    
    Image heart  = LoadImage("Pack_default/texture/Heart.png");
    Image half_heart  = LoadImage("Pack_default/texture/half_heart.png");
    Image water_image = LoadImage("Pack_default/texture/water.png");
    Image hunger_image = LoadImage("Pack_default/texture/hunger.png");
    Image hunger_half_image  = LoadImage("Pack_default/texture/hunger_half.png");
    //ImageColorTint(&water_image, {255, 255, 255, bar_transparent});
    //ImageColorTint(&hunger_image, {255, 255, 255, bar_transparent});
    //ImageColorTint(&hunger_half_image, {255, 255, 255, bar_transparent});
    Image grass = LoadImage("Pack_default/texture/grass_top.png");
    ImageColorTint(&grass, {100, 255, 100, 255});
    Image bar = LoadImage("Pack_default/texture/bar.png");
    Image swamp_grass_blockUV  = LoadImage("Pack_default/texture/grass_blockUV.png");
    Image savanna_grass_blockUV  = LoadImage("Pack_default/texture/grass_blockUV.png");
    Image craftingUI_backround = LoadImage("Pack_default/texture/demo_background.png");
    
    
    ImageResizeNN(&craftingUI_backround, craftingUI_backround.width*4, craftingUI_backround.height*4);
    
    
    
    // Health
    ImageResizeNN(&heart, heart.width*4, heart.height*4);
    ImageResizeNN(&half_heart, heart.width*1, heart.height*1);
    ImageResizeNN(&hunger_image, heart.width*1, heart.height*1);
    ImageResizeNN(&hunger_half_image, heart.width*1, heart.height*1);
    
    
    
    
    
    ImageColorTint(&heart, {255, 255, 255, 255});
    ImageColorTint(&hunger_half_image, {255, 255, 255, 255});
    
    Texture2D heart_texture = LoadTextureFromImage(heart);
    
    ImageColorTint(&half_heart, {255, 255, 255, 255});
    
    Texture2D half_heart_texture = LoadTextureFromImage(half_heart);
    Image item_selector  = LoadImage("Pack_default/texture/item_selector.png");
    


    Texture2D hunger_texture = LoadTextureFromImage(hunger_image);
    
    
    //ImageColorTint(&bar, {255, 255, 255, 155});
    ImageResizeNN(&bar, bar.width*5, bar.height*5);
    
    ImageResizeNN(&item_selector, item_selector.width*5, item_selector.height*5);
    
    Texture2D water = LoadTextureFromImage(water_image);
    Texture2D item_selector_texture = LoadTextureFromImage(item_selector);
    Texture2D bar_texture = LoadTextureFromImage(bar);
    Texture2D grass_texture = LoadTextureFromImage(grass);
    Texture2D hunger_half_texture = LoadTextureFromImage(hunger_half_image);
    
    ImageColorTint(&swamp_grass_blockUV, {200, 255, 200, 255});
    ImageColorTint(&savanna_grass_blockUV, {255, 222, 0, 255});
    
    
    Texture2D swamp_grass_blockUV_texture = LoadTextureFromImage(swamp_grass_blockUV);
    Texture2D savanna_grass_blockUV_texture = LoadTextureFromImage(savanna_grass_blockUV);
    Texture2D craftingUI_backround_texture = LoadTextureFromImage(craftingUI_backround);
    
    UnloadImage(heart);
    UnloadImage(half_heart);
    UnloadImage(hunger_image);
    UnloadImage(bar);
    UnloadImage(water_image);
    UnloadImage(grass);
    UnloadImage(swamp_grass_blockUV);
    UnloadImage(savanna_grass_blockUV);
    UnloadImage(item_selector);
    UnloadImage(craftingUI_backround);
    
    Vector3 death_postition = (Vector3) {0, 0, 0};


    // System
    int time_played = 0;
    bool death_sound = true;

    // Command World Arguments
    int seed_temp = SEED+rand()*rand();
    if (argv[1] == NULL || (int(*argv[1]) - 48) == 0) {

    } else {
        SEED = int(*argv[1]) - 48;
        int seed_temp = SEED+SEED*2;
    }
    
    // Biome temp
    float desert[] = {0.6, 1.0};
    float swamp[] = {0.2, 0.4};
    float savanna[] {0.5, 0.6};
    float snowy[] {0.2, 0.3};


    // World options
    int water_level = 15;
    
    float freq = 0.010;
    float depth = 5;
    int height = 30;
    
    if (argv[2] == NULL || (int(*argv[2]) - 48) == 0) {

    } else {
        water_level = std::stoi(argv[2]);
    }
    
    
    // Player variables
    int air_time = 0;
    bool in_water = false;
    bool sprint = false;
    bool swim = false;
    bool died = false;
    bool craft = false;
    bool jump = false;
    bool spawned = true;
    int air = 10;
    double jump_power = 0.05;
    double max_jump_power = 0.05;
    int hunger = 20;
    int max_health = 20;
    int health = max_health;
    int stopwatch = 0;
    int item_selector_x = screenWidth/2 - bar_texture.width/2;
    bool used_hotbar = false;
    int footstep_stopwatch = 0;
    int hotbar_item_slot = 1;
    int messages = 0;

    
    // You died screen
    float tick = 0.001;
    int value_max = 0.25;
    float value = 0;
    
    // Item selector adjustment
    int item_selector_width = item_selector.width - 10;
    
    
    // Player spawn
    camera.position = (Vector3){ world_size/2, 10+water_level, world_size/2 };
    
    
    

    // Remove extra faces
    rlEnableBackfaceCulling();
    
    
    // Choose random soundtrack
    PlayMusicStream(song[song_chosen]);
    
    in_water = false;
    
    //  Instructions in the top, right bar
    const char* desc = "";
    
    
    while (!WindowShouldClose())
    {
        // Move item selector
        item_selector_x -= (GetMouseWheelMove()*item_selector_width);
        hotbar_item_slot -= (GetMouseWheelMove()*item_selector_width)*0.01;
        if (hotbar_item_slot >= 10) {
            hotbar_item_slot = 1;
            item_selector_x = screenWidth/2 - bar_texture.width/2;
        } else if (hotbar_item_slot <= 0) {
            item_selector_x += item_selector_width*9;
            hotbar_item_slot = 9;
        }
        // Update
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        UpdateMusicStream(song[song_chosen]);
        time_played++;
        stopwatch++;
        footstep_stopwatch++;
        
        
        // Hotbar animation
        if (GetMouseWheelMove() != 0) {
            used_hotbar = true;
        }
        if (GetMouseWheelMove() == 0) {
            used_hotbar = false;
        }
        if (used_hotbar == true) {
            bar_transparent = 1;
        } else {
            bar_transparent = lerp(bar_transparent, 0.25, 0.01);
        }
        
        // Play footsteps on the amount of frames
        if (footstep_stopwatch >= 20) {
            footstep_stopwatch = 0;
        //    PlaySound(grass1);
        }
        
        // Collision system (uses world hightmap as data)
        if (camera.position.y <= std::round((perlin2d(camera.position.x, camera.position.z, freq, depth)*height))+2.0f and swim == false and died == false) {
            camera.position.y = std::round((perlin2d(camera.position.x, camera.position.z, freq, depth)*height))+2.0f;
            // Fall damage
            if (take_damage(air_time) >= 3 and spawned == false) {
                health -= take_damage(air_time);
                bar_transparent = 1;
                PlaySound(hit);
                PlaySound(fallsmall);
            }
            // When delt fall damage reset air_time
            spawned = false;
            air_time = 0;
            jump_power = max_jump_power;
        } else if (camera.position.y <= std::round((perlin2d(camera.position.x, camera.position.z, freq, depth)*height))+1.0f and swim == true and died == false) {
            spawned = false;
            camera.position.y = std::round((perlin2d(camera.position.x, camera.position.z, freq, depth)*height))+1.0f;
            jump_power = max_jump_power;
        } else {
            if (camera.position.y <= water_level + 2 and camera.position.y >= water_level - 0.5) {
                PlaySound(water_splash);
                in_water = true;
                air_time = 0;
            }
            if (camera.position.y <= water_level+1) {
                camera.position.y -= jump_power;
                jump_power += 0.00001;
            } 
            if (camera.position.y >= water_level){
                in_water = false;
                swim = false;
                cameraMode = CAMERA_FIRST_PERSON;
                camera.position.y -= jump_power;
                air_time++;
                jump_power += 0.001;
            }
            

        }
        UpdateCamera(&camera, cameraMode);
        if (IsKeyPressed('E')){
            craft = !craft;
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        if (IsKeyPressed('R')){
            if (died == true) {
                spawned = true;
                PlaySound(respawn);
                bar_transparent = 1;
                air_time = 0;
                camera.fovy = lerp(camera.fovy, 70, 0.005);
                camera.position = (Vector3){ world_size/2, 10+water_level, world_size/2 };
                health = max_health;
                death_sound = true;
                hunger = 20;
                died = false;
                value = 0;
            }
        }
        if (IsKeyPressed(KEY_LEFT_SHIFT)){
            sprint = !sprint;
            if (in_water == true) {
                cameraMode = CAMERA_FREE;
                PlaySound(water_swim1);
                swim = true;
            } else if (in_water == false) {
                cameraMode = CAMERA_FIRST_PERSON;
                swim = false;
            }
        }

        if (IsKeyPressed(KEY_SPACE)){
            if (in_water == true and swim == true) {
                desc = "Press Space to swim up";
            }
        }
        
        if (sprint == true and hunger >= 6) {
            camera.fovy = lerp(camera.fovy, 90, 0.1);  
        } else {
            camera.fovy = lerp(camera.fovy, 70, 0.1);
        }
        
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            sprint = true;
        } else {
            sprint = false;
        }

        
        BeginDrawing();

            ClearBackground(SKYBLUE);

            if (IsCursorHidden()) UpdateCamera(&camera, cameraMode);


            int chunk_size = 16*2;


            BeginMode3D(camera);
                
                
                
                // Terrain Generation
                bool uv_cube = false;
                for (int x = std::round(camera.position.x) - chunk_size/2; x < std::round(camera.position.x) + chunk_size and x < world_size; x++) {
                    for (int z = std::round(camera.position.z) - chunk_size/2; z < std::round(camera.position.z) + chunk_size and z < world_size; z++) {
                        if (std::round(perlin2d(x, z, freq, depth)*height) <= 0.35*height) {
                            block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = no_texture;
                            DrawModel(block, {x, 50, z}, 0.5f, WHITE);
                        }
                        if (true) {
                            block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass_texture;
                            UVmapped_cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass_blockUV;
                            uv_cube = true;
                        }
                        if (std::round(perlin2d(x, z, freq, depth)*height) <= water_level) {
                            plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;
                            DrawModel(plane, {x, water_level+1-(0.8/16), z}, 0.5f, WHITE);
                            if (std::round(perlin2d(x, z, freq, depth)*height) <= water_level-1) {
                                block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sand;
                                uv_cube = false;
                            } else {
                                if (perlin2d(x, z, freq, depth, seed_temp) >= desert[0] and perlin2d(x, z, freq, depth, seed_temp) <= desert[1]) {
                                    block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sand;
                                    uv_cube = false;
                                }
                                
                                if (perlin2d(x, z, freq, depth, seed_temp) >= swamp[0] and perlin2d(x, z, freq, depth, seed_temp) <= swamp[1]) {
                                    UVmapped_cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = swamp_grass_blockUV_texture;
                                    uv_cube = true;
                                }
                                if (perlin2d(x, z, freq, depth, seed_temp) >= savanna[0] and perlin2d(x, z, freq, depth, seed_temp) <= savanna[1]) {
                                    UVmapped_cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = savanna_grass_blockUV_texture;
                                    uv_cube = true;
                                }
                                
                                if (perlin2d(x, z, freq, depth, seed_temp) >= snowy[0] and perlin2d(x, z, freq, depth, seed_temp) <= snowy[1]) {
                                    block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = snow;
                                    uv_cube = false;
                                }
                            }
                            
                        } else {
                            if (perlin2d(x, z, freq, depth, seed_temp) >= desert[0] and perlin2d(x, z, freq, depth, seed_temp) <= desert[1]) {
                                block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sand;
                                uv_cube = false;
                            }
                            
                            if (perlin2d(x, z, freq, depth, seed_temp) >= swamp[0] and perlin2d(x, z, freq, depth, seed_temp) <= swamp[1]) {
                                UVmapped_cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = swamp_grass_blockUV_texture;
                                uv_cube = true;
                            }
                            if (perlin2d(x, z, freq, depth, seed_temp) >= savanna[0] and perlin2d(x, z, freq, depth, seed_temp) <= savanna[1]) {
                                UVmapped_cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = savanna_grass_blockUV_texture;
                                uv_cube = true;
                            }
                            
                            if (perlin2d(x, z, freq, depth, seed_temp) >= snowy[0] and perlin2d(x, z, freq, depth, seed_temp) <= snowy[1]) {
                                block.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = snow;
                                uv_cube = false;
                            }
                       
                        }

                        if (uv_cube == true) {
                            DrawModel(UVmapped_cube, {x, std::round(perlin2d(x, z, freq, depth)*height), z}, 0.5f, WHITE);
                        } else {
                            DrawModel(block, {x, std::round(perlin2d(x, z, freq, depth)*height), z}, 0.5f, WHITE);
                        }

                        
                    }
                }

            EndMode3D();


            // Chat
            


            // Gamma
            
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(WHITE, 0.50));


            // GUI
            DrawTexture(crosshair, crosshair.width + screenWidth/2, crosshair.height + screenHeight/2, WHITE);

            DrawRectangle(screenWidth-300, 50, 290, 145, Fade(DARKBLUE, 1));
            DrawTextEx(minfont, desc, (Vector2){ screenWidth-290, 50.0f }, (float)minfont.baseSize, 2, RAYWHITE);
            //DrawTextEx(minfont, "hi!", (Vector2){ screenWidth-290, 65.0f }, (float)minfont.baseSize, 2, RAYWHITE);
            DrawText(TextFormat("- Position: (%d, %d, %d)", (int) std::round(camera.position.x), (int) std::round(camera.position.y), (int) std::round(camera.position.z)), 610, 60, 10, BLACK);
            DrawText(TextFormat("- Hunger: (%d)", hunger), 610, 70, 10, BROWN);
            DrawText(TextFormat("- Fall Time: (%d)", hotbar_item_slot), 610, 80, 10, RED);

            
            if (stopwatch >= 60) {
                stopwatch = 0;
                if (health <= 19 and died == false) {
                    health += 1;
                }
            }
            
            
            DrawRectangleLines(screenWidth-300, 50, 290, 145, RAYWHITE);
            DrawTexture(bar_texture, screenWidth/2 - bar_texture.width/2, screenHeight - bar_texture.height*2.5, Fade(WHITE, bar_transparent));
            
            
            // Selector
            
            DrawTexture(item_selector_texture, item_selector_x, screenHeight - bar_texture.height*2.5, Fade(WHITE, bar_transparent));
            
          
            
            // Hunger
            
            for (int i = 0; i < std::round(hunger/2); i++) {
                if (hunger % 2 == 0) {
                    DrawTexture(hunger_texture, ((screenWidth/2 - bar_texture.width/2) + -i*hunger_texture.width + bar_texture.width) - hunger_texture.width, (screenHeight - bar_texture.height*2.5) - hunger_texture.height, Fade(WHITE, bar_transparent));   
                } else {
                    if (i != (hunger/2)) {
                        DrawTexture(hunger_texture, ((screenWidth/2 - bar_texture.width/2) + -i*hunger_texture.width + bar_texture.width) - hunger_texture.width, (screenHeight - bar_texture.height*2.5) - hunger_texture.height, Fade(WHITE, bar_transparent));
                        if (i == (hunger/2)-1) {
                            i++;
                        }

                    }
                    
                    if (i == (hunger/2)-1) {
                        DrawTexture(hunger_half_texture, ((screenWidth/2 - bar_texture.width/2) + -i*hunger_texture.width + bar_texture.width) - hunger_texture.width, (screenHeight - bar_texture.height*2.5) - hunger_texture.height, Fade(WHITE, bar_transparent));

                    }




                }
                
                



            
            }
            
            for (int i = 0; i < std::round(health/2); i++) {
                if (health % 2 == 0) {
                    DrawTexture(heart_texture, (screenWidth/2 - bar_texture.width/2) + i*heart_texture.width, (screenHeight - bar_texture.height*2.5) - heart_texture.height, Fade(WHITE, bar_transparent));   
                } else {
                    if (i != (health/2)) {
                        DrawTexture(heart_texture, (screenWidth/2 - bar_texture.width/2) + i*heart_texture.width, (screenHeight - bar_texture.height*2.5) - heart_texture.height, Fade(WHITE, bar_transparent));
                    }
                    if (i == (health/2)-1) {
                        DrawTexture(half_heart_texture, (screenWidth/2 - bar_texture.width/2) + (i*heart_texture.width)+heart_texture.width, (screenHeight - bar_texture.height*2.5) - heart_texture.height, Fade(WHITE, bar_transparent));
                    }
                }
                
                
                
            }
            if (died == false) {
                death_postition = camera.position;
            }
            
            if (health <= 0) {
                jump_power = 0.0000;
                died = true;
                if (death_sound == true) {
                    PlaySound(death);
                    death_sound = false;
                }
                
                camera.position = death_postition;
                tick = 0.1;
                sprint = false;
                value = lerp(value , 0.75, tick);
                camera.fovy = lerp(camera.fovy, 10, 0.01);
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade({255, 0, 0, 255}, value));

                // Chat
                DrawRectangle(0, screenHeight * 0.6, screenWidth / 2, 50, Fade(BLACK, 0.50));
                DrawTextEx(minfont, "<player> hit the ground too hard", (Vector2){ 0, (screenHeight * 0.6) + 10 }, 25.0, 1, RAYWHITE);
                DrawTextEx(minfont, "You died!", (Vector2){(screenWidth/2)-minfont.baseSize*2, screenHeight/3}, (float)minfont.baseSize*2, 2, RAYWHITE);
                
            }
            if (craft == true) {
                DrawTexture(craftingUI_backround_texture, craftingUI_backround_texture.width/2, 10, WHITE);

                
                
                
                
                
                
                
            }

            
            


        EndDrawing();
    }

    UnloadMusicStream(battle1);
    UnloadMusicStream(battle2);
    UnloadMusicStream(battle3);
    UnloadMusicStream(battle4);

    UnloadTexture(bar_texture);
    UnloadTexture(grass_texture);
    UnloadTexture(crosshair);
    UnloadTexture(water);
    UnloadTexture(brick);
    UnloadTexture(sand);
    UnloadTexture(stone);
    UnloadTexture(grass_blockUV);
    UnloadTexture(iron_ore);
    UnloadTexture(heart_texture);
    UnloadTexture(half_heart_texture);
    UnloadTexture(swamp_grass_blockUV_texture);
    UnloadTexture(savanna_grass_blockUV_texture);
    UnloadTexture(hunger_texture);
    UnloadTexture(hunger_half_texture);
    UnloadTexture(item_selector_texture);
    UnloadTexture(craftingUI_backround_texture);
    UnloadTexture(snow);
    
    UnloadModel(block);
    UnloadModel(plane);
    UnloadModel(UVmapped_cube);
    UnloadSound(water_splash);
    UnloadSound(hit);
    UnloadSound(fallsmall);
    UnloadSound(water_swim1);
    UnloadSound(death);
    UnloadSound(respawn);

    UnloadSound(grass1);
    UnloadSound(grass2);
    UnloadSound(grass3);
    UnloadSound(grass4);
    UnloadSound(grass5);
    UnloadSound(grass6);












    CloseAudioDevice();

    CloseWindow();

    return 0;
}
