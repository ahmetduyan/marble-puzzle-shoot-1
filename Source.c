﻿#include <raylib.h> 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#define MAX_BALL 20
const int screenWidth = 1500;
const int screenHeight = 800;

typedef enum { GAMEPLAY, START , GAMEOVER } GameScreen;

typedef struct target {
	float x;
	float y;
	float radius;
	Color color;
	bool active;
	bool moving;
}target;

typedef struct node {
	target* data;
	struct node* next;
	struct node* previous;
}node;

typedef struct {
	Vector2 ballPos;
	Vector2 ballSpeed;
	float radius;
	Color color;
	bool isFired;
	bool active;
}bullet;

GameScreen currentScreen = GAMEPLAY;
int maxball = MAX_BALL;
node* head = NULL;
target hedef[999] = { 0 };
bullet mermi = { 0 };
Vector2 mouse = { 0 };
double aimingAngle = 0;
Rectangle health = { 20,20,120,15 };
int healthCounter = 0;
Vector2 hold = { 0 };
node* holdBallNext = NULL;
node* holdBallPre = NULL;
int activeCounter = 0;
int totalActive = MAX_BALL;
int score = 0;
FILE* fptr = NULL;


void initGame();
void initGame2();
void updateGame();
void targetCreator(node**, target*);
void drawTargets(node*);
void freeTargets(node*);
void updateTarget(node**);
void bulletFire();
Color giveColor();
Color giveColorBullet(node*);
bool isSameColor(Color, Color);
GameScreen updateScreen();

int checkCollision(node*, bullet*);
target* shotTargetIndex(node**, bullet*);
int whereTarget(node*);
target* createOne(bullet);
node* addTargetBetween(target* newCreated, target* shotTargetIndex);
void stepBack(node*, node*);
void isBoom();
int highScore(int);


Texture2D kurbaga;
Texture2D ending;
Texture2D background;
Texture2D redball;
Texture2D blueball;
Texture2D greenball;
Texture2D yellowball;
Texture2D purpleball;
Texture2D blackball;
Texture2D gameover;
Music music;
Sound effect;

int main(void) {
	InitWindow(screenWidth, screenHeight, "marble puzzle shoot");
	SetTargetFPS(120);
	initGame();
	InitAudioDevice();

	kurbaga = LoadTexture("images/kurbaga.png");
	background = LoadTexture("images/background.png");
	redball = LoadTexture("images/redballl.png");
	blueball = LoadTexture("images/blueballl.png");
	greenball = LoadTexture("images/greenballl.png");
	yellowball = LoadTexture("images/yellowball.png");
	purpleball = LoadTexture("images/purpleball.png");
	blackball = LoadTexture("images/blackball.png");
	ending = LoadTexture("images/ending.png");
	gameover = LoadTexture("images/gameover.png");
	music = LoadMusicStream("sounds/sound.wav");
	effect = LoadSound("sounds/effect.wav");

	Texture2D textureMap = LoadTexture("images/yenimapp4.png");

	while (!WindowShouldClose()) {
		ClearBackground(LIGHTGRAY);

		updateGame();
		updateTarget(&head);

		PlayMusicStream(music);
		UpdateMusicStream(music);

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) bulletFire();

		if (checkCollision(head, &mermi)) {
			createOne(mermi);
			stepBack(head, addTargetBetween(createOne(mermi), shotTargetIndex(&head, &mermi)));
			isBoom();
		}

		BeginDrawing();
		switch (updateScreen()) {
		case GAMEPLAY:
			DrawTexture(background, 0, 0, WHITE);
			drawTargets(head);
			DrawTexture(kurbaga, screenWidth / 2 - 56, screenHeight / 2 - 65, WHITE);
			DrawTexture(ending, 570 - 56, screenHeight / 2 - 30, WHITE);
			DrawRectangleRec(health, RED);
			DrawText("HEALTH", 45, 20, 18, LIGHTGRAY);
			DrawText(TextFormat("SCORE: %d", score), 1350, 22, 20, RED);
			if (mermi.active == true) {
				if (isSameColor(mermi.color, RED)) {
					DrawTexture(redball, mermi.ballPos.x, mermi.ballPos.y, WHITE);
				}
				if (isSameColor(mermi.color, BLUE)) {
					DrawTexture(blueball, mermi.ballPos.x, mermi.ballPos.y, WHITE);
				}
				if (isSameColor(mermi.color, GREEN)) {
					DrawTexture(greenball, mermi.ballPos.x, mermi.ballPos.y, WHITE);
				}
				if (isSameColor(mermi.color, YELLOW)) {
					DrawTexture(redball, mermi.ballPos.x, mermi.ballPos.y, WHITE);
				}
				if (isSameColor(mermi.color, PURPLE)) {
					DrawTexture(blueball, mermi.ballPos.x, mermi.ballPos.y, WHITE);
				}
				if (isSameColor(mermi.color, BLACK)) {
					DrawTexture(greenball, mermi.ballPos.x, mermi.ballPos.y, WHITE);
				}
			}
			break;
		case GAMEOVER:
			DrawTexture(gameover, 0, 0, WHITE);
			DrawText(TextFormat("SCORE: %d", score), screenWidth/2-120, 600, 50, RED);
			DrawText(TextFormat("HIGH SCORE: %d", highScore(score)), screenWidth/2-120, 700, 50, RED);
		}
		EndDrawing();

	}
	freeTargets(head);
	UnloadMusicStream(music);
	CloseAudioDevice();
	UnloadTexture(textureMap);
	CloseWindow();
	return 0;
}

