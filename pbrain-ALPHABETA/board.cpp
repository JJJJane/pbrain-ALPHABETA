#include "board.h"



//初始化棋盘
void boardInit()
{
	int x,y;
	  Psquare p;

	  //alocate the board
	  delete[] board;
	 height2=height+2;
	 // board= new Tsquare[(width+10)*(height2)];
	  board= new Tsquare[(width+2)*(height2)];
	  boardb= board + height2;// 5*height2;
	  boardk= boardb+ width*height2;
	  // 5 4 3
	  // 6 8 2
	  // 7 0 1
	  diroff[0]=sizeof(Tsquare);
	  diroff[4]=-diroff[0];
	  diroff[1]=sizeof(Tsquare)*(1+height2);
	  diroff[5]=-diroff[1];
	  diroff[2]=sizeof(Tsquare)* height2;
	  diroff[6]=-diroff[2];
	  diroff[3]=sizeof(Tsquare)*(-1+height2);
	  diroff[7]=-diroff[3];
	  diroff[8]=0;
  
	  //initialize the board
	 
	  p=board;
	 /*
	  for(x=-5; x<=width+4; x++){
		for(y=-1; y<=height; y++){
			p->z= (x<0 || y<0 || x>=width || y>=height) ? OUTSIDE_MOVE : EMPTY_MOVE;
		  p->x= (short)x;
		  p->y= (short)y;
		  p++;
		}
	  }
	  */
	   for(x=-1; x<=width; x++){
		for(y=-1; y<=height; y++){
		p->z= (x<0 || y<0 || x>=width || y>=height) ? OUTSIDE_MOVE : EMPTY_MOVE;
			//p->z = EMPTY_MOVE;  
		p->x= (short)x;
		  p->y= (short)y;
		  p++;
		}
	  }
}

//设置（x,y）位置上的棋子type
void SetChessOnBoard(int x,int y,int type)
{
	Square(x,y)->z = type;
}


Psquare Square(int x,int y)
{
	return  boardb + x*height2+(y+1);
}


ChessAnalyzeData::ChessAnalyzeData()//构造
{
	adjsameNxt = 0;
	adjemptyNxt = 0;
	jumpsameNxt = 0;
	jumpemptyNxt = 0;
	jumpjumpsameNxt = 0;
	adjsamePre = 0;
	adjemptyPre = 0;
	jumpsamePre = 0;
	jumpemptyPre = 0;
	jumpjumpsamePre = 0;
}

//在p0点，direction代表方向上分析得到相关分析数据
void AnalysisLine(Psquare p0,int direction,ChessAnalyzeData* data)
{
	int s = direction;
	data->pNxt = data->pPrv = p0;
	//prv

	do
	{
		prvP(data->pPrv,1);
		data->adjsamePre++;
	} while (data->pPrv->z == p0->z);
	//记录adjsame前连续空位数
	while (data->pPrv->z == EMPTY_MOVE)
	{
		prvP(data->pPrv,1);
		data->adjemptyPre++;
	}
	if(data->adjemptyPre != 0)
	{
		//记录adjempty前连续同色棋子数
		while (data->pPrv->z == p0->z)
		{
			prvP(data->pPrv,1);
			data->jumpsamePre ++;
		}
		if (data->jumpsamePre != 0)
		{
			//记录jumpsame前的空位
			while (data->pPrv->z == EMPTY_MOVE)
			{
				prvP(data->pPrv,1);
				data->jumpemptyPre ++;
			}
			if (data->jumpemptyPre != 0)
			{
				//记录jumpempty前的连续同色棋子数
				while (data->pPrv->z == p0->z)
				{
					prvP(data->pPrv,1);
					data->jumpjumpsamePre++;
				}
			}
		}
	}
	//nxt
	do
	{
		nxtP(data->pNxt,1);
		data->adjsameNxt++;
	} while (data->pNxt->z == p0->z);
	//记录adjsame后连续空位数
	while (data->pNxt->z == EMPTY_MOVE)
	{
		nxtP(data->pNxt,1);
		data->adjemptyNxt++;
	}
	if(data->adjemptyNxt != 0)
	{
		//记录adjempty后连续同色棋子数
		while (data->pNxt->z == p0->z)
		{
			nxtP(data->pNxt,1);
			data->jumpsameNxt ++;
		}
		if (data->jumpsameNxt != 0)
		{
			//记录jumpsame后的空位
			while (data->pNxt->z == EMPTY_MOVE)
			{
				nxtP(data->pNxt,1);
				data->jumpemptyNxt ++;
			}
			if (data->jumpemptyNxt != 0)
			{
				//记录jumpempty后的连续同色棋子数
				while (data->pNxt->z == p0->z)
				{
					nxtP(data->pNxt,1);
					data->jumpjumpsameNxt++;
				}
			}
		}
	}
}

//禁手

