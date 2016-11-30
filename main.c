#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define path_train "C:\\Users\\11033\\Desktop\\test3\\Iris-train.txt"
#define path_test  "C:\\Users\\11033\\Desktop\\test3\\Iris-test.txt"
#define seed 6
#define alpha 0.06
#define lanbda 0.0001

#define size_of_data 75





typedef struct dataset
{
	double x[4];
	int label;
}dataset,*Ptrdataset;

typedef struct nn4_10_3
{
	double W1[10][4], W2[3][10];
	double b1[10], b2[3];
}*Ptrnn,nn;

//文件读取
int readfile(char *path, dataset *set)
{
	int i = 0;
	FILE *fp = NULL;
	if ((fp = fopen(path, "r")) == NULL)
	{
		printf("cannot open this file\n");
		exit(0);
	}
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
//网络初始化
int InitNet(Ptrnn n)
{
	int i, j;

	for ( i = 0	; i < 10; i++)
	{
		for ( j = 0; j < 4; j++)
		{
			n->W1[i][j] = rand() / (RAND_MAX+0.0)-0.5;
		}
	}
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			n->W2[i][j] =  rand() / (RAND_MAX + 0.0) - 0.5;
		}
	}
	for (i = 0; i < 10; i++)
	{
		n->b1[i] = rand() / (RAND_MAX + 0.0) - 0.5;
	}
	for (i = 0; i < 3; i++)
	{
		n->b2[i] = rand() / (RAND_MAX + 0.0) - 0.5;
	}
	return 0;
}
//网络所有权值归零（批量下降累加时使用）
int ZeroNet(Ptrnn n)
{
	int i, j;

	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 4; j++)
		{
			n->W1[i][j] = 0;
		}
	}
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			n->W2[i][j] = 0;
		}
	}
	for (i = 0; i < 10; i++)
	{
		n->b1[i] = 0;
	}
	for (i = 0; i < 3; i++)
	{
		n->b2[i] = 0;
	}
	return 0;
}
//打印所有权值
int printW(Ptrnn n)
{
	int i, j;

	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 4; j++)
		{
			printf("n->W1[%d][%d]=%f\n",i,j,n->W1[i][j]);
		}
		printf("\n");
	}
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			printf("n->W2[%d][%d]=%f\n", i, j, n->W2[i][j]);
		}
		printf("\n");
	}
	for (i = 0; i < 10; i++)
	{
		printf("n->b1[%d]=%f\n", i, n->b1[i]);
	}
	printf("\n");
	for (i = 0; i < 3; i++)
	{
		printf("n->b2[%d]=%f\n", i, n->b2[i]);
	}
	printf("\n");
	return 0;
}
//训练输出内存归零
int Inittrain(double *delta2, double *delta3, double *a, double *h)
{
	int i, j;
	for (i = 0; i < 10; i++)
	{
		delta2[i] = 0;
		a[i] = 0;
	}
	for (j = 0; j < 3; j++)
	{
		delta3[j] = 0;
		h[j] = 0;
	}
	return 0;
}
//测试内存归零
int Inittest(double *a2, double *h)
{
	int i, j;
	for (i = 0; i < 10; i++)
	{
		a2[i] = 0;
	}
	for (j = 0; j < 3; j++)
	{
		h[j] = 0;
	}
	return 0;
}
//sigmoid函数
double sigmoid(double z)
{
	return 1 / (1 + exp(-z));
}
//输入值归一化
int minmax_scaling(dataset *set)
{
	double min, max;
	int i=0,j=0;
	while (j<4)
	{
		max = min = set[0].x[j];
		for ( i = 0; i < size_of_data; i++)
		{
			if (set[i].x[j]<min)
			{
				min = set[i].x[j];
			}
			if (set[i].x[j]>max)
			{
				max = set[i].x[j];
			}
		}
		
		for (i = 0; i < size_of_data; i++)
		{
			set[i].x[j] = (set[i].x[j] - min) / (max - min);
		}
		j++;
	}
	return 0;
}