void initGame() {
	int sonuncuNum = maxball;
	
	for (int num = 1; num <= maxball; num++) {
		if (num <= 3) {
			hedef[num].x = 80;
			hedef[num].y = 80 - num * 40;
			hedef[num].radius = 20;
			hedef[num].color = (Color){ 255, 255, 255, 0 };
			hedef[num].active = false;
			hedef[num].moving = true;
		}
		else if (sonuncuNum - 3 <= num) {
			hedef[num].x = 80;
			hedef[num].y = 80 - num * 40;
			hedef[num].radius = 20;
			hedef[num].color = (Color){ 255, 255, 255, 0 };
			hedef[num].active = false;
			hedef[num].moving = true;
		}
		else {
			hedef[num].x = 80;
			hedef[num].y = 80 - num * 40;
			hedef[num].radius = 20;
			hedef[num].color = giveColor();
			hedef[num].active = true;
			hedef[num].moving = true;
		}

		targetCreator(&head, &hedef[num]);
	}

	mermi.ballPos = (Vector2){ screenWidth / 2, screenHeight / 2 };
	mermi.ballSpeed = (Vector2){ 0 , 0 };
	mermi.radius = 20.0;
	mermi.color = giveColorBullet(head);
	mermi.isFired = false;
	mermi.active = true;
}

void initGame2() {
	int sonuncuNum = maxball;

	for (int num = 1; num <= maxball; num++) {
		if (num <= 3) {
			hedef[num].x = 80;
			hedef[num].y = 80 - num * 40;
			hedef[num].radius = 20;
			hedef[num].color = (Color){ 255, 255, 255, 0 };
			hedef[num].active = false;
			hedef[num].moving = true;
		}
		else if (sonuncuNum - 3 <= num) {
			hedef[num].x = 80;
			hedef[num].y = 80 - num * 40;
			hedef[num].radius = 20;
			hedef[num].color = (Color){ 255, 255, 255, 0 };
			hedef[num].active = false;
			hedef[num].moving = true;
		}
		else {
			hedef[num].x = 80;
			hedef[num].y = 80 - num * 40;
			hedef[num].radius = 20;
			hedef[num].color = giveColor();
			hedef[num].active = true;
			hedef[num].moving = true;
		}

		targetCreator(&head, &hedef[num]);
	}

	mermi.ballPos = (Vector2){ screenWidth / 2, screenHeight / 2 };
	mermi.ballSpeed = (Vector2){ 0 , 0 };
	mermi.radius = 20.0;
	mermi.color = giveColorBullet(head);
	mermi.isFired = false;
	mermi.active = true;
}


void targetCreator(node** head, target* hedef) {
	node* new_node = (node*)malloc(sizeof(node));
	if (new_node == NULL) {
		return;
	}

	new_node->data = (target*)malloc(sizeof(target));
	if (new_node->data == NULL) {
		free(new_node);
		return;
	}

	new_node->data->x = hedef->x;
	new_node->data->y = hedef->y;
	new_node->data->radius = hedef->radius;
	new_node->data->color = hedef->color;
	new_node->data->active = hedef->active;
	new_node->data->moving = hedef->moving;
	new_node->next = NULL;
	new_node->previous = NULL;

	if (*head == NULL) {
		*head = new_node;
	}
	else {
		node* current = *head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = new_node;
		new_node->previous = current;
	}
}

