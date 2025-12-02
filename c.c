// Linux practice code

// unit 6 
// code 1
#include <stdio.h>          // printf, scanf
#include <pthread.h>        // pthread_create, pthread_join
#include <stdlib.h>         // malloc, free

int arr[100], n;            // global array and its size accessible to thread

void* avg(void* a){         // thread function: returns void*
    int s = 0;              // accumulator for sum
    for(int i = 0; i < n; i++) s += arr[i]; // sum all elements
    double *x = malloc(sizeof(double));     // allocate space to return value
    *x = (n>0) ? (double)s / n : 0.0;       // compute average (safe against n==0)
    return x;               // return pointer (interpreted by pthread_join)
}

int main(){
    if(scanf("%d", &n) != 1) return 1;      // read n, exit on bad input
    for(int i = 0; i < n; i++) scanf("%d", &arr[i]); // read array elements

    pthread_t t;            // thread handle
    void *res;              // will hold returned pointer from thread

    pthread_create(&t, NULL, avg, NULL); // create thread running avg()
    pthread_join(t, &res);               // wait until thread finishes and get result

    printf("Average = %.2f\n", *(double*)res); // print dereferenced returned value
    free(res);              // free memory allocated inside thread
    return 0;
}

// code 2
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int arr[100], n;            // global array and size

void* maxi(void* a){
    int *m = malloc(sizeof(int)); // allocate return space
    if(n == 0){ *m = 0; return m; } // guard for empty array
    *m = arr[0];            // init max with first element
    for(int i = 1; i < n; i++) if(arr[i] > *m) *m = arr[i]; // find max
    return m;               // return pointer to max
}

int main(){
    if(scanf("%d", &n) != 1) return 1;
    for(int i = 0; i < n; i++) scanf("%d", &arr[i]);

    pthread_t t;
    void *res;
    pthread_create(&t, NULL, maxi, NULL); // start thread
    pthread_join(t, &res);                // wait & collect result

    printf("Max = %d\n", *(int*)res);     // print value
    free(res);                             // free returned memory
    return 0;
}

// code 3 
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int arr[100], n;

void* mini(void* a){
    int *m = malloc(sizeof(int));
    if(n == 0){ *m = 0; return m; } // handle empty array
    *m = arr[0];            // init min
    for(int i = 1; i < n; i++) if(arr[i] < *m) *m = arr[i]; // find min
    return m;
}

int main(){
    if(scanf("%d", &n) != 1) return 1;
    for(int i = 0; i < n; i++) scanf("%d", &arr[i]);

    pthread_t t;
    void *res;
    pthread_create(&t, NULL, mini, NULL);
    pthread_join(t, &res);

    printf("Min = %d\n", *(int*)res);
    free(res);
    return 0;
}


// exp 7 
// code 1
#include <stdio.h>           // printf
#include <pthread.h>         // pthreads
#include <semaphore.h>       // sem_t, sem_init, sem_wait, sem_post

int buf;                    // single-slot buffer shared by P and C
sem_t e, f;                 // e = empty count, f = full count

void* P(){
    for(int i = 1; i <= 5; i++){
        sem_wait(&e);       // wait until there is an empty slot
        buf = i;            // produce: write into buffer
        printf("P %d\n", i);// indicate produced value
        sem_post(&f);       // signal that buffer is now full
    }
    return NULL;
}

void* C(){
    for(int i = 1; i <= 5; i++){
        sem_wait(&f);       // wait until buffer is full
        printf("C %d\n", buf);// consume: read from buffer
        sem_post(&e);       // signal that buffer is empty again
    }
    return NULL;
}

int main(){
    pthread_t p, c;        // thread IDs for producer and consumer
    sem_init(&e, 0, 1);    // initialize empty semaphore to 1 (buffer free)
    sem_init(&f, 0, 0);    // initialize full semaphore to 0 (no data yet)
    pthread_create(&p, NULL, P, NULL); // start producer thread
    pthread_create(&c, NULL, C, NULL); // start consumer thread
    pthread_join(p, NULL); // wait producer finish
    pthread_join(c, NULL); // wait consumer finish
    return 0;
}


// code 2
#include <stdio.h>
#include <pthread.h>

int rc = 0, data = 0;                             // rc = reader count, data = shared resource
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;    // mutex to protect rc
pthread_mutex_t w = PTHREAD_MUTEX_INITIALIZER;    // writer mutex: exclusive access for writer

void* R(){
    pthread_mutex_lock(&m);   // lock to safely update rc
    rc++;                     
    if(rc == 1) pthread_mutex_lock(&w); // first reader locks writer to block writers
    pthread_mutex_unlock(&m); // done updating rc

    printf("R %d\n", data);   // read the shared data (critical section for readers)
    // reading assumed atomic/independent so no data modification here

    pthread_mutex_lock(&m);   // lock to safely decrement rc
    rc--;
    if(rc == 0) pthread_mutex_unlock(&w); // last reader unlocks writer
    pthread_mutex_unlock(&m); // release mutex protecting rc
    return NULL;
}

void* W(){
    pthread_mutex_lock(&w);   // obtain exclusive access (blocks readers and writers)
    data++;                   // write to shared data
    printf("W %d\n", data);   // indicate write
    pthread_mutex_unlock(&w); // release exclusive lock
    return NULL;
}

int main(){
    pthread_t r1, r2, w1;
    pthread_create(&r1, NULL, R, NULL); // start reader1
    pthread_create(&r2, NULL, R, NULL); // start reader2
    pthread_create(&w1, NULL, W, NULL); // start writer
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w1, NULL);
    return 0;
}
