# CrossRiverProblem
Artificial Intelligence

一、摘要

本次实验选择野人过河问题，以状态空间法实现。算法采用深度优先的思想，基于链栈形式存储过程状态，并通过二维数组存储历史状态，避免了再次进入历史状态引发死循环的潜在问题。这里设计了5元状态描述安全过河方案的过程，而未采用课程ppt上的P、Q操作与三元状态混合表示过程的方案，这样做避免了繁杂的PQ定义过程，因此实现了对任意数目的m,c,n问题的求解。

二、程序详解

（备注：程序基于C语言编写，因采用了动态数组因此必须采用允许C99的编译器编译。）

对于野人过河问题，我设计了5个主要模块，分别为定义类型、栈函数、输入函数、求解函数和主函数。
在定义类型的模块里，我设计了5元状态结构体element来存储问题求解过程中的状态，element包含的元素为：左岸传教士人数，左岸野人人数，船只位置（左岸为1，右岸为0），下一步船只携带的传教士人数，下一步船只携带的野人人数。
在求解模块中，我首先创建了二维数组maze[start.m+1][start.c+1]存储每种状态的性质，在二维数组中，我设计了可行（未经历此状态）、可行（经历过，船在左岸）、可行（经历过，船在右岸）、不可行四种状态标记状态历史，并编写judge函数判断左岸m,c的数量是否符合题意，遍历左岸所有的mc状态，将judge的结果写入刚刚创建的二维数组maze。求解过程中，遇可行状态则压入链栈，并且在二维数组maze中有所记录，避免陷入死循环，如遇不可行的状态分支，链栈会进行弹出操作。对小船的运输方案，这里采用“区分小船位置（0，1），遍历全部方案”的思路进行，设计上未采用ppt上的PQ操作实际上是为了避免PQ带来的有限性，以达到实现手动输入c、m、n的目的。

三、结果

实现了手动输入m,c,n，然后输出安全渡河的过程；如无安全渡河的方案，也给出提示。
