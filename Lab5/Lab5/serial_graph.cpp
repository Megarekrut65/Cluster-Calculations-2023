#include "serial_graph.h"

void serial_floyd(int* pMatrix, int Size) {
	int t1, t2;
	for (int k = 0; k < Size; k++)
		for (int i = 0; i < Size; i++)
			for (int j = 0; j < Size; j++)
				if ((pMatrix[i * Size + k] != -1) &&
					(pMatrix[k * Size + j] != -1)) {
					t1 = pMatrix[i * Size + j];
					t2 = pMatrix[i * Size + k] + pMatrix[k * Size + j];
					pMatrix[i * Size + j] = Min(t1, t2);
				}
}