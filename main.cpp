﻿#define DEBUG
#include<iostream>
#include"vector.h"
#include"timer.h"
#include"mempool.h"
#include"construct.h"
#include"mutex.h"
#include<Windows.h>
#include<thread>
#include"deque.h"
using namespace std;
using namespace TA;
void mempooltest() {
	mempool pool;
	int** x;
	int t = 0;
	x = new int* [10000];
	timer timer;
	for (int m = 0; m < 10; m++) {
		timer.start();
		for (int j = 1; j < 200; j++) {
			for (int i = 0; i < 10000; i++) {
				//cout << i<<" ";
				x[i] = (int*)pool.allocate(sizeof(int));
			}
			for (int i = 0; i < 10000; i++) {
				pool.deallocate(x[i], sizeof(int));
			}
		}

		timer.stop();
		cout << timer.get() << endl;
		t += timer.get();
	}
	cout << endl << t / 10;

	/*测试memcpy*/
	cout << endl << "test memcpy:" << endl;
	int* a = (int*)pool.allocate(sizeof(int) * 10);
	int* b=(int*)pool.allocate(sizeof(int)*10);
	for (int i = 0; i < 10; i++) {
		a[i] = i;
	}
	for (int i = 0; i < 10; i++) {
		cout << a[i] << endl;
	}
	pool.memcpy((char*)a, (char*)b, 10*sizeof(int));
	for (int i = 0; i < 10; i++) {
		cout << b[i] << endl;
	}
	cout <<  "test memcpy end" << endl;

	/*测试大型allo*/
	char* w = (char*)pool.allocate(10000);
	pool.deallocate(w,10000);
}

void constructtest() {
	int* a=new int[10];
	for (int i = 0; i < 10; i++) {
		construct(&a[i],i);
		cout << a[i]<<endl;
	}
	int* b = new int[10];
}

void vectortest() {
	vector<int > v;
	for (int i = 0; i < 100; i++) {
		v.push(i);
	}
	for (int i = 0; i < 100; i++) {
		cout<<v[i]<<endl;
	}
}

mutex lk;
bool volatile start=false;
int v = 0;
void func(int id) {
	while (!start) {}
	for (int i = 0; i < 5; i++) {
		lk.lock();
		++v;
		cout << id <<" -> "<< v<< endl;
		lk.unlock();
		Sleep(1000);
	}
}
void locktest() {
	thread t[1000];
	for (int i = 0; i <5; i++) {
		t[i]=thread(func, i);
	}
	for (int i = 0; i < 5; i++) {
		t[i].detach();
	}

	Sleep(1000);
	start = true;

	Sleep(10000);

	cout << v;
}

void dequetest() {
	deque<int> d;
	for (int i = 0; i < 10;i++) {
		d.push_back(i);
	}
	for (int i = 0; i < 10; i++) {
		d.push_front(i);
	}
	for (int i = 0; i < 18; i++) {
		cout << d.pop_back()<<endl;
	}
}
int main() {
	//mempooltest();
	//constructtest();
	//vectortest();
	//locktest();
	dequetest();
	return 0;
}