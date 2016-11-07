#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int number_of_servers = 2;
int number_of_clients = 2;
int totalTicks = 1;

double muX = 0.01;
double *mu = &muX;
double lambdaX = 0.005;
double *lambda = &lambdaX;

int currentTick;


pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cond_tick_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waiting_for_tick_cond = PTHREAD_COND_INITIALIZER;


double RNG() {
  return (double)rand() / (double)RAND_MAX;
}

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
}

void deq() {
  front1 = front;

  if (front1 == NULL) {
    printf("ERROR: Trying to deq an empty queue.\n");
    return;
  }
  else {
      if (front1 -> next != NULL) {
        front1 = front1 ->   next;
        printf("Job deq successful.\n");
        free(front);
        front = front1;
      }
      else {
        printf("Job deq successful.\n");
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

void *server_sim(void *mu) {

  double* moo;
  moo = (double*) mu;

  int jobExecutingBool = 0;
  double deathrate = *moo;
  double tempRand;

  //Seed RNG in this thread
  srand(time(0));

  while (currentTick < totalTicks) {

    //Check if currently working on job
    if (jobExecutingBool == 0) {

      if(qIsEmpty() == 0) {
        tempRand = RNG();
        printf("%f\n",tempRand);

        if(tempRand < deathrate){
          jobExecutingBool = 0;
          printf("Finished job this tick\n");
        }
        else {
          printf("Did not finish job this tick\n");
        }
      }
      else {
        printf("No job to work on this tick\n");
      }
    }
    else {
      pthread_mutex_lock(&server_mutex);
      deq();
      pthread_mutex_unlock(&server_mutex);
    }
    pthread_cond_wait(&waiting_for_tick_cond,&cond_tick_mutex);
  }
  pthread_exit(NULL);

}

void *client_sim(void *lambda) {

  double* lam;
  lam = (double*) lambda;

  double birthrate = *lam;
  double tempRand;

  //Seed RNG in this thread
  srand(time(0));

  while (currentTick < totalTicks) {
    tempRand = RNG();
    printf("%f\n",tempRand);

    if(tempRand < birthrate) {
      pthread_mutex_lock(&client_mutex);
      enq();
      pthread_mutex_unlock(&client_mutex);
      printf("Did create job this tick\n");
    }
    else {
      printf("Did not create job this tick\n");
    }
    pthread_cond_wait(&waiting_for_tick_cond,&cond_tick_mutex);
  }
  pthread_exit(NULL);
}

void *clock_sim(void *unused) {

  currentTick = 0;

  while(currentTick < totalTicks) {
    sleep(5);
    pthread_cond_broadcast(&waiting_for_tick_cond);
    currentTick++;
    printf("--------------Current tick is %d\n",currentTick);
  }
  pthread_exit(NULL);
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

  //Main waits for threads to finish
  pthread_join(server_thread1, NULL);
  pthread_join(server_thread2, NULL);
  pthread_join(client_thread1, NULL);
  pthread_join(client_thread2, NULL);
  pthread_join(clock_thread, NULL);

  printf("Back in main\n");

  return 0;
}
