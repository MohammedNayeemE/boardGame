#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char* TITLE = "2048";
#define WIDTH 800
#define HEIGHT 600
#define GRID_SIZE 4
#define TILE_SIZE 100
#define PADDING 10 
#define FONT_SIZE 32

int board[GRID_SIZE][GRID_SIZE] = {
	{2 , 0 , 0 , 2} , 
	{0 , 0 , 0 , 0} , 
	{2 , 2 , 0 , 0} , 
	{0 , 0 , 0 , 2}
};

void init_renderer() {
	srand(time(NULL));
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WIDTH , HEIGHT , TITLE);
	SetTargetFPS(60);
}

void spawn_random_tile() {

	int cnt = 0 , l = 0;
	int refTable[16][2];

	for(int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 4 ; j++) {
			if(board[i][j] == 0) cnt++;
			refTable[l][0] = i;
			refTable[l][1] = j;
			l++;
		}
	}


	if(cnt > 0) {
		int choice = rand() % cnt ;
		int value = rand() % 2 ? 2 : 4;
		int x = refTable[choice][0] , y = refTable[choice][1];
		/*printf("%d %d\n" ,x , y);*/
		board[x][y] = value;
	}

}

void draw_tiles() {
	int total = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING;
	int startX = (WIDTH - total) / 2; 
	int startY = (HEIGHT - total) / 2;

	for(int row = 0 ; row < GRID_SIZE ; row++) {
		for(int col = 0 ; col < GRID_SIZE ; col++) {
			int x = startX + PADDING + col * (TILE_SIZE + PADDING);
			int y = startY + PADDING + row * (TILE_SIZE + PADDING);
			int value = board[row][col];
			Rectangle rec = {x , y , TILE_SIZE , TILE_SIZE};
			DrawRectangleRounded(rec , 0.2 , 6 , YELLOW);

			if(value != 0) {
				char text[8];
				sprintf(text , "%d" , value);
				int textWidth = MeasureText(text , FONT_SIZE);
				DrawText(text , x + (TILE_SIZE - textWidth) / 2, y + (TILE_SIZE - FONT_SIZE) / 2 , FONT_SIZE , BLACK);
			}
		}
	}

}

void manage_inputs() {
	if(IsKeyPressed(KEY_LEFT)) spawn_random_tile();
}

void game_loop() {
	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		draw_tiles();
		manage_inputs();
		EndDrawing();
	}
}

void close_renderer() {
	CloseWindow();
}

int main() {
	init_renderer();
	game_loop();
	close_renderer();

	return 0;
}
