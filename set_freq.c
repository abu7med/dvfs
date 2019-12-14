/*#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include <sched.h>
*/

#include <cpufreq.h>
#include <sys/time.h> 
#include <stdio.h>

char my_governer[] = {"userspace"};


long available_freqs[] ={3600000, 3300000, 2700000, 2100000, 1400000};

#define min_freq 1400000
#define max_freq 3600000


void setup_core(int i, long my_freq){

                cpufreq_modify_policy_governor(i, my_governer);


                cpufreq_modify_policy_min(i, min_freq);
                cpufreq_modify_policy_max(i, max_freq);

                cpufreq_set_frequency(i, my_freq);

}

int main(){

        setup_core(0,1400000);
        setup_core(1,1400000);

        printf("%ld\n", cpufreq_get_freq_hardware(0));
        printf("%ld\n", cpufreq_get_freq_hardware(1));
        printf("%ld\n", cpufreq_get_freq_hardware(2));
        printf("%ld\n", cpufreq_get_freq_hardware(3));

        return 0;
}
