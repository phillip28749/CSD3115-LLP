/************************************************************************************//*!
\file          colconvert.cpp
\project       A4
\author        Chen Yen Hsun, 20027671
\par           email: c.yenhsun\@digipen.edu
\date          March 25, 2023
\brief         opt color convertor

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*//*************************************************************************************/
#define CACHE_SIZE 64
#define BLOCK_DIM (CACHE_SIZE/(int)sizeof(int))
void opt_col_convert_single_threaded(int* G, int dim)
{

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
	for(;i < blocks ; ++i)
	{
		int x_offset = i *  BLOCK_DIM;
		int dimx_offset = x_offset + BLOCK_DIM;
		for(j =i ;j<blocks; ++j)
		{
			if(i == j)
				continue;
			//ss << "------- iteration : " << i << "," << j << "---------\n";
			int y_offset = j * BLOCK_DIM;
			int dimy_offset = y_offset + BLOCK_DIM;
			for(int x = x_offset; x < dimx_offset ; ++x)
			{
				for(int y = y_offset  ;  y < dimy_offset ; ++y)
				{
					int pos1 = (x) * dim + y; //x',y'
					int pos2 = (y) * dim + x; //x ,y
					int val1 = G[pos1];
					int val2 = G[pos2];
					int result = val1 || val2;
					//++counter;
					G[pos1] = result;
					G[pos2] = result;
					//return;
					//ss << "[" << pos1 << "][" << pos2 << "]\n";
				}
			}
		}
	}
	for(int z = 0;z < blocks;++z)
	{
		int offset = z * BLOCK_DIM;
		int dimz_offset = offset + BLOCK_DIM;
		int x_count = 0;
		for(int x = offset; x < dimz_offset ; ++x,++x_count)
		{
			for(int y = offset + x_count ;  y < dimz_offset ; ++y)
			{
				int pos1 = (x) * dim + y;
				int pos2 = (y) * dim + x;
				int val1 = G[pos1];
				int val2 = G[pos2];
				int result = val1 || val2;
				//++counter;
				G[pos1] = result;
				G[pos2] = result;
				//ss << "[" << pos1 << "][" << pos2 << "]\n";
			}
		}
	}
	for(int i = 0; i < dim ; ++i)
	{
		for(int x =  blocks * BLOCK_DIM ; x < dim ;++x)
		{
			int val1 = G[x * dim + i];
			int val2 = G[i * dim + x];
			int result = val1 || val2;
			G[x * dim + i] =result;
			G[i * dim + x] =result;
		}
	}
}