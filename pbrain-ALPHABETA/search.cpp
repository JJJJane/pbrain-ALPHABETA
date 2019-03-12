#include "search.h"

 //初始化(棋盘等)              
void brain_init(){
	pipeOut("DEBUG brain_init");
	if(width<5 || width>MAX_BOARD || height<5 || height>MAX_BOARD){
    pipeOut("ERROR size of the board");
    return;
  }
  
  //初始化棋盘
  boardInit();
  evaluationInit(width,height);
  initHistoryTable(width,height);
  pipeOut("OK");
}

//开始
void brain_restart()
{
	pipeOut("DEBUG brain_restart");
	boardInit();
	evaluationInit(width,height);
	 initHistoryTable(width,height);
  pipeOut("OK");
}


int isFree(int x, int y)
{
	return x>=0 && y>=0 && x<width && y<height && Square(x,y)->z == EMPTY_MOVE;
}
//AI 下子
void brain_my(int x,int y)
{
	pipeOut("DEBUG brain_my [%d,%d]",x,y);
  if(isFree(x,y)){
	  SetChessOnBoard(x,y,TURN_MOVE);
  }else{
    pipeOut("ERROR my move [%d,%d]",x,y);
  }
}

//对手下子
void brain_opponents(int x,int y) 
{
	pipeOut("DEBUG brain_opponents [%d , %d]",x,y);
  if(isFree(x,y)){
	  SetChessOnBoard(x,y,OPPONENT_MOVE);
  }else{
    pipeOut("ERROR opponents's move [%d,%d]",x,y);
  }
}

//棋盘外
void brain_block(int x,int y)
{
  if(isFree(x,y)){
	  SetChessOnBoard(x,y,OUTSIDE_MOVE);
  }else{
    pipeOut("ERROR winning move [%d,%d]",x,y);
  }
}

//undo
int brain_takeback(int x,int y)
{
	pipeOut("DEBUG brain_takeback [%d , %d]" ,x, y);
	if(!isFree(x,y)){
	  SetChessOnBoard(x,y,EMPTY_MOVE);
    return 0;
  }
  return 2;
}



#define MATCH_SPARE 7      //how much is time spared for the rest of game

//算法停止时间
DWORD stopTime()
{
	return start_time + min(info_timeout_turn, info_time_left/MATCH_SPARE)-30;
}


//计算下棋
void brain_turn() 
{ 
	pipeOut("DEBUG brain_turn");

	//使用negamax+迭代加深
	IterDeeping();
	//使用简单搜索，固定深度
	//SimpleSearch();
}

void brain_end()
{
	freeResource();
}


//这里未实现
#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x,int y)
{
  HDC dc;
  HWND wnd;
  RECT rc;
  char c;
  wnd=GetForegroundWindow();
  dc= GetDC(wnd);
  GetClientRect(wnd,&rc);
  c=(char)(board[x][y]+'0');
  TextOut(dc, rc.right-15, 3, &c, 1);
  ReleaseDC(wnd,dc);
}

#endif


//迭代加深搜索+NegaMax/alphaBeta搜索 
void IterDeeping()
{
	//分配搜索时间   
    int nMinTimer =  GetTickCount() + unsigned int((min(info_timeout_turn, info_time_left/MATCH_SPARE)-30)*0.618f);   
	reSetHistoryTable(width,height);

	Mov resultMov;
	//迭代加深搜索   
    for(int depth=1; depth<20;(depth < 3 ? depth *= 2 : depth +=1))   
    {   
		bestMove.val = -10000;
		pipeOut("DEBUG MAX depth:%d,",depth);
           
        int score;
		//负极大值搜索 or // alpha-beta搜索
		//score = NegaMax(depth,0,depth);    
		score = alphabeta(depth,-9999,9999,0,depth);                     
		pipeOut("DEBUG Depth value:%d",score);
		pipeOut("DEBUG BestMove:[%d,%d],%d",bestMove.x,bestMove.y,bestMove.val);
		pipeOut("DEBUG Time use:%d,time left:%d",GetTickCount()- start_time,info_timeout_turn-(GetTickCount()-start_time));
		
		// 若强行终止思考，停止搜索   
		if(terminate)   
            break;   
   
        // 若时间已经达到规定时间的一半，再搜索一层的时间可能不够，停止搜索。   
        if( GetTickCount() > nMinTimer)   
            break;   
   
        // 在规定的深度内，遇到杀棋，停止思考。   
		if( score == 9999)   
              break;   

    }   
	do_mymove(bestMove.x,bestMove.y);
}

