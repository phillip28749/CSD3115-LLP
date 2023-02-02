#include <stdio.h>

void sq_root_compute_varargs(unsigned int n1, ...);
void sq_root_compute_array(int num_of_elements, unsigned int *array_of_elements);



void test1()
{
		printf("sq_root_compute_varargs \n");

		sq_root_compute_varargs((unsigned int) 3,
								(unsigned int) 0);


}

void test2()
{
		printf("sq_root_compute_array \n");

		unsigned int a[3] = {25, 169, 5683};

		sq_root_compute_array(3, a );
}


void test3()
{
		printf("sq_root_compute_varargs \n");

		sq_root_compute_varargs((unsigned int) 3,
								(unsigned int) 25,
								(unsigned int) 169,
								(unsigned int) 9810, 
								(unsigned int) 1169,
								(unsigned int) 19810, 
								(unsigned int) 1369,
								(unsigned int) 99810, 
								(unsigned int) 0);

}	


void test(int i)
{
    switch(i)
    {
        case 1:
        test1();
        break;
        case 2:
        test2();
        break;
        case 3:
        test3();
        break;
        default:
        test1();
    }
}