void updateGame() {

	node* current = head;
	while (current->next != NULL) {
		if (current->data->active == true) activeCounter++;
		current = current->next;
	}
	totalActive = activeCounter;

	if (totalActive == 0) {
		maxball += 5;
		initGame2();
		SetTargetFPS(120);
	}
	else activeCounter = 0;

	mermi.ballPos.x += mermi.ballSpeed.x;
	mermi.ballPos.y += mermi.ballSpeed.y;

	if (mermi.isFired == true) {
		mermi.ballPos.x += cos(aimingAngle) * 4.0f;
		mermi.ballPos.y += sin(aimingAngle) * 4.0f;
	}

	if (mermi.ballPos.x > (float)screenWidth + 20.0 || mermi.ballPos.x < -20.0 || mermi.ballPos.y >(float)screenHeight + 20.0 || mermi.ballPos.y < -20.0) {
		mermi.active = false;
	}

	if (mermi.active == false) {
		if (totalActive > 0) {
			mermi.color = giveColorBullet(head);
			mermi.ballPos = (Vector2){ screenWidth / 2, screenHeight / 2 };
			mermi.ballSpeed.x = 0.0;
			mermi.ballSpeed.y = 0.0;
			mermi.isFired = false;
			mermi.active = true;
		}
		else if (totalActive <= 0) {
			mermi.color = GREEN;
			mermi.ballPos = (Vector2){ screenWidth / 2, screenHeight / 2 };
			mermi.ballSpeed.x = 0.0;
			mermi.ballSpeed.y = 0.0;
			mermi.isFired = false;
			mermi.active = true;
		}
	}

	if (mermi.active == true && totalActive <= 0) {
		mermi.isFired = false;
	}
}

target* createOne(bullet mermi) {
	target* newCreated = (target*)malloc(sizeof(target));
	if (newCreated != NULL) {
		node* current = head;

		while (current->data != shotTargetIndex(&head, &mermi)) {
			current = current->next;
		}

		switch (whereTarget(current)) {
		case 1: // aþaðý
			newCreated->x = current->data->x;
			newCreated->y = current->data->y - 40;
			break;
		case 2: // yukarý
			newCreated->x = current->data->x;
			newCreated->y = current->data->y + 40;
			break;
		case 3: //saða
			newCreated->x = current->data->x - 40;
			newCreated->y = current->data->y;
			break;
		case 4: // sola
			newCreated->x = current->data->x + 40;
			newCreated->y = current->data->y;
			break;
		}

		newCreated->radius = 20;
		newCreated->color = mermi.color;
		newCreated->active = true;
		newCreated->moving = true;
	}
	return newCreated;
}


node* addTargetBetween(target* newCreated, target* shotTargetIndex) {
	node* current = head;
	node* shotted = NULL;

	node* new = (node*)malloc(sizeof(node));
	if (new == NULL) return NULL;

	while (current->next != NULL) {
		current = current->next;
	}

	while (current->previous->data != shotTargetIndex) {
		current = current->previous;
	}

	shotted = current->previous;

	new->data = newCreated;

	new->next = current;
	new->previous = shotted;
	current->previous = new;
	shotted->next = new;

	return new;
}

void updateTarget(node** head) {
	node* current = *head;

	while (current->next != NULL) {
		target* selected = current->data;
		if (selected->moving == true) {

			if ((selected->x == 80) && (selected->y < screenHeight - 80))selected->y++;
			if ((selected->y == screenHeight - 80) && (selected->x < screenWidth - 80)) selected->x++;
			if ((selected->x == screenWidth - 80) && (selected->y > 80))selected->y--;
			if ((selected->y == 80) && (selected->x > 300)) current->data->x--;
			if ((selected->x == 300) && (selected->y < screenHeight - 160)) current->data->y++;

			if ((selected->y == screenHeight - 160) && (selected->x < screenWidth - 300) && (selected->x != 80)) selected->x++;
			if ((selected->x == screenWidth - 300) && (selected->y > 160) && (selected->y != screenHeight - 80)) selected->y--;
			if ((selected->y == 160) && (selected->x > 550) && (selected->x != screenWidth - 80)) selected->x--;
			if ((selected->x == 550) && (selected->y < screenHeight / 2) && (selected->y != 80)) selected->y++;

			if ((selected->x == 550) && (selected->y == screenHeight / 2)) selected->active = false;

			if ((selected->x == 550) && (selected->y == screenHeight / 2 - 10) && (selected->moving == true) && (selected->active == true)) {
				healthCounter++;
			}

			if ((health.width >= 80) && (healthCounter == 1)) health.width--;
			if ((health.width >= 40) && (healthCounter == 2)) health.width--;
			if ((health.width >= 0) && (healthCounter == 3)) health.width--;
		
		}
		current = current->next;
	}
}

