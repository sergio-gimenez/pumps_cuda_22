#include <stdio.h>
#include "ee_tools.h"

int main (int argc, char *argv[] )
{
   printf("num cpus = %d\n", ee_get_num_cpus());
   return 0;

}
