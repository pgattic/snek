/*

	SNEK
	Just a simple Snake game, nothing more :)

	Hoping to turn this code into a solid cross-platform game

	Targets I hope to hit:
	- Nintendo DS
	- Game Boy Advance
	- Wii
	- Desktop (probably with raylib)
	- Game Boy Color?

*/



#include <nds.h>
#include <gl2d.h>

#define SCALE 1				// Scale of the game's canvas (used to upscale the game)
#define GAME_FRAMERATE 15
#define BLOCK_SIZE 8		// Size of each "tile" of the map

#define FOOD_GROWTH 5		// Amount of length the food adds to the snake


typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
	int x;
	int y;
} IntVec2; // just a simple 2d int vector, I don't have any need to use floats.

typedef struct {
	unsigned short color;
	int score;
	Direction direction;
	IntVec2 body[1000];	// just a huge array of coordinates
} Player;

void initializePlayer(Player *player, unsigned short color, int score, int x, int y, Direction direction) { // use to reset the player location, score, etc.
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
	const int frameSkips = 60 / GAME_FRAMERATE;

	// Initialize the screens
	videoSetMode( MODE_5_3D );
	consoleDemoInit();
	glScreen2D();

	// Game Initialization
	Player player1;					// Ready Player 1
	IntVec2 food = {12, 8};

	initializePlayer(&player1, RGB15(31, 0, 0), 5, 0, 0, RIGHT);
	int frameCounter = 0;			// Frame counter used for "random" number generation and to delay the game's logic to the right speed
	char paused = 0;

	int keysBuffer = 0;				// Stores the previous frame's keys (if different from "keys", a new key was pressed this frame)

	// Main game loop
	while (1) {

		// Check Keypresses
		scanKeys();
		int keys = keysDown();

		if (!paused) {
			if (keys & (KEY_RIGHT | KEY_A) && player1.direction != LEFT) { player1.direction = RIGHT; }
			if (keys & (KEY_LEFT | KEY_Y) && player1.direction != RIGHT) { player1.direction = LEFT; }
			if (keys & (KEY_UP | KEY_X) && player1.direction != DOWN) { player1.direction = UP; }
			if (keys & (KEY_DOWN | KEY_B) && player1.direction != UP) { player1.direction = DOWN; }
		}
		if (keys & KEY_START && !(keysBuffer & KEY_START)) {
			paused = !paused;
		}

		keysBuffer = keys;

		// Perform game logic
		if (frameCounter % frameSkips == 0 && !paused) {
		
			// There is no unshift in C, so this is what I got haha
			for (int i = player1.score - 1; i > 0; i--) {
				player1.body[i].x = player1.body[i-1].x;
				player1.body[i].y = player1.body[i-1].y;
			}

			// Move the player's head (first body segment)
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

			// Wall collision
			if (player1.body[0].y < 0 || player1.body[0].y >= mapHeight || player1.body[0].x < 0 || player1.body[0].x >= mapWidth) {
				initializePlayer(&player1, RGB15(31, 0, 0), 5, 0, 0, RIGHT);
			}

			// Body collision
			for (int i = 4; i < player1.score; i++) {
				if (player1.body[0].x == player1.body[i].x && player1.body[0].y == player1.body[i].y) {
					initializePlayer(&player1, RGB15(31, 0, 0), 5, 0, 0, RIGHT);
					break;
				}
			}

			// Food collision
			if (player1.body[0].x == food.x && player1.body[0].y == food.y) {

				// Gotta make sure the next body segments' data aren't gonna mess things up
				int tempX = player1.body[player1.score-1].x;
				int tempY = player1.body[player1.score-1].y;
				for (int i = player1.score - 1; i < player1.score + FOOD_GROWTH; i++) {
					player1.body[i].x = tempX;
					player1.body[i].y = tempY;
				}

				// Feed player, generate new food
				player1.score += FOOD_GROWTH;
				food.x = frameCounter % mapWidth;
				food.y = frameCounter % mapHeight;
			}
		}

		// Draw
		glBegin2D();

			// Make the background white
			glBoxFilled(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, RGB15(31, 31, 31));

			// Draw each player body segment (TODO: reduce box drawing for consecutive segments)
			for (int i = 0; i < player1.score; i++) {
				int x = player1.body[i].x * BLOCK_SIZE * SCALE;
				int y = player1.body[i].y * BLOCK_SIZE * SCALE;
				glBoxFilled(x, y, x + BLOCK_SIZE - 1, y + BLOCK_SIZE - 1, player1.color);
			}

			// draw food
			glBoxFilled(food.x * BLOCK_SIZE, food.y * BLOCK_SIZE, food.x * BLOCK_SIZE + BLOCK_SIZE - 1, food.y * BLOCK_SIZE + BLOCK_SIZE - 1, RGB15(0, 31, 0));

		glEnd2D();
		glFlush(0);

		swiWaitForVBlank();

		frameCounter++;
	}

	return 0;
}
