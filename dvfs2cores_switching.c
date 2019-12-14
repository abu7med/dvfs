/*

./example 1.4 3.6 3.6 1.4 
cores 0-1 switch from 1.4 to 3.6 and cores 6-7 from 3.6 to 1.4

"hyperthreads" 6-7 seem to be mapped properly like 0 and 1

*/


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

//frequencies
long f1min,f1max, f2min, f2max;

static __inline__ uint64_t getNanoTime ( void ) {

	struct timespec nano_time;
	clock_gettime(CLOCK_MONOTONIC, &nano_time);

	return ((uint64_t)(((uint64_t)nano_time.tv_sec * 1000000000) +
			(uint64_t)nano_time.tv_nsec));
}

void* work (void *threadid){

	int *id = (int*) threadid;

	int freq1, freq2;
	if(*id == 0){
		freq1 = f1min;
		freq2 = f1max;
	}
	else{
		freq1 = f2min;
		freq2 = f2max;
	}

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

    int core1 = *id;
    int core2 = core1 + 1;

    cpufreq_set_frequency(core1, freq1);
    cpufreq_set_frequency(core2, freq1);

    //warm up the cache
    int sum;

	for (int i = 0; i<1000000; i++){
		sum++;
	}
		
	//app

	//number of iterations
	for(int i =0; i<1000; i++){

		sum = 0;
		
	    cpufreq_set_frequency(core2, freq2);
		
	    t1 = getNanoTime();

	    cpufreq_set_frequency(core1, freq2);

	//work to do
		for(int j=0; j<100000; j++){
			sum += j;
		}

		t2 = getNanoTime();

		elapsedTime = t2 - t1;
		
		avgTime += elapsedTime;

		//long my_freqs = cpufreq_get_freq_hardware(id);
		//printf ("cpu id %d @ %ld: Time: %ld millis\n", *id, my_freqs,elapsedTime);

		//switch back the freq
		cpufreq_set_frequency(core1, freq1);
		cpufreq_set_frequency(core2, freq1);

		//delay
		for(int j=0; j<1000000; j++){
			sum += j;
		}

	}

	avgTime /= 1000.0;

	long my_freqs = cpufreq_get_freq_hardware(*id);

	printf ("cpu id %d @ %ld: Time: %f ns\n", *id, my_freqs, avgTime);

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

int main(int argc, char **argv){

	float a;
  
// f1 f2 is entered in GHZ e.g: 1.4 1.4
	if(argc == 5){
    sscanf (argv[1], "%f", &a);
    f1min = (long)(a * 1000000);

    sscanf (argv[2], "%f", &a);
    f1max = (long)(a * 1000000);

    sscanf (argv[3], "%f", &a);
    f2min = (long)(a * 1000000);

    sscanf (argv[4], "%f", &a);
    f2max = (long)(a * 1000000);
  }
  
  else{
    f1min = min_freq;
    f2min = min_freq;

    f1max = max_freq;
    f2max = max_freq;
  }

	pthread_t threads[4];
	int id[4] = {0, 1, 6, 7};
	
	pthread_create(&threads[0], NULL, work, &id[0]);
	pthread_create(&threads[1], NULL, busy_wait, &id[1]);

	pthread_create(&threads[2], NULL, work, &id[2]);
	pthread_create(&threads[3], NULL, busy_wait, &id[3]);

	for(int i = 0; i < 4; i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}