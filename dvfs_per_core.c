//Implement a test to check if the DVFS per core works

//created a simple program that will run a nested for loop 
//across the cores

#include <pthread.h>
#include <iostream>
#include <time.h>

int num_of_threads = 4;

void* work (void *threadid){
	
	clock_t start= clock();

	int *id = (int*) threadid;

	//-----------------------------------------------------------
	 
	//Tried pinning the threads to the cores by using sched_setaffinity
	//All threads seemed to finish at the same time

	/*
	cpu_set_t cpuset;
	int cpu = *id;
	CPU_ZERO(&cpuset);
	CPU_SET( cpu , &cpuset);
	sched_setaffinity(0, sizeof(cpuset), &cpuset);

	*/

	//Pinned the 1 thread to a physical core to avoid hyperthreading

	cpu_set_t cpuset;
	int cpu = *id * 2;

	CPU_SET(cpu,&cpuset);

	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

	//-----------------------------------------------------------

	int sum;

	for(int i =0; i<100000; i++){
		sum = 0;
		for(int j=0; j<100000; j++){
			sum += j;
		}
	}
	clock_t end = clock();
	double time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

	std::cout<<"Thread id: "<<*id<<" "<<"Value: "<<sum<<" Time: "<<time_elapsed<<std::endl;

	return NULL;
}

int main(){

	pthread_t threads[num_of_threads];

	int id[num_of_threads];

	for(int i=0; i<num_of_threads; i++){	
		id[i]=i;
		pthread_create(&threads[i], NULL, work, &id[i]);
	}

	for(int i=0; i<num_of_threads; i++){
		pthread_join(threads[i], NULL);
	}		

	return 0;
}