int whereTarget(node* given) {
	node* current = given;

	target* selected = current->data;
	if ((selected->x == 80) && (selected->y < screenHeight - 80)) return 1; //aþaðý gidiyor
	if ((selected->y == screenHeight - 80) && (selected->x < screenWidth - 80)) return 3; // saða gidiyor
	if ((selected->x == screenWidth - 80) && (selected->y > 80)) return 2; // yukarý gidiyor
	if ((selected->y == 80) && (selected->x > 300)) return 4; // sola gidiyor
	if ((selected->x == 300) && (selected->y < screenHeight - 160)) return 1;

	if ((selected->y == screenHeight - 160) && (selected->x < screenWidth - 300) && (selected->x != 80)) return 3;
	if ((selected->x == screenWidth - 300) && (selected->y > 160) && (selected->y != screenHeight - 80)) return 2;
	if ((selected->y == 160) && (selected->x > 550) && (selected->x != screenWidth - 80)) return 4;
	if ((selected->x == 550) && (selected->y < screenHeight / 2) && (selected->y != 80)) return 1;

	else return 0;
}


int checkCollision(node* head, bullet* mermi) {
	node* current = head;

	while (current->next != NULL) {
		Vector2 hedefCenter = { current->data->x, current->data->y };
		Vector2 mermiCenter = { mermi->ballPos.x, mermi->ballPos.y };
		if (current->data->active == true && CheckCollisionCircles(hedefCenter, 20, mermiCenter, 20)) {
			mermi->active = false;
			mermi->isFired = false;
			return 1;
		}
		current = current->next;
	}
	return 0;
}

target* shotTargetIndex(node** head, bullet* mermi) {
	node* current = *head;

	while (current->next != NULL) {
		Vector2 hedefCenter = { current->data->x, current->data->y };
		Vector2 mermiCenter = { mermi->ballPos.x, mermi->ballPos.y };

		if (current->data->active == true && CheckCollisionCircles(hedefCenter, 20, mermiCenter, 20)) {
			return current->data;
		}
		current = current->next;
	}
	return NULL;
}

void isBoom() {
	node* vurulan = head;
	node* eklenen = NULL;


	while (vurulan->next != NULL && vurulan->data != shotTargetIndex(&head, &mermi)) {
		vurulan = vurulan->next;
	}
	eklenen = vurulan->next;
	eklenen = eklenen->previous;

	if ((isSameColor(eklenen->data->color, mermi.color) && isSameColor(eklenen->data->color, eklenen->next->data->color) && isSameColor(eklenen->data->color, eklenen->previous->data->color))
		|| (isSameColor(eklenen->data->color, mermi.color) && isSameColor(eklenen->data->color, eklenen->next->data->color) && isSameColor(eklenen->next->data->color, eklenen->next->next->data->color))
		|| (isSameColor(eklenen->data->color, mermi.color) && isSameColor(eklenen->data->color, eklenen->previous->data->color) && isSameColor(eklenen->previous->data->color, eklenen->previous->previous->data->color))) {

		
		eklenen->data->active = false; //ekleneni yok et
		score += 10;
		
		hold = (Vector2){ eklenen->previous->data->x, eklenen->previous->data->y };
		holdBallNext = eklenen->next;
		holdBallPre = eklenen->previous;

		node* current = eklenen;
		while (current->data->active == false && isSameColor(current->next->data->color, current->data->color)) { //eklenenin arkasýndakileri de yok et
			holdBallNext = current->next;
			current->next->data->active = false;
			score += 10;
			current = current->next;
		}
		holdBallNext = current->next;
		PlaySound(effect);
		current = eklenen;
		while (current->data->active == false && isSameColor(current->previous->data->color, current->data->color)) { //eklenenin önündekileri de yok et
			hold = (Vector2){ current->previous->data->x, current->previous->data->y };
			holdBallPre = current->previous;
			current->previous->data->active = false;
			score += 10;
			current = current->previous;
		}
		holdBallPre = current->previous;

		while (!(holdBallNext->data->x == hold.x && holdBallNext->data->y == hold.y)) {
			while (current->previous != NULL) { //öndekileri durdur
				current->previous->data->moving = false;
				current = current->previous;
			}
			updateTarget(&head);
		}

		current = head;
		while (current->next != NULL) {
			current->data->moving = true;
			current = current->next;
		}

		holdBallPre->next = holdBallNext;
		holdBallNext->previous = holdBallPre;
	}
}

