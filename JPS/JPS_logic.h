#pragma once

#define GRID_LEN 20
#define GRID_NUM 25

enum eDir
{
	nDir,
	LL,
	LU,
	UU,
	RU,
	RR,
	RD,
	DD,
	LD,
};

enum nColor
{
	NONE,
	WALL,
	START,
	END,
	OPEN,
	CLOSE,
	RAN1,
	RAN2,
	RAN3
};

struct Node
{
	int _x = -999;
	int _y = -999;
	Node* _parent = nullptr;
	float _g = -999.f;
	float _h = -999.f;
	float _f = -999.f;
};

extern char g_map[GRID_NUM][GRID_NUM * 2];
extern std::list<Node*> g_openList;// -Heap..F 작은 노드... x, y 검색
extern std::list<Node*> g_closeList; // -List..x, y 검색
extern Node g_startPos;
extern Node g_endPos;
extern HWND g_hWnd;
extern HDC g_hdc;
extern nColor g_throwColor;
extern HBRUSH g_colorArr[10];
extern int g_randNum;

void JPS_Find(void);
void SetStart(void);
bool PathFind(int Sx, int Sy, int Ex, int Ey);

bool CheckTile(int X, int Y);
bool CheckRange(int X, int Y);
eDir CheckDir(Node* pNode);

/////////
bool ThrowNode(Node* pNode);
bool ThrowAndMake(int x, int y, Node* pNode, eDir dir);
bool ClimbingRecursive(int x, int y, Node* pNode, eDir dir);
/////////

void AddOpenList(int x, int y, Node* pNode);
void ReleaseList(void);