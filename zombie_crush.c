/*
	this is a game developed using the sdl2 library
	pleas note that if you want to compile it you have to use this command:

	gcc -Wall -o [NOME FILE EXECUTABLE]  [NOME FILE SORGENTE (.c)] $(pkg-config --cflags --libs sdl2) -lm	     ### SE solo SDL2
	gcc -Wall -o [NOME FILE EXECUTABLE]  [NOME FILE SORGENTE (.c)] $(pkg-config --cflags --libs SDL2_image) -lm  ### SE anche 		SDL2_image

	@EusebioAlberto
*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "my_functions.c"
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

typedef struct c{
	char r;
	char g;
	char b;
}Color;

typedef struct pedina{
	_Bool status;
	SDL_Rect body;
	SDL_Texture *color;
	SDL_Texture *dead_color;	

}Candy;

void CreateField(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Texture *colors[][2]);
void CheckSequence(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], _Bool *field_changed, int *score);
void MoveDown(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Texture *colors[][2]);
void ChangeCandy(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Event *event, int* movements,_Bool *ON);
void Draw(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Renderer *renderer);
int main(void){
	srand(time(0));

	int movements = 0;
	int max_score = 0;
	do {
		printf("\nChoose the MAX_SCORE you want to reach: ");
		scanf("%d", &max_score);
		printf("\nChoose the number of MOVEMENTS you want to do: ");
		scanf("%d", &movements);

	}while(movements == 0 || max_score == 0);


	if(SDL_Init(SDL_INIT_EVERYTHING)){
		printf("\nERROR INITIALIZING SDL: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	IMG_Init(IMG_INIT_PNG);

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){ //I have to check it in this way because IMG_Init() returns the argument given
 		printf("\nERROR INITIALIZING SDL_image: %s\n", IMG_GetError());
		IMG_Quit();
		return 1;
	}

	//Creates the window and renderer structures
	SDL_Window *window = SDL_CreateWindow("Candy Crush", 
						SDL_WINDOWPOS_CENTERED, 
						SDL_WINDOWPOS_CENTERED, 
						SCREEN_WIDTH,SCREEN_HEIGHT, 
						SDL_WINDOW_SHOWN
					      );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//creates a event structure
	SDL_Event event;
	
	//game loop variables
	_Bool ON = 1;

	//other variables
	int size = 100;
	int score = 0;
	_Bool field_changed;
	Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size];
	SDL_Texture *colors[][2] = {
			{IMG_LoadTexture(renderer, "./assets/banana.png"),IMG_LoadTexture(renderer, "./assets/banana_dead.png")}, //banana
			{IMG_LoadTexture(renderer, "./assets/apple.png"),IMG_LoadTexture(renderer, "./assets/apple_dead.png")},	  //apple
			{IMG_LoadTexture(renderer, "./assets/cherry.png"),IMG_LoadTexture(renderer, "./assets/cherry_dead.png")},  //cherry
			{IMG_LoadTexture(renderer, "./assets/pumpkin.png"),IMG_LoadTexture(renderer, "./assets/pumpkin_dead.png")} //pumpkin
			};
	CreateField(size, field, colors);

	do{
		MoveDown(size,field,colors);
		CheckSequence(size,field, &field_changed, &score);
	}while(field_changed != 0);

	score = 0;
	system("clear");

	while(ON){

		Draw(size, field, renderer);

		ChangeCandy(size, field, &event, &movements, &ON);

		Draw(size, field, renderer);
		do{
			MoveDown(size,field,colors);
			CheckSequence(size,field, &field_changed, &score);
			Draw(size, field, renderer);
		}while(field_changed != 0);
	}

	do{		
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT){
			break;
		}
		if(score >= max_score){
			SDL_SetRenderDrawColor(renderer, 0,255,0, SDL_ALPHA_OPAQUE);
		}else{
			SDL_SetRenderDrawColor(renderer, 255,0,0, SDL_ALPHA_OPAQUE);
		}
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	
	}while(1);

	//quit the system
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();
}

void CreateField(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Texture *colors[][2]){
	if((SCREEN_HEIGHT%size) != 0 || (SCREEN_HEIGHT%size) != 0){
		printf("%p", NULL);
	}	
	
	int random = 0;	

	for(int i = 0; i < SCREEN_HEIGHT/size; i++){
		for(int j = 0; j < SCREEN_WIDTH/size; j++){
			random = rand()%4;
			field[i][j].body.y = i*size; 
			field[i][j].body.x = j*size;
			field[i][j].body.w = size;
			field[i][j].body.h = size;
			field[i][j].color = (SDL_Texture *) colors[random][0];
			field[i][j].dead_color = (SDL_Texture *) colors[random][1];
			field[i][j].status = 1;
		}
	}
}


void Draw(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, 0,255,0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	for(int i = 0; i < SCREEN_HEIGHT/size; i++){
		for(int j = 0; j < SCREEN_WIDTH/size; j++){
			if(field[i][j].status == 1){
				SDL_RenderCopy(renderer, field[i][j].color, NULL, &field[i][j].body);
			}else{
				SDL_RenderCopy(renderer, field[i][j].dead_color, NULL, &field[i][j].body);
			}
		}
	}
	SDL_RenderPresent(renderer);
	delay(200);
}

void CheckSequence(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], _Bool *field_changed, int *score){
	SDL_Texture *color;
	int sequence = 1;
	*field_changed = 0;
	//This checks horizontally
	for(int i = 0; i < SCREEN_HEIGHT/size; i++){
		sequence = 1;
		color = field[i][0].color;
		for(int j = 1; j < SCREEN_WIDTH/size; j++){
			if(color != field[i][j].color){
				color = field[i][j].color;
				sequence = 1;
			}else{
				sequence++;
				
			}
			//in this way it sets all the line off 
			if(sequence >= 3){
				for(int n = j; n > (j-sequence); n--){
					field[i][n].status = 0;
					*field_changed = 1;
				}
				*score += sequence;
				printf("SCORE: %d", *score);
				printf("\n");
			}
		}
	}
	//This checks vertically
	for(int j = 0; j < SCREEN_WIDTH/size; j++){
		sequence = 1;
		color = field[0][j].color;
		for(int i = 1; i < SCREEN_HEIGHT/size; i++){
			if(color != field[i][j].color){
				color = field[i][j].color;
				sequence = 1;
			}else{
				sequence++;
				
			}
			//in this way it sets all the line off 
			if(sequence >= 3){
				for(int n = i; n > (i-sequence); n--){
					field[n][j].status = 0;
					*field_changed = 1;
				}
				*score += sequence;
				printf("SCORE: %d", *score);
				printf("\n");
			}
		}
	}

}

void MoveDown(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Texture* colors[][2]){
	int random = 0;
	for(int i = (SCREEN_HEIGHT/size)-1; i >= 0 ; i--){
		for(int j = 0; j < SCREEN_WIDTH/size; j++){
			if(field[i][j].status == 0){
				for(int n = 1; n <= i ; n++){
					if(field[i-n][j].status == 1){	
						field[i][j].color = field[i-n][j].color;
						field[i][j].dead_color = field[i-n][j].dead_color;
						field[i][j].status  = 1; //If the candy moves then change its state
						field[i-n][j].status = 0;
						break;
					}
				}
				if(field[i][j].status == 0){ //if it is still 0 then nothing has moved so invent
					random = rand()%4;
					field[i][j].color = colors[random][0];
					field[i][j].dead_color = colors[random][1];
					field[i][j].status = 1;
				}
					
			}
			
		}
	}
}

void ChangeCandy(int size ,Candy field[SCREEN_HEIGHT/size][SCREEN_WIDTH/size], SDL_Event *event,int * movements,_Bool *ON){
	int selections[2][2] = {0}; //In this way I avoid a core dump
	int x,y, d_x,d_y;
	_Bool pressed = 0;

	for(int i = 0; i<2; i++){
		while(1){
			SDL_PollEvent(event);
			if(event->type == SDL_QUIT || *movements == 0 || (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_q)){
				*ON = 0;
				return;
			}
			if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && !pressed){
				pressed = 1;
				SDL_GetMouseState(&x,&y);
				for(int j = 0; j<SCREEN_HEIGHT/size;j++){
					for(int k = 0; k<SCREEN_WIDTH/size;k++){
						d_x = x-field[j][k].body.x;
						d_y = y-field[j][k].body.y;
						if(field[j][k].status == 1 && (d_x <= field[j][k].body.w && d_x >= 0) && (d_y <= field[j][k].body.h && d_y >= 0)){
							selections[i][0] = j;
							selections[i][1] = k;
						}
					}
				}
			}
			if(event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT && pressed){
				pressed = 0;
				break;
			}
		}
		
	}
	
	int j1 = selections[0][0];
	int k1 = selections[0][1];
	int j2 = selections[1][0];
	int k2 = selections[1][1];
	if((abs(j1-j2) == 1 && abs(k1-k2) == 0) || (abs(j1-j2) == 0 && abs(k1-k2) == 1)){ //so i'm forced to move only on rows or colons
		SDL_Texture* buff = field[j1][k1].color;
		SDL_Texture* dead_buff = field[j1][k1].dead_color;
		field[j1][k1].color = field[j2][k2].color;
		field[j1][k1].dead_color = field[j2][k2].dead_color;
		field[j2][k2].color = buff;
		field[j2][k2].dead_color = dead_buff;		
		(*movements)--;
	}
}
