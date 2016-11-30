#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define path_train "C:\\Users\\11033\\Desktop\\test4\\traindata.txt"
#define path_test  "C:\\Users\\11033\\Desktop\\test4\\testdata.txt"
#define size_of_data 75

typedef struct dataset
{
	double x[4];
	int label;
}dataset, *Ptrdataset;

typedef struct bitreenode
{
	int data;
	double t;
	int attr;
	struct bitreenode *lchild, *rchild;
}TNode,*PTNode;

int InitTree(PTNode t)
{
	t->data = 0;
	t->attr = -1;
	t->t = -1;
	t->lchild = NULL;
	t->rchild = NULL;
}

int readfile_train(char *path, dataset *set)
{
	int i = 0;
	FILE *fp = NULL;
	if ((fp = fopen(path, "r")) == NULL)
	{
		printf("cannot open this file\n");
		exit(0);
	}
	fscanf(fp, "traindata=[", NULL);
	while (i<size_of_data)
	{
		if (fscanf(fp, "%lf %lf %lf %lf %d ", &set[i].x[0], &set[i].x[1], &set[i].x[2], &set[i].x[3], &set[i].label) == EOF)
		{
			break;
		}
		i++;
	}
	return 1;
}

int readfile_test(char *path, dataset *set)
{
	int i = 0;
	FILE *fp = NULL;
	if ((fp = fopen(path, "r")) == NULL)
	{
		printf("cannot open this file\n");
		exit(0);
	}
	fscanf(fp, "testdata=[", NULL);
	while (i<size_of_data)
	{
		if (fscanf(fp, "%lf %lf %lf %lf %d ", &set[i].x[0], &set[i].x[1], &set[i].x[2], &set[i].x[3], &set[i].label) == EOF)
		{
			break;
		}
		i++;
	}
	return 1;
}

int cmp(const void *a, const void *b)
{
	return *(double *)a > *(double *)b ? 1 : -1;
}

double info(dataset *set, int size)
{
	double sum = 0, n = 0;
	double p;
	int i,j;
	for ( i = 1; i < 4; i++)
	{
		n = 0;
		for ( j = 0; j < size; j++)
		{
			if (set[j].label == i)
				n++;
		}
		p = (n+0.0) / size;
		if (p != 0)
			sum += -p*log2(p);
	}
	return sum;
}

double infoA(dataset *set, int size, int attr, double t)
{
	double infod1 = 0;
	double infod2 = 0;
	double p,sum;
	int i, j;
	int sizeofd1 = 0, sizeofd2 = 0, n1 = 0, n2 = 0;
	
	for (i = 1; i < 4; i++)
	{
		n1 = n2 = 0;
		sizeofd1 = sizeofd2 = 0;
		for (j = 0; j < size; j++)
		{
			if (set[j].x[attr] > t)
			{
				sizeofd1++;
				if (set[j].label == i)
					n1++;
			}
			else
			{
				sizeofd2++;
				if (set[j].label == i)
					n2++;
			}
		}
		p = (n1 + 0.0) / sizeofd1;
		if(p!=0)
			infod1 += -p*log2(p);
		p = (n2 + 0.0) / sizeofd2;
		if (p != 0)
			infod2 += -p*log2(p);
	}
	sum = ((sizeofd1+0.0) / size)*infod1 + ((sizeofd2+0.0) / size)*infod2;
	return sum;
}

int sort(dataset *set, int size, int attr, double *gain_r, double *t_r)
{
	double *a;
	int i = 0;
	double gain, t;
	double gain0=0, t0;

	a = (double*)malloc(sizeof(double)*size);

	for (i = 0; i < size; i++)
		a[i] = set[i].x[attr];
	qsort(a, size, sizeof(a[0]), cmp);
	i = 0;
	while (i < size-1)
	{
		if (a[i] == a[i + 1])
			i++;
		else
		{
			t = (a[i] + a[i + 1]) / 2;
			gain = info(set, size) - infoA(set, size, attr, t);
			//printf("gain=%f,t=%f\n", gain, t);
			if (gain > gain0)
			{
				gain0 = gain;
				t0 = t;
			}
			i++;
		}
	}
	//printf("\ngain=%f,t=%f\n\n", gain0, t0);
	free(a);
	*gain_r = gain0;
	*t_r = t0;
}

