#ifndef EVALUATION_H
#define EVALUATION_H
#include "board.h"



#define TOBEANALYZE 0
#define FIVE	1
#define FOUR	2
#define SFOUR	3
#define THREE	4
#define STHREE	5
#define TWO		6

#define ANALYZED 9





extern int** PosValue;
extern int m_nRecordCount[2][7];
extern int*** m_nRecord;



void evaluationInit(int width,int height);

int evaluate(int player);

//void AnalysisLine(Psquare p0,int direction,ChessAnalyzeData* data);

void AnalysisBoardType(ChessAnalyzeData* data,int direction,int*** m_nRecord,int x,int y);

void SetAnalyzed(int direction,int*** m_nRecord,int x,int y,int leftEdge,int rightEdge);

void SetBoardType(int direction,int*** m_nRecord,int x,int y,int type);

bool isRecordedTwo(int direction,int*** m_nRecord,int x,int y);

void SetLeftBoardType(int direction,int*** m_nRecord,int x,int y,int leftEdge,int type);

void SetRightBoardType(int direction,int*** m_nRecord,int x,int y,int rightEdge,int type);

#endif