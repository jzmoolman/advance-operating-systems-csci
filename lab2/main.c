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

static queue_t *head = NULL;

void queue_add(const long num) {
    queue_t *node = malloc(sizeof(queue_t));
    pthread_mutex_lock(&mutex);

    node->next = head;
    node->num = num;
    head = node;

    pthread_mutex_unlock(&mutex);
}

queue_t* get_next() {
    pthread_mutex_lock(&mutex);
    queue_t *ret = head;

    if (head) {
        head = head->next;
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
        queue_add(i);
    }
}


void *consumer_thread(void* _args) {
    queue_t *node;
    thread_args_t *args = _args;

    printf("thread id %d\n", args->id);
    printf("thread type %d\n", args->type);
    printf("thread sum %d\n", args->num_primes);

    while((node = get_next())) {
        if (is_prime(node->num)) {
            printf("%ld ", node->num);
            args->num_primes++;
            printf("num primes %d\n", args->num_primes);
        }
        free(node);
    }
}

long find_number_of_prime_range(const int num_producer, const int num_comsumers, const unsigned long hi) {
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

    threads = malloc(sizeof(pthread_t) * num_comsumers);
    args = malloc(sizeof(thread_args_t) * num_comsumers);

    for ( int i = 0 ; i < num_comsumers ;i++) {
        args[i].id = i;
        args[i].type = 2;
        args[i].num_primes = 0;
        pthread_create(threads + i, NULL, consumer_thread, args+i);
    }

    for (int i = 0; i < num_comsumers; i++) {
        pthread_join(threads[i], NULL);
        sum += args[i].num_primes;
    }

    free(args);
    free(threads);
    return sum;
}

long find_number_of_primes(const int num_comsumers, const unsigned long hi) {
    return find_number_of_prime_range(1, consumer_thread, hi);
}

long find_number_of_primes_mp(const int num_producer, const int num_comsumers, const unsigned long hi) {
    return find_number_of_prime_range(num_producer, consumer_thread, hi);
}
    
long find_number_of_primes_chuck(const int num_comsumers, const unsigned long hi, const int chunk) {
    return find_number_of_prime_range(1, consumer_thread, hi);
}

void printQueue() {
    queue_t* node = head;
    printf("[");
    while ( node) {
        printf("%lu,", node->num);
        node = node->next;
    }
    printf("]\n");
}

int main(int argc, char *argv[]) {
    //produce(100000);
    int count = find_number_of_prime_range(4, 5, 100);
    printf("Numbers of primes: %d\n" , count);
    printQueue();
    //setup_consumers(1);
}