void stepBack(node* head, node* newCreated) {
	node* current = head;

	while (current->next != NULL) {
		current = current->next;
	}

	while (current->previous != NULL && current->previous != newCreated) {
		current = current->previous;
	}
	current = current->previous;


	while (current != NULL) {
		int hamle = 40;

		switch (whereTarget(current)) {
		case 1: // aþaðý
			if (current->data->x == 80) {

				while (current->data->y < screenHeight - 80 && hamle > 0) {
					current->data->y += 1;
					hamle -= 1;
				}
				while (current->data->y == screenHeight - 80 && hamle > 0) {
					current->data->x += 1;
					hamle -= 1;
				}

			}
			else if (current->data->x == 300) {

				while (current->data->y < screenHeight - 160 && hamle > 0) {
					current->data->y += 1;
					hamle -= 1;
				}
				while (current->data->y == screenHeight - 160 && hamle > 0) {
					current->data->x += 1;
					hamle -= 1;
				}

			}
			else if (current->data->x == 550) {

				while (current->data->y < screenHeight / 2 && hamle > 0) {
					current->data->y += 1;
					hamle -= 1;
				}
				while (current->data->y >= screenHeight / 2 && hamle > 0) {
					current->data->y = screenHeight / 2;
					hamle -= 1;
				}

			}
			break;

		case 2: // yukarý
			if (current->data->x == screenWidth - 80) {

				while (current->data->y > 80 && hamle > 0) {
					current->data->y -= 1;
					hamle -= 1;
				}
				while (current->data->y == 80 && hamle > 0) {
					current->data->x -= 1;
					hamle -= 1;
				}

			}
			else if (current->data->x == screenWidth - 300) {

				while (current->data->y > 160 && hamle > 0) {
					current->data->y -= 1;
					hamle -= 1;
				}
				while (current->data->y == 160 && hamle > 0) {
					current->data->x -= 1;
					hamle -= 1;
				}

			}
			break;

		case 3: //saða
			if (current->data->y == screenHeight - 80) {

				while (current->data->x < screenWidth - 80 && hamle > 0) {
					current->data->x += 1;
					hamle -= 1;
				}
				while (current->data->x == screenWidth - 80 && hamle > 0) {
					current->data->y -= 1;
					hamle -= 1;
				}

			}
			else if (current->data->y == screenHeight - 160) {

				while (current->data->x < screenWidth - 300 && hamle > 0) {
					current->data->x += 1;
					hamle -= 1;
				}
				while (current->data->x == screenWidth - 300 && hamle > 0) {
					current->data->y -= 1;
					hamle -= 1;
				}

			}

		case 4: // sola
			if (current->data->y == 80) {

				while (current->data->x > 300 && hamle > 0) {
					current->data->x -= 1;
					hamle -= 1;
				}
				while (current->data->x == 300 && hamle > 0) {
					current->data->y += 1;
					hamle -= 1;
				}

			}
			else if (current->data->y == 160) {

				while (current->data->x > 550 && hamle > 0) {
					current->data->x -= 1;
					hamle -= 1;
				}
				while (current->data->x == 550 && hamle > 0) {
					current->data->y += 1;
					hamle -= 1;
				}

			}
			break;
		}
		current = current->previous;
	}
	return;
}


