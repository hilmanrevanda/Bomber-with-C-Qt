#include<SDL2/SDL.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_mixer.h>
#include<SDL2/SDL_TTF.h>
#include<stdlib.h>
#define BOM 3
#define MAKS 11
#define MAKSx2 18
#define Buang 1000
#define Geser 50
#undef main

const int W =1050, H = 750, FRAME_RATE = 25;

void error1(const char what[]) {
    printf("%s: %s\n",what ,SDL_GetError() );
    exit(1);
}

void door_random(){ // fungsi random
    int x = 1050, y = 750;
    int Pos[x][y];
}

long fsize(char file_Name[80]){
    FILE * fp = fopen( file_Name, "a+" );
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long end_Pos=ftell(fp);
    fseek(fp,prev,SEEK_SET);
    fclose(fp);
    return end_Pos;
}

int Peraturan(){ //fungsi FILE
long bf=fsize("peraturan.txt");
int n;
char line[100];
FILE *fr;
if (bf>0){
    fr = fopen ("peraturan.txt", "a+");
    while (fgets (line, sizeof line, fr) != NULL ){
        fputs (line, stdout);
    }
    fclose(fr);
}
else {
    printf("File kosong\n");
}
}

void position_init(SDL_Rect &pos,int x,int y,int w, int h){
    pos.x=x;
    pos.y=y;
    pos.w=w;
    pos.h=h;
}

enum Sound {SoundBombExplosion,SoundGameMenu,SoundGameStart,SoundTotal};
Mix_Chunk * audio[SoundTotal];

