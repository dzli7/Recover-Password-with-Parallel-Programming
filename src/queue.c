#include "queue.h"
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

// TODO (student): Write this!
typedef struct block {
    void *payload;
    void *next;
} block_t;

typedef struct queue{
    size_t size;
    block_t *head;
    block_t *tail;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} queue_t;

size_t get_size(queue_t *queue) {
    assert(queue != NULL);
    return queue->size;
}

queue_t *queue_init(void) {
    queue_t *queue = malloc(sizeof(queue_t));
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void queue_enqueue(queue_t *queue, void *value) {
    pthread_mutex_lock(&queue->lock);
    block_t *new_block = malloc(sizeof(block_t));
    new_block->payload = value;
    new_block->next = NULL;
    if (queue->size == 0) {
        queue->head = new_block;
        queue->tail = new_block;
    }
    else {
        queue->tail->next = (void *) new_block;
        queue->tail = new_block;
    }
    queue->size++;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}



void *queue_dequeue(queue_t *queue) {
    pthread_mutex_lock(&queue->lock);
    if (queue == NULL) {return NULL;}
    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    
    block_t *block = queue->head;
    queue->head = block->next;
    void *payload = block->payload;
    queue->size--;
    if (queue->size == 0) {
        queue->tail = NULL;
    }
    free(block);
    pthread_mutex_unlock(&queue->lock);
    return payload;
}


void queue_free(queue_t *queue) {
    free(queue);
}