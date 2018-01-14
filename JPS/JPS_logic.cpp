#include "stdafx.h"
#include "hoxy_Header.h"
#include "JPS_logic.h"

bool CheckTile(int X, int Y)
{
	if (g_map[Y][X] == nColor::WALL ||
		!CheckRange(X, Y))
	{
		return false;
	}

	return true;
}

bool CheckRange(int X, int Y)
{
	if (X < 0 || X > GRID_NUM * 2 - 1 || Y < 0 || Y > GRID_NUM - 1)
	{
		return false;
	}

	return true;
}

void ReleaseList(void)
{
	std::list<Node*>::iterator nowIter = g_openList.begin();
	std::list<Node*>::iterator endIter = g_openList.end();

	while (nowIter != endIter)
	{
		if ((*nowIter) == &g_startPos ||
			(*nowIter) == &g_endPos)
		{
			nowIter++;
			continue;
		}

		delete (*nowIter);
		nowIter = g_openList.erase(nowIter);
	}

	nowIter = g_closeList.begin();
	endIter = g_closeList.end();

	while (nowIter != endIter)
	{
		if ((*nowIter) == &g_startPos ||
			(*nowIter) == &g_endPos)
		{
			nowIter++;
			continue;
		}

		delete (*nowIter);
		nowIter = g_closeList.erase(nowIter);
	}

	g_endPos._parent = nullptr;
}

// 외부에서는 JPS_Find 호출, 과정을 보여주기 위해 타이머로 반복적으로 호출
void JPS_Find(void)
{
	SetStart();
	SetTimer(g_hWnd, 1, 5, NULL);
}

void SetStart(void)
{
	// 그리기 위한 시작노드, 끝 노드 설정
	g_map[g_startPos._y][g_startPos._x] = nColor::START;
	g_map[g_endPos._y][g_endPos._x] = nColor::END;

	// 시작전에 출발점 노드 생성,  오픈리스트 삽입.
	Node* startNode = new Node;
	startNode->_x = g_startPos._x;
	startNode->_y = g_startPos._y;
	startNode->_g = 0;
	startNode->_h = (float)(abs(g_endPos._x - g_startPos._x) + abs(g_endPos._y - g_startPos._y));
	startNode->_f = startNode->_g + startNode->_h;
	startNode->_parent = nullptr;

	g_openList.push_front(startNode);
}

// 다음 한칸 리턴, 한칸을 가기위해 길을 새로 찾는다
bool PathFind(int Sx, int Sy, int Ex, int Ey) // int* x, int* y
{
	// g값 : 부모의 g값 + 부모로부터 현재까지의 거리(h값과 유사)
	// h값 : 목적지 까지의 x축기준의 길이 + y축기준의 길이(절대값)

	if (g_openList.empty())
	{
		// 오픈이 비었으면 끝
		return false;
	}

	///////////////////////////
	g_randNum = rand() % 3;
	///////////////////////////

	if (g_randNum == 0)
	{
		g_throwColor = nColor::RAN1;
	}
	else if (g_randNum == 1)
	{
		g_throwColor = nColor::RAN2;
	}
	else if (g_randNum == 2)
	{
		g_throwColor = nColor::RAN3;
	}

	// openList에서 pop
	std::list<Node*>::iterator iter = g_openList.begin();
	Node* popNode = *iter;
	g_openList.pop_front();

	// closeList로 다시 넣는다
	g_closeList.push_front(popNode);

	if (g_map[popNode->_y][popNode->_x] != nColor::START &&
		g_map[popNode->_y][popNode->_x] != nColor::END)
	{
		g_map[popNode->_y][popNode->_x] = nColor::CLOSE;
	}

	// 끝인가
	if (popNode->_x == g_endPos._x &&
		popNode->_y == g_endPos._y)
	{
		g_endPos._parent = popNode;

		// 찾기 결과 그리기 위한 연결
		return true;
	}

	// 노드 생성
	ThrowNode(popNode);

	return false;
}

