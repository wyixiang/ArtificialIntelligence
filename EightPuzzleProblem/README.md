# EightPuzzleProblem

一、	摘要

此次试验使用 A* 算法解决八数码问题，使用单链表作为 open 表及 closed 表的存储结构，其中 open 表对 F_value 降序排序，以简化 open 到 closed 的转移过程。算法中以每个数码当前状态与目标状态的距离之和作为 f* 的估计，以每个状态经历的步数作为 g* 的估计。根据 A* 算法设计的程序认为题目给出的三个初始状态都有解。

二、	代码说明

代码共分为五个模块，分别为定义类型、链表函数、计算模块、输入输出模块以及逻辑模块。
类型定义了状态节点、数码坐标以及链表三种结构，其中状态节点包含 3 * 3 的数码存储数组、f 估计值、g 估计值、父亲节点指针四种成员。
链表函数包含了链表创建、链表销毁、链表元素查找、链表插入四种基本链表函数，以及为方便此次算法使用而创建的链表函数转移函数。
计算模块包含了数码坐标输出函数、f估计值计算函数、新状态节点创建函数。数码坐标输出为 f 估计值计算提供了简便可行的操作，而新状态节点创建过程中通过边界检测保证了新节点的合法性，并计算了 f 估计值、g 估计值以方便后续操作。
输入输出模块中，输出模块在程序执行结束给出目标节点后，通过状态节点中存储的父亲节点指针进行回溯，将目标节点到初始结点的路径存储在一个新的单链表中，然后依次输出。
此次程序设计中的算法逻辑模块由主函数担当，主函数以 A* 算法为基础，调用上面各类函数，最终成功则返回 1。

三、	实验结果

三个初始状态都有解。