//关键点禁手检测
int KeyPointForbiddenCheck(Psquare p0,int direction,int shift){
	int result = 0;
	ChessAnalyzeData checkData[4]; 
	
	int s = direction;
	Psquare p = p0;
	
	nxtP(p,shift);
	p->z = 1;
	
	result = ForbiddenCheck(checkData,p);
	p->z = 0;
	
	return result;
}
//禁手检测
int ForbiddenCheck(ChessAnalyzeData *checkData,Psquare p0)
{
	//禁手类型统计
	int CountThree = 0;
	int CountFour = 0;
	int adjsame[4];
	for(int i=0;i<4;i++){
		AnalysisLine(p0,diroff[i],&checkData[i]);
		adjsame[i] = checkData[i].adjsamePre + checkData[i].adjsameNxt - 1;
		if (adjsame[i]==5)
		{
			return NO_FORBIDDEN;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		
		int direction = diroff[i];
		//开始禁手分析

		if(adjsame[i] > 5 )//长链禁手
			return LONGFBD;

		if (adjsame[i] == 4) //四子相连
		{
			//活四冲四判断
			bool isFour = false;
			if(checkData[i].adjemptyNxt > 0)
			{
				//递归判断关键点是否可下
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					isFour = true;
			}

			if(checkData[i].adjemptyPre > 0)
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
					isFour = true;
			}

			if(isFour)
				CountFour++;
		}
		else if(adjsame[i] == 3 )  //三子相连
		{
			//冲四检查
			if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 1)
			{
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					CountFour++;
			}

			if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre ==1)
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
					CountFour++;
			}

			//活三检查
			bool isThree = false;

			if((checkData[i].adjemptyNxt > 2 ||checkData[i].adjemptyNxt == 2 && checkData[i].jumpsameNxt == 0) &&
				(checkData[i].adjemptyPre > 1 || checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 0))
			{
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					isThree = true;
			}

			if((checkData[i].adjemptyPre > 2 ||checkData[i].adjemptyPre == 2 && checkData[i].jumpsamePre == 0) &&
				(checkData[i].adjemptyNxt > 1 || checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 0))
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
					isThree = true;
			}

			if(isThree)
				CountThree++;

		}

		else if(adjsame[i] == 2) //两子相连
		{
			//冲四判断
			if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 2)
			{
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					CountFour++;
			}

			if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 2)
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
					CountFour++;
			}

			//活三判断
			if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 1 && (checkData[i].jumpemptyNxt > 1
				||checkData[i].jumpemptyNxt == 1 && checkData[i].jumpjumpsameNxt == 0) && (checkData[i].adjemptyPre > 1
				||checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 0))
			{
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					CountThree++;
			}

			if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 1 && (checkData[i].jumpemptyPre > 1
				||checkData[i].jumpemptyPre == 1 && checkData[i].jumpjumpsamePre == 0) && (checkData[i].adjemptyNxt > 1
				||checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 0))
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
				 CountThree++;
			}
		}

		else if(adjsame[i] == 1) //单独一子
		{
			//冲四判断
			if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 3)
			{
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					CountFour++;
			}

			if(checkData[i].adjemptyPre ==1 && checkData[i].jumpsamePre ==3)
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
					CountFour++;
			}

			//活三判断
			if(checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt ==2 && (checkData[i].jumpemptyNxt >1 ||
				checkData[i].jumpemptyNxt == 1 && checkData[i].jumpjumpsameNxt == 0) &&
				(checkData[i].adjemptyPre > 1 || checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre == 0))
			{
				if(KeyPointForbiddenCheck(p0,direction,checkData[i].adjsameNxt) == NO_FORBIDDEN)
					CountThree++;
			}

			if(checkData[i].adjemptyPre == 1 && checkData[i].jumpsamePre ==2 && (checkData[i].jumpemptyPre >1 ||
				checkData[i].jumpemptyPre == 1 && checkData[i].jumpjumpsamePre == 0) &&
				(checkData[i].adjemptyNxt > 1 || checkData[i].adjemptyNxt == 1 && checkData[i].jumpsameNxt == 0))
			{
				if(KeyPointForbiddenCheck(p0,-direction,checkData[i].adjsamePre) == NO_FORBIDDEN)
					CountThree++;
			}
		}
		//
		//检测禁手分析结果，适时返回结果，减少计算
		if(CountFour > 1)//四四禁手
		{
			return FOUR_FOUR_FBD;
		}
		if(CountThree > 1)//三三禁手
		{
			return THREE_THREE_FBD;
		}
	}
	
	return NO_FORBIDDEN;
}



void MakeMove(Mov m,int player)
{
	SetChessOnBoard(m.x,m.y,player+1);
}
void UnmakeMove(Mov m)
{
	SetChessOnBoard(m.x,m.y,EMPTY_MOVE);
}

int distance(Psquare p1,Psquare p2)
{
	return max( abs(p1->x - p2->x), abs(p1->y - p2->y) );
}

Mov* MergeSort(Mov* source,int count)
{
	int s = 1;
	
	Mov* targetBuff = new Mov[count];

	MergePass(source,targetBuff,s,count);
	s +=s;

	while(s<count)
	{
		MergePass(targetBuff,source,s,count);
		s += s;
		MergePass(source,targetBuff,s,count);
		s += s;
	}
	delete[] source;
	
	return targetBuff;
}

void MergePass(Mov* source,Mov* target,const int s,const int n)
{
	int i =0;

	while(i <= n-2*s)
	{
		Merge(source,target,i,i+s-1,i+2*s-1);
		i = i+2*s;
	}

	if (i+s <n)
	{
		Merge(source,target,i,i+s-1,n-1);
	}
	else
	{
		for (int j = i; j <= n-1; j++)
		{
			target[j] = source[j];
		}
	}
}

//
void Merge(Mov* source,Mov* target,int l,int m,int r)
{
	int i= l;
	int j= m+1;
	int k= l;
	while((i <= m) && (j <= r))
	{
		if(source[i].val > source[j].val)
			target[k++] = source[i++];
		else
			target[k++] = source[j++];
	}

	if(i > m)
	{
		for (int q = j; q <= r; q++)
		{
			target[k++] = source[q];
		}
	}
	else
	{
		for (int q = i; q <= m; q++)
		{
			target[k++] = source[q];
		}
	}
}