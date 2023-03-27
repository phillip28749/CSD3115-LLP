void opt_col_convert_single_threaded(int *G, int dim)
{
    int i, j;
    for (i = 0; i < dim; ++i)
    for (j = 0; j < dim; ++j) {
    G[j * dim + i] = G[j * dim + i] || G[i * dim + j];
    }
}	