// 방향 체크
eDir CheckDir(Node * pNode)
{
	if ((pNode->_parent->_x - pNode->_x > 0) && (pNode->_parent->_y - pNode->_y == 0))
	{
		wcout << L"DIR : LL" << endl;
		return eDir::LL;
	}
	else if ((pNode->_parent->_x - pNode->_x > 0) && (pNode->_parent->_y - pNode->_y > 0))
	{
		wcout << L"DIR : LU" << endl;
		return eDir::LU;
	}
	else if ((pNode->_parent->_x - pNode->_x == 0) && (pNode->_parent->_y - pNode->_y > 0))
	{
		wcout << L"DIR : UU" << endl;
		return eDir::UU;
	}
	else if ((pNode->_parent->_x - pNode->_x < 0) && (pNode->_parent->_y - pNode->_y > 0))
	{
		wcout << L"DIR : RU" << endl;
		return eDir::RU;
	}
	else if ((pNode->_parent->_x - pNode->_x < 0) && (pNode->_parent->_y - pNode->_y == 0))
	{
		wcout << L"DIR : RR" << endl;
		return eDir::RR;
	}
	else if ((pNode->_parent->_x - pNode->_x < 0) && (pNode->_parent->_y - pNode->_y < 0))
	{
		wcout << L"DIR : RD" << endl;
		return eDir::RD;
	}
	else if ((pNode->_parent->_x - pNode->_x == 0) && (pNode->_parent->_y - pNode->_y < 0))
	{
		wcout << L"DIR : DD" << endl;
		return eDir::DD;
	}
	else // if ((pNode->_parent->_x - pNode->_x > 0) && (pNode->_parent->_y - pNode->_y < 0))
	{
		wcout << L"DIR : LD" << endl;
		return eDir::LD;
	}
}

bool ThrowAndMake(int x, int y, Node* pNode, eDir dir)
{
	// 맵 끝이거나 벽을 만나면 빠져나감
	if (!CheckTile(x, y))
	{
		return false;
	}

	if (x == g_endPos._x && y == g_endPos._y)
	{
		AddOpenList(x, y, pNode);
		return true;
	}

	if (g_randNum == 0)
	{
		g_map[y][x] = nColor::RAN1;
	}
	else if (g_randNum == 1)
	{
		g_map[y][x] = nColor::RAN2;
	}
	else if (g_randNum == 2)
	{
		g_map[y][x] = nColor::RAN3;
	}

	switch (dir)
	{
	case eDir::LL:
	{
		if (
			((!CheckTile(x, y - 1)) && CheckTile(x - 1, y - 1)) ||
			((!CheckTile(x, y + 1)) && CheckTile(x - 1, y + 1))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			return ThrowAndMake(x - 1, y, pNode, eDir::LL);
		}
	}
	break;
	case eDir::LU:
	{
		if (
			(!CheckTile(x + 1, y) && CheckTile(x + 1, y - 1)) ||
			(!CheckTile(x, y + 1) && CheckTile(x - 1, y + 1)) ||
			(ClimbingRecursive(x, y - 1, pNode, eDir::UU)) ||
			(ClimbingRecursive(x - 1, y, pNode, eDir::LL))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			ThrowAndMake(x - 1, y - 1, pNode, eDir::LU);
		}
	}
	break;
	case eDir::UU:
	{
		if (
			((!CheckTile(x - 1, y)) && CheckTile(x - 1, y - 1)) ||
			((!CheckTile(x + 1, y)) && CheckTile(x + 1, y - 1))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			ThrowAndMake(x, y - 1, pNode, eDir::UU);
		}
	}
	break;
	case eDir::RU:
	{
		if (
			(!CheckTile(x - 1, y) && CheckTile(x - 1, y - 1)) ||
			(!CheckTile(x, y + 1) && CheckTile(x + 1, y + 1)) ||
			(ClimbingRecursive(x, y - 1, pNode, eDir::UU)) ||
			(ClimbingRecursive(x + 1, y, pNode, eDir::RR))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			return ThrowAndMake(x + 1, y - 1, pNode, eDir::RU);
		}
	}
	break;
	case eDir::RR:
	{
		if (
			((!CheckTile(x, y - 1)) && CheckTile(x + 1, y - 1)) ||
			((!CheckTile(x, y + 1)) && CheckTile(x + 1, y + 1))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			return ThrowAndMake(x + 1, y, pNode, eDir::RR);
		}
	}
	break;
	case eDir::RD:
	{
		if (
			(!CheckTile(x - 1, y) && CheckTile(x - 1, y + 1)) ||
			(!CheckTile(x, y - 1) && CheckTile(x + 1, y - 1)) ||
			(ClimbingRecursive(x + 1, y, pNode, eDir::RR)) ||
			(ClimbingRecursive(x, y + 1, pNode, eDir::DD))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			return ThrowAndMake(x + 1, y + 1, pNode, eDir::RD);
		}
	}
	break;
	case eDir::DD:
	{
		if (
			((!CheckTile(x - 1, y)) && CheckTile(x - 1, y + 1)) ||
			((!CheckTile(x + 1, y)) && CheckTile(x + 1, y + 1))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			ThrowAndMake(x, y + 1, pNode, eDir::DD);
		}
	}
	break;
	case eDir::LD:
	{
		if (
			(!CheckTile(x + 1, y) && CheckTile(x + 1, y + 1)) ||
			(!CheckTile(x, y - 1) && CheckTile(x - 1, y - 1)) ||
			(ClimbingRecursive(x - 1, y, pNode, eDir::LL)) ||
			(ClimbingRecursive(x, y + 1, pNode, eDir::DD))
			)
		{
			AddOpenList(x, y, pNode);
			return true;
		}
		else
		{
			return ThrowAndMake(x - 1, y + 1, pNode, eDir::LD);
		}
	}
	break;
	}

	return false;
}

