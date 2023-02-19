#include <nds.h>
#include <gl2d.h>

#define SCALE 1
#define GAME_FRAMERATE 15
#define BLOCK_SIZE 8

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
	int x;
	int y;
} IntVec2;

typedef struct {
	unsigned short color;
	int score;
	Direction direction;
	IntVec2 body[1000];
} Player;

void initializePlayer(Player *player, unsigned short color, int score, int x, int y, Direction direction) {
	player->color = color;
	player->score = score;
	player->direction = direction;
	for (int i = 0; i < score; i++) {
		player->body[i].x = x;
		player->body[i].y = y;
	}
}

int main(void) {
	const int screenWidth = SCREEN_WIDTH;
	const int screenHeight = SCREEN_HEIGHT;
	const int mapWidth = screenWidth / BLOCK_SIZE;
	const int mapHeight = screenHeight / BLOCK_SIZE;

	videoSetMode( MODE_5_3D );
	consoleDemoInit();
	glScreen2D();

	// Game Initialization
	Player player1;					// Ready Player 1
	IntVec2 food = {12, 8};

	initializePlayer(&player1, RGB15(31, 0, 0), 5, 0, 0, RIGHT);
	int frameCounter = 0;
	char paused = 0;
	int startBuffer = 0;

	// Main game loop
	while (1) {
		// Update
		if (frameCounter % 4 == 0 && !paused) {
		
			for (int i = player1.score - 1; i > 0; i--) {
				player1.body[i].x = player1.body[i-1].x;
				player1.body[i].y = player1.body[i-1].y;
			}

			switch (player1.direction) {
				case UP:
					player1.body[0].y--;
					break;
				case DOWN:
					player1.body[0].y++;
					break;
				case LEFT:
					player1.body[0].x--;
					break;
				case RIGHT:
					player1.body[0].x++;
					break;
			}

			if (player1.body[0].y < 0 || player1.body[0].y >= mapHeight || player1.body[0].x < 0 || player1.body[0].x >= mapWidth) { // detect wall collision
				initializePlayer(&player1, RGB15(31, 0, 0), 5, 0, 0, RIGHT);
			}

			for (int i = 4; i < player1.score; i++) {
				if (player1.body[0].x == player1.body[i].x && player1.body[0].y == player1.body[i].y) {
					initializePlayer(&player1, RGB15(31, 0, 0), 5, 0, 0, RIGHT);
					break;
				}
			}

			if (player1.body[0].x == food.x && player1.body[0].y == food.y) {
				int tempX = player1.body[player1.score-1].x;
				int tempY = player1.body[player1.score-1].y;
				for (int i = player1.score - 1; i < player1.score + 5; i++) {
					player1.body[i].x = tempX;
					player1.body[i].y = tempY;
				}
				player1.score += 5;
				food.x = frameCounter % mapWidth;
				food.y = frameCounter % mapHeight;
			}
		}
		frameCounter++;

		// Draw
		glBegin2D();

			glBoxFilled(0, 0, 255, 191, RGB15(31, 31, 31));
			for (int i = 0; i < player1.score; i++) {
				int x = player1.body[i].x * BLOCK_SIZE * SCALE;
				int y = player1.body[i].y * BLOCK_SIZE * SCALE;
				glBoxFilled(x, y, x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1, player1.color);
			}
			glBoxFilled(food.x * BLOCK_SIZE, food.y * BLOCK_SIZE, food.x * BLOCK_SIZE + BLOCK_SIZE - 1, food.y * BLOCK_SIZE + BLOCK_SIZE - 1, RGB15(0, 31, 0));

		glEnd2D();
		glFlush(0);
		swiWaitForVBlank();
		scanKeys();
		int key = keysDown();

		
		if (!paused) {
			if (key & (KEY_RIGHT | KEY_A) && player1.direction != LEFT) { player1.direction = RIGHT; }
			if (key & (KEY_LEFT | KEY_Y) && player1.direction != RIGHT) { player1.direction = LEFT; }
			if (key & (KEY_UP | KEY_X) && player1.direction != DOWN) { player1.direction = UP; }
			if (key & (KEY_DOWN | KEY_B) && player1.direction != UP) { player1.direction = DOWN; }
		}
		if (key & KEY_START && !startBuffer) {paused = !paused;}
		startBuffer = key & KEY_START;

	}

	return 0;
}
