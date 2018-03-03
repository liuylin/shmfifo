/*************************************************************************
	> File Name: shmfifo.c
	> Author: 
	> Mail: 
	> Created Time: Fri 05 Jan 2018 07:45:48 PM PST
 ************************************************************************/

#include"shmfifo.h"
#include<assert.h>

union semun
{
    int val;
};
//blksz:size of one bllock
//blks:number of block
shmfifo_t *shmfifo_init(int key,int blksz,int blks)
{
    shmfifo_t *fifo = (shmfifo_t*)malloc(sizeof(shmfifo_t));
    assert(fifo);
    int shmid;
    int size = sizeof(shmhead_t)+blks*blksz;
    shmid = shmget(key,0,0);
    if(shmid == -1) //shibai   chuangjian
    {
        shmid = shmget(key,size,IPC_CREAT|0644);
        fifo->shmid = shmid;
        if(fifo->shmid == -1)
        {
            perror("shm creat");
            exit(1);
        }
        
      fifo->p_shm = (shmhead_t*)shmat(fifo->shmid,NULL,0);
      fifo->p_payload =(char*) (fifo->p_shm+1);
        fifo->p_shm->blocks = blks;
        fifo->p_shm->blksize = blksz;
        fifo->p_shm->rd_index = 0;
        fifo->p_shm->wr_index = 0;
        fifo->sem_full = semget(key,1,IPC_CREAT|0644);
        fifo->sem_empty = semget(key+1,1,IPC_CREAT|0644);
        fifo->sem_mutex = semget(key+2,1,IPC_CREAT|0644);
        
        union semun su;
        su.val = blks;
        //keshengchandegeshu
        semctl(fifo->sem_full,0,SETVAL,su);
        su.val = 0;
        //shifouweikong
        semctl(fifo->sem_empty,0,SETVAL,su);
        su.val = 1;
        //huchiyongdexinhaoaliang
        semctl(fifo->sem_mutex,0,SETVAL,su);
    }
    else
    {
        fifo->shmid = shmid;
        fifo->p_shm = (shmhead_t*)shmat(shmid,NULL,0);
        fifo->p_payload = (char*)(fifo->p_shm+1);

        fifo->sem_full = semget(key,0,0);
        fifo->sem_empty = semget(key+1,0,0);
        fifo->sem_mutex = semget(key+2,0,0);
    }
    return fifo;
}
void shmfifo_put(shmfifo_t *fifo,const void* buf)
{
    memcpy(fifo->p_payload+fifo->p_shm->blksize*fifo->p_shm->wr_index,buf,fifo->p_shm->blksize);
    fifo->p_shm->wr_index = (fifo->p_shm->wr_index+1) % fifo->p_shm->blocks;
}

void shmfifo_get(shmfifo_t *fifo,void *buf)
{
    memcpy(buf,fifo->p_payload+fifo->p_shm->blksize*fifo->p_shm->rd_index,fifo->p_shm->blksize);
    
    fifo->p_shm->rd_index = (fifo->p_shm->rd_index+1) % fifo->p_shm->blocks;
}

void shmfifo_destroy(shmfifo_t *fifo)
{
    semctl(fifo->sem_empty,0,IPC_RMID,0);
    semctl(fifo->sem_full,0,IPC_RMID,0);
    semctl(fifo->sem_mutex,0,IPC_RMID,0);
    shmdt(fifo->p_shm);
    shmctl(fifo->shmid,IPC_RMID,0);
    free(fifo);
}
