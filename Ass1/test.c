#include <stdio.h>

// void sq_root_compute_array(int num_of_elements, unsigned int *array_of_elements) {

//     for (int i = 0; i < num_of_elements; i++) {

//         unsigned int val = array_of_elements[i];
//         unsigned int root = 0;

//         //duplicate
//         for (unsigned int i = 1; i <= val; i += 2) {
//             root++;
//             val -= i;
//             if (i + 2 > val) {
//                 break;
//             }
//         }

//         printf("Square root of %u is %u\n", val, root);
//     }
// }

long arith
(long x, long y, long z)
{
  long t1 = x+y;
  long t2 = z+t1;
  long t3 = x+4;
  long t4 = y * 48;
  long t5 = t3 + t4;
  long rval = t2 * t5;
  return rval;
}



