#ifndef ALPHABETA_H
#define ALPHABETA_H
#include "skelet\pisqpipe.h"
#include <windows.h>
#include "evaluation.h"
#include "board.h" 
#include "historytable.h"

const char *infotext="name=\"mysearch\", author=\"  xyh  \", version=\"1.0\", country=\"China\", www=\"#####\"";

extern int firstPlayer;
 int** m_HistoryTable[2];

int** PosValue;
int m_nRecordCount[2][7];
int*** m_nRecord;

DWORD stopTime();



Psquare board,boardb,boardk;

Mov bestMove;


Mov* GenerateMoves(int& moveLen,int player);
void MakeMove(Mov m,int player);
void UnmakeMove(Mov);
int distance(Psquare p0,Psquare p1);


//固定深度搜索
void SimpleSearch();
//迭代加深搜索+alphaBeta搜索 
void IterDeeping();
//极小极大搜索
int NegaMax00(int depth,int player,int MaxDepth);
//极小极大搜索+历史表启发
int NegaMax(int depth, int player, int MaxDepth);
//alphaBeta搜索
int alphabeta(int depth,int alpha,int beta,int player,int MaxDepth);
//判断是否是禁手
bool isBan(Psquare p0,int player);
//任意选择一步
Mov mov_rd(Mov* moveList,int count);
//判断执行move后棋局是否结束
bool isGameOver(Mov move);

void freeResource();
#endif