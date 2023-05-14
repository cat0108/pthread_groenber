#pragma once
#include<pthread.h>
#include<iostream>
#include<semaphore.h>

class BitMap {

	int size;//总的开辟数组的个数
	int range;//总元素个数
public:
	unsigned int* bits = NULL;//总的开辟数组
	BitMap() { };
	BitMap(const BitMap& bit)
	{
		this->bits = bit.bits;
		this->size = bit.size;
		this->range = bit.range;

	}
	BitMap(int range)
	{
		this->range = range;
		size = (int)(range / 32 + 1);
		bits = new unsigned int[size];
	}
	void add(int indata)
	{
		int index = (int)(indata / 32);
		int bit_index = indata % 32;
		bits[index] |= 1 << bit_index;//将位图的某一位置为1
	}
	void remove(int indata)
	{
		int index = (int)(indata / 32);
		int bit_index = indata % 32;
		bits[index] &= ~(1 << bit_index);//将该位置为0
	}
	//判断某一位的值是否存在
	bool Find(int indata)
	{
		int index = int(indata / 32);
		int bit_index = indata % 32;
		return (bits[index] >> bit_index) & 1;
	}
	void initialize()
	{
		for (int i = 0; i < size; i++)
			bits[i] = 0;
	}
	bool judge_zero()
	{
		bool flag = 1;
		for (int i = 0; i < size; i++)//可以并行化判断是否全为0
			if (bits[i] != 0)
				flag = 0;
		return flag;
	}
	int Getsize() { return size; }
	unsigned int* Getbits() { return bits; }
	int Getrange() { return range; }
};

//采用并行化判断是否为0
//定义线程数据结构,需要传递一个类
typedef struct {
	int t_id;
	BitMap bit;
}threadParam_t;
const int Num_thread = 3;
unsigned int my_max[Num_thread];


//找首项
void* threadFunc_find_max(void* Param)
{
	threadParam_t* p = (threadParam_t*)Param;//传递参数
	BitMap bit = p->bit;
	int t_id = p->t_id;
	int i = bit.Getsize() - 1-t_id;
	while (bit.bits[i] == 0 && i >= Num_thread)
	{
		i-=Num_thread;
	}
	for (int j = 0; j < 32; j++)
		if (bit.Find(i * 32 + j) == 1)
			my_max[t_id] = i * 32 + j;
	pthread_exit(NULL);
	return NULL;
}

//寻找最大值过程也可以并行化
unsigned int  find_max(BitMap bit)
{
	pthread_t* handles = new pthread_t[Num_thread];//创建线程句柄
	threadParam_t* param = new threadParam_t[Num_thread];//需要传递的参数
	for (int t_id = 0; t_id < Num_thread; t_id++)
	{
		param[t_id].t_id = t_id;
		param[t_id].bit = bit;
		pthread_create(&handles[t_id], NULL, threadFunc_find_max, &param[t_id]);
	}
	for (int t_id = 0; t_id < Num_thread; t_id++)
		pthread_join(handles[t_id], NULL);//获得返回值
	unsigned int all_max = 0;
	for (int t_id = 0; t_id < Num_thread; t_id++)
		if (my_max[t_id] >= all_max)
			all_max = my_max[t_id];
	return all_max;
}

