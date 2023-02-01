#include <stdio.h>
#include <stdarg.h>

void sq_root_compute_array(int num_of_elements, unsigned int *array_of_elements) {

    for (int i = 0; i < num_of_elements; i++) {

        unsigned int val = array_of_elements[i];
        unsigned int root = 0;

        //duplicate
        for (unsigned int i = 1; i <= val; i += 2) {
            root++;
            val -= i;
            if (i + 2 > val) {
                break;
            }
        }

        printf("Square root of %u is %u\n", val, root);
    }
}

void sq_root_compute_varargs(unsigned int count, ...) {

    va_list ap;
    va_start(ap, count);

    for (unsigned int i = 0; i < count; i++) {

        unsigned int val = va_arg(ap, unsigned int);
        unsigned int root = 0;

        //duplicate
        for (unsigned int i = 1; i <= val; i += 2) {

            root++;
            val -= i;

            if (i + 2 > val) {
                break;
            }
        }
        printf("Square root of %u is %u\n", val, root);
    }

    va_end(ap);
}


int main(){

    int a[]={ 25, 169, 9810};
    sq_root_compute_array(3, a);
    //sq_root_compute_varargs(25, 169, 9810, 0);
    sq_root_compute_varargs(3, 25, 169, 9810);

    return 0;


    
}