#include<stdio.h> 
#include<stdlib.h> 

/****************定义类型***************/
typedef struct fc   //figure coordinate
{
	int x;
	int y;
}fc;

typedef struct Node
{
	int fig[3][3];
	int f;
	int g;
	struct Node *father;
}*PNode,Node;

typedef struct List
{
	Node elem;
	struct List *next;
}ListNode,*ListNodePtr;
typedef ListNodePtr List,*ListPtr;
/*******************************/

/****************定义链表函数***************/
int ListInit(ListPtr PL)
{
	*PL = (ListNodePtr)malloc(sizeof(ListNode));
	if (*PL)
	{
		(*PL)->next = NULL;
		return 1;
	}
	return 0;
}

//如果没找到则不改变pre的值
//输出目标节点的前置节点指针
int ListSearch(List L, Node n, ListNodePtr *pre)
{
	int i, j, break_flag=1;
	ListNodePtr p = L->next,q=L;
	//*pre = NULL;
	
	while (p)
	{
		break_flag = 1;
		for (i = 0; i < 3 && break_flag; i++)
			for (j = 0; j < 3 && break_flag; j++)
				if (p->elem.fig[i][j] != n.fig[i][j]) 
					break_flag=0;

		if (i == 3 && j == 3)
		{
			*pre = q;
			return 1;
		}
		q = q->next;
		p = p->next;
	}
	return 0;
}

void ListClear(List L)
{
	ListNodePtr p = L->next, q = L;
	while (p)
	{
		q->next = p->next;
		free(p);
		p = q->next;
	}
}

void ListDestory(List L)
{
	ListClear(L);
	free(L);
}

int ListInsert(List L, Node N)
{
	ListNodePtr s,p=L->next,q=L;

	s = (ListNodePtr)malloc(sizeof(ListNode));
	if (s == NULL) return 0;

	while (p != NULL)
	{
		if (N.f < p->elem.f) break;
		p = p->next;
		q = q->next;
	}
	s->elem = N;
	s->next = p;
	q->next = s;
	
	return 1;
}

//移动open元素到close
int RemoveAndInsert(List open, List closed)
{
	ListNodePtr s;
	//open remove
	s = open->next;
	open->next = s->next;
	//closed insert
	s->next = closed->next;
	closed->next = s;
}
/*******************************/

/**************部分计算模块*****************/
int FindFigure(int fig[3][3],fc *pos,int value)
{
	int i, j;
	for( i = 0 ; i < 3 ; i++)
		for ( j = 0; j < 3; j++)
			if (fig[i][j]==value)
			{
				pos->x = i;
				pos->y = j;
				return 1;
			}
	return 0;
}

int fValue(int fig[9])
{
	int i,j,f=0;
	int target[3][3] = { 1,2,3,8,0,4,7,6,5 };
	fc pos;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
		{
			FindFigure(fig, &pos, target[i][j]);
			f += abs(pos.x - i) + abs(pos.y - j);
		}
	return f;
}

int NewNode(PNode fa,PNode new,int d,fc zero)
{
	int i,j;
	int diradd[4][2] = { { 0,1 },{ 1,0 },{ 0,-1 },{ -1,0 } };
	fc newzero;
	//FindFigure(fa->fig,&zero,0);

	newzero.x = zero.x + diradd[d][0];
	newzero.y = zero.y + diradd[d][1];

	if (newzero.x < 0 || newzero.x > 2 || newzero.y < 0 || newzero.y > 2)
		return 0;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			new->fig[i][j] = fa->fig[i][j];

	new->fig[newzero.x][newzero.y] = 0;
	new->fig[zero.x][zero.y] = fa->fig[newzero.x][newzero.y];
	new->g = fa->g + 1;
	new->father = fa;
	new->f = new->g + fValue(new->fig);
	return 1;
}
/*******************************/

/***************输入输出模块****************/
//问题输入函数
int ProblemGet(PNode n)
{
	int i, j;
	printf("请输入你的八数码问题(空格用零表示）\n");
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			scanf_s("%d", &n->fig[i][j]);
	n->g = 0;
	n->father = NULL;
	n->f = fValue(n->fig);
	printf("\n原图为：\n");
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			if (n->fig[i][j]) 
				printf("%d ", n->fig[i][j]);
			else 
				printf("  ");
		printf("\n");
	}
}

//结果输出函数
int ResultPrint(Node n)
{
	int i, j,NO=0;

	List L;
	ListInit(&L);
	ListNodePtr s, p, q;
	Node m;
	

	s = (ListNodePtr)malloc(sizeof(ListNode));
	if (s == NULL) 
		return 0;

	s->elem = n;
	s->next = L->next;
	L->next = s;

	//ListInsert(L, n);

	while (L->next->elem.father)
	{
		s = (ListNodePtr)malloc(sizeof(ListNode));
		if (s == NULL) 
			return 0;
		m = *(L->next->elem.father);
		s->elem = m;
		s->next = L->next;
		L->next = s;


		//ListInsert(L, *(L->next->elem.father));
	}

	printf("\n移动过程：\n");

	while (L->next)
	{
		printf("\nstep %d:\n", NO++);
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
				if (L->next->elem.fig[i][j]) 
					printf("%d ", L->next->elem.fig[i][j]);
				else 
					printf("  ");
			printf("\n");
		}
		L->next = L->next->next;
	}

	printf("\n移动结束！");
	return 1;
}

/*******************************/

int main()
{
	//环境初始化
	int d;
	List open, closed;
	Node n,s;
	ListNodePtr oldpre,old;
	fc zero;

	ListInit(&open);
	ListInit(&closed);
	//问题初始化
	ProblemGet(&n);
	ListInsert(open, n);
	
	while (open->next)
	{
		RemoveAndInsert(open, closed);//取最优节点
		s = closed->next->elem;
		if (s.f == s.g)//是否目标
		{
			ResultPrint(s);
			ListDestory(open);
			ListDestory(closed);
			getchar(); getchar();
			return 1;
		}
		FindFigure(s.fig, &zero, 0);//找到零节点坐标
		for (d = 0; d < 4; d++)//依次分析四种后续情况
		{
			if (NewNode(&(closed->next->elem), &n, d,zero))//如果新节点合法
			{
				if (ListSearch(open,n,&oldpre)||ListSearch(closed,n,&oldpre))//如果不是初次发现的节点
				{
					old = oldpre->next;
					if (n.g < old->elem.g)
					{
						oldpre->next = old->next;
						ListInsert(open, n);
					}
				}
				else//否则插入open表
				{
					ListInsert(open, n);
				}
			}
		}
	}
	printf("无解！");
	return 0;
}
