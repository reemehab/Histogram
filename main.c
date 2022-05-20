#include <stdio.h>
#include <stdlib.h>
struct pointer
{
    int start ;
    int end;
};
int isBetween(int number, int start, int end)
{
    if(number >= start && number <end)
        return 1;
    return 0;
}
int main()
{
    FILE *ptr;
    ptr = fopen("D:\\codeblocks\\hpc3\\dataset.txt", "r");
    int numBars;
    printf("Enter number of Bars : \n");
    scanf("%d", &numBars);

    struct pointer points[numBars];
    int start, end;
    printf("Please Enter %d points in each line :", numBars);
    for(int i=0 ; i<numBars ; i++)
    {
        scanf("%d %d", &start, &end);
        points[i].start = start;
        points[i].end = end;
    }
    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }
    int number, sizee;
    printf("please enter size of array \n");
    scanf("%d", &sizee);
    int array[sizee], index, x, count =0;
    index = fscanf(ptr, "%d", &x);
    while (index != EOF) //taking the data points from the file and assigning it to the array
    {
        array[count++] = x;
        index = fscanf(ptr, "%d", &x );
    }
    int counter[numBars];
    for(int i=0 ; i< numBars ; i++)
        counter[i]=0;
    for(int i=0 ; i < sizee ; i++ )
    {
        for(int j =0 ; j < numBars ; j++ )
        {
            if(isBetween(array[i], points[j].start , points[j].end) == 1)
                counter[j]++;
        }
    }
    for(int i=0 ; i < numBars ; i++)
        printf("%d \n" , counter[i]);

    fclose(ptr);

    return 0;
}
