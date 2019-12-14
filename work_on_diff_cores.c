#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include <sched.h>


#include <cpufreq.h>
#include <pthread.h>
#include <sys/time.h> 
#include <stdio.h>

char my_governer[] = {"userspace"};


#define min_freq 1400000
#define max_freq 3600000

#define num_of_threads 2

void* work (void *threadid){


        int *id = (int*) threadid;

        cpu_set_t cpuset;
        int cpu = *id;

        CPU_SET(cpu,&cpuset);

        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

        int sum;

        struct timeval t1, t2;
        double elapsedTime;

        gettimeofday(&t1, NULL);

        for(int i =0; i<10000; i++){
                sum = 0;
                for(int j=0; j<100000; j++){
                        sum += j;
                }
        }

        gettimeofday(&t2, NULL);

        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
        printf ("cpu id %d @ %ld: Time: %f millis\n", cpu, cpufreq_get_freq_har$


return NULL;

}

int main(){

        pthread_t threads[num_of_threads];
        int id[num_of_threads] = {0,2};

        for(int i=0; i<num_of_threads; i++){    
                pthread_create(&threads[i], NULL, work, &id[i]);
        }

        for(int i=0; i<num_of_threads; i++){
                pthread_join(threads[i], NULL);
        }

        return 0;
}