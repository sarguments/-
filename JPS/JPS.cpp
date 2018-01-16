#include "stdafx.h"
#include "hoxy_header.h"
#include "JPS.h"
#include "JPS_logic.h"

#define MAX_LOADSTRING 100

//////////////////////////////////////////////////////////////////////

//// JPS ////
char g_map[GRID_NUM][GRID_NUM * 2];
Node g_startPos;
Node g_endPos;

nColor g_throwColor;

std::list<Node*> g_openList;// -Heap..F 작은 노드... x, y 검색
std::list<Node*> g_closeList; // -List..x, y 검색

// 전역 변수:
HINSTANCE g_hInst;
HWND g_hWnd;
HDC g_hdc;
HDC g_memDC;
HBITMAP g_memBit;
RECT g_rect;
HPEN g_oldPen;
HPEN g_goalPen;
HBRUSH g_colorArr[10];

bool g_isLButtonDown;
bool g_isRButtonDown;
bool g_isErase;
bool g_isFindEnd;

bool g_isBreOn;

// 마우스 좌표
int g_mouseX;
int g_mouseY;

int g_breStartX;
int g_breStartY;
int g_breEndX;
int g_breEndY;

int g_OldbreStartX;
int g_OldbreStartY;
int g_OldbreEndX;
int g_OldbreEndY;

////////////////////
int g_randNum;
////////////////////

void Init(void);
void DrawGrid(void);
void DrawRect(void);
void DrawLine(void);
void DrawClear(void);
void DrawAllClear(void);
void lineBresenham(int startX, int startY, int destX, int destY);

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CCmdStart myCmd;

	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	///////////////////////// 그리기 초기화 /////////////////////////////////
	Init();
	/////////////////////////////////////////////////////////////////////////

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void Init(void)
{
	// 색상 초기화
	for (int i = 0; i < GRID_NUM; i++)
	{
		ZeroMemory(g_map[i], GRID_NUM * 2);
	}

	// TODO : 시작, 끝
	g_startPos._x = 3;
	g_startPos._y = 3;

	g_map[g_startPos._y][g_startPos._x] = nColor::START;

	g_endPos._x = 7;
	g_endPos._y = 8;

	g_map[g_endPos._y][g_endPos._x] = nColor::END;

	GetClientRect(g_hWnd, &g_rect);

	g_hdc = GetDC(g_hWnd);
	g_memDC = CreateCompatibleDC(g_hdc);
	g_memBit = CreateCompatibleBitmap(g_hdc, g_rect.right, g_rect.bottom);

	SelectObject(g_memDC, g_memBit);

	g_colorArr[nColor::START] = CreateSolidBrush(RGB(0, 203, 0));
	g_colorArr[nColor::END] = CreateSolidBrush(RGB(238, 68, 0));
	g_colorArr[nColor::OPEN] = CreateSolidBrush(RGB(152, 251, 152));
	g_colorArr[nColor::CLOSE] = CreateSolidBrush(RGB(175, 238, 238));

	// TODO : RAN1
	g_colorArr[nColor::RAN1] = CreateSolidBrush(RGB(33, 100, 44));
	g_colorArr[nColor::RAN2] = CreateSolidBrush(RGB(133, 200, 44));
	g_colorArr[nColor::RAN3] = CreateSolidBrush(RGB(233, 100, 144));
	g_goalPen = CreatePen(PS_SOLID, 2, RGB(254, 255, 2));

	// 그리드 그리기
	DrawGrid();
	InvalidateRect(g_hWnd, NULL, FALSE);
}

void DrawGrid(void)
{
	HBRUSH localBrush = GetStockBrush(WHITE_BRUSH);
	FillRect(g_memDC, &g_rect, localBrush);

	for (int i = 0; i < GRID_NUM; i++)
	{
		for (int j = 0; j < GRID_NUM * 2; j++)
		{
			Rectangle(g_memDC, (GRID_LEN * j) - j, (GRID_LEN * i) - i,
				(GRID_LEN + GRID_LEN * j) - j, (GRID_LEN + GRID_LEN * i) - i);
		}
	}
}

