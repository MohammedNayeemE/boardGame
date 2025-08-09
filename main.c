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
	{0 , 0 , 0 , 0} , 
	{0 , 0 , 0 , 0} , 
	{0 , 0 , 0 , 0} , 
	{0 , 0 , 0 , 0}
};

int Score = 0, BestScore = 0;

void LoadFile(const char* fileName) {
	FILE *file = fopen(fileName, "r");
	if (file != NULL) {
		fscanf(file, "%d", &BestScore);
		fclose(file);
	} else {
		BestScore = 0; 
	}
}

void SaveFile(const char* fileName) {
	FILE *file = fopen(fileName, "w");
	if (file != NULL) {
		fprintf(file, "%d", BestScore);
		fclose(file);
	}
}


void spawn_random_tile() {
	int cnt = 0, l = 0;
	int refTable[16][2];

	for(int i = 0; i < GRID_SIZE; i++) {
		for(int j = 0; j < GRID_SIZE; j++) {
			if(board[i][j] == 0) {
				refTable[cnt][0] = i;
				refTable[cnt][1] = j;
				cnt++;
			}
		}
	}

	if(cnt > 0) {
		int choice = rand() % cnt;
		int value = 2; 
		int x = refTable[choice][0], y = refTable[choice][1];
		board[x][y] = value;
	}
}

void init_renderer() { 
	srand(time(NULL)); 
	SetTraceLogLevel(LOG_NONE); 
	InitWindow(WIDTH , HEIGHT , TITLE);
	SetTargetFPS(60);
	spawn_random_tile();
	spawn_random_tile();
}
void draw_tiles() {
	int total = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING;
	int startX = (WIDTH - total) / 2; 
	int startY = (HEIGHT - total) / 2;

	for(int row = 0; row < GRID_SIZE; row++) {
		for(int col = 0; col < GRID_SIZE; col++) {
			int x = startX + PADDING + col * (TILE_SIZE + PADDING);
			int y = startY + PADDING + row * (TILE_SIZE + PADDING);
			int value = board[row][col];
			Rectangle rec = {x , y , TILE_SIZE , TILE_SIZE};
			DrawRectangleRounded(rec , 0.2 , 6 , YELLOW);

			if(value != 0) {
				char text[8];
				sprintf(text , "%d" , value);
				int textWidth = MeasureText(text , FONT_SIZE);
				DrawText(text , x + (TILE_SIZE - textWidth) / 2, 
	     y + (TILE_SIZE - FONT_SIZE) / 2 , FONT_SIZE , BLACK);
			}
		}
	}
}

int merge(int *line) {
	int temp[GRID_SIZE] = {0};
	int index = 0;
	for(int i = 0 ; i < GRID_SIZE ; i++) {
		if(line[i] != 0) temp[index++] = line[i];
	}
	int moved = 0;
	for(int i = 0 ; i < GRID_SIZE - 1 ; i++) {
		if(temp[i] > 0 && temp[i] == temp[i + 1]) {
			temp[i] += temp[i];
			Score += temp[i];
			temp[i + 1] = 0;
			moved = 1;
		}
	}

	int final[GRID_SIZE] = {0};
	index = 0;

	for(int i = 0 ; i < GRID_SIZE ; i++) {
		if(temp[i] != 0) final[index++] = temp[i];
	}

	for(int i = 0 ; i < GRID_SIZE ; i++) {
		if(line[i] != final[i]) {
			moved = 1;
			line[i] = final[i];
		}
	}

	return moved;


}
int move_left() {
	int moved = 0;
	for (int r = 0; r < GRID_SIZE; r++) {
		moved |= merge(board[r]);
	}
	return moved;
}

int move_right() {
	int moved = 0;
	for (int r = 0; r < GRID_SIZE; r++) {
		int line[GRID_SIZE];
		for (int c = 0; c < GRID_SIZE; c++) line[c] = board[r][GRID_SIZE-1-c];
		moved |= merge(line);
		for (int c = 0; c < GRID_SIZE; c++) board[r][GRID_SIZE-1-c] = line[c];
	}
	return moved;
}

int move_up() {
	int moved = 0;
	for (int c = 0; c < GRID_SIZE; c++) {
		int line[GRID_SIZE];
		for (int r = 0; r < GRID_SIZE; r++) line[r] = board[r][c];
		moved |= merge(line);
		for (int r = 0; r < GRID_SIZE; r++) board[r][c] = line[r];
	}
	return moved;
}

int move_down() {
	int moved = 0;
	for (int c = 0; c < GRID_SIZE; c++) {
		int line[GRID_SIZE];
		for (int r = 0; r < GRID_SIZE; r++) line[r] = board[GRID_SIZE-1-r][c];
		moved |= merge(line);
		for (int r = 0; r < GRID_SIZE; r++) board[GRID_SIZE-1-r][c] = line[r];
	}
	return moved;
}

void manage_inputs() {
	if(IsKeyPressed(KEY_LEFT)) {
		if(move_left()) spawn_random_tile();
	}
	if (IsKeyPressed(KEY_RIGHT)) {
		if (move_right()) spawn_random_tile();
	}
	if (IsKeyPressed(KEY_UP)) {
		if (move_up()) spawn_random_tile();
	}
	if (IsKeyPressed(KEY_DOWN)) {
		if (move_down()) spawn_random_tile();
	}
}

void draw_score() {
	char score[64];
	sprintf(score , "Score: %d" , Score);
	DrawText(score , 10 , 20 , 32 , GREEN);

	char best[64];
	sprintf(best , "Best: %d" , BestScore);
	DrawText(best , 10 , 60 , 32 , ORANGE);
}

void game_loop() {
	while(!WindowShouldClose()) {
		if (Score > BestScore) {
			BestScore = Score;
			SaveFile("score.txt");
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);
		draw_tiles();
		draw_score();
		manage_inputs();
		EndDrawing();
	}
}

void close_renderer() {
	CloseWindow();
}

int main() {
	init_renderer();
	LoadFile("score.txt");
	game_loop();
	close_renderer();
	return 0;
}

