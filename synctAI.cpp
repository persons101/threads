/*
    File: synctAI.cpp
    Example: 6 producers, 6 consumers, bounded buffer of 10 ints using pthreads.
    At the end main prints min, max and average of all produced integers.
*/

#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cstdint>
#include <stdlib.h>

#define NUM_PROD 6
#define NUM_CONS 6
#define BUF_SIZE 10
#define ITEMS_PER_PROD 1000  // change if you want more/less produced items
#define MAX_RANDOM 1000      // generated values in [0, MAX_RANDOM]

int buffer[BUF_SIZE];
int head = 0, tail = 0, count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

long long produced_total = 0;
long long consumed_total = 0;
const long long TOTAL_ITEMS = (long long)NUM_PROD * ITEMS_PER_PROD;

long long sum_produced = 0;
int min_produced = INT_MAX;
int max_produced = INT_MIN;

void* producer(void* arg) {
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)pthread_self();
        for (int i = 0; i < ITEMS_PER_PROD; ++i) {
                int value = (int)(rand_r(&seed) % (MAX_RANDOM + 1));
                pthread_mutex_lock(&mutex);
                while (count == BUF_SIZE) {
                        pthread_cond_wait(&not_full, &mutex);
                }
                // insert
                buffer[tail] = value;
                tail = (tail + 1) % BUF_SIZE;
                count++;

                // update statistics (producers-only stats)
                sum_produced += value;
                if (value < min_produced) min_produced = value;
                if (value > max_produced) max_produced = value;

                produced_total++;

                // if this was the last produced item, wake all consumers; else wake one
                if (produced_total == TOTAL_ITEMS)
                        pthread_cond_broadcast(&not_empty);
                else
                        pthread_cond_signal(&not_empty);

                pthread_mutex_unlock(&mutex);
        }
        pthread_exit(0);
}

void* consumer(void* arg) {
        while (true) {
                pthread_mutex_lock(&mutex);
                while (count == 0 && produced_total < TOTAL_ITEMS) {
                        pthread_cond_wait(&not_empty, &mutex);
                }
                // If buffer empty and no more will be produced -> exit
                if (count == 0 && produced_total == TOTAL_ITEMS) {
                        pthread_mutex_unlock(&mutex);
                        break;
                }
                // remove
                int value = buffer[head];
                head = (head + 1) % BUF_SIZE;
                count--;
                consumed_total++;

                // signal producer(s) that there is space
                pthread_cond_signal(&not_full);
                pthread_mutex_unlock(&mutex);

                // consume value (no-op for stats since stats are for produced values)
                (void)value;
        }
        pthread_exit(0);
}

int main() {
        pthread_t prod[NUM_PROD];
        pthread_t cons[NUM_CONS];

        // create consumers first (optional)
        for (int i = 0; i < NUM_CONS; ++i) {
                if (pthread_create(&cons[i], nullptr, consumer, nullptr) != 0) {
                        perror("pthread_create consumer");
                        return 1;
                }
        }

        // create producers
        for (int i = 0; i < NUM_PROD; ++i) {
                if (pthread_create(&prod[i], nullptr, producer, nullptr) != 0) {
                        perror("pthread_create producer");
                        return 1;
                }
        }

        // join producers
        for (int i = 0; i < NUM_PROD; ++i) {
                pthread_join(prod[i], nullptr);
        }

        // At this point all producers have finished producing.
        // Wake any consumers that might still be waiting.
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&not_empty);
        pthread_mutex_unlock(&mutex);

        // join consumers
        for (int i = 0; i < NUM_CONS; ++i) {
                pthread_join(cons[i], nullptr);
        }

        // print statistics about produced integers
        if (produced_total > 0) {
                double average = (double)sum_produced / (double)produced_total;
                printf("Produced items: %lld\n", produced_total);
                printf("Min produced: %d\n", min_produced);
                printf("Max produced: %d\n", max_produced);
                printf("Average produced: %.3f\n", average);
        } else {
                printf("No items were produced.\n");
        }

        return 0;
}