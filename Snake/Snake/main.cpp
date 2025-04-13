#include<iostream>
#include<Windows.h>
#include<conio.h>
using namespace std;
#define H 20
#define W 40
const int dir[4][2] = {
	{-1,0},//向上
	{+1,0}, //向下
	{0,-1},//向左
	{0,1},//向右
};

const int thelast_dir[4][2] = {
	{1,0},//向上
	{-1,0}, //向下
	{0,+1},//向左
	{0,-1},//向右
};

enum BlockType {
	EMPTY = 0,
	FOOD = 1,

};
struct Map {
	BlockType data[H][W];
	int HasFood;
};

struct Pos {
	int x;
	int y;
};
struct SnakeNode {
	Pos position;
	SnakeNode* next;
};

struct Snake {
	SnakeNode* head;
	SnakeNode* last;
	SnakeNode* Before_last;
	SnakeNode* Log_Lastbefore;
	int snakeDir;
	int snakeLength;
	int moveFrequency;
	int lastMovetime;
};
void drawUnit(Pos p, const char unit[]) {
	COORD coord;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	coord.X = p.y + 1; // 列 (X)
	coord.Y = p.x + 1; // 行 (Y)
	SetConsoleCursorPosition(h, coord);
	cout << unit;

}
void CrashSnake(Snake* snk) {
	SnakeNode* p = snk->head->next;
	while (p != NULL) {
		

		if (snk->head->position.x == p->position.x && snk->head->position.y == p->position.y) {
			drawUnit({ H / 2, W / 2 - 4 }, "Game Over");
			while (1);

		}
		p = p->next;
	}
}
void initMap(Map* map) {
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			map->data[i][j] = BlockType::EMPTY;
		}
	}
	map->HasFood = 0;
}
void addSnake(Snake* snake, int x,int y) {
	
	SnakeNode* newNode = new SnakeNode;
	newNode->next = nullptr;
	newNode->position = { x,y };
	if (snake->head == nullptr) {
		snake->head = newNode; // 如果是第一个节点
		snake->last = newNode;
		snake->Before_last = newNode;
	}
	else {
		snake->Log_Lastbefore = snake->last;
		snake->last->next = newNode; // 添加到链表末尾
		snake->last = newNode;       // 更新最后一个节点
	
	}

	snake->snakeLength++;
}

void initSnake(Snake* snake) {
	snake->head = nullptr;
	snake->last = nullptr;
	snake->Log_Lastbefore = nullptr;
	snake->snakeDir = 1; // 0,1,2,3初始方向
	snake->snakeLength = 0;
	snake->lastMovetime = 0;
	snake->moveFrequency = 200;


	int startX = H / 2; // 初始位置的 x 坐标
	int startY = W / 2; // 初始位置的 y 坐标
	addSnake(snake, startX, startY); // 添加第一个节点
}

void hideCursor() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curInfo = { 1,FALSE };
	SetConsoleCursorInfo(h, &curInfo);
}
void checkDir(Snake* snk) {
	if (_kbhit()) {
		switch (_getch())
		{
		case 'w':
			if (snk->snakeDir != 1) {
				snk->snakeDir = 0;
			}
			break;
		case's':
			if (snk->snakeDir != 0) {
				snk->snakeDir = 1;
				break;
			}
		case'a':
			if (snk->snakeDir != 3) {
				snk->snakeDir = 2;
				break;
			}
		case'd':
			if (snk->snakeDir != 2) {
				snk->snakeDir = 3;
				break;
			}

		default:
				break;
		}
	}
}
void drawMap(Map*map) {
	system("cls");
	cout << "┏";
	for (int i = 0; i < W; i++) {
		cout << "─";
	}

	cout << "┓"<<endl;

	for (int i = 0; i < H; i++) {
		cout << "┃";
		for (int j = 0; j < W; j++) {
			cout << " ";
			
		}
		cout << "┃"<<endl;

	}

	cout << "┗";
	for (int i = 0; i < W; i++) {
		cout << "─";
	}

	cout << "┛" << endl;


}
void addSnake_Tomove(Snake* snk) {
	SnakeNode* p = (*snk).head;
	SnakeNode* newNode = new SnakeNode;
	newNode->position.x = p->position.x + dir[(*snk).snakeDir][0];
	newNode->position.y = p->position.y + dir[(*snk).snakeDir][1];
	newNode->next = p;
	(*snk).head = newNode;

}
void Move(Snake*snk) {
	addSnake_Tomove(snk);
	
}
void Free_Tail(Snake* snk) {
	SnakeNode* p = (*snk).head;
	if (snk->head == nullptr || snk->last == nullptr) return;
	delete snk->last; 
	snk->last = snk->Log_Lastbefore;
	snk->last->next = nullptr;
	while (p != nullptr && p->next != nullptr) {  //更新尾结点日志
		(*snk).Log_Lastbefore = p;
		p = p->next;
	}
}
void CheckEating(Snake* snk, Map* map) {
	if (map->data[snk->head->position.x][snk->head->position.y] == BlockType::FOOD) {
		map->data[snk->head->position.x][snk->head->position.y] = BlockType::EMPTY;

		SnakeNode* tail = new SnakeNode;

		tail->position.x = snk->last->position.x + thelast_dir[(*snk).snakeDir + 1][0];
		tail->position.y = snk->last->position.y + thelast_dir[(*snk).snakeDir][1];
		tail->next = NULL;
		snk->Log_Lastbefore = snk->last;
		snk->last->next = tail;
		snk->last = tail;
		drawUnit(tail->position, "?");
		map->HasFood -= 1;

	}
}
void To_Do_Move(Snake* snk, Map* map) {
	Pos tail = (*snk).last->position;
	drawUnit(tail, " ");//   一。  先删除尾巴，1删动画,2删物理结构
	Free_Tail(snk);

	Move(snk);//二。   移动
	CheckEating(snk, map);
	Pos head = (*snk).head->position;
	drawUnit(head, "?");
	
}
bool Checkout_Bound(Pos p) {
	if (p.x == H  || p.x == 0) {
		return true;
	}
	if (p.y == W  || p.y == 0) {
		return true;
	}
	return false;
}
void CheckfoodGenerate(Snake* snake, Map* map) {
	if (map->HasFood <= 15) {
		while (1)
		{
			int x = rand() % H;
			int y = rand() % W;
			SnakeNode* p = snake->head;
			while (p != NULL) {
				if (p->position.x == x && p->position.y == y) {
					break;
				}
				p = p->next;
			}
			map->data[x][y] = BlockType::FOOD;
			//map->HasFood = true;
			drawUnit({ x , y }, "▲");
			map->HasFood += 1;
			return;
		}
	}
}
void checkSnakeMove(Snake* snk, Map* map) {

	checkDir(snk);
	To_Do_Move(snk, map);

	Sleep(150);
	//CheckfoodGenerate(snk, map);
	CrashSnake(snk);
	if (Checkout_Bound(snk->head->position)) {
		drawUnit({ H / 2, W / 2 - 4 }, "Game Over");
		while (1);
	}

	
}

void drawSnake(Snake* snake)
{
	SnakeNode* current = snake->head;
	while (current!=nullptr)
	{
		drawUnit(current->position, "A");
		current = current->next;
		

	}
}
int main() {
	Map map;
	initMap(&map);
	drawMap(&map);
	hideCursor();
	Snake snk;
	initSnake(&snk);
	addSnake(&snk, H/2, W/2+1);
	addSnake(&snk, H/2, W/2+1+1);
	drawSnake(&snk);
	while (1) {
	checkSnakeMove(&snk, &map);
	CheckfoodGenerate(&snk, &map);
	}

}