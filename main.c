#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define GRID_SIZE 4
#define TILE_SIZE 100
#define PADDING 10
#define FONT_SIZE 32
#define FILE_PATH "data/score.txt" 

typedef struct {
	int value;
	float x, y;             
	float targetX, targetY; 
	float scale;            
} Tile;

Tile tiles[GRID_SIZE][GRID_SIZE];
int Score = 0, BestScore = 0;

float lerp(float a, float b, float t) { return a + (b - a) * t; }

void LoadFile(const char* fileName) {
	FILE *file = fopen(fileName, "r");
	if (file) { fscanf(file, "%d", &BestScore); fclose(file); }
	else BestScore = 0;
}
void SaveFile(const char* fileName) {
	FILE *file = fopen(fileName, "w");
	if (file) { fprintf(file, "%d", BestScore); fclose(file); }
}

void get_tile_position(int row, int col, float *px, float *py) {
	int total = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * PADDING;
	int startX = (WIDTH - total) / 2;
	int startY = (HEIGHT - total) / 2;
	*px = startX + PADDING + col * (TILE_SIZE + PADDING);
	*py = startY + PADDING + row * (TILE_SIZE + PADDING);
}

int is_game_over() {
	for(int r = 0 ; r < GRID_SIZE ; r++) {
		for(int c = 0 ; c < GRID_SIZE ; c++) {
			if(tiles[r][c].value == 0) return 0;
			if(r + 1 < GRID_SIZE && tiles[r][c].value == tiles[r + 1][c].value) return 0;
			if(c + 1 < GRID_SIZE && tiles[r][c].value == tiles[r][c + 1].value) return 0;
		}
	}
	return 1;
}

void spawn_random_tile() {
	int cnt = 0;
	int refTable[GRID_SIZE * GRID_SIZE][2];
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			if (tiles[r][c].value == 0) {
				refTable[cnt][0] = r;
				refTable[cnt][1] = c;
				cnt++;
			}
		}
	}
	if (cnt > 0) {
		int choice = rand() % cnt;
		int r = refTable[choice][0];
		int c = refTable[choice][1];
		tiles[r][c].value = 2;
		get_tile_position(r, c, &tiles[r][c].x, &tiles[r][c].y);
		tiles[r][c].targetX = tiles[r][c].x;
		tiles[r][c].targetY = tiles[r][c].y;
		tiles[r][c].scale = 0.5f; 
	}
}

void init_renderer() {
	srand(time(NULL));
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WIDTH, HEIGHT, "2048");
	SetTargetFPS(60);
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			tiles[r][c].value = 0;
			get_tile_position(r, c, &tiles[r][c].x, &tiles[r][c].y);
			tiles[r][c].targetX = tiles[r][c].x;
			tiles[r][c].targetY = tiles[r][c].y;
			tiles[r][c].scale = 1.0f;
		}
	}
	spawn_random_tile();
	spawn_random_tile();
}

void update_tile_positions(float delta) {
	float speed = 8.0f;
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			tiles[r][c].x = lerp(tiles[r][c].x, tiles[r][c].targetX, delta * speed);
			tiles[r][c].y = lerp(tiles[r][c].y, tiles[r][c].targetY, delta * speed);
			tiles[r][c].scale = lerp(tiles[r][c].scale, 1.0f, delta * 8);
		}
	}
}

int merge_line(Tile *line[GRID_SIZE]) {
	int moved = 0;
	int temp[GRID_SIZE] = {0};
	int idx = 0;
	for (int i = 0; i < GRID_SIZE; i++) if (line[i]->value != 0) temp[idx++] = line[i]->value;
	for (int i = 0; i < GRID_SIZE - 1; i++) {
		if (temp[i] != 0 && temp[i] == temp[i+1]) {
			temp[i] *= 2;
			Score += temp[i];
			temp[i+1] = 0;
			line[i]->scale = 1.3f;             
			moved = 1;
		}
	}
	idx = 0;
	for (int i = 0; i < GRID_SIZE; i++) {
		if (temp[i] != 0) {
			if (line[idx]->value != temp[i]) moved = 1;
			line[idx++]->value = temp[i];
		}
	}
	while (idx < GRID_SIZE) line[idx++]->value = 0;
	return moved;
}

