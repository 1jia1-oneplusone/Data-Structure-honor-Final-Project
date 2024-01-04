#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <set>
#include <vector>
#include <queue>
#include <functional>
#include <map>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <chrono>
#include <graphics.h>
#include <conio.h>
#include "..\\tinyxml\\tinystr.h"
#include "..\\tinyxml\\tinystr.cpp"
#include "..\\tinyxml\\tinyxml.h"
#include "..\\tinyxml\\tinyxml.cpp"
#include "..\\tinyxml\\tinyxmlerror.cpp"
#include "..\\tinyxml\\tinyxmlparser.cpp"
using namespace std;

#include "global.cpp"
#include "parse_map.cpp"
#include "calculate.cpp"
#include "shortest_path.cpp"
#include "draw.cpp"
#include "UI.cpp"


#define ll long long
#define dbl double

int main()
{
	ui_initialize();
	
	choose_map();
	
	if(isbreak)return 0;
	
	Load_map();
	
//	main_draw1(1);//输出地图
	
	build_graph();
	
	main_ui();
	
	closegraph();//关闭ui窗口
	
	return 0;
}

/*
choice tag:
area
bridge
building
bus
covered
ferry
foot
historic
internet_access:fee
oneway
takeaway
tunnel
wheelchair
*/