//负极大值搜索+历史表启发
int NegaMax(int depth,int player,int MaxDepth)
{
	if (depth <=0)//预判层上的伪叶子结点，计算其启发评价值
	{
		return evaluate(player);
	}

	int bestMoveIndex = -1;
	int bestVal = -10000;
	int moveListLen  =0;
	Mov* moveList=GenerateMoves(moveListLen,player);

	if(moveListLen == 0)
	{
		pipeOut("gen movelist is empty");
		delete[] moveList;
		moveList = NULL;
		return evaluate(player);
	}
	for (int i = 0; i < moveListLen; i++)
	{
		moveList[i].val = getHistoryScore(moveList[i],player);
	}

	moveList = MergeSort(moveList,moveListLen); //历史启发排序，alpha-beta时可用

	for(int i = 0;i<moveListLen;i++)
	{
		if (terminate || GetTickCount() >= stopTime())
		{
			pipeOut("DEBUG It's time to terminate");
			break;
		}

		MakeMove(moveList[i],player);

		if (isGameOver(moveList[i]))//儿子结点为胜负已分状态（真正的叶子结点），表明player方走这一步走法后会获胜
		{
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
			UnmakeMove(moveList[i]);
			delete[] moveList;
			moveList = NULL;
			return 9999;
		}

		moveList[i].val = -NegaMax(depth - 1,1 - player,MaxDepth);

		UnmakeMove(moveList[i]);

		if(bestVal < moveList[i].val)
		{

			bestVal = moveList[i].val;
			bestMoveIndex = i;
			if(depth == MaxDepth)
			{	
				bestMove = moveList[i];
			}
		}

	}

	if(bestMoveIndex != -1)
	{
		enterHistoryScore(moveList[bestMoveIndex],depth,player);
	}

	if(depth == MaxDepth)
	{
		bestMove = moveList[bestMoveIndex];
	}
	delete[] moveList;
	moveList = NULL;

	return bestVal;
}

//固定深度+NegaMax搜索 
void SimpleSearch()
{
	Mov resultMov;
	int depth = 4;
	
	bestMove.val = -10000;
	int score;
	//负极大值搜索
	score = alphabeta(depth, -9999, 9999, 0, depth);
	pipeOut("DEBUG Depth value:%d", score);
	pipeOut("DEBUG BestMove:[%d,%d],%d", bestMove.x, bestMove.y, bestMove.val);
	do_mymove(bestMove.x, bestMove.y);
}

//负极大值搜索
int NegaMax00(int depth, int player, int MaxDepth)
{
	if (depth <= 0)//伪叶子结点
	{
		return evaluate(player);
	}

	int bestMoveIndex = -1;
	int bestVal = -10000;
	int moveListLen = 0;
	Mov* moveList = GenerateMoves(moveListLen, player);

	if (moveListLen == 0)
	{
		pipeOut("gen movelist is empty");
		delete[] moveList;
		moveList = NULL;
		return evaluate(player);
	}

	for (int i = 0; i<moveListLen; i++)
	{
		if (terminate || GetTickCount() >= stopTime())
		{
			pipeOut("DEBUG It's time to terminate");
			break;
		}

		MakeMove(moveList[i], player);

		if (isGameOver(moveList[i]))//儿子结点为胜负已分状态，真正的叶子结点
		{
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
			UnmakeMove(moveList[i]);
			delete[] moveList;
			moveList = NULL;
			return 9999;
		}

		moveList[i].val = -NegaMax(depth - 1, 1 - player, MaxDepth);

		UnmakeMove(moveList[i]);
		// if(movelist[i].val>alpha){
		// 	alpha=movelist[i].val;
		// 	if (depth == MaxDepth)
		// 	{
		// 		bestMove = moveList[i];
		// 	}
		// 	if(alpha>=beta)
		// 		break;
		// }
		if (bestVal < moveList[i].val)
		{

			bestVal = moveList[i].val;
			bestMoveIndex = i;
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
		}

	}

	if (depth == MaxDepth)
	{
		bestMove = moveList[bestMoveIndex];
	}
	delete[] moveList;
	moveList = NULL;

	return bestVal;
}

