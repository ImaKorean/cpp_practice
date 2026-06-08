#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdlib.h>


#pragma region Define
#define BufferWidth 80	// 가로 버퍼 크기
#define BufferHeight 40 // 세로 버퍼 크기
#define EnemiesCount 10
#define HealthCount 4
#pragma endregion
#pragma region DoubleBuffer
//버퍼 초기화


HANDLE hBuffer[2];	//창 두게를 제어하는 핸들
int screenIndex;	//hBuffer[screenIndex], screenIndex == 0 or 1


void InitBuffer();
void FlipBuffer();
void ClearBuffer();
void WriteBuffer(int x, int y, const char* shape, int color);
void CloseBuffer();

#pragma endregion
#pragma region Enum
enum Color
{
	BLACK,
	DARKBLUE,
	DARKGREEN,
	DARKCYAN,
	DARKRED,
	DARKMAGENTA,
	DARKYELLOW,
	GRAY,
	DARKGRAY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	YELLOW,
	WHITE,
};

enum SCENE_ID {
	LOGO,
	MENU,
	STAGE,
};

enum Menu_ID {
	START,
	QUIT
};


#pragma endregion
#pragma region Game
void LogoInit();
void LogoProgress();
void LogoRender(int cherryColor, int hide);
void LogoRelease();

void MenuInit();
void MenuProgress();
void MenuRender();
void MenuRelease();

void StageInit();
void StageProgress();
void StageRender();
void StageRelease();
#pragma endregion
#pragma region Vairables

SCENE_ID id;
Menu_ID menuId;

int time = 0;
int maxSpawnTime = 50;
int sleeptime = 0;
int spawn = 0;
int j = 0;

#pragma endregion
#pragma region structs
typedef struct Obj
{
	int x;
	int y;
	Color color;
	const char* shape;
}Hero, Cursur;

Cursur* cur = nullptr;

Hero* heros = nullptr;

struct Enemies
{
	int x;
	int y;
	Color color;
	const char* shape;
	int state;
};

Enemies* enemies[EnemiesCount] = {};

struct Health
{
	int x;
	int y;
	Color color;
	const char* shape;
};

Health* enemies[HealthCount] = {};

#pragma endregion


int main() {
	id = LOGO;
	int cherryColor=0;
	int hide = 0;
	int framecount=0;
	sleeptime = 100;
	LogoInit();
	MenuInit();
	InitBuffer();
	while (true)
	{

		switch (id)
		{
		case LOGO:

			framecount++;

			switch ((framecount / 10 % 4)) {
			case 0: cherryColor = DARKRED; hide = 0; break;
			case 1: cherryColor = DARKYELLOW; hide = 1; break;
			case 2: cherryColor = DARKGRAY; hide = 0; break;
			case 3: cherryColor = CYAN; hide = 1; break;
			}

			LogoProgress();
			LogoRender(cherryColor, hide);
			break;
		case MENU:
			MenuProgress();
			MenuRender();
			break;
		case STAGE:
			StageProgress();
			StageRender();
			break;
		default:
			break;
		}

		FlipBuffer();
		ClearBuffer();

		
		Sleep(sleeptime);
	}

	CloseBuffer();
	return 0;
}