int move_left() {
	int moved = 0;
	for (int r = 0; r < GRID_SIZE; r++) {
		Tile *line[GRID_SIZE];
		for (int c = 0; c < GRID_SIZE; c++) line[c] = &tiles[r][c];
		moved |= merge_line(line);
		for (int c = 0; c < GRID_SIZE; c++) get_tile_position(r, c, &line[c]->targetX, &line[c]->targetY);
	}
	return moved;
}
int move_right() {
	int moved = 0;
	for (int r = 0; r < GRID_SIZE; r++) {
		Tile *line[GRID_SIZE];
		for (int c = 0; c < GRID_SIZE; c++) line[c] = &tiles[r][GRID_SIZE - 1 - c];
		moved |= merge_line(line);
		for (int c = 0; c < GRID_SIZE; c++) get_tile_position(r, GRID_SIZE - 1 - c, &line[c]->targetX, &line[c]->targetY);
	}
	return moved;
}
int move_up() {
	int moved = 0;
	for (int c = 0; c < GRID_SIZE; c++) {
		Tile *line[GRID_SIZE];
		for (int r = 0; r < GRID_SIZE; r++) line[r] = &tiles[r][c];
		moved |= merge_line(line);
		for (int r = 0; r < GRID_SIZE; r++) get_tile_position(r, c, &line[r]->targetX, &line[r]->targetY);
	}
	return moved;
}
int move_down() {
	int moved = 0;
	for (int c = 0; c < GRID_SIZE; c++) {
		Tile *line[GRID_SIZE];
		for (int r = 0; r < GRID_SIZE; r++) line[r] = &tiles[GRID_SIZE - 1 - r][c];
		moved |= merge_line(line);
		for (int r = 0; r < GRID_SIZE; r++) get_tile_position(GRID_SIZE - 1 - r, c, &line[r]->targetX, &line[r]->targetY);
	}
	return moved;
}

Color get_tile_color(int value) {
	switch(value) {
		case 2: return (Color){238, 228, 218, 255};
		case 4: return (Color){237, 224, 200, 255};
		case 8: return (Color){242, 177, 121, 255};
		case 16: return (Color){245, 149, 99, 255};
		case 32: return (Color){246, 124, 95, 255};
		case 64: return (Color){246, 94, 59, 255};
		case 128: return (Color){237, 207, 114, 255};
		case 256: return (Color){237, 204, 97, 255};
		case 512: return (Color){237, 200, 80, 255};
		case 1024: return (Color){237, 197, 63, 255};
		case 2048: return (Color){237, 194, 46, 255};
		default: return (Color){60, 58, 50, 255};
	}

}

void draw_tiles() {
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			Tile t = tiles[r][c];
			Color col = get_tile_color(t.value); 
			DrawRectangleRounded((Rectangle){t.x, t.y, TILE_SIZE * t.scale, TILE_SIZE * t.scale}, 0.2, 6, col);
			if (t.value != 0) {
				char buf[8];
				sprintf(buf, "%d", t.value);
				int tw = MeasureText(buf, FONT_SIZE);
				DrawText(buf, t.x + (TILE_SIZE * t.scale - tw) / 2, t.y + (TILE_SIZE * t.scale - FONT_SIZE) / 2, FONT_SIZE, BLACK);
			}
		}
	}
}
void draw_score() {
	char score[64];
	sprintf(score, "Score: %d", Score);
	DrawText(score, 10, 20, 32, WHITE);
	char best[64];
	sprintf(best, "Best: %d", BestScore);
	DrawText(best, 10, 60, 32, WHITE);
}

void game_over_screen() {
	ClearBackground(WHITE);
	DrawText("GAME OVER", WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, HEIGHT / 2, 40, RED);
	DrawText("Press R to restart", WIDTH / 2 - MeasureText("Press R to restart", 20) / 2, HEIGHT / 2 + 50, 20, DARKGRAY);

	if (IsKeyPressed(KEY_R)) {
		Score = 0;
		init_renderer();
	}
}

void game_loop() {
	while (!WindowShouldClose()) {
		float delta = GetFrameTime();
		if (Score > BestScore) { BestScore = Score; SaveFile(FILE_PATH); }
		if (IsKeyPressed(KEY_LEFT)) { if (move_left()) spawn_random_tile(); }
		if (IsKeyPressed(KEY_RIGHT)) { if (move_right()) spawn_random_tile(); }
		if (IsKeyPressed(KEY_UP)) { if (move_up()) spawn_random_tile(); }
		if (IsKeyPressed(KEY_DOWN)) { if (move_down()) spawn_random_tile(); }
		update_tile_positions(delta);
		int check  = is_game_over();
		BeginDrawing();
		ClearBackground(BLACK);
		if(check) game_over_screen();
		else {
			draw_tiles();
			draw_score();
		}
		EndDrawing();
	}
}

void close_renderer() { CloseWindow(); }

int main() {
	init_renderer();
	LoadFile(FILE_PATH);
	game_loop();
	close_renderer();
	return 0;
}

