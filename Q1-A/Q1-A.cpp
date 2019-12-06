// ConsoleApplication1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>

int main()
{
	for (int i = 0; i < 81; ++i)
		std::cout << (i / 9 + 1) << " X " << (i % 9 + 1) << " = " << (i / 9 + 1)*(i % 9 + 1) << '\n';
	return 0;
}