// 대각선 체크할때 타고 올라가는 함수(true 일 경우 노드생성)
bool ClimbingRecursive(int x, int y, Node* pNode, eDir dir)
{
	// 맵 끝이거나 벽을 만나면 빠져나감
	if (!CheckTile(x, y))
	{
		return false;
	}

	if (x == g_endPos._x && y == g_endPos._y)
	{
		return true;
	}

	if (g_randNum == 0)
	{
		g_map[y][x] = nColor::RAN1;
	}
	else if (g_randNum == 1)
	{
		g_map[y][x] = nColor::RAN2;
	}
	else if (g_randNum == 2)
	{
		g_map[y][x] = nColor::RAN3;
	}

	switch (dir)
	{
	case eDir::UU:
	{
		wcout << L"recursive climbing : UU  // x : " << x << L", y : " << y << endl;
		if (
			((!CheckTile(x - 1, y)) && CheckTile(x - 1, y - 1)) ||
			((!CheckTile(x + 1, y)) && CheckTile(x + 1, y - 1))
			)
		{
			return true;
		}
		else
		{
			return ClimbingRecursive(x, y - 1, pNode, eDir::UU);
		}
	}
	break;
	case eDir::RR:
	{
		wcout << L"recursive climbing : RR  // x : " << x << L", y : " << y << endl;
		if (
			((!CheckTile(x, y - 1)) && CheckTile(x + 1, y - 1)) ||
			((!CheckTile(x, y + 1)) && CheckTile(x + 1, y + 1))
			)
		{
			return true;
		}
		else
		{
			return ClimbingRecursive(x + 1, y, pNode, eDir::RR);
		}
	}
	break;
	case eDir::DD:
	{
		wcout << L"recursive climbing : DD  // x : " << x << L", y : " << y << endl;
		if (
			((!CheckTile(x - 1, y)) && CheckTile(x - 1, y + 1)) ||
			((!CheckTile(x + 1, y)) && CheckTile(x + 1, y + 1))
			)
		{
			return true;
		}
		else
		{
			return ClimbingRecursive(x, y + 1, pNode, eDir::DD);
		}
	}
	break;
	case eDir::LL:
	{
		wcout << L"recursive climbing : LL  // x : " << x << L", y : " << y << endl;
		if (
			((!CheckTile(x, y - 1)) && CheckTile(x - 1, y - 1)) ||
			((!CheckTile(x, y + 1)) && CheckTile(x - 1, y + 1))
			)
		{
			return true;
		}
		else
		{
			return ClimbingRecursive(x - 1, y, pNode, eDir::LL);
		}
	}
	break;
	}

	return false;
}