#pragma region DoubleBuffer
//버퍼 초기화
void InitBuffer()
{
	//현재 스크린 index값은 0
	screenIndex = 0;

	//size.x = 80, size.y = 40 : 화면 크기
	COORD size = { BufferWidth, BufferHeight };

	//창 크기 왼쪽 : 0, 위쪽 : 0 오른쪽 : 80 - 1, 아래쪽 : 40 - 1
	SMALL_RECT rect = { 0,0,BufferWidth - 1, BufferHeight - 1 };

	//0번 버퍼 만들기
	hBuffer[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	//0번 화면 버퍼 사이즈 설정(만든 버퍼 주소, 크기)
	SetConsoleScreenBufferSize(hBuffer[0], size);
	//0번 창 사이즈 설정
	SetConsoleWindowInfo(hBuffer[0], TRUE, &rect);

	//1번 버퍼 만들기
	hBuffer[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	//1번 화면 버퍼 사이즈 설정(만든 버퍼 주소, 크기)
	SetConsoleScreenBufferSize(hBuffer[1], size);
	//1번 창 사이즈 설정
	SetConsoleWindowInfo(hBuffer[1], TRUE, &rect);

	CONSOLE_CURSOR_INFO info;	//콘솔 커서 정보 구조체
	info.dwSize = 1;			//커서 크기
	info.bVisible = FALSE;		//커서 안보이게

	//0 & 1 각각의 버퍼에 커서 정보 등록
	SetConsoleCursorInfo(hBuffer[0], &info);
	SetConsoleCursorInfo(hBuffer[1], &info);
}


//버퍼 뒤집기 0->1 1->0 
void FlipBuffer()
{
	//해당 버퍼 활성화
	SetConsoleActiveScreenBuffer(hBuffer[screenIndex]);

	//[0]->[1] / [1]->[0]
	screenIndex = !screenIndex;
}

//버퍼 창 깨끗이
void ClearBuffer()
{
	//시작 위치
	//pos.x = 0; pos.y = 0;
	COORD pos = { 0,0 };
	//매개변수 값 넣어줄려고
	DWORD dw;

	//화면을 ' '로 채움
	FillConsoleOutputCharacter(hBuffer[screenIndex], ' ', BufferWidth * BufferHeight, pos, &dw);
}


//문자열 원하는 위치에 써주기 & 색깔 까지
//WriteBuffer(x좌표, y좌표, 문자열, 색깔 인덱스)
void WriteBuffer(int x, int y, const char* shape, int color)
{
	//위치 설정
	//pos.x = x * 2; pos.y = y;
	COORD pos = { x * 2, y };

	//커서 위치 이동
	SetConsoleCursorPosition(hBuffer[screenIndex], pos);

	//색깔 바꿔주기
	SetConsoleTextAttribute(hBuffer[screenIndex], color);

	//매개변수 값을 넣기 위해
	DWORD dw;

	//해당 버퍼에 쓰기
	WriteFile(hBuffer[screenIndex], shape, strlen(shape), &dw, NULL);
}

//버퍼 해제
void CloseBuffer()
{
	//버퍼 닫기
	CloseHandle(hBuffer[0]);
	CloseHandle(hBuffer[1]);
}


#pragma endregion

#pragma region Game
void LogoInit()
{


}
void LogoProgress()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		LogoRelease();
		id = MENU;
		MenuInit();
		return;
	}

}
void LogoRender(int cherryColor, int hide)
{

	for (int i = 0; i < 20; i++)
	{
		if (rand() % 3 == 0)
		{
			int x =rand() % 80;
			int y =rand() % 40;

			WriteBuffer(x, y, "*", cherryColor);
		}
	}

	// 체리 (5 ~ 24)
	WriteBuffer(10, 5, "                   d888P", cherryColor);
	WriteBuffer(10, 6, "         d8b d8888P:::P", cherryColor);
	WriteBuffer(10, 7, "       d:::888b::::::P", cherryColor);
	WriteBuffer(10, 8, "      d:::dP8888b:d8P", cherryColor);
	WriteBuffer(10, 9, "     d:::dP 88b  Yb   .d8888b.", cherryColor);
	WriteBuffer(10, 10, "    d::::P  88Yb  Yb .P::::::Y8b", cherryColor);
	WriteBuffer(10, 11, "    8:::8   88`Yb  YbP::::   :::b", cherryColor);
	WriteBuffer(10, 12, "    8:::P   88 `8   8!:::::::::::b", cherryColor);
	WriteBuffer(10, 13, "    8:dP    88  Yb d!!!::::::::::8", cherryColor);
	WriteBuffer(10, 14, "    8P    ..88   Yb8!!!::::::::::P", cherryColor);
	WriteBuffer(10, 15, "     .d8:::::Yb  d888VKb:!:!::!:8", cherryColor);
	WriteBuffer(10, 16, "    d::::::  ::dP:::::::::b!!!!8", cherryColor);
	WriteBuffer(10, 17, "   8!!::::::::P::::::::::::b!8P", cherryColor);
	WriteBuffer(10, 18, "   8:!!::::::d::::::: ::::::b", cherryColor);
	WriteBuffer(10, 19, "   8:!:::::::8!:::::::  ::::8", cherryColor);
	WriteBuffer(10, 20, "   8:!!!:::::8!:::::::::::::8", cherryColor);
	WriteBuffer(10, 21, "   Yb:!!:::::8!!::::::::::::8", cherryColor);
	WriteBuffer(10, 22, "    8b:!!!:!!8!!!:!:::::!!:dP", cherryColor);
	WriteBuffer(10, 23, "     `8b:!!!:Yb!!!!:::::!d88", cherryColor);
	WriteBuffer(10, 24, "         \"\"\"  Y88!!!!!!!d8P", cherryColor);

	// 타이틀 (28 ~ 35)
	WriteBuffer(10, 28, "====================================", WHITE);
	WriteBuffer(10, 29, "         FOOD DODGE GAME", WHITE);
	WriteBuffer(10, 30, "====================================", WHITE);

	if (hide == 0) {
		WriteBuffer(10, 35, "      PRESS ENTER TO START", WHITE);
	}
	else if (hide == 1) {
		WriteBuffer(10, 35, "      PRESS ENTER TO START", BLACK);
	}

}
void LogoRelease()
{
}
void MenuInit()
{
	cur = (Cursur*)malloc(sizeof(Cursur));
	cur -> x = 30;
	cur -> y = 27; //35
	cur->shape = "";
}
void MenuProgress()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		if (menuId == START) {
			MenuRelease();
			id = STAGE;
			StageInit();
		}
		else {
			exit(true);
		}
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (cur->y == 35) {
			cur->y = 27;
		}
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		if (cur->y == 27) {
			cur->y = 35;
		}
	}
}
void MenuRender()
{
	if (cur == nullptr)
		return; 

	for (int i = 0; i < 20; i++)
	{
		if (rand() % 3 == 0)
		{
			int x = rand() % 80;
			int y = rand() % 40;

			WriteBuffer(x, y, "*", WHITE);
		}
	}


	WriteBuffer(2, 2, "   _______   ", YELLOW);
	WriteBuffer(2, 3, " /-------\\ ", YELLOW);
	WriteBuffer(2, 4, "| LETTUCE |", GREEN);
	WriteBuffer(2, 5, "| BEEF    |", RED);
	WriteBuffer(2, 6, " \\_______/ ", YELLOW);

	WriteBuffer(18, 2, "     /\\", YELLOW);
	WriteBuffer(18, 3, "    /oo\\", YELLOW);
	WriteBuffer(18, 4, "   /oooo\\", YELLOW);
	WriteBuffer(18, 5, "  /oooooo\\", YELLOW);
	WriteBuffer(18, 6, " /________\\", WHITE);

	WriteBuffer(34, 2, "   ___", WHITE);
	WriteBuffer(34, 3, " .'o o'.", WHITE);
	WriteBuffer(34, 4, "( o o o )", WHITE);
	WriteBuffer(34, 5, " '.___.'", WHITE);
	WriteBuffer(34, 6, "    ||", YELLOW);
	WriteBuffer(34, 7, "   /  \\", YELLOW);

	WriteBuffer(18, 10, "   ,--.", WHITE);
	WriteBuffer(18, 11, " / o  o\\", WHITE);
	WriteBuffer(18, 12, "|  oo  |", WHITE);
	WriteBuffer(18, 13, " \\____/", WHITE);
	WriteBuffer(18, 14, "    ||===", YELLOW);

	WriteBuffer(34, 10, "    | |", WHITE);
	WriteBuffer(34, 11, "  __|_|__", WHITE);
	WriteBuffer(34, 12, " |^^^^^^|", MAGENTA);
	WriteBuffer(34, 13, " |______|", WHITE);

	WriteBuffer(2, 10, "  || || || ", YELLOW);
	WriteBuffer(2, 11, "  || || || ", YELLOW);
	WriteBuffer(2, 12, "  || || || ", YELLOW);
	WriteBuffer(2, 13, " /--------\\", RED);
	WriteBuffer(2, 14, "|  FRIES  |", RED);
	WriteBuffer(2, 15, " \\________/", RED);

	// START

	 

	if (cur->y == 27) {
		WriteBuffer(18, 25, "  #####  #######   ###   ######  ####### ", RED);
		WriteBuffer(18, 26, " ##         #     ## ##  ##   ##    #    ", RED);
		WriteBuffer(18, 27, "  #####     #    ####### ######     #    ", RED);
		WriteBuffer(18, 28, "      ##    #    ##   ## ##  ##     #    ", RED);
		WriteBuffer(18, 29, " #####      #    ##   ## ##   ##    #    ", RED);
		menuId = START;
	}
	else {
		WriteBuffer(18, 25, "  #####  #######   ###   ######  ####### ", WHITE);
		WriteBuffer(18, 26, " ##         #     ## ##  ##   ##    #    ", WHITE);
		WriteBuffer(18, 27, "  #####     #    ####### ######     #    ", WHITE);
		WriteBuffer(18, 28, "      ##    #    ##   ## ##  ##     #    ", WHITE);
		WriteBuffer(18, 29, " #####      #    ##   ## ##   ##    #    ", WHITE);
	}

	if (cur->y == 35) {
		WriteBuffer(22, 33, "  ####   #    # ### ####### ", RED);
		WriteBuffer(22, 34, " #    #  #    #  #     #    ", RED);
		WriteBuffer(22, 35, " #  # #  #    #  #     #    ", RED);
		WriteBuffer(22, 36, " #    #  #    #  #     #    ", RED);
		WriteBuffer(22, 37, "  ####    ####  ###    #    ", RED);
		menuId = QUIT;
	}
	else {
		WriteBuffer(22, 33, "  ####   #    # ### ####### ", WHITE);
		WriteBuffer(22, 34, " #    #  #    #  #     #    ", WHITE);
		WriteBuffer(22, 35, " #  # #  #    #  #     #    ", WHITE);
		WriteBuffer(22, 36, " #    #  #    #  #     #    ", WHITE);
		WriteBuffer(22, 37, "  ####    ####  ###    #    ", WHITE);
	}

	// QUIT

	WriteBuffer(cur->x, cur->y, cur->shape, cur->color);
}
void MenuRelease()
{
	if (cur != nullptr) {
		free(cur);
		cur = nullptr;
	}

}
void StageInit()
{
	heros = (Hero*)malloc(sizeof(Hero));
	heros->x = 20;
	heros->y = 37;
	heros->shape = "★";
	heros->color = BLUE;
	int k = 0;

	for (int i = 0; i < EnemiesCount; i++) {
		enemies[i] = (Enemies*)malloc(sizeof(Enemies));
		enemies[i]->x = 2+k;
		enemies[i]->y = 1;
		enemies[i]->shape = "○";
		enemies[i]->color = RED;
		k += 4;
	}
}

void StageProgress()
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if ((heros->x) > 0)
			heros->x--;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if ((heros->x) < 39)
			heros->x++;
	}
}

void StageRender()
{
	WriteBuffer(heros->x, heros->y, heros->shape, heros->color);
	if (time == 0) {
		j = rand() % 10;
		enemies[j]->state = 1;
	}
	for (int i = 0; i < EnemiesCount; i++) {
		if (enemies[i]->state == 1) {
			WriteBuffer(enemies[i]->x, enemies[i]->y, enemies[i]->shape, enemies[i]->color);
			enemies[i]->y++;
			if (enemies[i]->y == 38) {
				enemies[i]->y = 0;
				enemies[i]->state = 0;
				enemies[rand() % 10]->state = 1;
			}
		}

		if ((enemies[i]->x == heros->x) && (enemies[i]->y == heros -> y)) {
			exit(true);
		}

		time++;
		if (time == maxSpawnTime) {
			time = 0;
			if (sleeptime > 15) {
				sleeptime--;
			}
		}
	}

}

void StageRelease()
{
	free(heros);
	heros = nullptr;
}
#pragma endregion
