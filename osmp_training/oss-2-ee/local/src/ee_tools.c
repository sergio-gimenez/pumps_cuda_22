#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>

#include "ee_tools.h"

int ee_get_num_cpus ( void )
{
   cpu_set_t mask;

   sched_getaffinity( 0, sizeof(mask), &mask );
#if 0
   int count = 0;
   int NCPUS = sysconf(_SC_NPROCESSORS_ONLN);

   for ( int i=0; i<NCPUS; i++ ) {
      if ( CPU_ISSET( i, &mask ) ) count++;
   }

   return count;
#else
   return (int) CPU_COUNT(&mask);
#endif
}


