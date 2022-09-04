#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef struct thread_arg_s {
  int id;
  long* buffer_a;
  long* buffer_b;
  long* buffer_c;
  int length;
} thread_args_t;

typedef struct sum_arg_s {
  int id;
  long* buffer;
  long result;
  int length;
} sum_args_t;

typedef struct quicksort_arg_s {
  int level;
  int id;
  long* buffer;
  int p;
  int r;
} quicksort_args_t;


void* proc_thread(void* arg) {
  printf("thread id %d running\n", ((thread_args_t*)arg)->id);
  for ( int i = 0; i < ((thread_args_t*)arg)->length; i++ ) {
    ((thread_args_t*)arg)->buffer_c[i] = ((thread_args_t*)arg)->buffer_a[i] + ((thread_args_t*)arg)->buffer_b[i];
  }
  printf("thread id %d ending\n", ((thread_args_t*)arg)->id);
}

void* sum_arg_proc(void* arg) {
  printf("thread id %d running\n", ((sum_args_t*)arg)->id);
  ((sum_args_t*)arg)->result = 0;
  for ( int i = 0; i < ((sum_args_t*)arg)->length; i++ ) {
    ((sum_args_t*)arg)->result += ((sum_args_t*)arg)->buffer[i];
  }
  printf("thread id %d ending\n", ((sum_args_t*)arg)->id);
}

void parallel_sum_array(long* buffer_a, long* buffer_b, long* buffer_c, int length, int thread_cnt) {
  pthread_t *threads = malloc(sizeof(pthread_t) * thread_cnt);
  thread_args_t *args = malloc(sizeof(thread_args_t) * thread_cnt);

  printf("length %d thread count %d\n", length, thread_cnt);
  int offset = 0;
  for ( int i = 0; i < thread_cnt; i++ ){
    args[i].id = i;
    args[i].buffer_a = buffer_a + offset;
    args[i].buffer_b = buffer_b + offset;
    args[i].buffer_c = buffer_c + offset;
    args[i].length = length / thread_cnt;
    if ( i == thread_cnt-1 ) {
      args[i].length += length % thread_cnt;
    }
    pthread_create(threads+i, NULL, proc_thread, args+i);
    offset += args[i].length;
  }

  for ( int i = 0; i < thread_cnt; i++ ) {
    printf("join thread id %d\n", i);
    pthread_join(threads[i], NULL);
  }

  free(args);
  free(threads);
}

long parallel_sum(long* buffer, int length, int thread_cnt) {
  pthread_t *threads = malloc(sizeof(pthread_t) * thread_cnt);
  sum_args_t *args = malloc(sizeof(sum_args_t) * thread_cnt);

  printf("parallel_sum\n");
  printf("length %d thread count %d\n", length, thread_cnt);
  int offset = 0;
  for ( int i = 0; i < thread_cnt; i++ ){
    args[i].id = i;
    args[i].buffer = buffer + offset;
    args[i].length = length / thread_cnt;
    args[i].result = 0;
    if ( i == thread_cnt-1 ) {
      args[i].length += length % thread_cnt;
    }
    pthread_create(threads+i, NULL, sum_arg_proc, args+i);
    offset += args[i].length;
  }

  long result = 0;
  for ( int i = 0; i < thread_cnt; i++ ) {
    printf("join thread id %d\n", i);
    pthread_join(threads[i], NULL);
    result += args[i].result;
  }

  free(args);
  free(threads);
  return result;
}

int partitionsort(long* buffer,int p, int r ) {

  int pvalue = buffer[r];
  int i = p-1;
  for (int j = p; j < r; j++) {
    if ( buffer[j] <= pvalue) {
      i++;
      int tmp = buffer[i];
      buffer[i] = buffer[j];
      buffer[j] = tmp;

    }
  }
  int tmp = buffer[i+1];
  buffer[i+1] = buffer[r];
  buffer[r] = tmp;
  return  i+1;
}

void print_buffer(long* buffer, int length) {
  printf("Buffer = [");
  for ( int i = 0; i < length; i++) {
    printf("%lu,", buffer[i]);
  }
  printf("]\n");
}

void* quicksort(void* arg) {
  quicksort_args_t* args = (quicksort_args_t*)arg;
  printf("thread level %d thread id %d\n", args->level, args->id);
  if (args->p < args->r) {
    int q = partitionsort(args->buffer,args-> p, args->r);
    printf("p = %d r = %d q = %d\n", args->p, args->r, q);
    quicksort_args_t* largs = malloc(sizeof(quicksort_args_t));
    largs->level = args->level+1;
    largs->id = 0; 

    largs->buffer = args->buffer;
    largs->p = args->p;
    largs->r = q-1;
    
    pthread_t lt;
    pthread_create(&lt,NULL, quicksort, largs );
    
    quicksort_args_t* rargs = malloc(sizeof(quicksort_args_t));
    rargs->level = args->level+1;
    rargs->id = 1; 
    rargs->buffer = args->buffer;
    rargs->p = q+1;
    rargs->r = args->r;
    
    pthread_t rt;
    pthread_create(&rt,NULL, quicksort, rargs );
    
    pthread_join(lt,NULL);
    pthread_join(rt, NULL);

    free(largs);
    free(rargs);
  }
}


int main(int argc,char** argv) {
  printf("Advance Operating Systems - lab1\n");
  int length = 100;
  srand(time(NULL));

  long *array_a = malloc(sizeof(long) * length);
  for ( int i = 0; i < length; i++ ) {
      array_a[i] = rand()%100;
      //array_a[i] = i;
  }

  long *array_b = malloc(sizeof(long) * length);
  for ( int i = 0; i < length; i++ ) {
      //array_b[i] = rand()%1000;
      //array_b[i] = i;
  }

  long *array_c = malloc(sizeof(long) * length);
  for ( int i = 0; i < length; i++ ) {
      array_c[i] = 0; 
  }

  print_buffer(array_a, length);
  print_buffer(array_b, length);
  print_buffer(array_c, length);

  parallel_sum_array(array_a, array_b, array_c, length, 4);
  long sum = parallel_sum(array_c, length, 4);

  print_buffer(array_c, length);

  printf("Sum Array C %lu\n", sum);

  printf("\n");
  printf("Quicksort\n");

  quicksort_args_t args;
  args.level = 0;
  args.id = 0;
  args.buffer = array_a;
  args.p = 0;
  args.r = length-1;

  quicksort(&args);
  print_buffer(args.buffer, length);

  return 0;
}