void DrawRect(void)
{
	// 배열 검사해서 번호에 맞는 색 사각형 그리기
	HBRUSH localBrush = NULL;
	HBRUSH oldBrush = NULL;

	for (int i = 0; i < GRID_NUM; i++)
	{
		for (int j = 0; j < GRID_NUM * 2; j++)
		{
			switch (g_map[i][j])
			{
			case nColor::START:
			{
				// 초록
				localBrush = g_colorArr[nColor::START];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::END:
			{
				// 빨강
				localBrush = g_colorArr[nColor::END];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::WALL:
			{
				// 회색
				localBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::OPEN:
			{
				// 연두
				localBrush = g_colorArr[nColor::OPEN];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::CLOSE:
			{
				// 하늘
				localBrush = g_colorArr[nColor::CLOSE];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::NONE:
			{
				localBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::RAN1:
			{
				localBrush = g_colorArr[nColor::RAN1];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::RAN2:
			{
				localBrush = g_colorArr[nColor::RAN2];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			case nColor::RAN3:
			{
				localBrush = g_colorArr[nColor::RAN3];
				oldBrush = (HBRUSH)SelectObject(g_memDC, localBrush);
			}
			break;
			}

			// 겹쳐지는 선 길이 만큼 뺀다
			Rectangle(g_memDC, (GRID_LEN * j) - j, (GRID_LEN * i) - i,
				GRID_LEN + (j * GRID_LEN) - j, GRID_LEN + (i * GRID_LEN) - i);

			SelectObject(g_memDC, oldBrush);
		}
	}
}

void DrawLine(void)
{
	if (g_endPos._parent == nullptr)
	{
		return;
	}

	int endNodeX = (GRID_LEN * g_endPos._x) - g_endPos._x + (GRID_LEN / 2);
	int endNodeY = (GRID_LEN * g_endPos._y) - g_endPos._y + (GRID_LEN / 2);

	int parNodeX = 0;
	int parNodeY = 0;

	g_oldPen = (HPEN)SelectObject(g_memDC, g_goalPen);

	MoveToEx(g_memDC, endNodeX, endNodeY, NULL);

	Node* nowNode = &g_endPos;

	while (nowNode->_parent != nullptr)
	{
		parNodeX = (GRID_LEN * nowNode->_parent->_x) - nowNode->_parent->_x + (GRID_LEN / 2);
		parNodeY = (GRID_LEN * nowNode->_parent->_y) - nowNode->_parent->_y + (GRID_LEN / 2);

		LineTo(g_memDC, parNodeX, parNodeY);
		nowNode = nowNode->_parent;
	}

	SelectObject(g_memDC, g_oldPen);
}

void DrawClear(void)
{
	ReleaseList();

	// 색상 초기화
	for (int i = 0; i < GRID_NUM; i++)
	{
		for (int j = 0; j < GRID_NUM * 2; j++)
		{
			if (g_map[i][j] == nColor::START ||
				g_map[i][j] == nColor::END ||
				g_map[i][j] == nColor::WALL)
			{
				continue;
			}

			g_map[i][j] = nColor::NONE;
		}
	}

	DrawGrid();

	InvalidateRect(g_hWnd, NULL, FALSE);
}

void DrawAllClear(void)
{
	ReleaseList();

	// 색상 초기화
	for (int i = 0; i < GRID_NUM; i++)
	{
		for (int j = 0; j < GRID_NUM * 2; j++)
		{
			if (g_map[i][j] == nColor::START ||
				g_map[i][j] == nColor::END)
			{
				continue;
			}

			g_map[i][j] = nColor::NONE;
		}
	}

	DrawGrid();

	InvalidateRect(g_hWnd, NULL, FALSE);
}

// TODO : 브레즌햄
void lineBresenham(int startX, int startY, int destX, int destY) 
{
	// 기울기
	int dx = destX - startX;
	int dy = destY - startY;

	// 직선의 방향에 따라 선이 나아갈 좌표
	int stepx = (dx > 0) ? 1 : ((dx == 0) ? 0 : -1);
	int stepy = (dy > 0) ? 1 : ((dy == 0) ? 0 : -1);

	// 기울기 절대값으로
	dx = abs(dx);
	dy = abs(dy);

	dx <<= 1;                                     // dx*2 와 같은 의미(비트연산)
	dy <<=  1;                                    // dy*2 와 같은 의미(비트연산)

	if (g_isBreOn)
	{
		g_map[startY][startX] = nColor::WALL;
	}
	else
	{
		g_map[startY][startX] = nColor::NONE;
	}

	if (dx > dy) {
		int fraction = dy - (dx >> 1);     // dx>>1 은 dx/2와 같은 의미(비트연산)
		while (startX != destX) {
			if (fraction >= 0) {
				startY += stepy;
				fraction -= dx;                 // fraction -= 2*dx 과 같은 의미
			}
			startX += stepx;
			fraction += dy;                     // fraction -= 2*dy 과 같은 의미

			/////////////////////////////
			if (g_isBreOn)
			{
				g_map[startY][startX] = nColor::WALL;
			}
			else
			{
				g_map[startY][startX] = nColor::NONE;
			}
			/////////////////////////////
		}
	}
	else {
		int fraction = dx - (dy >> 1);
		while (startY != destY) {
			if (fraction >= 0) {
				startX += stepx;
				fraction -= dy;
			}
			startY += stepy;
			fraction += dx;

			/////////////////////////////
			if (g_isBreOn)
			{
				g_map[startY][startX] = nColor::WALL;
			}
			else
			{
				g_map[startY][startX] = nColor::NONE;
			}
			/////////////////////////////
		}
	}
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JPS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_JPS);
	wcex.lpszClassName = L"myJPSclass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	g_hWnd = CreateWindowW(L"myJPSclass", L"JPS TITLE", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
	{
		switch (wParam)
		{
		case 1:
		{
			g_isFindEnd = PathFind(g_startPos._x, g_startPos._y, g_endPos._x, g_endPos._y);
			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		break;
		}
	}
	break;
	case WM_RBUTTONDBLCLK:
	{
		// 목적지 설정
		int xCount = (g_mouseX + (g_mouseX / GRID_LEN)) / GRID_LEN;
		int yCount = (g_mouseY + (g_mouseY / GRID_LEN)) / GRID_LEN;

		// 범위 벗어나면 예외
		if ((xCount < 0) || (xCount >= GRID_NUM * 2) || (yCount < 0) || (yCount >= GRID_NUM))
		{
			wcout << L"out of area" << endl;
			break;
		}

		g_map[g_endPos._y][g_endPos._x] = nColor::NONE;

		g_endPos._x = xCount;
		g_endPos._y = yCount;

		g_map[g_endPos._y][g_endPos._x] = nColor::END;

		// TODO : endPos = nullptr, 선그리기 제어용
		g_endPos._parent = nullptr;

		wcout << L"R double CLICK" << endl;

		InvalidateRect(g_hWnd, NULL, FALSE);
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		// 시작점 설정
		int xCount = (g_mouseX + (g_mouseX / GRID_LEN)) / GRID_LEN;
		int yCount = (g_mouseY + (g_mouseY / GRID_LEN)) / GRID_LEN;

		// 범위 벗어나면 예외
		if ((xCount < 0) || (xCount >= GRID_NUM * 2) || (yCount < 0) || (yCount >= GRID_NUM))
		{
			wcout << L"out of area" << endl;
			break;
		}

		g_map[g_startPos._y][g_startPos._x] = nColor::NONE;

		g_startPos._x = xCount;
		g_startPos._y = yCount;

		g_map[g_startPos._y][g_startPos._x] = nColor::START;

		wcout << L"L double CLICK" << endl;

		InvalidateRect(g_hWnd, NULL, FALSE);
	}
	break;
	// TODO : 브레즌햄 테스트
	case WM_LBUTTONDOWN:
	{
		wcout << L"L button DOWN" << endl;
		g_isLButtonDown = true;

		// 겹쳐지는 선 길이 만큼 더해서 GRID_LEN 으로 나누기
		int xCount = (g_mouseX + (g_mouseX / GRID_LEN)) / GRID_LEN;
		int yCount = (g_mouseY + (g_mouseY / GRID_LEN)) / GRID_LEN;

		// 범위 벗어나면 예외
		if ((xCount < 0) || (xCount >= GRID_NUM * 2) || (yCount < 0) || (yCount >= GRID_NUM))
		{
			wcout << L"out of area" << endl;
			break;
		}

		wcout << L"xCount : " << xCount << L" // yCount : " << yCount << endl;

		// 시작, 끝점은 벽 그리기에서 제외
		if (g_map[yCount][xCount] == nColor::START ||
			g_map[yCount][xCount] == nColor::END ||
			!CHECKRANGE(xCount, yCount))
		{
			break;
		}

		//if (g_map[yCount][xCount] == nColor::WALL)
		//{
		//	g_map[yCount][xCount] = nColor::NONE;
		//	g_isErase = true;
		//}
		//else
		//{
		//	g_map[yCount][xCount] = nColor::WALL;
		//}

		g_breStartX = xCount;
		g_breStartY = yCount;

		//lineBresenham(4, 4, 14, 10);

		//InvalidateRect(g_hWnd, NULL, FALSE);
	}
	break;
	case WM_LBUTTONUP:
	{
		wcout << L"L button up" << endl;
		g_isLButtonDown = false;
		g_isErase = false;
		g_isBreOn = false;

		g_OldbreStartX = -1;
		g_OldbreStartY = -1;
		g_OldbreEndX = -1;
		g_OldbreEndY = -1;
	}
	break;
	case WM_MOUSEMOVE:
	{
		g_mouseX = GET_X_LPARAM(lParam);
		g_mouseY = GET_Y_LPARAM(lParam);

		if (!g_isLButtonDown)
		{
			break;
		}

		int xCount = (g_mouseX + (g_mouseX / GRID_LEN)) / GRID_LEN;
		int yCount = (g_mouseY + (g_mouseY / GRID_LEN)) / GRID_LEN;

		// 시작, 끝점은 벽 그리기에서 제외
		if (g_map[yCount][xCount] == nColor::START ||
			g_map[yCount][xCount] == nColor::END ||
			!CHECKRANGE(xCount, yCount))
		{
			break;
		}

		//if (g_map[yCount][xCount] == nColor::WALL &&
		//	g_isErase == true)
		//{
		//	g_map[yCount][xCount] = nColor::NONE;
		//}
		//else if (g_isErase == false)
		//{
		//	g_map[yCount][xCount] = nColor::WALL;
		//}
		//else
		//{
		//	break;
		//}
		
		g_breEndX = xCount;
		g_breEndY = yCount;

		if (g_isBreOn == true &&
			(g_breStartX != g_OldbreStartX ||
			g_OldbreStartY != g_breStartY ||
			g_OldbreEndX != g_breEndX ||
			g_OldbreEndY != g_breEndY)) 
		{
			g_isBreOn = false;
			lineBresenham(g_OldbreStartX, g_OldbreStartY, g_OldbreEndX, g_OldbreEndY);

			g_OldbreStartX = g_breStartX;
			g_OldbreStartY = g_breStartY;
			g_OldbreEndX = g_breEndX;
			g_OldbreEndY = g_breEndY;
		}

		g_isBreOn = true;
		lineBresenham(g_breStartX, g_breStartY, g_breEndX, g_breEndY);

		InvalidateRect(g_hWnd, NULL, FALSE);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		wcout << L"R button DOWN" << endl;
		g_isRButtonDown = true;

		DrawClear();

		//시작전에 출발점 노드 생성, 오픈리스트 삽입.
		Node* startNode = new Node();
		startNode->_x = g_startPos._x;
		startNode->_y = g_startPos._y;
		startNode->_g = 0.f;
		startNode->_h = (float)abs(g_startPos._x - g_endPos._x) + (float)abs(g_startPos._y - g_endPos._y);
		startNode->_f = startNode->_g + startNode->_h;

		g_openList.push_front(startNode);

		SetTimer(hWnd, 1, 200, NULL);
	}
	break;
	case WM_RBUTTONUP:
	{
		wcout << L"R button up" << endl;
		g_isRButtonDown = false;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_DELETE:
		{
			DrawAllClear();

			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		DrawRect();

		if (g_isFindEnd)
		{
			DrawLine();
			KillTimer(hWnd, 1);

			g_isFindEnd = false;
		}

		BitBlt(g_hdc, 0, 0, g_rect.right, g_rect.bottom, g_memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}