int id3(PTNode T, dataset *set, int size, int *attr)
{
	int i = 0, attr0, done=1, usedattr=0;
	double gain0=0, gain, t0, t;
	dataset d1[75], d2[75];
	int size1, size2;
	int a[4];
	for (i = 0; i < 4; i++)
		a[i] = attr[i];

	size1 = size2 = 0;

	//训练集已分类完成
	for (i = 0; i < size; i++)
	{
		if (set[0].label != set[i].label)
			done = 0;
	}
	if (done == 1)
	{
		T->data = set[0].label;
		return 1;
	}
	//属性用完
	for (i = 0; i < 4; i++)
	{
		if (attr[i] == 1)
			usedattr++;
	}
	if (usedattr == 4)
	{
		int n[4];
		n[0] = n[1] = n[2] = n[3] = 0;
		for (i = 0; i < size; i++)
		{
			n[set[i].label]++;
		}
		if (n[1] >= n[2] && n[1] >= n[3])
			T->data = 1;
		if (n[2] >= n[1] && n[2] >= n[3])
			T->data = 2;
		if (n[3] >= n[1] && n[3] >= n[2])
			T->data = 3;
		return 1;
	}


	//开始分类
	for (i = 0; i < 4; i++)
	{
		if (attr[i] == 1)
			continue;
		sort(set, size, i, &gain, &t);
		if (gain > gain0)
		{
			gain0 = gain;
			t0 = t;
			attr0 = i;
		}
	}
	T->attr = attr0;
	T->t = t0;
	a[attr0] = 1;
	for (i = 0; i < size; i++)
	{
		if (set[i].x[attr0] > t0)
		{
			d1[size1] = set[i];
			size1++;
		}
		else
		{
			d2[size2] = set[i];
			size2++;
		}
	}
//	for (i = 0; i < 4; i++)
	//	printf("%d\t", a[i]);
	//printf("\n");
	T->lchild = (PTNode)malloc(sizeof(TNode));
	InitTree(T->lchild);
	id3(T->lchild, d1, size1, a);
	T->rchild = (PTNode)malloc(sizeof(TNode));
	InitTree(T->rchild);
	id3(T->rchild, d2, size2, a);
	return 0;
}

double split_infoA(dataset *set, int size, int attr, double t)
{
	double sum;
	int i, j;
	int sizeofd1 = 0, sizeofd2 = 0;

	for (i = 1; i < 4; i++)
	{
		sizeofd1 = sizeofd2 = 0;
		for (j = 0; j < size; j++)
		{
			if (set[j].x[attr] > t)
			{
				sizeofd1++;
			}
			else
			{
				sizeofd2++;
			}
		}
	}
	sum = -((sizeofd1 + 0.0) / size)*log2((sizeofd1 + 0.0) / size) - ((sizeofd2 + 0.0) / size)*log2((sizeofd2 + 0.0) / size);
	return sum;
}

int sortc45(dataset *set, int size, int attr, double *gain_r, double *t_r)
{
	double *a;
	int i = 0;
	double gain, t, gain_ratio;
	double gain_ratio0 = 0, t0;

	a = (double*)malloc(sizeof(double)*size);

	for (i = 0; i < size; i++)
		a[i] = set[i].x[attr];
	qsort(a, size, sizeof(a[0]), cmp);
	i = 0;
	while (i < size - 1)
	{
		if (a[i] == a[i + 1])
			i++;
		else
		{
			t = (a[i] + a[i + 1]) / 2;
			gain = info(set, size) - infoA(set, size, attr, t);
			gain_ratio = gain / split_infoA(set, size, attr, t);
			//printf("gain=%f,t=%f\n", gain, t);
			if (gain_ratio > gain_ratio0)
			{
				gain_ratio0 = gain_ratio;
				t0 = t;
			}
			i++;
		}
	}
	//printf("\ngain=%f,t=%f\n\n", gain0, t0);
	free(a);
	*gain_r = gain_ratio0;
	*t_r = t0;
}

