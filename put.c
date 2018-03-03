/*************************************************************************
	> File Name: put.c
	> Author: 
	> Mail: 
	> Created Time: Fri 05 Jan 2018 08:26:20 PM PST
 ************************************************************************/

#include"shmfifo.h"

typedef struct food
{
    int id;
    char name[20];
}food_t;

int main()
{
   shmfifo_t *fifo = shmfifo_init(1234,sizeof(food_t),3) ;
    food_t f;
    int i;
    for(i = 0;i<10;i++)
    {
        f.id = i+1;
        sprintf(f.name,"food %d",i+1);
        shmfifo_put(fifo,&f);
        printf("write %d\n",i+1);

    }
}


