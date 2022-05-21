#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
struct pointer
{
    int start;
    int end;
};
int isBetween(int number, int start, int end)
{
    if (number >= start && number < end)
        return 1;
    return 0;
}
int main(int argc, char *argv[])

{
    int i, j;
    int my_rank; // rank of process    /
    int p;       // number of process    */
    int index, x, count = 0;
    int number, sizee;
    int portionSize, *portionArr;
    int *sums;
    FILE *ptr;
    int *array;
    ptr = fopen("/shared/dataset.txt", "r");
    int numBars;
    // scanf("%d", &numBars);
    numBars = 2;
    int *counter = malloc(numBars * sizeof(int)), localCounter[numBars];
    struct pointer points[numBars];
    int start, end;
    //  printf("Please Enter %d points in each line :", numBars);
    /*for( i=0 ; i<numBars ; i++)
    {
        scanf("%d %d", &start, &end);
        points[i].start = start;
        points[i].end = end;
    }*/
    points[0].start = 0;
    points[0].end = 15;
    points[1].start = 15;
    points[1].end = 30;
    // printf("please enter size of array \n");
    // scanf("%d", &sizee);
    sizee = 20;

    for (i = 0; i < numBars; i++)
    {
        counter[i] = 0;
        localCounter[i] = 0;
    }

    MPI_Status status; // return status for recieve

    // Start up MPI
    MPI_Init(&argc, &argv);

    // Find out process rank /
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    if (my_rank == 0)
    {

        if (NULL == ptr)
        {
            printf("file can't be opened \n");
        }
        array = malloc(sizee * sizeof(int));
        index = fscanf(ptr, "%d", &x);
        while (index != EOF) // taking the data points from the file and assigning it to the array
        {
            array[count++] = x;
            index = fscanf(ptr, "%d", &x);
        }

        portionSize = sizee / (p);
        int remSize = 20 - p * portionSize;
        i = 0;
        for (; i < remSize; i++)
        {
            for (j = 0; j < numBars; j++)
            {
                if (isBetween(array[i + (p * portionSize)], points[j].start, points[j].end) == 1)
                {
                    localCounter[j]++;
                }
            }
        }

    }
    MPI_Bcast(&portionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    portionArr = malloc(portionSize * sizeof(int));
    /* for(; i<sizee; i++){
      printf("my Rank = %d , Arr= %d \n" , my_rank ,portionArr);

  }*/
    MPI_Scatter(array, portionSize, MPI_INT, portionArr, portionSize, MPI_INT, 0, MPI_COMM_WORLD);
    i = 0;
    for (; i < portionSize; i++)
    {
        for (j = 0; j < numBars; j++)
        {
            if (isBetween(portionArr[i], points[j].start, points[j].end) == 1)
            {
                localCounter[j]++;
                // printf("my Rank = %d , partionArr= %d \n" , my_rank ,portionArr[i]);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    sums = malloc(p * sizeof(int));
    j = 0;
    for (; j < numBars; j++)
    {
        MPI_Allgather(&localCounter[j], 1, MPI_INT, sums, 1, MPI_INT, MPI_COMM_WORLD);
        for (i = 0; i < p; i++)
        {
            counter[j] += sums[i];
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank == 0)
    {
        i = 0;
        for (; i < numBars; i++)
        {
            printf(" The range start with %d, end with %d , with count %d \n ", points[i].start, points[i].end, counter[i]);
        }
    }
    MPI_Finalize();
    fclose(ptr);

    return 0;
}