int train(Ptrnn n,int time)
{
	int i,j,setNum=0, Num = 0;
	double delta2[10], delta3[3];
	double a[10], h[3];
	double z=0,d=0;
	int y[3][3] = { { 1,0,0 },{ 0,1,0 },{ 0,0,1 } };
	nn delta;
	double label;

	dataset set[size_of_data];

	readfile(path_train, set);
	minmax_scaling(set);
	//for(i=0;i<size;i++)
	//	printf("i=%d\t%lf %lf %lf %lf %d \n", i, set[i].x[0], set[i].x[1], set[i].x[2], set[i].x[3], set[i].label);

	while (setNum<time)
	{
		ZeroNet(&delta);
		for (Num=0;Num < size_of_data;Num++)
		{
			////////////////初始化
			Inittrain(delta2, delta3, a, h);

			///////////////////////正向传播
			for (i = 0; i < 10; i++)
			{
				z = 0;
				for (j = 0; j < 4; j++)
				{
					z += n->W1[i][j] * set[Num].x[j];
				}
				z += n->b1[i];
				a[i] = sigmoid(z);
			}

			for (i = 0; i < 3; i++)
			{
				z = 0;
				for (j = 0; j < 10; j++)
				{
					z += n->W2[i][j] * a[j];
				}
				z += n->b2[i];
				h[i] = sigmoid(z);
			}

			////////////////////////计算权重修改值
			for (i = 0; i < 3; i++)
			{
				delta3[i] = -(y[set[Num].label][i] - h[i])  *  (h[i] * (1 - h[i]));
			}

			for (i = 0; i < 10; i++)
			{
				d = 0;
				for (j = 0; j < 3; j++)
				{
					d += n->W2[j][i] * delta3[j];
				}
				delta2[i] = d*(a[i] * (1 - a[i]));
			}

			for (i = 0; i < 10; i++)
			{
				for (j = 0; j < 4; j++)
				{
					delta.W1[i][j] += set[Num].x[j] * delta2[i];
				}
			}

			for (i = 0; i < 10; i++)
			{
				delta.b1[i] += delta2[i];
			}

			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 10; j++)
				{
					delta.W2[i][j] += a[j] * delta3[i];
				}
			}

			for (i = 0; i < 3; i++)
			{
				delta.b2[i] += delta3[i];
			}
		}

		///////////////修改权值
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 4; j++)
			{
				n->W1[i][j] -= alpha*(delta.W1[i][j]/ size_of_data + lanbda*n->W1[i][j]);
			}
		}

		for (i = 0; i < 10; i++)
		{
			n->b1[i] -= alpha*delta.b1[i]/ size_of_data;
		}

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 10; j++)
			{
				n->W2[i][j] -= alpha*(delta.W2[i][j]/ size_of_data + lanbda*n->W2[i][j]);
			}
		}
		
		for (i = 0; i < 3; i++)
		{
			n->b2[i] -= alpha*delta.b2[i]/ size_of_data;
			//if(i==0)
			//printf("delta.b2[%d]=%f\n", i, delta.b2[i]);
		}

		setNum++;
	}
	return 0;
}

int test(Ptrnn n)
{
	int i, j, Num = 0;
	double a[10], h[3];
	double z = 0;
	int y[3][3] = { { 1,0,0 },{ 0,1,0 },{ 0,0,1 } };
	int cor = 0;

	dataset set[size_of_data];

	readfile(path_test, set);
	minmax_scaling(set);

	while (Num < size_of_data)
	{
		Inittest(a, h);
		///////////////////////
		for (i = 0; i < 10; i++)
		{
			z = 0;
			for (j = 0; j < 4; j++)
			{
				z += n->W1[i][j] * set[Num].x[j];
			}
			z += n->b1[i];
			a[i] = sigmoid(z);
			//printf("z=%f,a=%f\n", z, a2[i]);
		}

		for (i = 0; i < 3; i++)
		{
			z = 0;
			for (j = 0; j < 10; j++)
			{
				z += n->W2[i][j] * a[j];
			}
			z += n->b2[i];
			h[i] = sigmoid(z);
		}

		/*for (i = 0; i < 3; i++)
		{
			printf("y[%d]=%d h[%d]=%f\t", i, y[set[Num].label][i], i, h[i]);
		}
		printf("\n");*/
		
		int t = 0;
		for ( i = 1; i < 3; i++)
		{
			if (h[i] >= h[t])
				t = i;
		}
		if (y[set[Num].label][t]==1)
		{
			cor++;
		}
		Num++;
	}
	printf("\ncor=%f\n", cor/75.0);
	return 0;
}

int main()
{
	for (int i = 0; i < 10; i++)
	{
		nn n;
		srand(i+28);
		InitNet(&n);
		//printW(&n);
		train(&n, 10000);
		//printW(&n);
		test(&n);
	}
		

	
	//for(i=0;i<size;i++)
	//	printf("i=%d\t%lf %lf %lf %lf %d \n", i, set[i].x[0], set[i].x[1], set[i].x[2], set[i].x[3], set[i].label);
}