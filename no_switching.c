#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include <sched.h>


#include <cpufreq.h>
#include <pthread.h>
#include <sys/time.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

//flag to stop the busy wait
#define true 1
#define false 0
int flag = true;

#define min_freq 1400000
#define max_freq 3600000

//switch from f1 to f2
long f1;

static __inline__ uint64_t getNanoTime ( void ) {

	struct timespec nano_time;
	clock_gettime(CLOCK_MONOTONIC, &nano_time);

	return ((uint64_t)(((uint64_t)nano_time.tv_sec * 1000000000) +
			(uint64_t)nano_time.tv_nsec));
}

void* work (void *threadid){

	int *id = (int*) threadid;

	//pin the thread
	cpu_set_t p_mask;
	CPU_ZERO(&p_mask);
	CPU_SET(*id, &p_mask);

	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &p_mask);

	//initialize variables
	uint64_t t1, t2;
    uint64_t elapsedTime = 0.0;
    double avgTime = 0.0;

    //set the freq
    cpufreq_set_frequency(0, f1);
    cpufreq_set_frequency(1, f1);

    //warm up the cache
    int sum;

	for (int i = 0; i<1000000; i++){
		sum++;
	}
		
	//app

	//number of iterations
	for(int i =0; i<1000; i++){

		sum = 0;
		
	    t1 = getNanoTime();

	//work to do
		for(int j=0; j<100000; j++){
			sum += j;
		}


		t2 = getNanoTime();

		elapsedTime = t2 - t1;
		
		avgTime += elapsedTime;

		//long my_freqs = cpufreq_get_freq_hardware(0);
		//printf ("cpu id %d @ %ld: Time: %ld ns\n", *id, my_freqs,elapsedTime);
	}

	avgTime /= 1000.0;

	long my_freqs = cpufreq_get_freq_hardware(0);

	printf ("cpu id %d @ %ld: Average Time: %f ns\n", *id, my_freqs, avgTime);

	//stop the second thread
	flag = false;
	
return NULL;

}

void* busy_wait(void *threadid){

	int *id = (int*) threadid;

	cpu_set_t p_mask;
	CPU_ZERO(&p_mask);
	CPU_SET(*id, &p_mask);

	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &p_mask);

	while(flag == true){		
	}

	return NULL;
}

void setup_core(long freq){
	
	cpufreq_set_frequency(0, f1);
	cpufreq_set_frequency(1, f1);

}

int main(int argc, char **argv){

	float a;
  
// f1 is entered in GHZ e.g: 1.4
	if(argc == 2){
    sscanf (argv[1], "%f", &a);
    f1 = (long)(a * 1000000);
  }
  
  else{
    f1 = max_freq;
  }


	pthread_t threads[2];
	int id[2] = {0, 1};
	
	pthread_create(&threads[0], NULL, work, &id[0]);
	pthread_create(&threads[1], NULL, busy_wait, &id[1]);

	for(int i = 0; i < 2; i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}