int c45(PTNode T, dataset *set, int size, int *attr)
{
	int i = 0, attr0, done = 1, usedattr = 0;
	double gain0 = 0, gain, t0, t;
	dataset d1[75], d2[75];
	int size1, size2;
	int a[4];
	for (i = 0; i < 4; i++)
		a[i] = attr[i];

	size1 = size2 = 0;

	//训练集已分类完成
	for (i = 0; i < size; i++)
	{
		if (set[0].label != set[i].label)
			done = 0;
	}
	if (done == 1)
	{
		T->data = set[0].label;
		return 1;
	}
	//属性用完
	for (i = 0; i < 4; i++)
	{
		if (attr[i] == 1)
			usedattr++;
	}
	if (usedattr == 4)
	{
		int n[4];
		n[0] = n[1] = n[2] = n[3] = 0;
		for (i = 0; i < size; i++)
		{
			n[set[i].label]++;
		}
		if (n[1] >= n[2] && n[1] >= n[3])
			T->data = 1;
		if (n[2] >= n[1] && n[2] >= n[3])
			T->data = 2;
		if (n[3] >= n[1] && n[3] >= n[2])
			T->data = 3;
		return 1;
	}


	//开始分类
	for (i = 0; i < 4; i++)
	{
		if (attr[i] == 1)
			continue;
		sortc45(set, size, i, &gain, &t);
		if (gain > gain0)
		{
			gain0 = gain;
			t0 = t;
			attr0 = i;
		}
	}
	T->attr = attr0;
	T->t = t0;
	a[attr0] = 1;
	for (i = 0; i < size; i++)
	{
		if (set[i].x[attr0] > t0)
		{
			d1[size1] = set[i];
			size1++;
		}
		else
		{
			d2[size2] = set[i];
			size2++;
		}
	}
	//	for (i = 0; i < 4; i++)
	//	printf("%d\t", a[i]);
	//printf("\n");
	T->lchild = (PTNode)malloc(sizeof(TNode));
	InitTree(T->lchild);
	c45(T->lchild, d1, size1, a);
	T->rchild = (PTNode)malloc(sizeof(TNode));
	InitTree(T->rchild);
	c45(T->rchild, d2, size2, a);
	return 0;
}

int test_one(PTNode T, dataset set)
{
	if (T->data == 0)
		if (set.x[T->attr] > T->t)
			return test_one(T->lchild, set);
		else
			return test_one(T->rchild, set);
	else
		return T->data;
}

int test(PTNode T, dataset *set, int size)
{
	int i;
	int n = 0;
	double p;
	for (i = 0; i < size; i++)
	{
		if (set[i].label == test_one(T, set[i]))
			n++;
	}
	p = (n + 0.0) / size;
	printf("Correct rate=%f\n", p);
}

int printtree(PTNode T,int i)
{
	if (T->data == 0)
	{
		printf("a=%d\tt=%f\n", T->attr, T->t);
		printtree(T->lchild, i + 1);
		printtree(T->rchild, i + 1);
	}
	else
		printf("done:%d\n", T->data);
}

int main()
{
	int i = 0;
	dataset set[size_of_data];
	PTNode T;
	int arrt[4] = { 0,0,0,0 };

	T = (PTNode)malloc(sizeof(TNode));
	InitTree(T);
	readfile_train(path_train, set);
	
	//id3(T, set, size_of_data, arrt);
	c45(T, set, size_of_data, arrt);

	readfile_test(path_test, set);

	test(T, set, size_of_data);

	printtree(T, 0);

	return 0;
}