int main(int argc, int **argv) {
    int q, w, u; // untuk ngelooping
    puts( "Initializing SDL..\n\n\n" );
    Peraturan();
    if(SDL_Init(SDL_INIT_EVERYTHING)) error1("SDL_Init Error");
    SDL_Window *win = SDL_CreateWindow("Bomber Team", 100, 100, W, H, SDL_WINDOW_SHOWN);
    if (!win) error1("SDL_CreateWindow Error");
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!ren) error1("SDL_CreateRenderer Error");

    SDL_Texture * bg= IMG_LoadTexture(ren,"bg.jpg");
    if(!bg)error1("Fail to load");

    //Start menu
    SDL_Texture * bgstart= IMG_LoadTexture(ren,"cover.jpg");
    if(!bgstart)error1("Fail to load");

    //GameOver BG
    SDL_Texture * bggameover= IMG_LoadTexture(ren,"GameOver.jpg");
    if(!bggameover)error1("Fail to load");

    //Player
    int PlayerLove = 3;
    SDL_Texture * Player= IMG_LoadTexture(ren,"a.gif");
    if(!Player)error1("Fail to load");

    SDL_Rect Player_pos;
    position_init(Player_pos,50, 0,50,50);

    //Key
    SDL_Texture * Key= IMG_LoadTexture(ren,"Key.gif");
    if(!Key)error1("Fail to load");

    SDL_Rect Key_pos;
    position_init(Key_pos,850,400,50,50);

    //Crate
    SDL_Texture * Crate= IMG_LoadTexture(ren,"Crate.png");
    if(!Crate)error1("Fail to load");

    //Bomb
    SDL_Texture * Bomb= IMG_LoadTexture(ren,"Bomb.png");
    if(!Bomb)error1("Fail to load");

    SDL_Rect Bombpos[3];
    for(q = 0; q < 3; q++){
        position_init(Bombpos[q],-100,0,50,50);
    }

    //Door
    SDL_Texture * Door= IMG_LoadTexture(ren,"door.gif");
    if(!Door)error1("Fail to load");

    SDL_Rect Door_pos;
    srand(time(NULL));
    int x,y;
    x = (rand() * 100)% 1050 ;
    while(1){
        y = (rand() * 100)% 700;
        if((y)%100 == 0){
            y += 50;
            break;
        }
    }
    position_init(Door_pos,x,y,50,50);
    /*Door_pos.h = 50; Door_pos.w = 50;*/

    //Stone
    SDL_Texture * Stone= IMG_LoadTexture(ren,"Stone.png");
    if(!Stone)error1("Fail to load");

    //Struct Map [START]
    typedef struct{
        SDL_Rect pos;
        int Con_y;
        int Con_x;
    }Map;

    Map CRT[MAKSx2][MAKSx2];
    Map STN[MAKSx2][MAKSx2];
    Map Pos;

    Pos.Con_y = 100;
    Pos.Con_x = 100;
    for( q = 0; q < MAKSx2; q++){
        for(w = 0; w < MAKSx2; w++){
            CRT[q][w].pos.y = Pos.Con_y;
            CRT[q][w].pos.x = Pos.Con_x;
            CRT[q][w].pos.h = 50;
            CRT[q][w].pos.w = 50;
            Pos.Con_x += 50;
        }
        Pos.Con_x = 100;
        Pos.Con_y += 50;
    }
    Pos.Con_y = 0;
    Pos.Con_x = 0;
    for( q = 0; q < MAKS; q++){
        for(w = 0; w < MAKS; w++){
            STN[q][w].pos.y = Pos.Con_y;
            STN[q][w].pos.x = Pos.Con_x;
            STN[q][w].pos.h = 50;
            STN[q][w].pos.w = 50;
            Pos.Con_x += 100;
        }
        Pos.Con_x = 0;
        Pos.Con_y += 100;
    }
    //Struct Map [END]

    //Music
    int flag = MIX_INIT_OGG || MIX_INIT_MP3;
    if (Mix_Init(flag) & flag != flag)
        error1("Failed to init OGG and MP3 support");
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024))
        error1("Fail to open audio");
    const char files[SoundTotal][50]={"explosion.wav","Music.wav","Track1.wav"};
    for(int i=0;i<SoundTotal;i++){
        audio[i]=Mix_LoadWAV(files[i]);
        if(!audio[i])error1("Audio fail to load");
    }

    //Music Play
    Mix_PlayChannel(-1, audio[SoundGameMenu], -1);

    // game logic
    SDL_Event a,b;
    bool Gamerunning = false;
    bool Gameending = false;
    bool StartMenu = true;
    int bmb = 0, X = 0;
    while(StartMenu){ // your first game loop
        while(SDL_PollEvent(&b))
            switch(b.type){
            case SDL_QUIT:
                StartMenu = false;
                Gamerunning = false;
                break;
            case SDL_KEYDOWN:
                switch(b.key.keysym.scancode) {
                case SDL_SCANCODE_SPACE:
                    StartMenu = false;
                    Gamerunning = true;
                    break;
                }
                break;
            }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,bgstart,NULL,NULL);
        SDL_RenderPresent(ren);
    }

    printf("\nDoor Clue : X: %d Y: %d\n", Door_pos.x, Door_pos.y);
    Mix_HaltChannel(-1);
    Mix_PlayChannel(-1, audio[SoundGameStart], -1);
    while(Gamerunning){ // your second game loop
        if (PlayerLove == 0){ // Player Mati
            Gamerunning = false;
            Gameending = true;
            break;
        }

        if(Player_pos.x  == Key_pos.x && Player_pos.y == Key_pos.y){ //
            Key_pos.x = -1000;
        }
        if(Player_pos.x  == Door_pos.x && Player_pos.y == Door_pos.y && Key_pos.x == -1000){
            Player_pos.x = -1000;
            Gamerunning = false;
            Gameending = true;

        }
        //printf("\nX = %d Y = %d\n", Player_pos.x, Player_pos.y);
        while(SDL_PollEvent(&a))
            switch(a.type){
            case SDL_QUIT:
                Gamerunning = false;
                break;
            case SDL_KEYDOWN:
                switch(a.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    for(q = 0; q < MAKSx2; q++){
                        for(w =0; w < MAKSx2; w++){
                            if( (Player_pos.x+Geser) == CRT[q][w].pos.x && Player_pos.y  == CRT[q][w].pos.y){
                                X++;
                            }
                        }
                    }
                    if (X != 0) {
                        X = 0;
                        } //crate
                    else if((Player_pos.y)%100 == 0)Player_pos.x += 0; //stone
                    //else if(Player_pos.x+50 == Crate_pos.x && Player_pos.y  == Crate_pos.y) Player_pos.x += 0;
                    else if(Player_pos.x+Geser <= W-Player_pos.w)Player_pos.x += Geser; // kalau nabrak windows x != ++
                    break;
                case SDL_SCANCODE_LEFT:
                    for(q = 0; q < MAKSx2; q++){
                        for(w =0; w < MAKSx2; w++){
                            if((Player_pos.x-Geser) == CRT[q][w].pos.x && Player_pos.y  == CRT[q][w].pos.y){
                                X++;
                            }
                        }
                    }
                    if (X != 0) {
                        X = 0;
                        }
                    else if((Player_pos.y)%100 == 0)Player_pos.x -= 0;
                    //else if(Player_pos.x-50 == Crate_pos.x && Player_pos.y  == Crate_pos.y) Player_pos.x -= 0;
                    else if(Player_pos.x-Geser>=0)Player_pos.x -= Geser;
                    break;
                case SDL_SCANCODE_UP:
                    for(q = 0; q < MAKSx2; q++){
                        for(w =0; w < MAKSx2; w++){
                            if((Player_pos.y-Geser) == CRT[q][w].pos.y && Player_pos.x  == CRT[q][w].pos.x){
                                X++;
                            }
                        }
                    }
                    if (X != 0) {
                        X = 0;
                        }
                    //if((Player_pos.y-50) == Crate_pos.y && Player_pos.x  == Crate_pos.x) Player_pos.y += 0;
                    else if((Player_pos.x)%100 != 0 && Player_pos.y-Geser >= 0 )Player_pos.y -= Geser;
                    break;
                case SDL_SCANCODE_DOWN:
                    for(q = 0; q < MAKSx2; q++){
                        for(w =0; w < MAKSx2; w++){
                            if((Player_pos.y+Geser) == CRT[q][w].pos.y && Player_pos.x  == CRT[q][w].pos.x) X++;
                        }
                    }
                    if (X != 0) {
                        X = 0;
                        }//if((Player_pos.y+50) == Crate_pos.y && Player_pos.x  == Crate_pos.x) Player_pos.y += 0;
                    else if((Player_pos.x)%100 != 0 && Player_pos.y+Geser <= H-Player_pos.h){
                        Player_pos.y += Geser;
                    }
                    break;
                case SDL_SCANCODE_SPACE:
                if(bmb != 3) {
                        Bombpos[bmb].x = Player_pos.x;
                        Bombpos[bmb].y = Player_pos.y;
                        bmb++;
                }
                else if(bmb == 3){
                    Mix_PlayChannel(-1 ,audio[SoundBombExplosion], 0);
                    for(u = 0; u < 3; u++){
                    for(q = 0; q < MAKSx2; q++){
                        for(w =0; w < MAKSx2; w++){
                                if( (Bombpos[u].x+Geser == CRT[q][w].pos.x && Bombpos[u].y == CRT[q][w].pos.y)||
                                    (Bombpos[u].y-Geser == CRT[q][w].pos.y && Bombpos[u].x == CRT[q][w].pos.x)||
                                    (Bombpos[u].x-Geser == CRT[q][w].pos.x && Bombpos[u].y == CRT[q][w].pos.y)||
                                    (Bombpos[u].y+Geser == CRT[q][w].pos.y && Bombpos[u].x == CRT[q][w].pos.x)){
                                    CRT[q][w].pos.x -= 1000;
                                    Bombpos[u].x -= 1000;
                                    bmb = 0;
                                }
                            }
                        }
                    }

                    for(u = 0; u < 3; u++){
                    if( (Bombpos[u].x+Geser == Player_pos.x && Bombpos[u].y == Player_pos.y)||
                        (Bombpos[u].y-Geser == Player_pos.y && Bombpos[u].x == Player_pos.x)||
                        (Bombpos[u].x-Geser == Player_pos.x && Bombpos[u].y == Player_pos.y)||
                        (Bombpos[u].y+Geser == Player_pos.y && Bombpos[u].x == Player_pos.x)||
                        (Bombpos[u].x == Player_pos.x && Bombpos[u].y == Player_pos.y)){
                        Bombpos[u].x = -100;
                        Player_pos.x = 50;
                        Player_pos.y = 0;
                        PlayerLove--;
                        bmb = 0;
                    }
                    else {
                        Bombpos[u].x = -100;
                        bmb= 0;
                    }
                    }
                    break;
                }
                break;
                }
                break;
            }

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,bg,NULL,NULL);
        SDL_RenderCopy(ren,Key,NULL,&Key_pos);
        SDL_RenderCopy(ren,Door,NULL,&Door_pos);
        for(q = 0; q < MAKSx2; q++){
            for(w = 0; w < MAKSx2; w++){
                SDL_RenderCopy(ren,Crate,NULL,&CRT[q][w].pos);
            }
        }
        for(q = 0; q < MAKS; q++){
            for(w = 0; w < MAKS; w++){
                SDL_RenderCopy(ren,Stone,NULL,&STN[q][w].pos);
            }
        }
        for(w = 0; w < 3; w++){
                SDL_RenderCopy(ren,Bomb,NULL,&Bombpos[w]);
        }
        SDL_RenderCopy(ren,Player,NULL,&Player_pos);
        SDL_RenderPresent(ren);
    }
    while(Gameending){ // your third game loop
        while(SDL_PollEvent(&b))
            switch(b.type){
            case SDL_QUIT:
                Gameending = false;
                break;
            case SDL_KEYDOWN:
                switch(b.key.keysym.scancode) {
                case SDL_SCANCODE_SPACE:
                    Gameending = false;
                    //Back to Game Menu
                    break;
                }
                break;
            }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,bggameover,NULL,NULL);
        SDL_RenderPresent(ren);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}