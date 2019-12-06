// Q1-B.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>

#define INF INT_MAX	//무한대

//각 인덱스를 노드로 표현
struct Node
{
	int x, y;								//노드의 x,y
	int index;							//노드의 인덱스(ex: 0,1,8,9)	
	Node* parent = nullptr;	//부모 노드
	int distance = 1;					//시작 노드로부터의 거리
	int weight = 0;					//노드의 가중치

	//SetParent : 부모를 설정하는 함수
	void SetParent(Node* parentNode, int EndX, int EndY)		
	{
		parent = parentNode;							//부모 노드 설정
		distance += parentNode->distance;	//시작점으로부터 거리 누적
		weight = distance + abs(x - EndX) + abs(y - EndY); //가중치 = distance와 도착점까지의 거리 + 맨하탄 방법
	}

	//==연산자 오버로드
	bool operator ==(Node const& RNode)
	{
		return x == RNode.x && y == RNode.y;
	}

	//!=연산자 오버로드
	bool operator !=(Node const& RNode)
	{
		return x != RNode.x && y != RNode.y;
	}
};

class Escape
{
public:
	Escape(int** InArr, int size);
	~Escape();

public:
	//결과 출력 함수
	void  PrintResult();

private:
	//시작점과 목표점을 반환하는 함수
	void FindPoint(int& StartX, int& StartY, int& EndX, int& EndY);

	//경로를 탐색할 재귀함수
	bool Search(int EndX, int EndY);

	//닫힌 노드에 있는지 확인
	bool IsClose(Node& node);

	//열린 노드에 있는지 확인
	size_t IsOpen(Node& node);

private:
	std::vector<Node> OpenNode;		//열린 노드
	std::vector<Node> CloseNode;		//닫힌 노드

	std::vector<Node> ResultNode;	//최종 경로

	Node** Arr;	//노드 배열
	int size;			//배열 크기
};

//생성자
Escape::Escape(int ** InArr, int size)
{
	//정수형 배열을 노드 배열로 재생성
	Arr = new Node*[size];
	for (int i = 0; i < size; ++i)
	{
		Arr[i] = new Node[size];
		for (int j = 0; j < size; ++j)
		{
			Arr[i][j].index = InArr[i][j];
			Arr[i][j].x = j;
			Arr[i][j].y = i;
		}
	}

	//배열 크기 저장
	this->size = size;
}

//소멸자
Escape::~Escape()
{
	//생성자에서 생성한 노드 배열 소멸
	for (int i = 0; i < size; ++i)
		delete[] Arr[i];
	delete[] Arr;
}

void Escape::PrintResult()
{
	int StartX, StartY, EndX, EndY;	//시작 노드, 목표 노드 

	//시작 노드, 목표 노드 찾기
	FindPoint(StartX, StartY, EndX, EndY);

	//시작 노드를 오픈 노드에 추가
	OpenNode.push_back(Arr[StartY][StartX]);

	//경로 탐색
	if (Search(EndX, EndY))
	{
		//경로의 인덱스 표시
		for (Node& node : ResultNode)
		{
			if (0 == node.index)
			{
				Arr[node.y][node.x].index = 5;
			}
		}

		//전체 노드 출력
		for (int y = 0; y < size; ++y)
		{
			for (int x = 0; x < size; ++x)
			{
				std::cout << Arr[y][x].index;
			}
			std::cout << std::endl;
		}
	}
	else
		std::cout << "No Path" << std::endl;	//경로 탐색 실패
}

void Escape::FindPoint(int & StartX, int & StartY, int & EndX, int & EndY)
{
	bool StartCheck = false, EndCheck = false;

	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			if (!StartCheck && 8 == Arr[y][x].index)
			{
				StartX = x;
				StartY = y;
				StartCheck = true;
			}
			else if (!EndCheck && 9 == Arr[y][x].index)
			{
				EndX = x;
				EndY = y;
				EndCheck = true;
			}
			else if (StartCheck && EndCheck)
				break;	//시작점과 목표점 탐색 완료
		}
	}
}

