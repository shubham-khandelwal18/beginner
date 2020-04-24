// reader-writer synchronization problem with reader priority
//gcc -o rw rw.c -lpthread    <-- in Linux
//gcc rw.c -o rw -lpthread -std=gnu90     <---in termux 
#include<stdio.h> 
#include<stdlib.h>
#include<pthread.h>
#include <semaphore.h>

#define N 10       // number of times a thread remains in CS

static int readern=0,writern=0;//number of readers , writers

sem_t noreader,mutexr,nowriter,mutexw; // binary semaphores
sem_t ncnt;//semaphore to change count value

void *reader(void *); /* prototype of reader routine */ 
void *writer(void *); /* prototype of writer routine */  
 
int main(int argc, char *argv[]) 
{  
    pthread_t p1,p2,p3[3];//five threads
    int i=0;
    // initialize the semaphores
    	sem_init(&noreader, 0, 1); // initialise  noreader=1,this shared only among threads of this process
    	sem_init(&mutexr, 0, 1); // initialise  mutexr=1
    	sem_init(&mutexw, 0, 1); // initialise  mutexw=1
    	sem_init(&nowriter, 0, 1); // initialise  nowriter=1
    	sem_init(&ncnt, 0, 1); // initialise  ncnt=1
    	srand( (long)time(NULL) );//random generator initialise

       /* create five threads representing  */ 
        pthread_create(p3+0, NULL, reader,NULL );   //reader
       if(pthread_create(&p1, NULL, writer,NULL) != 0){ printf("pthread_create() failure."); exit(1);  } // writer
  // for(i=0;i<3;i++)
         pthread_create(p3+1, NULL, reader,NULL );   //reader
         pthread_create(&p2, NULL, writer,NULL ) ;   // writer
	 pthread_create(p3+2, NULL, reader,NULL );   //reader

   /* wait for the join of 2 threads */ 
         reader( &i);//this function my main thread
         if(!pthread_join(p1, NULL )==0) { printf("thr_join() failure.");exit(1);} 
         pthread_join(p2, NULL ); 
         for(i=0;i<3;i++)  
         	pthread_join(p3[i], NULL );
    
    	printf("\nAll threads terminated\n");
    	return (0); 
} 
 

void *reader(void  *arg) 
{  char flag=0;
 int count=0;
  
   while(1)
    { 
    //thread exits when flag is set
       if(flag){printf("\n\t reader %u ==== count %d EXIT  \n",pthread_self(),count);sem_post(&ncnt);pthread_exit(NULL);}

       sem_wait(&noreader);
       sem_wait(&mutexr);// lock
       readern +=1;
     
       if(readern==1){sem_wait(&nowriter); }// block writer
       sem_post(&mutexr);//unlock
       sem_post(&noreader);//unlock

       // critical section start
       sem_wait(&ncnt);//lock
       if(count >3)flag=1;
       else count++;
       sem_post(&ncnt);//lock

       printf("\n reader %u in CS reading count %d  Number of reades in CS %d  ",pthread_self(),count,readern);
       usleep(rand() % 20000000); /*block time */ 
     // critical section end
       sem_wait(&mutexr);// lock
       readern -=1;printf("\n reader %u leaves CS,Number of readers in CS %d ",pthread_self(),readern);
      
       if(readern==0){sem_post(&nowriter); }// allow to writer enter
       sem_post(&mutexr);//unlock 
       
       usleep(rand() % 40000000); /* sleep time */ 
       sched_yield();
    }
   
} 


void *writer(void  *arg) 
{  char flag=0;
 int count=0;//
    while(1)
    { 
     //thread exits when flag is set
       if(flag){printf("\n\t\t writer  %u === count %d EXIT.\n  ",pthread_self(),count);sem_post(&ncnt);pthread_exit(NULL);}

       sem_wait(&mutexw);// lock 
       writern +=1;
      
       if(writern==1){sem_wait(&noreader);}//reader cann't enter
       sem_post(&mutexw);//unlock
       sem_wait(&nowriter);//enter if no writer

       // critical start
       sem_wait(&ncnt);//lock
       if(count >3)flag=1;
       else count++;
       sem_post(&ncnt);//lock
       
        printf("\n\t\t writer %u in CS writing count=%d  \n",pthread_self(),count);
        usleep(rand() % 10000000); /*block time */ 
      // critical end
        sem_post(&nowriter);//unlock to other

       sem_wait(&mutexw);// lock 
       writern -=1;printf("\n\t\t writer  %u leaves CS",pthread_self());
       
       if(writern==0){sem_post(&noreader);}//reader can enter
       sem_post(&mutexw);//unlock
       usleep(rand() % 50000000); /*block time */      
       sched_yield();
      
    }
 } 