void AddOpenList(int x, int y, Node* pNode)
{
	if (CheckTile(x, y))
	{
		Node localNode;
		localNode._parent = pNode;
		localNode._x = x;
		localNode._y = y;
		//////////////////////////////////////////////////////////////////////////////
		localNode._g = pNode->_g + (float)abs(pNode->_x - x) + (float)abs(pNode->_y - y);
		//////////////////////////////////////////////////////////////////////////////
		localNode._h = (float)abs(g_endPos._x - x) + (float)abs(g_endPos._y - y);
		localNode._f = localNode._g + localNode._h;

		// 일치하는 노드를 찾고
		auto iter = std::find_if(g_openList.begin(), g_openList.end(),
			[=](Node* param) {
			if (param->_x == localNode._x &&
				param->_y == localNode._y)
			{
				return true;
			}
			return false;
		});

		if (iter != g_openList.end())
		{
			// 같은것 찾음

			while (1)
			{
				// TODO : g값 큰것 찾음
				auto iter = std::find_if(g_openList.begin(), g_openList.end(),
					[=](Node* param) {
					if (param->_x == localNode._x &&
						param->_y == localNode._y &&
						param->_g > localNode._g)
					{
						return true;
					}
					return false;
				});

				if (iter != g_openList.end())
				{
					// TODO : 찾음
					// 해당 노드의 부모를 localNode(x,y 위치는 같지만 g가 더 작은)의 부모로 변경
					(*iter)->_parent = localNode._parent;

					// 부모 바꿨으면 재계산
					(*iter)->_g = localNode._g;
					(*iter)->_f = (*iter)->_g + (*iter)->_h;

					// 재정렬.. f값이 작은게 앞으로
					g_openList.sort([](Node* a, Node* b)
					{	// true 가 앞으로, false가 뒤로
						return a->_f < b->_f;
					});
				}
				else
				{
					break;
				}
			}

			return;
		}

		// 일치하는 노드를 찾고
		iter = std::find_if(g_closeList.begin(), g_closeList.end(),
			[=](Node* param) {
			if (param->_x == localNode._x &&
				param->_y == localNode._y)
			{
				return true;
			}
			return false;
		});

		if (iter != g_closeList.end())
		{
			// 같은거 찾음

			while (1)
			{
				// TODO : g값 큰것 찾음
				auto iter = std::find_if(g_closeList.begin(), g_closeList.end(),
					[=](Node* param) {
					if (param->_x == localNode._x &&
						param->_y == localNode._y &&
						param->_g > localNode._g)
					{
						return true;
					}
					return false;
				});

				if (iter != g_closeList.end())
				{
					// TODO : 찾음
					// 해당 노드의 부모를 localNode(x,y 위치는 같지만 g가 더 작은)의 부모로 변경
					(*iter)->_parent = localNode._parent;

					// 부모 바꿨으면 재계산
					(*iter)->_g = localNode._g;
					(*iter)->_f = (*iter)->_g + (*iter)->_h;

					// 재정렬.. f값이 작은게 앞으로
					g_closeList.sort([](Node* a, Node* b)
					{	// true 가 앞으로, false가 뒤로
						return a->_f < b->_f;
					});
				}
				else
				{
					break;
				}
			}

			return;
		}

		// 노드 실제로 만들어서 오픈리스트에 넣는다
		Node* newNode = new Node();
		newNode->_parent = localNode._parent;
		newNode->_x = localNode._x;
		newNode->_y = localNode._y;
		newNode->_g = localNode._g;
		newNode->_h = localNode._h;
		newNode->_f = localNode._f;

		g_openList.push_front(newNode);

		// 재정렬.. f값이 작은게 앞으로
		g_openList.sort([](Node* a, Node* b)
		{	// true 가 앞으로, false가 뒤로
			return a->_f < b->_f;
		});

		// 색지정
		if (g_map[newNode->_y][newNode->_x] != nColor::START &&
			g_map[newNode->_y][newNode->_x] != nColor::END)
		{
			g_map[newNode->_y][newNode->_x] = nColor::OPEN;
		}
	}
}

