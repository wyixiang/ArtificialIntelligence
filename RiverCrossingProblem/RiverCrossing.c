#include<stdio.h>
#include<stdlib.h>



/*************定义类型****************/
typedef struct state
{
	int m;//传教士
	int c;//野人
}node;

typedef struct element
{
	int m;
	int c;
	int lb;//left boat
	int bm;//boat carry m
	int bc;
	
} element;

typedef struct LStack
{
	element elem;
	struct LStack *next;
}*PLStack,LStack;



/*************栈函数****************/

int InitStack(PLStack *S)
{
	*S=NULL;
	return 1;
}

int ifempty(PLStack S)
{
	if (S==NULL) return 1;
	else return 0;
}

int Push(PLStack *S, element e)
{
	PLStack p;
	p = (PLStack)malloc(sizeof(LStack));
	p->elem = e;
	p->next = *S;
	*S = p;
	return 1;
}

int Pop(PLStack *S, element *e)
{
	PLStack p;
	if (!ifempty(*S))
	{
		*e = (*S)->elem;
		p = *S;
		*S = (*S)->next;
		free(p);
		return 1;
	}
	else
		return 0;
}


/*************input****************/

int problem_get (int *m,int *c,int *b)
{
	printf("Please input the number of m,c,b\n");
	printf("Please input like m c b\n");
	printf("input example: 3 3 2\n");
	scanf("%d%d%d", m,c,b);
	if (*m>0&&*c>0&&*b>0)
	{
		printf("input success\n");
		return 1;
	}
	else
	{
		printf("input error\n");
		printf("Press any key to input again\n");
		getchar();getchar();
		return 0;
	}
}




/*************求解函数****************/

int judge(node strat,int m,int c,int boat_size)
{
	int rm,rc;
	rm=strat.m-m;
	rc=strat.c-c;
	if( (m>=c||m==0) && (rm>=rc||rm==0)  )//情况符合规定 
		return 1;
	else
		return 0;
}


int findway(node start,int boat_size)
{
	int m, c, bm, bc, lb, rm, rc, x;
	int nm, nc, nbl;//next m c
	int i,j;
	element elem, e;
	PLStack S1, S2;
	InitStack(&S1);//S1正常使用
	InitStack(&S2);//S2最后用来倒序S1
	
	
	int maze[start.m+1][start.c+1];  //创建状态记录，避免进入死循环 
	for (i = 0; i <= start.m; i++)
			for (j = 0; j <= start.c; j++)
				maze[i][j]=(!judge(start,i,j,boat_size))*4;//标记不符合规定的状态（judge取反） 
	
	
	maze[start.m][start.c] = 1; //初始状态作上标记 
	elem.m = start.m;
	elem.c = start.c;
	elem.bm = 0;
	elem.bc = 0;
	elem.lb = 1;
	
	Push(&S1, elem);//压入初始状态 
	
	while (!ifempty(S1)) //栈不为空
	{
		Pop(&S1, &elem);
		m = elem.m;
		c = elem.c;
		lb = elem.lb;
		bm = elem.bm;
		bc = elem.bc;
		rm = start.m - m;
		rc = start.c - c;

		 //下一种人数选择 
		
		if(lb==1)
		{
			if ( bm < boat_size-bc && bm < m )
			{
				bm++;
			}
			else //if ( elem.bc < boat_size )，这样超界时 bc= boat_size+1
			{
				bc++;
				bm = 0;	
			}
		}
		else
		{
			if ( bm < boat_size-bc && bm < rm )
			{
				bm++;
			}
			else //if ( elem.bc < boat_size )，这样超界时 bc= boat_size+1
			{
				bc++;
				bm = 0;	
			}
		}
		
			
			
		
		while ( (bc <= boat_size) && ((bc <= c && lb==1)||(bc <= rc && lb==0)) && (m>=c||m==0) ) 
		{
			if(lb==1)
			{
				nm=m-bm;
				nc=c-bc;
				nbl=0;
			}
			else//lb=0
			{
				nm=m+bm;
				nc=c+bc;
				nbl=1;
			}
			
			/******************如果达成目标********************/
			if (nm == 0 && nc == 0 && nbl == 0)
			{
				elem.m = m;
				elem.c = c;
				elem.bm = bm;
				elem.bc = bc;
				elem.lb = lb;
				Push(&S1, elem);
				
				elem.m = nm;
				elem.c = nc;
				elem.bm = 0;
				elem.bc = 0;
				elem.lb = nbl;
				Push(&S1, elem);
				
				printf("\nreuslts:\n");
				while (S1) //逆置序列 并输出方案路径序列 
				{
					Pop(&S1, &e);
					Push(&S2, e);
				}
				while (S2)
				{
					Pop(&S2, &e);
					if (e.m==0&&e.c==0)
						printf("-->(m=0,c=0,boat: right)\n succeed to carry all the people to right band\n");
					else
					{
						if(e.lb)
							printf("-->(m=%d,c=%d,boat: left )--> (carry m=%d c=%d to right band)\n", e.m, e.c, e.bm, e.bc);
						else
							printf("-->(m=%d,c=%d,boat: right)--> (carry m=%d c=%d to left  band)\n", e.m, e.c, e.bm, e.bc);
					}
					
				}
				return 1;
			}
			/**************************************/
			
			
			if (maze[nm][nc] == 0 || maze[nm][nc] == lb || maze[nm][nc] == lb+2) //找到可以继续的非目标的状态 
			{
				maze[nm][nc] = lb+1; //标记经过此状态 
				elem.m = m;
				elem.c = c;
				elem.bm = bm;
				elem.bc = bc;
				elem.lb = lb;
				Push(&S1, elem); //当前状态入栈 
				m = nm; //下一状态转化为当前状态 
				c = nc;
				bc = 0;
				bm = 0;
				lb = nbl;
				rm = start.m - m;
				rc = start.c - c;
			}
			
			//下一种人数选择 
			if(lb==1)
			{
				if ( bm < boat_size-bc && bm < m )
				{
					bm++;
				}
				else //if ( elem.bc < boat_size )，这样超界时 bc= boat_size+1
				{
					bc++;
					bm = 0;	
				}
			}
			else
			{
				if ( bm < boat_size-bc && bm < rm )
				{
					bm++;
				}
				else //if ( elem.bc < boat_size )，这样超界时 bc= boat_size+1
				{
					bc++;
					bm = 0;	
				}
			}
		}
	}
	printf("Can't find solution of the problem\n");
	return 0;
}





int main()
{
	node start;
	int boat_size;
	char YoN;
	do
	{
		while(!problem_get(&start.m,&start.c,&boat_size)){} 
		findway(start,boat_size);
		printf("do you want to input again?<Y/N>\n");
		getchar();
		YoN=getchar();
	}while(YoN =='y'||YoN =='Y');
	return 0;
}











