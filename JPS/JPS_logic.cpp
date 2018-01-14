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

// �ܺο����� JPS_Find ȣ��, ������ �����ֱ� ���� Ÿ�̸ӷ� �ݺ������� ȣ��
void JPS_Find(void)
{
	SetStart();
	SetTimer(g_hWnd, 1, 5, NULL);
}

void SetStart(void)
{
	// �׸��� ���� ���۳��, �� ��� ����
	g_map[g_startPos._y][g_startPos._x] = nColor::START;
	g_map[g_endPos._y][g_endPos._x] = nColor::END;

	// �������� ����� ��� ����,  ���¸���Ʈ ����.
	Node* startNode = new Node;
	startNode->_x = g_startPos._x;
	startNode->_y = g_startPos._y;
	startNode->_g = 0;
	startNode->_h = (float)(abs(g_endPos._x - g_startPos._x) + abs(g_endPos._y - g_startPos._y));
	startNode->_f = startNode->_g + startNode->_h;
	startNode->_parent = nullptr;

	g_openList.push_front(startNode);
}

// ���� ��ĭ ����, ��ĭ�� �������� ���� ���� ã�´�
bool PathFind(int Sx, int Sy, int Ex, int Ey) // int* x, int* y
{
	// g�� : �θ��� g�� + �θ�κ��� ��������� �Ÿ�(h���� ����)
	// h�� : ������ ������ x������� ���� + y������� ����(���밪)

	if (g_openList.empty())
	{
		// ������ ������� ��
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

	// openList���� pop
	std::list<Node*>::iterator iter = g_openList.begin();
	Node* popNode = *iter;
	g_openList.pop_front();

	// closeList�� �ٽ� �ִ´�
	g_closeList.push_front(popNode);

	if (g_map[popNode->_y][popNode->_x] != nColor::START &&
		g_map[popNode->_y][popNode->_x] != nColor::END)
	{
		g_map[popNode->_y][popNode->_x] = nColor::CLOSE;
	}

	// ���ΰ�
	if (popNode->_x == g_endPos._x &&
		popNode->_y == g_endPos._y)
	{
		g_endPos._parent = popNode;

		// ã�� ��� �׸��� ���� ����
		return true;
	}

	// ��� ����
	ThrowNode(popNode);

	return false;
}

// ���� üũ
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
	// �� ���̰ų� ���� ������ ��������
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

// �밢�� üũ�Ҷ� Ÿ�� �ö󰡴� �Լ�(true �� ��� ������)
bool ClimbingRecursive(int x, int y, Node* pNode, eDir dir)
{
	// �� ���̰ų� ���� ������ ��������
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

		// ��ġ�ϴ� ��带 ã��
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
			// ������ ã��

			while (1)
			{
				// TODO : g�� ū�� ã��
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
					// TODO : ã��
					// �ش� ����� �θ� localNode(x,y ��ġ�� ������ g�� �� ����)�� �θ�� ����
					(*iter)->_parent = localNode._parent;

					// �θ� �ٲ����� ����
					(*iter)->_g = localNode._g;
					(*iter)->_f = (*iter)->_g + (*iter)->_h;

					// ������.. f���� ������ ������
					g_openList.sort([](Node* a, Node* b)
					{	// true �� ������, false�� �ڷ�
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

		// ��ġ�ϴ� ��带 ã��
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
			// ������ ã��

			while (1)
			{
				// TODO : g�� ū�� ã��
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
					// TODO : ã��
					// �ش� ����� �θ� localNode(x,y ��ġ�� ������ g�� �� ����)�� �θ�� ����
					(*iter)->_parent = localNode._parent;

					// �θ� �ٲ����� ����
					(*iter)->_g = localNode._g;
					(*iter)->_f = (*iter)->_g + (*iter)->_h;

					// ������.. f���� ������ ������
					g_closeList.sort([](Node* a, Node* b)
					{	// true �� ������, false�� �ڷ�
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

		// ��� ������ ���� ���¸���Ʈ�� �ִ´�
		Node* newNode = new Node();
		newNode->_parent = localNode._parent;
		newNode->_x = localNode._x;
		newNode->_y = localNode._y;
		newNode->_g = localNode._g;
		newNode->_h = localNode._h;
		newNode->_f = localNode._f;

		g_openList.push_front(newNode);

		// ������.. f���� ������ ������
		g_openList.sort([](Node* a, Node* b)
		{	// true �� ������, false�� �ڷ�
			return a->_f < b->_f;
		});

		// ������
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
		// ����/����
		ThrowAndMake(pNode->_x - 1, pNode->_y, pNode, eDir::LL);
		ThrowAndMake(pNode->_x, pNode->_y - 1, pNode, eDir::UU);
		ThrowAndMake(pNode->_x + 1, pNode->_y, pNode, eDir::RR);
		ThrowAndMake(pNode->_x, pNode->_y + 1, pNode, eDir::DD);

		// �밢
		ThrowAndMake(pNode->_x - 1, pNode->_y - 1, pNode, eDir::LU);
		ThrowAndMake(pNode->_x + 1, pNode->_y - 1, pNode, eDir::RU);
		ThrowAndMake(pNode->_x + 1, pNode->_y + 1, pNode, eDir::RD);
		ThrowAndMake(pNode->_x - 1, pNode->_y + 1, pNode, eDir::LD);

		return true;
	}

	// ���� üũ
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