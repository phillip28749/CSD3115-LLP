#include <stdio.h>
#include <stdarg.h>

void sq_root_compute(unsigned int val);
void print(unsigned int val , unsigned int root);

void sq_root_compute_array(int num_of_elements, unsigned int *array_of_elements) {

    for (int i = 0; i < num_of_elements; i++) {

      sq_root_compute(array_of_elements[i]);
    }
}

void sq_root_compute_varargs(unsigned int count, ...) {

    va_list ap;
    va_start(ap, count);

    for (unsigned int i = 0; i < count; i++) {

        unsigned int val = va_arg(ap, unsigned int);
        
        sq_root_compute(val);
    }

    va_end(ap);
}

void sq_root_compute(unsigned int value){

    unsigned int root = 0;
    unsigned val = value;
    
    for (unsigned int i = 1; i <= val; i += 2) {
        root++;
        val -= i;
        if (i + 2 > val) {
            break;
        }
    }

    print(value, root);
}

void print(unsigned int val , unsigned int root){
    printf("Square root of %u is %u\n", val, root);
}


int main(){

    int a[]={ 25, 169, 9810};
    sq_root_compute_array(3, a);
    //sq_root_compute_varargs(25, 169, 9810, 0);
    sq_root_compute_varargs(3, 25, 169, 9810);

    return 0;


    
}

