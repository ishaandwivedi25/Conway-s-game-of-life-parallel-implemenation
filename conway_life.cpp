#include <iostream>
#include <mpi.h>
using namespace std;

int neighbourCount(int* grid, int i, int j, const int width, const int height)
{
	int count = 0;
	if (i != 0)
	{
		if (grid[(i - 1) * width + j] == 1)
			count++;

		if (j != width - 1)
		{
			if (grid[(i - 1) * width + (j + 1)] == 1)
				count++;
		}

		if (j != 0)
		{
			if (grid[(i - 1) * width + (j - 1)] == 1)
				count++;

			
		}
	}

	if (j != 0)
	{
		if (grid[i * width + (j - 1)] == 1)
			count++;
	}

	if (j != width-1)
	{
		if (grid[i * width + (j + 1)] == 1)
			count++;
	}

	if (i != height - 1)
	{
		if (grid[(i + 1) * width + j] == 1)
			count++;

		if (j != width - 1)
		{
			if (grid[(i + 1) * width + (j + 1)] == 1)
				count++;

			
		}

		if (j != 0)
		{
			if (grid[(i + 1) * width + (j - 1)] == 1)
				count++;
		}
	}

	return count;
}


void printGrid(int *grid, int st, int end, const int width, const int height)
{
	for (int i = st; i <= end; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			printf("%d ", grid[i * width + j]);
		}
		printf("\n");
	}
}

int main()
{
	int rank, nprocs;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


	int width = 10;
	int height = 10;
	int iterations = 100;

	int* grid = new int[width * height];
	int* nextGrid = new int[width * height];

	int k = 0;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			if (k % (width/3) == 0)
				grid[i * width + j] = 1;
			else
				grid[i * width + j] = 0;
			k++;
		}
	}

	if (rank == 0)
	{
		printf("Initial grid\n");
		printGrid(grid, 0, width - 1, width, height);
	}



	int heightPerProc = height / nprocs;

	int myStartRow = rank * heightPerProc;
	int myEndRow = myStartRow + heightPerProc - 1;

	if (rank == nprocs - 1)
	{
		myEndRow = height - 1;
	}

	for (int it = 0; it < iterations; ++it)
	{
		for (int j = myStartRow; j <= myEndRow; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				int neigh_count = neighbourCount(grid, i, j, width, height);

				if (grid[i * width + j] == 1 && neigh_count < 2)
				{
					nextGrid[i * width + j] = 0;
				}
				else if (grid[i * width + j] == 1 && neigh_count > 3)
				{
					nextGrid[i * width + j] = 0;
				}

				else if (grid[i * width + j] == 1 && neigh_count == 3)
				{
					nextGrid[i * width + j] = 1;
				}
				else
				{
					nextGrid[i * width + j] = grid[i * width + j];
				}
			}
		}

		int* temp = grid;
		grid = nextGrid;
		nextGrid = temp;


		if (rank % 2 == 0)
		{
			if (rank != nprocs - 1)
			{
				MPI_Send(&grid[myEndRow * width], width, MPI_CHAR, rank + 1, 100, MPI_COMM_WORLD);
			}

			if (rank != 0)
			{
				MPI_Status status;
				MPI_Recv(&grid[(myStartRow - 1) * width], width, MPI_INT, rank - 1, 100, MPI_COMM_WORLD, &status);
			}
		}
		else
		{
			if (rank != 0)
			{
				MPI_Status status;
				MPI_Recv(&grid[(myStartRow - 1) * width], width, MPI_INT, rank - 1, 100, MPI_COMM_WORLD, &status);

			}

			if (rank != nprocs - 1)
			{
				MPI_Send(&grid[myEndRow * width], width, MPI_CHAR, rank + 1, 100, MPI_COMM_WORLD);
			}
		}
	}


	printf("Final grid %d---%d\n", myStartRow, myEndRow);
	printGrid(grid, myStartRow, myEndRow, width, height);

	delete[] grid;
	delete[] nextGrid;

	MPI_Finalize();

	return 0;
}