bool ThrowNode(Node * pNode)
{
	if (pNode->_parent == nullptr)
	{
		// 수직/수평
		ThrowAndMake(pNode->_x - 1, pNode->_y, pNode, eDir::LL);
		ThrowAndMake(pNode->_x, pNode->_y - 1, pNode, eDir::UU);
		ThrowAndMake(pNode->_x + 1, pNode->_y, pNode, eDir::RR);
		ThrowAndMake(pNode->_x, pNode->_y + 1, pNode, eDir::DD);

		// 대각
		ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);
		ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);
		ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);
		ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);

		return true;
	}

	// 방향 체크
	eDir Direction = CheckDir(pNode);
	switch (Direction)
	{
	case eDir::LL:
	{
		wcout << L"After DirectionCheck LL Throw" << endl;
		ThrowAndMake(pNode->_x - 1, pNode->_y, pNode, eDir::LL);

		if ((!CheckTile(pNode->_x, pNode->_y - 1)) && CheckTile(pNode->_x - 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);
		}

		if ((!CheckTile(pNode->_x, pNode->_y + 1)) && CheckTile(pNode->_x - 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);
		}
	}
	break;
	case eDir::LU:
	{
		wcout << L"After DirectionCheck LU Throw" << endl;
		ThrowAndMake(pNode->_x - 1, pNode->_y, pNode, eDir::LL);
		ThrowAndMake(pNode->_x, pNode->_y - 1, pNode, eDir::UU);
		ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);

		//
		if ((!CheckTile(pNode->_x + 1, pNode->_y)) && CheckTile(pNode->_x + 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);
		}

		if ((!CheckTile(pNode->_x, pNode->_y + 1)) && CheckTile(pNode->_x - 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);
		}
	}
	break;
	case eDir::UU:
	{
		wcout << L"After DirectionCheck UU Throw" << endl;
		ThrowAndMake(pNode->_x, pNode->_y - 1, pNode, eDir::UU);

		if ((!CheckTile(pNode->_x - 1, pNode->_y)) && CheckTile(pNode->_x - 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);
		}

		if ((!CheckTile(pNode->_x + 1, pNode->_y)) && CheckTile(pNode->_x + 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);
		}
	}
	break;
	case eDir::RU:
	{
		wcout << L"After DirectionCheck RU Throw" << endl;
		ThrowAndMake(pNode->_x + 1, pNode->_y, pNode, eDir::RR);
		ThrowAndMake(pNode->_x, pNode->_y - 1, pNode, eDir::UU);
		ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);

		//
		if ((!CheckTile(pNode->_x - 1, pNode->_y)) && CheckTile(pNode->_x - 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);
		}

		if ((!CheckTile(pNode->_x, pNode->_y + 1)) && CheckTile(pNode->_x + 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);
		}
	}
	break;
	case eDir::RR:
	{
		wcout << L"After DirectionCheck RR Throw" << endl;
		ThrowAndMake(pNode->_x + 1, pNode->_y, pNode, eDir::RR);

		if ((!CheckTile(pNode->_x, pNode->_y - 1)) && CheckTile(pNode->_x + 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);
		}

		if ((!CheckTile(pNode->_x, pNode->_y + 1)) && CheckTile(pNode->_x + 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);
		}
	}
	break;
	case eDir::RD:
	{
		wcout << L"After DirectionCheck RD Throw" << endl;
		ThrowAndMake(pNode->_x + 1, pNode->_y, pNode, eDir::RR);
		ThrowAndMake(pNode->_x, pNode->_y + 1, pNode, eDir::DD);
		ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);

		//
		if ((!CheckTile(pNode->_x - 1, pNode->_y)) && CheckTile(pNode->_x - 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);
		}

		if ((!CheckTile(pNode->_x, pNode->_y - 1)) && CheckTile(pNode->_x + 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);
		}
	}
	break;
	case eDir::DD:
	{
		wcout << L"After DirectionCheck DD Throw" << endl;
		ThrowAndMake(pNode->_x, pNode->_y + 1, pNode, eDir::DD);

		if ((!CheckTile(pNode->_x - 1, pNode->_y)) && CheckTile(pNode->_x - 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);
		}

		if ((!CheckTile(pNode->_x + 1, pNode->_y)) && CheckTile(pNode->_x + 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);
		}
	}
	break;
	case eDir::LD:
	{
		wcout << L"After DirectionCheck LD Throw" << endl;
		ThrowAndMake(pNode->_x, pNode->_y + 1, pNode, eDir::DD);
		ThrowAndMake(pNode->_x - 1, pNode->_y, pNode, eDir::LL);
		ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);

		//
		if ((!CheckTile(pNode->_x, pNode->_y - 1)) && CheckTile(pNode->_x - 1, pNode->_y - 1))
		{
			ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);
		}

		if ((!CheckTile(pNode->_x + 1, pNode->_y)) && CheckTile(pNode->_x + 1, pNode->_y + 1))
		{
			ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);
		}
	}
	break;
	}

	return false;
}