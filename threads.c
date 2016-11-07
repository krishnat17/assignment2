#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int number_of_servers = 2;
int number_of_clients = 2;
int total_clock_ticks = 1000;

double muX = 0.01;
double *mu = &muX;
double lambdaX = 0.05;
double *lambda = &lambdaX;

int globalCurrentTick;
int globalPrevTick;


pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER;


struct job {
  struct job *next;
}*front, *rear, *temp, *front1;

void create_queue() {
  front = rear = NULL;
}

void enq() {
  if (rear == NULL) {
    rear = (struct job *) malloc(1*sizeof(struct job));
    rear -> next = NULL;
    front = rear;
  }
  else {
    temp = (struct job *) malloc(1*sizeof(struct job));
    rear -> next = temp;
    temp -> next = NULL;
    rear = temp;
  }
  printf("Job enq successful.");
}

void deq() {
  front1 = front;

  if (front1 == NULL) {
    printf("\n ERROR: Trying to deq an empty queue.");
    return;
  }
  else {
      if (front1 -> next != NULL) {
        front1 = front1 ->   next;
        printf("Job deq successful.");
        free(front);
        front = front1;
      }
      else {
        printf("Job deq successful.");
        free(front);
        front = rear = NULL;
      }
  }
}

int qIsEmpty() {
    if ((front == NULL) && (rear == NULL))
      return 1;
    else
      return 0;
}

void *myfunc(void *myvar) {
  char *msg;
  msg = (char*) myvar;

  int i;
  for (i=0;i<10;i++) {
    printf("%s %d\n", msg, i);
    sleep(1);
  }
  return 0;
}

void *server_sim(void *mu) {

  double *deathrate;
  deathrate = (double*) mu;

  int jobExecutingBool = 0;
  double tempRand;

  //Seed RNG in this thread
  srand(time(0));

  while (globalCurrentTick < total_clock_ticks) {
    //Check if currently working on job
    if (jobExecutingBool == 0) {
      if(qIsEmpty != 1) {
        pthread_mutex_lock(&server_mutex);
        deq();
        pthread_mutex_unlock(&server_mutex);
      }
    }
    else {
      tempRand = RNG();

      if(tempRand < deathrate){
        jobExecutingBool = 0;
      }

    }

  }

  return 0;
}

void *client_sim(void *lambda) {

  double *birthrate;
  birthrate = (double*) lambda;

  double tempRand;

  //Seed RNG in this thread
  srand(time(0));

  while (globalCurrentTick < total_clock_ticks) {

    tempRand = RNG();

    if(tempRand < birthrate) {
      pthread_mutex_lock(&client_mutex);
      enq();
      pthread_mutex_unlock(&client_mutex);
    }

  }

}

void *clock_sim(void *unused) {

  globalCurrentTick = 0;
  globalPrevTick = 0;

  while(globalCurrentTick < total_clock_ticks) {
    globalCurrentTick++;

  }

  return 0;
}

double RNG() {
  return (double)rand() / (double)RAND_MAX;
}

int main(int argc, char *argv[]) {

  //Create global job queue
  create_queue();

  //Spawn needed threads
  pthread_t server_thread1, server_thread2;
  pthread_t client_thread1, client_thread2;
  pthread_t clock_thread;


  pthread_create(&server_thread1, NULL, server_sim, (void*) mu);
  pthread_create(&server_thread2, NULL, server_sim, (void*) mu);

  pthread_create(&client_thread1, NULL, client_sim, (void*) lambda);
  pthread_create(&client_thread2, NULL, client_sim, (void*) lambda);

  pthread_create(&clock_thread, NULL, clock_sim, NULL);




  //Begin test section

  pthread_t thread1, thread2;
  char* msg1 = "First thread";
  char* msg2 = "Second thread";

  int ret1, ret2;

  ret1 = pthread_create(&thread1, NULL, myfunc, (void*) msg1);
  ret2 = pthread_create(&thread2, NULL, myfunc, (void*) msg2);

  printf("Main function after pthread_create\n");

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);

  printf("Main function after pthread_join\n");

  printf("First thread ret1 = %d\n", ret1);
  printf("Second thread ret2 = %d\n", ret2);

  //End test section

  return 0;
}
