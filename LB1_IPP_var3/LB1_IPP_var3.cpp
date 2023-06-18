#include <windows.h>
#include <iostream>
#include <random>
#include <thread>
#include <processthreadsapi.h>
#define BYTE 1048576
#define THREADS_AMOUNT 2

typedef struct data {
	int maxSize;
	int elements;
	int* buff;
	bool full; //прапорець для перевірки на те, чи досягнуто ліміту
} DATA, * PDATA;


DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	PDATA pArr = (PDATA)lpParam;
	while (true)
	{
		if (!pArr->full)
		{
			if ((pArr->elements + 131072) * sizeof(int) <= pArr -> maxSize) {
				pArr->elements += 131072;
				if (pArr->buff != NULL)
				{
					delete pArr->buff;
				}
				pArr->buff = new int[pArr->elements];
				std::cout << "thread_id(" << std::this_thread::get_id() <<
					") " << (float)pArr->elements * sizeof(int) / BYTE << "/" << (float)pArr -> maxSize / BYTE << " mb" << std::endl;
			}
			else
			{
				pArr->full = true;
			}
		}
		Sleep(250);
	}
	return 0;
}
int main()
{
	PDATA pArr[THREADS_AMOUNT];
	DWORD dwThreadIdArray[THREADS_AMOUNT];
	HANDLE hThreadArray[THREADS_AMOUNT];
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(BYTE, BYTE*10);
	for (int i = 0; i < THREADS_AMOUNT; i++)
	{
		pArr[i] = (PDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(DATA));
		if (pArr[i] == NULL)
		{
			ExitProcess(2);
		}
		pArr[i]->maxSize = dist(rng);
		pArr[i]->full = false;
		pArr[i]->elements = 0;
		hThreadArray[i] = CreateThread(
			NULL,
			0,
			ThreadFunction,
			pArr[i],
			0,
			&dwThreadIdArray[i]);
		if (hThreadArray[i] == NULL)
		{
			ExitProcess(3);
		}
	}while (true)
	{
		for (int i = 0; i < THREADS_AMOUNT; i++)
		{
			if (hThreadArray[i] != NULL && pArr[i]->full)
			{
				std::cout << "thread_id(" << dwThreadIdArray[i] << ") done" << std::endl;
					TerminateThread(hThreadArray[i], NULL);
				CloseHandle(hThreadArray[i]);
				if (pArr[i] != NULL)
				{
					HeapFree(GetProcessHeap(), 0, pArr[i]);
					hThreadArray[i] = NULL;
					dwThreadIdArray[i] = NULL;
				}
			}
		}
	}
	return 0;
}