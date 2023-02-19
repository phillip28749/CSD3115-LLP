/************************************************************************/
/*! Start Header
\file edge.cpp
\author 
\par email: c.yenhsun@digipen.edu
\par DigiPen login: c.yenhsun
\par Course: Low-Level Programming
\par Assignment #2
\date 19/2/2023
\brief
Optimise Kirsch edge detection C code using sse and avx operations

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
***********************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
//#include <tmmintrin.h>
#include <immintrin.h> ///AVX
#include "bmp.h"
#include "measure.h"
//#include <x86intrin.h>
#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include "Windows.h"
#include "malloc.h"
#define posix_memalign(address, alignment, size) *(address)=_aligned_malloc((size), (alignment))
#define sleep(s) Sleep(1000*s)
#else
#include <cstring>
#include "unistd.h"
#define memcpy_s(d, n, s, c) memcpy(d, s, c)
#endif
/* just used for time measurements */
#define REP 10
#define MIN(X, Y) (((X)<(Y))?X:Y)

/* Kirsch operator */
void kirsch_operator_avx(unsigned char *data_out,
                         unsigned char *data_in, unsigned height,
                         unsigned width)
{
  unsigned int size, i, j, lay;

	//Increase to multiple of 32 due to AVX 256 bits
	//Because we are doing unrolling factor 32, best if arrays are in multiple of 32
	//num_of_ints_per_row will be a multiple of 32
	unsigned num_of_ints_per_row = width;
//	while ((num_of_ints_per_row << 1) % 32)
//		num_of_ints_per_row++;
	num_of_ints_per_row = (num_of_ints_per_row+31)/32*32;

	size = height * width;
	unsigned short *color_rows[3];

  	/*
	For our sanity's sake, we divide the 3 colours into
	3 separate arrays. Each starting at an address that is 64-byte aligned.
	*/

	//Allocate memory based on 64 bytes multiple
	//color_rows[0] is red, color_rows[1] is green, color_rows[2] is blue
    color_rows[0] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));
    color_rows[1] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));
    color_rows[2] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));

	/////////////////////////////////////////////
	//Copy data into the allocated memory
	unsigned char *my_data_temp_in = data_in;
	for (lay = 0; lay < 3; lay++) {

		//Get pointer to each row
		unsigned short *temp_rows = color_rows[lay];

		//Copy data
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				temp_rows[i * num_of_ints_per_row + j] = my_data_temp_in[i * width + j];
			}
		}

		//Offset to next color group
		my_data_temp_in += size;
	}

  ////////////////////////////////////////////
	typedef union {
		__m256i value;
		unsigned char vector[32];
	} vec_or_val;

  //Layer count, each time for a color row
	for (lay = 0; lay < 3; lay++) {

    	//Now this loop is for the y rows.
		//Start from 1 because we don't do the algorithm on the 0th row
		unsigned short *my_data_in_rmp = color_rows[lay];
		for (i = 1; i < height - 1; ++i) {
      /*
			In order to obtain the correct mask value for each row,
			we need to perform additions or subtractions in parallel.
			Note that we do not need to do multiplications because 1+1=2.

			So we always maintain an upper row, middle row and a lower row.
			Loading 16 pixel values at each time.
			*/

			//Get pointers to y-1 (upper row), y (mid row), y+1(bottom row)
			__m256i *upper_row_ptr = (__m256i *) & my_data_in_rmp[(i - 1) * num_of_ints_per_row];
			__m256i *middle_row_ptr = (__m256i *) & my_data_in_rmp[i * num_of_ints_per_row];
			__m256i *lower_row_ptr = (__m256i *) & my_data_in_rmp[(i + 1) * num_of_ints_per_row];

			//Load the memory into the row values
			__m256i upper_row, lower_row, middle_row;
			upper_row = _mm256_load_si256(upper_row_ptr);
			middle_row = _mm256_load_si256(middle_row_ptr);
			lower_row = _mm256_load_si256(lower_row_ptr);

			__m256i current_3_upper_row, current_3_lower_row, current_3_middle_row;
      		__m256i current_5_upper_row, current_5_lower_row, current_5_middle_row;

            //Get the 3x3
			current_3_upper_row  = _mm256_add_epi16(_mm256_slli_epi16(upper_row, 1), upper_row);
			current_3_middle_row = _mm256_add_epi16(_mm256_slli_epi16(middle_row, 1), middle_row);
			current_3_lower_row  = _mm256_add_epi16(_mm256_slli_epi16(lower_row, 1), lower_row);
            //Get the 5x5 masks
			current_5_upper_row  = _mm256_add_epi16(_mm256_slli_epi16(upper_row, 2), upper_row);
			current_5_middle_row = _mm256_add_epi16(_mm256_slli_epi16(middle_row, 2), middle_row);
			current_5_lower_row  = _mm256_add_epi16(_mm256_slli_epi16(lower_row, 2), lower_row);

			

			//Skip to next batch of 16 pixels (each pixel is 16 bits)
			upper_row_ptr++;
			middle_row_ptr++;
			lower_row_ptr++;

			//Load the next row data into variable
			__m256i next_upper_row = _mm256_load_si256(upper_row_ptr);
			__m256i next_middle_row = _mm256_load_si256(middle_row_ptr);
			__m256i next_lower_row = _mm256_load_si256(lower_row_ptr);

			__m256i next_3_upper_row, next_3_lower_row, next_3_middle_row;
			__m256i next_5_upper_row, next_5_lower_row, next_5_middle_row;

            //Get the 3x3
			next_3_upper_row  = _mm256_add_epi16(_mm256_slli_epi16(next_upper_row, 1), next_upper_row);
			next_3_middle_row = _mm256_add_epi16(_mm256_slli_epi16(next_middle_row, 1), next_middle_row);
			next_3_lower_row  = _mm256_add_epi16(_mm256_slli_epi16(next_lower_row, 1), next_lower_row);
            //Get the 5x5 masks
			next_5_upper_row  = _mm256_add_epi16(_mm256_slli_epi16(next_upper_row, 2), next_upper_row);
			next_5_middle_row = _mm256_add_epi16(_mm256_slli_epi16(next_middle_row, 2), next_middle_row);
			next_5_lower_row  = _mm256_add_epi16(_mm256_slli_epi16(next_lower_row, 2), next_lower_row);

        
      		//j is pixel counter, adds 32 per loop, so it travels per pixel for the whole row
			//Again, it starts from j = 1 beause 0th pixel is ignored
			for (j = 1; j < width - 33; j = j + 32) {
				//printf("remaining j = %d\n", j);
				//This k inner loop is needed, within your x and y loop, because
				//1 __m256i varaible holds the variables for only 16 shorts
				//2 __m256i variables can therefore have 32 shorts, 
				//and we later 'pack' these 2 __m256i variables into 1 variable, taking away the short portion
				__m256i total_sum1, total_sum2;
				for (int k = 0; k < 2; k++) {

					/*
					next_hmask_values has upper_row + twice_middle_row + lower_row
					for  positions 16-31

					we need the values of that for positions 2 - 17

					real_hmask_left holds the values of the positions 2-17
					*/
					//Build the left mask that contains data 2 to 17
					//by shifting bits around, 
					//		shift the next mask by 6 px left to get     [17 16 0  0  0  0  0  0  0  0 0 0 0 0 0 0],
					//		shift the right mask by 2 px right to get   [0  0  15 14 13 12 11 10 9  8 7 6 5 4 3 2]
					//		OR together to get						    [17 16 15 14 13 12 11 10 9  8 7 6 5 4 3 2]
					// Now we can add the 2 masks together


					__m128i temp128i_1 = _mm256_extracti128_si256(next_5_upper_row, 0x0);
					__m128i temp128i_2 = _mm256_extracti128_si256(current_5_upper_row, 0x0);
					__m128i temp128i_3 = _mm256_extracti128_si256(current_5_upper_row, 0x1); 

					__m128i temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
					__m128i temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
					__m256i upper_row_5_pos_2_17 = _mm256_set_m128i(temp128i_lower, temp128i_upper);
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
					__m256i upper_row_5_pos_1_16 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					temp128i_1 = _mm256_extracti128_si256(next_3_upper_row, 0x0);			//lower 16 bytes from next
					temp128i_2 = _mm256_extracti128_si256(current_3_upper_row, 0x0); 		//lower 16 bytes from current
					temp128i_3 = _mm256_extracti128_si256(current_3_upper_row, 0x1); 		//higher 16 bytes from current

                    //Get the values for positions 2 to 17
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
					__m256i upper_row_3_pos_2_17 = _mm256_set_m128i(temp128i_lower, temp128i_upper);
                    //Get the values for positions 1 to 16
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
					__m256i upper_row_3_pos_1_16 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

                    //Get the values for positions 2 to 17
					temp128i_1 = _mm256_extracti128_si256(next_5_middle_row, 0x0);			//lower 16 bytes from next
					temp128i_2 = _mm256_extracti128_si256(current_5_middle_row, 0x0); 		//lower 16 bytes from current
					temp128i_3 = _mm256_extracti128_si256(current_5_middle_row, 0x1); 		//higher 16 bytes from current

                    //Get the values for positions 2 to 17
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
					__m256i middle_row_5_pos_2_17 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

                    //Get the values for positions 1 to 16
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));

					__m256i middle_row_5_pos_1_16 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					/*---------------------------------------------
					middle row 3x for position 2-17 & 1-16
					---------------------------------------------*/
					temp128i_1 = _mm256_extracti128_si256(next_3_middle_row, 0x0);			//lower 16 bytes from next
					temp128i_2 = _mm256_extracti128_si256(current_3_middle_row, 0x0); 		//lower 16 bytes from current
					temp128i_3 = _mm256_extracti128_si256(current_3_middle_row, 0x1); 		//higher 16 bytes from current

					//take 2 pixel from next & remaining 6 pixel from current <for FIRST 16 batch>
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
					//take 2 pixel from next & remaining 6 pixel from current <for NEXT 16 batch>
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
					//the values of the positions 2-17
					__m256i middle_row_3_pos_2_17 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					// [16,-,-,-,...,-] OR [-,15,14,...,9] = [16,15,14,...,9]
					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
					//[8,-,-,-,...,-] OR [-,7,6,...,2] = [8,7,6,...,2]
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
					//<------the values of the positions 1-16------->
					// [lower,upper] = [16,15,14,...,9,8,7,6,...,2]
					__m256i middle_row_3_pos_1_16 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					/*----------------------------------------------------------------------------------
					LOWER VALUES
					----------------------------------------------------------------------------------*/

                    /*---------------------------------------------
                    lower row 5x for position 2-17 & 1-16
                    ---------------------------------------------*/
					temp128i_1 = _mm256_extracti128_si256(next_5_lower_row, 0x0);
					temp128i_2 = _mm256_extracti128_si256(current_5_lower_row, 0x0);
					temp128i_3 = _mm256_extracti128_si256(current_5_lower_row, 0x1);

					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
					__m256i lower_row_5_pos_2_17 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
					__m256i lower_row_5_pos_1_16 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

                    /*---------------------------------------------
                    lower row 3x for position 2-17 & 1-16
                    ---------------------------------------------*/
					temp128i_1 = _mm256_extracti128_si256(next_3_lower_row, 0x0);
					temp128i_2 = _mm256_extracti128_si256(current_3_lower_row, 0x0);
					temp128i_3 = _mm256_extracti128_si256(current_3_lower_row, 0x1);

					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
					__m256i lower_row_3_pos_2_17 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					temp128i_lower = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
					temp128i_upper = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
					__m256i lower_row_3_pos_1_16 = _mm256_set_m128i(temp128i_lower, temp128i_upper);

					/*---------------------------------------------
					APPLYING MASK
					---------------------------------------------*/
					__m256i mask_upper, mask_middle, mask_lower, totalsum, maximum;

                    /*--------------------------------
                    {5, 5, -3},
                    {5, 0, -3},            rotation 1
                    {-3, -3, -3}
                    --------------------------------*/
					mask_upper = _mm256_add_epi16(current_5_upper_row, _mm256_add_epi16(upper_row_5_pos_1_16,upper_row_5_pos_2_17));
					mask_middle = _mm256_add_epi16(current_3_middle_row, middle_row_3_pos_2_17);
					mask_lower = _mm256_add_epi16(current_3_lower_row, _mm256_add_epi16(lower_row_3_pos_1_16,lower_row_3_pos_2_17));
					maximum = totalsum = _mm256_sub_epi16(mask_upper,_mm256_add_epi16(mask_middle, mask_lower));
					/*--------------------------------
					{5, 5, -3},
					{5, 0, -3},            rotation 2
					{-3, -3, -3}
					--------------------------------*/
					mask_upper = _mm256_add_epi16(current_5_upper_row, _mm256_sub_epi16(upper_row_5_pos_1_16, upper_row_3_pos_2_17));
					mask_middle = _mm256_sub_epi16(current_5_middle_row, middle_row_3_pos_2_17);
					mask_lower = _mm256_add_epi16(current_3_lower_row, _mm256_add_epi16(lower_row_3_pos_1_16, lower_row_3_pos_2_17));
					totalsum = _mm256_add_epi16(mask_upper,_mm256_sub_epi16(mask_middle, mask_lower));
					maximum = _mm256_max_epi16(totalsum, maximum);
					/*--------------------------------
					{5, -3, -3},
					{5, 0, -3},            rotation 3 
					{5, -3, -3}
					--------------------------------*/
					mask_upper = _mm256_sub_epi16(current_5_upper_row, _mm256_add_epi16(upper_row_3_pos_1_16, upper_row_3_pos_2_17));
					mask_middle = _mm256_sub_epi16(current_5_middle_row, middle_row_3_pos_2_17);
					mask_lower = _mm256_sub_epi16(current_5_lower_row, _mm256_add_epi16(lower_row_3_pos_1_16, lower_row_3_pos_2_17));
					totalsum = _mm256_add_epi16(mask_upper,_mm256_add_epi16(mask_middle, mask_lower));
					maximum = _mm256_max_epi16(totalsum, maximum);
					/*--------------------------------
					{-3, -3, -3},
					{5, 0, -3},            rotation 4 
					{5, 5, -3}
					--------------------------------*/
					mask_upper = _mm256_add_epi16(current_3_upper_row, _mm256_add_epi16(upper_row_3_pos_1_16, upper_row_3_pos_2_17));
					mask_middle = _mm256_sub_epi16(current_5_middle_row, middle_row_3_pos_2_17);
					mask_lower = _mm256_sub_epi16(_mm256_add_epi16(current_5_lower_row,lower_row_5_pos_1_16), lower_row_3_pos_2_17);
					totalsum = _mm256_sub_epi16(_mm256_add_epi16(mask_middle, mask_lower),mask_upper);
					maximum = _mm256_max_epi16(totalsum, maximum);
					/*--------------------------------
					{-3, -3, -3},
					{-3, 0, -3},           rotation 5 
					{5, 5, 5}
					--------------------------------*/
					mask_upper = _mm256_add_epi16(current_3_upper_row, _mm256_add_epi16(upper_row_3_pos_1_16, upper_row_3_pos_2_17));
					mask_middle = _mm256_add_epi16(current_3_middle_row, middle_row_3_pos_2_17);
					mask_lower = _mm256_add_epi16(_mm256_add_epi16(current_5_lower_row,lower_row_5_pos_1_16), lower_row_5_pos_2_17);
					totalsum = _mm256_sub_epi16(mask_lower,_mm256_add_epi16(mask_upper, mask_middle));
					maximum = _mm256_max_epi16(totalsum, maximum);
					/*--------------------------------
					{-3, -3, -3},
					{-3, 0, 5},            rotation 6 
					{-3, 5, 5}
					--------------------------------*/
					mask_upper = _mm256_add_epi16(current_3_upper_row, _mm256_add_epi16(upper_row_3_pos_1_16, upper_row_3_pos_2_17));
					mask_middle = _mm256_sub_epi16(middle_row_5_pos_2_17,current_3_middle_row);
					mask_lower = _mm256_sub_epi16(_mm256_add_epi16(lower_row_5_pos_2_17,lower_row_5_pos_1_16), current_3_lower_row);
					totalsum = _mm256_sub_epi16(_mm256_add_epi16(mask_lower, mask_middle),mask_upper);
					maximum = _mm256_max_epi16(totalsum, maximum);
					/*--------------------------------
					{-3, -3, 5},
					{-3, 0, 5},            rotation 7 
					{-3, -3, 5}
					--------------------------------*/
					mask_upper = _mm256_sub_epi16(upper_row_5_pos_2_17, _mm256_add_epi16(upper_row_3_pos_1_16, current_3_upper_row));
					mask_middle = _mm256_sub_epi16(middle_row_5_pos_2_17,current_3_middle_row);
					mask_lower = _mm256_sub_epi16(lower_row_5_pos_2_17, _mm256_add_epi16(current_3_lower_row,lower_row_3_pos_1_16));
					totalsum = _mm256_add_epi16(mask_upper,_mm256_add_epi16(mask_middle,mask_lower));
					maximum = _mm256_max_epi16(totalsum, maximum);
					/*--------------------------------
					{-3, 5, 5},
					{-3, 0, 5},            rotation 8 
					{-3, -3, -3}
					--------------------------------*/
					mask_upper = _mm256_sub_epi16(_mm256_add_epi16(upper_row_5_pos_1_16, upper_row_5_pos_2_17), current_3_upper_row);
					mask_middle = _mm256_sub_epi16(middle_row_5_pos_2_17,current_3_middle_row);
					mask_lower = _mm256_add_epi16(current_3_lower_row, _mm256_add_epi16(lower_row_3_pos_1_16,lower_row_3_pos_2_17));
					totalsum = _mm256_sub_epi16(_mm256_add_epi16(mask_upper,mask_middle),mask_lower);
					maximum = _mm256_max_epi16(totalsum, maximum);
                
					maximum = _mm256_srli_epi16(maximum, 3);//divide by 8
					//kmask_max = _mm256_min_epi16(max_value.value, kmask_max);
					if (k == 0)
						total_sum1 = maximum;
					else
						total_sum2 = maximum;

					upper_row 		= next_upper_row;
					middle_row 		= next_middle_row;
					lower_row 		= next_lower_row;

					current_5_upper_row		= next_5_upper_row;
					current_5_middle_row 	= next_5_middle_row;
					current_5_lower_row		= next_5_lower_row;

					current_3_upper_row 	= next_3_upper_row;
					current_3_middle_row 	= next_3_middle_row;
					current_3_lower_row 	= next_3_lower_row;

						
					//Skip to next batch of 16 pixels (each pixel is 16 bits)
					upper_row_ptr++;
					middle_row_ptr++;
					lower_row_ptr++;

					//Load the next row data into variable
					next_upper_row = _mm256_load_si256(upper_row_ptr);
					next_middle_row = _mm256_load_si256(middle_row_ptr);
					next_lower_row = _mm256_load_si256(lower_row_ptr);

					next_3_upper_row, next_3_lower_row, next_3_middle_row;
					next_5_upper_row, next_5_lower_row, next_5_middle_row;
					next_3_upper_row = _mm256_add_epi16(_mm256_slli_epi16(next_upper_row, 1), next_upper_row);
					next_3_middle_row = _mm256_add_epi16(_mm256_slli_epi16(next_middle_row, 1), next_middle_row);
					next_3_lower_row = _mm256_add_epi16(_mm256_slli_epi16(next_lower_row, 1), next_lower_row);
					next_5_upper_row = _mm256_add_epi16(_mm256_slli_epi16(next_upper_row, 2), next_upper_row);
					next_5_middle_row = _mm256_add_epi16(_mm256_slli_epi16(next_middle_row, 2), next_middle_row);
					next_5_lower_row = _mm256_add_epi16(_mm256_slli_epi16(next_lower_row, 2), next_lower_row);
				}

				//This is the packing portion, we compress 2 _m256i into 1 and store it into memory
				//but need to take care of packed 16-bit integers. Refer to instrincis manual.
				__m128i total_sum1_low = _mm256_extracti128_si256(total_sum1, 0x0);//low
				__m128i total_sum1_hi = _mm256_extracti128_si256(total_sum1, 0x1);//high
				__m128i total_sum2_low = _mm256_extracti128_si256(total_sum2, 0x0);//low
				__m128i total_sum2_hi = _mm256_extracti128_si256(total_sum2, 0x1);//high
				total_sum1 = _mm256_set_m128i(total_sum2_low, total_sum1_low);
				total_sum2 = _mm256_set_m128i(total_sum2_hi, total_sum1_hi);

				__m256i total_sum = _mm256_packus_epi16(total_sum1, total_sum2);

				vec_or_val real_sum;
				_mm256_store_si256(&real_sum.value, total_sum);

				memcpy_s(&data_out[lay * size + i * width + j], 32, real_sum.vector, 32);
			}

			//This is the clean up part! 
			//The image size might not be multiple of 16, so we need to deal with the remaining pixels manually.
			//Notice that this code here is pretty much copy pasted from the basis algorithm
            /* Kirsch matrices for convolution */
            int kirsch[8][3][3] = {
                {
                {5, 5, 5},
                {-3, 0, -3},           /*rotation 1 */
                {-3, -3, -3}
                },
                {
                {5, 5, -3},
                {5, 0, -3},            /*rotation 2 */
                {-3, -3, -3}
                },
                {
                {5, -3, -3},
                {5, 0, -3},            /*rotation 3 */
                {5, -3, -3}
                },
                {
                {-3, -3, -3},
                {5, 0, -3},            /*rotation 4 */
                {5, 5, -3}
                },
                {
                {-3, -3, -3},
                {-3, 0, -3},           /*rotation 5 */
                {5, 5, 5}
                },
                {
                {-3, -3, -3},
                {-3, 0, 5},            /*rotation 6 */
                {-3, 5, 5}
                },
                {
                {-3, -3, 5},
                {-3, 0, 5},            /*rotation 7 */
                {-3, -3, 5}
                },
                {
                {-3, 5, 5},
                {-3, 0, 5},            /*rotation 8 */
                {-3, -3, -3}
                }
            };

			for (; j < width - 1; j++)
			{
				int max_sum;
				max_sum = 0;

				for (unsigned m = 0; m < 8; ++m) {
					int sum;
					sum = 0;
					/* Convolution part */
					for (int k = -1; k < 2; k++)
						for (int l = -1; l < 2; l++) {
							sum =
								sum + kirsch[m][k + 1][l + 1] *
								(int) data_in[lay * size +
                                                (i + k) * width + (j + l)];
						}
					if (sum > max_sum)
						max_sum = sum;
				}
				max_sum = max_sum/8 > 255 ? 255: max_sum/8;
				data_out[lay * size + i * width + j] = max_sum;
			}

    	}

  	}
	for (int i = 0; i < 3; i++)
		std::free(color_rows[i]);
}

