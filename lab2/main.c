#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_args_s {
    int id;
    int type; // 1 = producer; 2 = consumer
    unsigned long lo;
    unsigned long hi;
    int num_primes;
    int chunkid_max;
} thread_args_t;

int is_prime(const long num) {
    if (num < 2) { return 0; }
    if (num == 2) { return 1; }
    if ((num % 2) == 0) { return 0; }
    for (long i = 3; i < num; i += 2) {
        if ((num % i) == 0) { return 0; }
    }
    return 1;
}

typedef struct queue_s {
    long num;
    struct queue_s *next;
} queue_t;

// static queue_t *head = NULL;
// The thread id = index to array
static queue_t **heads = NULL;
static int next_chunkid = 1;


void queue_add(const long num, int chunkid) {
    queue_t *node = malloc(sizeof(queue_t));
    pthread_mutex_lock(&mutex);
    node->next = heads[chunkid];
    node->num = num;
    heads[chunkid] = node;
    pthread_mutex_unlock(&mutex);
}

int get_nextChunkid(int max) {
    int ret;
    pthread_mutex_lock(&mutex);
    int pre_chunkid = next_chunkid;

    while (1) { 
        ret = next_chunkid;
        next_chunkid++;
        if (next_chunkid  > max) {
            next_chunkid=1; 
        }
        if (heads[ret] != NULL) {
            break;
        }
        if (pre_chunkid == next_chunkid) {
            // all queues are empty
            ret = -1;
            break;
        }

    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

queue_t* get_next(int chunkid) {
    pthread_mutex_lock(&mutex);

    queue_t *ret = heads[chunkid];

    if (heads[chunkid]) {
        heads[chunkid] = heads[chunkid]->next;
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

 void* producer_thread(void* _args) {
    thread_args_t *args = _args;
    printf("thread id %d\n", args->id);
    printf("thread type %d\n", args->type);
    printf("thread lo %lu\n", args->lo);
    printf("thread hi %lu\n", args->hi);
    for (long i = args->lo; i <= args->hi; i++) {
        queue_add(i,0);
    }
    return (void*)NULL;
}

void *consumer_thread(void* _args) {
    queue_t *node;
    thread_args_t *args = _args;

    printf("thread id %d\n", args->id);
    printf("thread type %d\n", args->type);
    printf("thread sum %d\n", args->num_primes);
    printf("thread lo %lu\n", args->lo);
    printf("thread hi %lu\n", args->hi);
    while (1) {

       int chunkid = get_nextChunkid(args->chunkid_max);
       if (chunkid < 0)  break; //No more data left in any queue
       while( (node = get_next(chunkid))) {
            if (is_prime(node->num)) {
                printf("%ld ", node->num);
                args->num_primes++;
                printf("num primes %d\n", args->num_primes);
            }
            free(node);
       }
    }

    return (void*)NULL;
}

long find_number_of_prime_range(const int num_producer, const int num_consumers, const unsigned long hi, int chunk)  {
    long sum = 0;
    pthread_t *threads = malloc(sizeof(pthread_t) * num_producer);
    thread_args_t *args = malloc(sizeof(thread_args_t) * num_producer);

    for ( int i = 0 ; i < num_producer; i++) {
        args[i].id = i;
        args[i].type = 1;
        if ( i == 0 ) {
            args[i].lo = 0;
            args[i].hi = hi/num_producer;
        } else {
            args[i].lo = args[i-1].hi+1;
            args[i].hi = args[i].lo + (hi/num_producer);
        }

        if (i == num_producer-1) {
           args[i].hi = hi; 
        }
        pthread_create(threads + i, NULL, producer_thread, args+i);
    }

    for (int i = 0; i < num_producer; i++) {
        pthread_join(threads[i], NULL);
    }

    free(args);
    free(threads);
    
    // if chuck is 1 then use offset 0 for head
    // if chuck > 1 then use 1 to chuck for head 
    // Split linklist into chunk sizes
    heads[1] = heads[0];
    if ( chunk > 1 ) {
        heads[1] = heads[0];

        int chunk_id = 1;
        queue_t* node = heads[chunk_id];

        int cnt = 0;
        int chunk_size = hi/chunk;
        while ( node ) {
            cnt++;
            if ( cnt == chunk_size && node->next != NULL) {
                cnt = 0;
                chunk_id++;
                heads[chunk_id] = node->next;
                node->next = NULL;
                node = heads[chunk_id];
            } else {
                node = node->next;
            }
        }
    }

    threads = malloc(sizeof(pthread_t) * num_consumers);
    args = malloc(sizeof(thread_args_t) * num_consumers);

    for ( int i = 0 ; i < num_consumers ;i++) {
        args[i].id = i;
        args[i].type = 2;
        args[i].num_primes = 0;
        args[i].chunkid_max = chunk;
        pthread_create(threads + i, NULL, consumer_thread, args+i);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_join(threads[i], NULL);
        sum += args[i].num_primes;
    }

    free(args);
    free(threads);
    return sum;
}

long find_number_of_primes(const int num_comsumers, const unsigned long hi) {
    heads = malloc(sizeof(queue_t) * 1);
    return find_number_of_prime_range(1, num_comsumers, hi, 1);
}

long find_number_of_primes_mp(const int num_producer, const int num_consumers, const unsigned long hi) {
    heads = malloc(sizeof(queue_t) * 1);
    return find_number_of_prime_range(num_producer, num_consumers, hi, 1);
    free(heads);
}
    
long find_number_of_primes_chuck(const int num_consumers, const unsigned long hi, const int chunk) {
    heads = malloc(sizeof(queue_t) * chunk);
    return find_number_of_prime_range(1, num_consumers, hi, chunk);
}

void printQueue(int chunkid) {
    queue_t* node = heads[chunkid];
    printf("[");
    while ( node) {
        printf("%lu,", node->num);
        node = node->next;
    }
    printf("]\n");
}

int main(int argc, char *argv[]) {
    int count = 0;
    printf("running ...find_number_of_primes(4, 100)\n");
    count = find_number_of_primes(4, 100);
    printf("Numbers of primes: %d\n" , count);
    printQueue(1);

    
    printf("running ...find_number_of_primes_mp(4, 100)\n");
    count = find_number_of_primes_mp(4, 6, 100);
    printf("Numbers of primes: %d\n" , count);
    printQueue(1);


    count = find_number_of_primes_chuck(1, 100, 4);
    printf("Numbers of primes: %d\n" , count);
    printQueue(1);
    printQueue(2);
    printQueue(3);
    printQueue(4);

}