//alphaBeta搜索,9999,-9999
int alphabeta(int depth,int alpha,int beta,int player,int MaxDepth)
{
	if (depth <=0)//预判层上的伪叶子结点，计算其启发评价值
	{
		return evaluate(player);
	}

	int bestMoveIndex = -1;
	int moveListLen  =0;
	Mov* moveList=GenerateMoves(moveListLen,player);

	if(moveListLen == 0)
	{
		pipeOut("gen movelist is empty");
		delete[] moveList;
		moveList = NULL;
		return evaluate(player);
	}
	for (int i = 0; i < moveListLen; i++)
	{
		moveList[i].val = getHistoryScore(moveList[i],player);
	}

	moveList = MergeSort(moveList,moveListLen); //历史启发排序，alpha-beta时可用

	for(int i = 0;i<moveListLen;i++)
	{
		if (terminate || GetTickCount() >= stopTime())
		{
			pipeOut("DEBUG It's time to terminate");
			break;
		}

		MakeMove(moveList[i],player);

		if (isGameOver(moveList[i]))//儿子结点为胜负已分状态（真正的叶子结点），表明player方走这一步走法后会获胜
		{
			if (depth == MaxDepth)
			{
				bestMove = moveList[i];
			}
			UnmakeMove(moveList[i]);
			delete[] moveList;
			moveList = NULL;
			return 9999;
		}

		moveList[i].val = -alphabeta(depth-1,-beta,-alpha,player,MaxDepth);
		UnmakeMove(moveList[i]);
		if(moveList[i].val>alpha){
			bestMoveIndex = i;
			alpha=moveList[i].val;
			if (depth == MaxDepth)
				bestMove = moveList[i];
		}
		if(alpha>beta)
			break;
	}
		if(bestMoveIndex != -1)
	{
		enterHistoryScore(moveList[bestMoveIndex],depth,player);
	}

	if (depth == MaxDepth)
	{
		bestMove = moveList[bestMoveIndex];
	}
	delete[] moveList;
	moveList = NULL;
	return alpha;
}

//产生当前棋盘的可行棋步，只获取棋盘上已有棋子三字以内的空棋位，并且考虑是否禁手
Mov* GenerateMoves(int& moveLen,int player)
{
	int range = 3;
	int count = 0;
	Psquare p = boardb;

	Mov* result = new Mov[width*height];

	//初始化标志位，避免重复加入最后队列
	int* makeSign = new int[width*height];
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			makeSign[i*width+j] = 0;
		}
	}

	//开始获取可行的棋位
	while(p != boardk)
	{
		if (p->z == TURN_MOVE || p->z == OPPONENT_MOVE)
		{
			for(int i = ( p->x - range < 0 ? 0 : p->x - range);i <= ( p->x + range > width ? width : p->x + range); i++)
			{
				for (int j = ( p->y - range <0 ? 0: p->y - range); j <= (p->y + range > height ? height:p->y + range); j++)
				{
					
					if ( Square(i,j)->z == EMPTY_MOVE && makeSign[i*width+j] == 0)
					{
						makeSign[i*width+j]=1;
						if (isBan(Square(i,j),player))//禁手判断，函数里会首先判断该选手是否需要禁手判断，在继续相关操作
						{
							continue;
						}
						result[count].x = i;
						result[count].y = j;
						result[count].val = 0;
						count++;
 					}
				}
			}	
		}
		p = (Psquare)(((char*)p)+ sizeof(Tsquare));
	}
	
	delete[] makeSign;
	//棋盘为空，则随机获取一个位置
	if (count == 0 )//棋盘为空或者棋盘已满
	{
		if (Square(0, 0)->z == EMPTY_MOVE) {//棋盘为空，从棋盘上随机选择一个位置
			srand(GetTickCount());
			int rndX = rand() % width;
			int rndY = rand() % height;
			for (int i = (rndX - range < 0 ? 0 : rndX - range); i <= (rndX + range > width ? width : rndX + range); i++)
			{
				for (int j = (rndY - range < 0 ? 0 : rndY - range); j <= (rndY + range > height ? height : rndY + range); j++)
				{
					makeSign[i*width + j] = 1;
					result[count].x = i;
					result[count].y = j;
					result[count].val = 0;
					count++;
				}
			}
		}
	}

	moveLen = count;
	return result;
}


//
Mov mov_rd(Mov* moveList,int count)
{
	srand(GetTickCount());
	int num = rand()%count;
	//pipeOut("DEBUG rnd :%d",num);
	return moveList[num];
}



//判断是否是禁手关键点
bool isBan(Psquare p0,int player)
{
	//检查禁手
	if (info_fb_check && player == firstPlayer )
	{
		ChessAnalyzeData checkData[4];
		p0->z = player+1;
		int fb_type = ForbiddenCheck(checkData,p0);
		p0->z =  EMPTY_MOVE;
		
		if(fb_type != NO_FORBIDDEN)
		{
			return true;
		}
	}
	return false;
}



//判断执行move后棋局是否结束
bool isGameOver(Mov move)
{
	Psquare p0 = Square(move.x,move.y);
	
	for(int i=0;i<4;i++)
	{
		int s = diroff[i];
		Psquare pPrv = p0;
		Psquare pNxt = p0;
		int count = -1;
		do
		{
			prvP(pPrv,1);
			count++;
		}while(pPrv->z == p0->z);
		do
		{
			nxtP(pNxt,1);
			count++;
		} while (pNxt->z == p0->z);
		
		if (count == 5)
			return true;
		else if(count > 5 && !info_exact5 )
			return true;
	}
	return false;
}



void freeResource()
{
	delete []PosValue;
	PosValue = NULL;
	
	delete []m_nRecord;
	m_nRecord = NULL;
};