#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
struct pointer
{
    int start;
    int end;
};

int power(int base, int exponent)
{
    int result = 1;
    for (exponent; exponent > 0; exponent--)
    {
        result = result * base;
    }
    return result;
}
int isBetween(int number, int start, int end)
{
    if (number > start && number <= end)
        return 1;
    return 0;
}

int getMax(int points[], int pointSize)
{
    int i;
    int max = points[0];
    for (i = 1; i < pointSize; i++)
    {
        if (points[i] > max)
        {
            max = points[i];
        }
    }
    return max;
}
int counter[1000];
int main(int argc, char *argv[])

{
    int root = 0;
    int i, j;
    int my_rank; // rank of process    /
    int p;       // number of process    */
    int index = 0, x, count = 0;
    int number, sizee;
    int portionSize, *portionArr;
    int *sums;
    FILE *file;
    int *array;
    file = fopen("/shared/dataset.txt", "r");
    int localCounter[1000];
    int numBars;
    int start1[1000], end1[1000];

    MPI_Status status; // return status for recieve

    // Start up MPI
    MPI_Init(&argc, &argv);

    // Find out process rank /
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    // numBars = 2;
    struct pointer *points;
    // numBars = 2;

    if (my_rank == 0)
    {

        scanf("%d", &numBars);

        scanf("%d", &sizee);


        array = malloc(sizee * sizeof(int));
        int ir = 0;

        index = fscanf(file, "%d", &x);

        while (index != EOF) // taking the data points from the file and assigning it to the array
        {
            array[count++] = x;
            index = fscanf(file, "%d", &x);
        }

        fclose(file);

        int Range = getMax(array, sizee) / numBars;

        int start = 0, end = Range;

        points = malloc(numBars * sizeof(*points));
        for (i = 0; i < numBars; i++)
        {
            points[i].start = start;
            points[i].end = end;
            start1[i] = points[i].start;
            end1[i] = points[i].end;
            start += Range;
            end += Range;
        }

        portionSize = sizee / (p);
        int remSize = sizee - p * portionSize;
#pragma omp parallel for schedule(static) private(j, i)
        for (i = 0; i < remSize; i++)
        {
            for (j = 0; j < numBars; j++)
            {
                if (isBetween(array[i + (p * portionSize)], points[j].start, points[j].end) == 1)
                {
                    localCounter[j]++;
                }
            }
        }

        for (i = 0; i < p - 1; i++)
        {
            MPI_Send(&numBars, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&sizee, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(counter, numBars, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(localCounter, numBars, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(start1, numBars, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(end1, numBars, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&numBars, 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&sizee, 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(counter, numBars, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(localCounter, numBars, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(start1, numBars, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(end1, numBars, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
    }

    MPI_Bcast(&portionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    portionArr = malloc(portionSize * sizeof(int));
    MPI_Scatter(array, portionSize, MPI_INT, portionArr, portionSize, MPI_INT, 0, MPI_COMM_WORLD);

#pragma omp parallel for schedule(static) private(j, i)
    for (i = 0; i < portionSize; i++)
    {
        for (j = 0; j < numBars; j++)
        {
            if (isBetween(portionArr[i], start1[j], end1[j]) == 1)
            {

                localCounter[j]++;
            }
        }
    }

    sums = malloc(p * sizeof(int));

    for (j = 0; j < numBars; j++)
    {
        MPI_Allgather(&localCounter[j], 1, MPI_INT, sums, 1, MPI_INT, MPI_COMM_WORLD);
        for (i = 0; i < p; i++)
        {
            //#pragma omp critical
            counter[j] += sums[i];
        }
    }

    if (my_rank == 0)
    {

        for (i = 0; i < numBars; i++)
        {
            printf(" The range start with %d, end with %d , with count %d \n ", points[i].start, points[i].end, counter[i]);
        }
    }
    MPI_Finalize();

    return 0;
}
