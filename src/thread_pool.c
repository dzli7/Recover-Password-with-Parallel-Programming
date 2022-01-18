#include "thread_pool.h"
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
// TODO (student): Write this!

typedef struct thread_pool {
    pthread_t *threads;
    size_t workers;
    queue_t *queue;
} thread_pool_t;

typedef struct work {
    void *aux;
    work_function_t work;
} work_t;

work_t *work_init(work_function_t work_function, void *aux) {
    work_t *work = malloc(sizeof(work_t));
    work->work = work_function;
    work->aux = aux;
    return work;
}

void *do_work(void *ptr) {
    queue_t *queue = (queue_t *) ptr;
    while (true) {
        work_t *work = queue_dequeue(queue);
        if (work == NULL) { return NULL; }
        (work->work)(work->aux);
        free(work);
    }
    return NULL;
}

thread_pool_t *thread_pool_init(size_t num_worker_threads) {
    thread_pool_t *thread_pool = malloc(sizeof(thread_pool_t));
    thread_pool->workers = num_worker_threads;
    thread_pool->queue = queue_init();
    thread_pool->threads = malloc(num_worker_threads * sizeof(pthread_t));

    for (size_t i = 0; i < num_worker_threads; i++) {
        pthread_create(&thread_pool->threads[i], NULL, do_work, thread_pool->queue);
    }

    return thread_pool;
}

void thread_pool_add_work(thread_pool_t *pool, work_function_t function, void *aux) {
    work_t *work = work_init(function, aux);
    queue_enqueue(pool->queue, (void *) work);
}

void thread_pool_finish(thread_pool_t *pool) {
    for (size_t i = 0; i < pool->workers; i++) {
        queue_enqueue(pool->queue, NULL);
    }   
    for (size_t i = 0; i < pool->workers; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    free(pool->queue);
    free(pool->threads);
    free(pool);
}
