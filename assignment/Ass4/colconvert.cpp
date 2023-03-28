/************************************************************************************//*!
\file          colconvert.cpp
\project       A4
\author        Chen Yen Hsun, 20027671
\par           email: c.yenhsun\@digipen.edu
\date          March 28, 2023
\brief         

File contains two implementations of a function that converts a 
column-oriented adjacency matrix representing a graph to an equivalent 
row-oriented matrix. The first implementation, "basic_col_convert,"   
uses a simple nested loop to perform the conversion. The second part of the 
code, optimized the matrix into smaller blocks and processes each block 
individually to improve cache locality. 

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*//*************************************************************************************/
#define CACHE_SIZE 64
#define BLOCK_DIM (CACHE_SIZE/(int)sizeof(int))
void opt_col_convert_single_threaded(int* G, int dim)
{
    // If the matrix size is too small, use the original code
	int blocks = dim / BLOCK_DIM;
	if(dim < BLOCK_DIM)
	{
		int i, j;

		for (i = 0; i < dim; ++i)

			for (j = 0; j < dim; ++j) {

				G[j * dim + i] = G[j * dim + i] || G[i * dim + j];

			}
		return;
	}

	int i = 0 ,j = 0;

    // Loop through all block pairs and update the matrix accordingly
	for(;i < blocks ; ++i)
	{
		int x_offset = i *  BLOCK_DIM;
		int dimx_offset = x_offset + BLOCK_DIM;
		for(j =i ;j<blocks; ++j)
		{
			if(i == j)
				continue;
			int y_offset = j * BLOCK_DIM;
			int dimy_offset = y_offset + BLOCK_DIM;
			for(int x = x_offset; x < dimx_offset ; ++x)
			{
				for(int y = y_offset  ;  y < dimy_offset ; ++y)
				{
					int pos1 = (x) * dim + y; //position in the matrix at row `x` and column `y`
					int pos2 = (y) * dim + x; //position in the matrix at row `y` and column `x`.
					int val1 = G[pos1];
					int val2 = G[pos2];
					int result = val1 || val2;

                    // Store the OR result back in the matrix for both the transposed and untransposed positions
					G[pos1] = result;
					G[pos2] = result;
				}
			}
		}
	}

    // Handle any remaining rows/columns that didn't get covered by blocks
	for(int z = 0;z < blocks;++z)
	{
		int offset = z * BLOCK_DIM;
		int dimz_offset = offset + BLOCK_DIM;
		int x_count = 0;
		for(int x = offset; x < dimz_offset ; ++x,++x_count)
		{
			for(int y = offset + x_count ;  y < dimz_offset ; ++y)
			{
				int pos1 = (x) * dim + y; // Current position in matrix
				int pos2 = (y) * dim + x; // Transposed position in matrix
				int val1 = G[pos1];
				int val2 = G[pos2];
				int result = val1 || val2;

				G[pos1] = result;
				G[pos2] = result;
			}
		}
	}

    // Handle the remaining rows/columns that didn't get covered by blocks
	for(int i = 0; i < dim ; ++i)
	{
		for(int x =  blocks * BLOCK_DIM ; x < dim ;++x)
		{
			int val1 = G[x * dim + i];
			int val2 = G[i * dim + x];
			int result = val1 || val2;

            // Store the OR result back in the matrix for both the transposed and untransposed positions
			G[x * dim + i] =result;
			G[i * dim + x] =result;
		}
	}
}