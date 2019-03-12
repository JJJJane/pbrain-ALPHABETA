#ifndef BROAD_H
#define BROAD_H
#include "skelet/pisqpipe.h"
#include <windows.h>

typedef unsigned long DWORD;

#define EMPTY_MOVE		0
#define TURN_MOVE		1
#define OPPONENT_MOVE	2
#define OUTSIDE_MOVE	3
//定义方向
//上下
#define UP_DOWN				0
//左上右下
#define LEFTUP_RIGHTDOWN	1
//左右
#define LEFT_RIGHT			2
//左下右上
#define RIGHTUP_LEFTDOWN	3

typedef int Tsymbol;
struct Tsquare;
typedef Tsquare *Psquare;



struct Tsquare                   
{
  Tsymbol z;   //0=nothing, 1=my, 2=opponent, 3=outside
  short x,y;   //coordinates 0..width-1, 0..height-1
 
};

#define MAX_BOARD 127

struct Mov
{
	int x;
	int y;
	int val;
	Mov* Nxt;
};


extern int diroff[9],width,height,height2;
extern Psquare board,boardb,boardk;

void boardInit();

void SetChessOnBoard(int x,int y,int type);

Psquare Square(int x,int y);

#define nxtP(p,i) (p=(Psquare)(((char*)p)+(i*s)))
#define prvP(p,i) (p=(Psquare)(((char*)p)-(i*s)))


//禁手判断
//禁手类型
#define NO_FORBIDDEN 0
#define THREE_THREE_FBD 1
#define FOUR_FOUR_FBD 2
#define LONGFBD 3

struct ChessAnalyzeData{
	int adjsameNxt;  //记录与(x, y)Next相邻的连续黑色棋子数
	int adjemptyNxt; //记录adjsame后连续空位数
	int jumpsameNxt; //记录adjempty后连续黑色棋子数
	int jumpemptyNxt; //记录jumpsame后的空位数
	int jumpjumpsameNxt; //记录jumpempty后的连续黑色棋子数

	int adjsamePre;  //记录与(x, y)pre相邻的连续黑色棋子数
	int adjemptyPre; //记录adjsame前连续空位数
	int jumpsamePre; //记录adjempty前连续黑色棋子数
	int jumpemptyPre; //记录jumpsame前的空位数
	int jumpjumpsamePre; //记录jumpempty前的连续黑色棋子数

	Psquare pNxt;
	Psquare pPrv;

	ChessAnalyzeData();//构造
};

void AnalysisLine(Psquare p0,int direction,ChessAnalyzeData* data);

//关键点禁手检测
int KeyPointForbiddenCheck(Psquare p,int direction,int shift);
//禁手检测
int ForbiddenCheck(ChessAnalyzeData *checkData,Psquare p0);


Mov* MergeSort(Mov* source,int count);
void MergePass(Mov* source,Mov* target,const int s,const int n);
void Merge(Mov* source,Mov* target,int l,int m,int r);
#endif