// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
using namespace std;

//class A
//{
//	string a;
//	string b;
//};

int main()
{
	//string a = "111111111111111111111111111111";
	
	int a = 1;
	int* b = &a;
	printf("%d\n", b);
	printf("%d\n", b + 1);
	printf("%d\n", b + sizeof(int));
	system("pause");
    return 0;
}