bool Escape::Search(int EndX, int EndY)
{
	//오픈 노드에서 하나 고르기
	{
		int OpenNodeLen = OpenNode.size();
		int MinWeigh = INF;
		int MinIndex = 0;
		for (int i = 0; i < OpenNodeLen; ++i)
		{
			if (OpenNode[i].weight < MinWeigh)
			{
				MinIndex = i;
				MinWeigh = OpenNode[i].weight;
			}
		}

		CloseNode.push_back(OpenNode[MinIndex]);			//고른 노드는 닫힌 노드에 추가
		OpenNode.erase(OpenNode.begin() + MinIndex);	//고른 노드는 열린 노드에서 삭제

	}

	Node SelectNode = CloseNode.back();		//고른 노드 가져오기

	//주변 노드 처리
	{
		Node node;
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				int NextX = SelectNode.x + (i ? -1 : 1)*(j ? 0 : 1);	//좌,우
				int NextY = SelectNode.y + (i ? -1 : 1)*(j ? 1 : 0);	//위,아래

				//벗어난 인덱스 제외
				if (NextX >= size || NextX < 0 ||
					NextY >= size || NextY < 0)
					continue;

				node = Arr[NextY][NextX];
				if (1 != node.index && !IsClose(node))	//벽이거나 닫힌 노드 제외
				{
					node.SetParent(&SelectNode, EndX, EndY);	//고른 노드를 부모로 설정

					if (9 == node.index)	//주변 노드가 목적지인 경우
					{
						ResultNode.push_back(node);

						//부모를 따라서 경로로 추가
						Node* ParentNode = node.parent;
						while (ParentNode)
						{
							ResultNode.push_back(*ParentNode);
							ParentNode = ParentNode->parent;
						}
						return true;	//목적지 찾음
					}
					else if (int num = IsOpen(node))	//주변 노드가 열린 노드에 있는 경우
					{
						if (OpenNode[num - 1].weight > node.weight)	//주변 노드와 기존 열린 노드 중 가중치가 작은 노드로 기존 열린 노드 갱신
							OpenNode[num - 1] = node;
					}
					else	//주변 노드가 열린 노드에 없는 경우
						OpenNode.push_back(node);
				}
			}
		}

		if (OpenNode.empty())
			return false;//경로 찾기 실패(경로가 없음)
	}

	//재귀
	return Search(EndX, EndY);
}

bool Escape::IsClose(Node & findnode)
{
	for (Node& node : CloseNode)
	{
		if (findnode == node)
		{
			return true;
		}
	}
	return false;
}

size_t Escape::IsOpen(Node & findnode)
{
	//찾는 노드가 몇번째 노드인지 반환(1~)
	//찾는 노드가 열린 노드에 없으면 0을 반환

	size_t OpenNodeLen = OpenNode.size();
	if (OpenNodeLen > 0)
	{
		for (size_t i = 0; i < OpenNodeLen; ++i)
		{
			if (findnode == OpenNode[i])
			{
				return i + 1;
			}
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////

int main()
{
	//배열 크기 입력
	int input = 0;
	std::cin >> input;
	std::cin.ignore();

	//배열 생성
	int** arr = new int*[input];
	if (arr)
	{
		for (int i = 0; i < input; ++i)
			arr[i] = new int[input];
	}

	//입력 받을 문자열
	std::string inputStr;

	//문자열 입력
	for (int i = 0; i < input; ++i)
	{
		std::getline(std::cin, inputStr);

		//문자열에서 배열로 치환
		if (inputStr.size())
		{
			for (int j = 0; j < input; ++j)
			{
				arr[i][j] = inputStr[j] - '0';
			}
		}

		inputStr.clear();
	}

	//경로 탐색
	Escape EscapeInst(arr, input);
	EscapeInst.PrintResult();

	//생성한 배열 소멸
	for (int i = 0; i < input; ++i)
		delete arr[i];
	delete arr;

	return 0;
}