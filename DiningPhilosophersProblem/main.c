#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define PHILOSOPHERS_COUNT 5
#define EATING_COUNT 2
#define QUEUE_CAPACITY 10000

pthread_mutex_t waiter;
pthread_mutex_t forks[PHILOSOPHERS_COUNT];
int phils[PHILOSOPHERS_COUNT];

struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}
 
struct Queue* veryHungryQueue;


int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}
 
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}
 
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)
                   % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}
 
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return -1;
    return queue->array[queue->front];
}
 
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return -1;
    return queue->array[queue->front];
}


int randInRange(int lower, int upper)
{
  return (rand() % (upper - lower + 1)) + lower;
}

void take_fork(int id, int phil){
    pthread_mutex_lock(&forks[id]);
    printf(MAG "Philosopher %d took %d fork\n" RESET, phil, id);
    sleep(1);
}

void leave_fork(int id, int phil){
    printf(CYN "Philosopher %d put down %d fork\n" RESET, phil, id);
    pthread_mutex_unlock(&forks[id]);
    sleep(1);
}

void give_forks(int left, int right, int phil){
    while (!(front(veryHungryQueue) == phil)) {
        sleep(0);
    }
    pthread_mutex_lock(&waiter);
    take_fork(left, phil);
    take_fork(right, phil);
    pthread_mutex_unlock(&waiter);
    dequeue(veryHungryQueue);
}


void leave_forks(int left, int right, int phil){
    leave_fork(left, phil);
    leave_fork(right, phil);
}

void* philosopher_run(void* arg){
    int* philosopher = (int*) arg;
    int right = (*philosopher + PHILOSOPHERS_COUNT - 1) % PHILOSOPHERS_COUNT;
    int left = (*philosopher) % PHILOSOPHERS_COUNT;
    
    int count = EATING_COUNT;
    while (count > 0) {
        printf(YEL "Philosopher %d is thinking...\n" RESET, *philosopher);
        sleep(randInRange(1,4));
        printf(RED "Philosopher %d is hungry\n" RESET, *philosopher);
        enqueue(veryHungryQueue, *philosopher);
        give_forks(left, right, *philosopher);
        printf(GRN "Philosopher %d started eating...\n" RESET, *philosopher);
        sleep(randInRange(2,5));
        printf(GRN "Philosopher %d finished eating.\n" RESET, *philosopher);
        leave_forks(left, right, *philosopher);
        count--;
    }
    printf(YEL "Philosopher %d is thinking...\n" RESET, *philosopher);
    return  NULL;
}

int main(void) {
    veryHungryQueue = createQueue(QUEUE_CAPACITY);
    
    pthread_mutex_init(&waiter, NULL);
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pthread_mutex_init(&forks[i], NULL);
        phils[i] = i;
    }
    pthread_t philosophers[PHILOSOPHERS_COUNT];
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pthread_create(&philosophers[i], NULL, philosopher_run, &phils[i]);
    }
    
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pthread_join(philosophers[i], NULL);
    }
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    return 0;
}