void drawTargets(node* head) {
	node* current = head;
	while (current->next != NULL) {
		if (current->data->active == true) {
			if (isSameColor(current->data->color, RED)) {
				DrawTexture(redball, current->data->x, current->data->y, WHITE);
			}
			if (isSameColor(current->data->color, BLUE)) {
				DrawTexture(blueball, current->data->x, current->data->y, WHITE);
			}
			if (isSameColor(current->data->color, GREEN)) {
				DrawTexture(greenball, current->data->x, current->data->y, WHITE);
			}
			if (isSameColor(current->data->color, YELLOW)) {
				DrawTexture(yellowball, current->data->x, current->data->y, WHITE);
			}
			if (isSameColor(current->data->color, PURPLE)) {
				DrawTexture(purpleball, current->data->x, current->data->y, WHITE);
			}
			if (isSameColor(current->data->color, BLACK)) {
				DrawTexture(blackball, current->data->x, current->data->y, WHITE);
			}
		}
		current = current->next;
	}
}

void freeTargets(node* head) {
	node* current = head;
	while (current != NULL) {
		node* next = current->next;
		free(current->data);
		free(current);
		current = next;
	}
}

void bulletFire() {
	if (mermi.isFired == false && mermi.active == true) {
		mermi.ballPos = (Vector2){ screenWidth / 2, screenHeight / 2 };
		Vector2 mouse = GetMousePosition();

		if (mouse.x > screenWidth / 2 && mouse.y < screenHeight / 2) aimingAngle = atan(-(mermi.ballPos.y - mouse.y) / (mouse.x - mermi.ballPos.x));
		if (mouse.x < screenWidth / 2 && mouse.y < screenHeight / 2) aimingAngle = PI - atan((mermi.ballPos.y - mouse.y) / (mouse.x - mermi.ballPos.x));
		if (mouse.x < screenWidth / 2 && mouse.y > screenHeight / 2) aimingAngle = (PI)-atan((mermi.ballPos.y - mouse.y) / (mouse.x - mermi.ballPos.x));
		if (mouse.x > screenWidth / 2 && mouse.y > screenHeight / 2) aimingAngle = (2 * PI) - atan((mermi.ballPos.y - mouse.y) / (mouse.x - mermi.ballPos.x));

		mermi.isFired = true;
	}
}

Color giveColor() {
	int random;
	if (45 > maxball && maxball >= 30) random = GetRandomValue(1, 4);
	else if (65 > maxball && maxball >= 45) random = GetRandomValue(1, 5);
	else if (maxball >= 65) random = GetRandomValue(1, 6);
	else random = GetRandomValue(1, 3);

	if (random == 2) return RED;
	else if (random == 3) return BLUE;
	else if (random == 4) return YELLOW;
	else if (random == 5) return PURPLE;
	else if (random == 6) return BLACK;
	else return GREEN;
}

Color giveColorBullet(node* head) {
	node* current = head;
	int random;
	if (45 > maxball && maxball >= 30) random = GetRandomValue(1, 4);
	else if (65 > maxball && maxball >= 45) random = GetRandomValue(1, 5);
	else if (maxball >= 65) random = GetRandomValue(1, 6);
	else random = GetRandomValue(1, 3);

	current = head;
	while (current->next != NULL) {
		if (random == 1 && isSameColor(current->data->color, RED) && (current->data->active == true)) {
			return RED;
		}
		else if (random == 2 && isSameColor(current->data->color, BLUE) && (current->data->active == true)) {
			return BLUE;
		}
		else if (random == 3 && isSameColor(current->data->color, GREEN) && (current->data->active == true)) {
			return GREEN;
		}
		else if (random == 4 && isSameColor(current->data->color, YELLOW) && (current->data->active == true)) {
			return YELLOW;
		}
		else if (random == 5 && isSameColor(current->data->color, PURPLE) && (current->data->active == true)) {
			return PURPLE;
		}
		else if (random == 6 && isSameColor(current->data->color, BLACK) && (current->data->active == true)) {
			return BLACK;
		}
		current = current->next;
	}
	giveColorBullet(head);
}

bool isSameColor(Color color1, Color color2) {
	return (color1.r == color2.r && color1.g == color2.g && color1.b == color2.b && color1.a == color2.a);
}

GameScreen updateScreen() {
	switch (healthCounter) { //ekran değişimi
	case 0: case 1: case 2:
		return GAMEPLAY; break;
	default:
		return GAMEOVER; break;
	}
}


int highScore(int score) {
	int oldScore = 0;
	fptr = fopen("highscore.txt", "r");
		fscanf(fptr, "%d", &oldScore);
	fclose(fptr);

	if (score>oldScore) {
		fptr = fopen("highscore.txt", "w");
		fprintf(fptr, "%d", score);
		fclose(fptr);
		return score;
	}

	return oldScore;
}