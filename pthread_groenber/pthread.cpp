#include<iostream>
#include"bitmap.h"
#include<fstream>
#include<vector>
#include<sstream>
#include<xmmintrin.h>
#include<emmintrin.h>
#include<immintrin.h>
#include<Windows.h>
#include<pthread.h>

using namespace std;

int Find_row(BitMap* bit1, int elimi1, int key)//找消元子是否存在对应首项，不存在返回-1，存在返回首项所在的行数
{
	for (int i = 0; i < elimi1; i++)
		if (find_max(bit1[i]) == key)
			return i;
	return -1;
}

//参数分别为消元子，被消元行，矩阵列数，消元子行数，被消元行行数
void Guass_elimination(BitMap* bit1, BitMap* bit2, int col, int elimi1, int elimi2)
{
	__m256i r0, r1;
	for (int i = 0; i < elimi2; i++)//对被消元行进行遍历
	{
		while (!bit2[i].judge_zero())//如果整行不为0则进行消元操作
		{
			int j;
			int temp = Find_row(bit1, elimi1 + elimi2, find_max(bit2[i]));//找消元子是否存在对应首项
			if (temp != -1)
			{
				for (j = 0; j + 8 <= bit2[j].Getsize(); j += 8)
				{
					r0 = _mm256_loadu_si256((__m256i*)(bit2[i].Getbits() + j));
					r1 = _mm256_loadu_si256((__m256i*)(bit1[temp].Getbits() + j));
					r1 = _mm256_xor_si256(r0, r1);
					_mm256_storeu_si256((__m256i*)(bit2[i].Getbits() + j), r1);
				}
				for (j; j < bit2[j].Getsize(); j++)
					bit2[i].Getbits()[j] = bit2[i].Getbits()[j] ^ bit1[temp].Getbits()[j];
			}
			else
			{

				for (j = 0; j + 8 <= bit2[j].Getsize(); j += 8)
				{
					r0 = _mm256_loadu_si256((__m256i*)(bit2[i].Getbits() + j));
					r1 = _mm256_loadu_si256(&r0);
					_mm256_storeu_si256((__m256i*)(bit1[elimi1 + i].Getbits() + j), r1);
				}
				for (j; j < bit2[j].Getsize(); j++)
					bit1[elimi1 + i].Getbits()[j] = bit2[i].Getbits()[j];
				break;
			}
		}
	}
}

int main()
{
	LARGE_INTEGER fre, begin, end;
	double gettime;
	int col, elimi1, elimi2;
	cout << "输入总列数，消元子，被消元行数" << endl;
	cin >> col >> elimi1 >> elimi2;

	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&begin);


	//读取被消元行,保存到BitMap中
	vector<vector<int>> matrix;
	ifstream inputfile("C:\\Users\\cat\\Desktop\\并行程序设计\\SIMD\\Groebnerdata\\测试样例1 矩阵列数130，非零消元子22，被消元行8\\被消元行.txt");
	if (inputfile.is_open())
	{
		string line;
		while (getline(inputfile, line))//一行一行地读取数据
		{
			vector<int> row;
			stringstream ss(line);//构造字符串输入流，用于数据转换
			int num;
			while (ss >> num)
				row.push_back(num);
			matrix.push_back(row);
		}
		inputfile.close();
	}

	BitMap* bit1 = new BitMap[elimi2];
	for (int i = 0; i < elimi2; i++)
	{
		bit1[i] = *(new BitMap(col + 1));
		bit1[i].initialize();
	}
	//赋值到位图中去
	for (int i = 0; i < (int)matrix.size(); i++)
	{
		for (int j = 0; j < (int)matrix[i].size(); j++)
		{
			/*cout << matrix[i][j]<<" ";*/
			bit1[i].add(matrix[i][j]);
		}

		/*cout << endl<<endl;*/
	}

	/*for (int i = 0; i < elimi2; i++) {
		for (int j = 0; j <= col; j++)
			cout << bit1[i].Find(j);
		cout << endl<<endl;
	}*/

	QueryPerformanceCounter(&end);
	gettime = (double)((end.QuadPart - begin.QuadPart) * 1000.0) / (double)fre.QuadPart;
	cout << "read time: " << gettime << " ms" << endl << endl;

	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&begin);


	//读取消元子,保存到BitMap中
	vector<vector<int>> matrix1;
	ifstream inputfile1("C:\\Users\\cat\\Desktop\\并行程序设计\\SIMD\\Groebnerdata\\测试样例1 矩阵列数130，非零消元子22，被消元行8\\消元子.txt");
	if (inputfile1.is_open())
	{
		string line;
		while (getline(inputfile1, line))//一行一行地读取数据
		{
			vector<int> row;
			stringstream ss(line);//构造字符串输入流，用于数据转换
			int num;
			while (ss >> num)
				row.push_back(num);
			matrix1.push_back(row);
		}
		inputfile1.close();
	}

	BitMap* bit2 = new BitMap[elimi1 + elimi2];
	for (int i = 0; i < elimi1 + elimi2; i++)
	{
		bit2[i] = *(new BitMap(col));
		bit2[i].initialize();
	}
	for (int i = 0; i < (int)matrix1.size(); i++)
	{
		for (int j = 0; j < (int)matrix1[i].size(); j++)
		{
			/*cout << matrix1[i][j] << " ";*/
			bit2[i].add(matrix1[i][j]);
		}

		/*cout << endl << endl;*/
	}
	/*for (int i = 0; i < elimi1 + elimi2; i++) {
		for (int j = 0; j <= col; j++)
			cout << bit2[i].Find(j);
		cout << endl << endl;
	}*/

	QueryPerformanceCounter(&end);
	gettime = (double)((end.QuadPart - begin.QuadPart) * 1000.0) / (double)fre.QuadPart;
	cout << "read time: " << gettime << " ms" << endl << endl;


	//时间测试

	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&begin);

	Guass_elimination(bit2, bit1, col, elimi1, elimi2);

	QueryPerformanceCounter(&end);
	gettime = (double)((end.QuadPart - begin.QuadPart) * 1000.0) / (double)fre.QuadPart;
	cout << "elimination time: " << gettime << " ms" << endl << endl;

	for (int i = 0; i < elimi2; i++) {
		for (int j = 0; j <= col; j++)
			cout << bit1[i].Find(j);
		cout << endl << endl;
	}
}
