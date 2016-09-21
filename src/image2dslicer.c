#include "../include/image2dSlicer.h"
png_bytep *row_pointers;


int ***allocate_dynamic_matrix(int row, int col,int depth)
{
    int ***ret_val;
    int i,j;

    ret_val = (int ***)malloc(sizeof(int **) * depth);
    if (ret_val == NULL)
    {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < depth; ++i)
    {
        ret_val[i] = (int **)malloc(sizeof(int*) * col);
        if (ret_val[i] == NULL)
        {
            perror("memory allocation failure");
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0; i < depth; ++i)
    {
        for (j = 0; j < col; ++j)
        {
            ret_val[i][j] = (int *)malloc(sizeof(int) * row);
            if (ret_val[i][j] == NULL)
            {
                perror("memory allocation failure");
                exit(EXIT_FAILURE);
            }
        }
    }

    return ret_val;
}

void deallocate_dynamic_matrix(int **matrix, int row)
{
    int i;

    for (i = 0; i < row; ++i)
        free(matrix[i]);
    free(matrix);
}

void writePGM(const char *filename, const PGMData *data, CuttingInfos *cuttingInfos)
{
    FILE *pgmFile;
    int i, j;
    int hi, lo;

    pgmFile = fopen(filename, "wb");
    if (pgmFile == NULL)
    {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }

    fprintf(pgmFile, "P6 ");
    switch (cuttingInfos->typeOfCut)
    {
    case (AXIAL):
        fprintf(pgmFile, "%d %d ", data->nx, data->ny);
        break;
    case (SAGITAL):
        fprintf(pgmFile, "%d %d ", data->ny, data->nz);
        break;
    case (CORONAL):
        fprintf(pgmFile, "%d %d ", data->nx, data->nz);
        break;
    }
    fprintf(pgmFile, "%d ", 4095);
    int k=0;
    saveImage(cuttingInfos,data, pgmFile);

    fclose(pgmFile);
    //deallocate_dynamic_matrix(data->matrix, data->row);
}

/*for reading:*/

PGMData* read3DPGM(const char *file_name, PGMData *data)
{
    FILE *pgmFile;
    char version[3];
    int i, j;
    int lo, hi;

    char str[2];
    unsigned char * value;


    FILE * pFile;
    pFile = fopen (file_name,"rb");

    fscanf (pFile, "%s", str);
    fscanf (pFile, "%d", &data->nx);
    fscanf (pFile, "%d", &data->ny);
    fscanf (pFile, "%d", &data->nz);

    fscanf (pFile, "%f", &data->dx);
    fscanf (pFile, "%f", &data->dy);
    fscanf (pFile, "%f", &data->dz);
    fscanf (pFile, "%d", &data->binary);
    if (data->binary==16)
    {
        data->binary=12;
    }
    data->max_gray=pow(2,(data->binary))-1;
    int test=data->max_gray;
    int t=data->nx;
    int dkas= data->ny;
    printf("%d %d",data->nx,data->ny);
    data->matrix = allocate_dynamic_matrix(data->nx, data->ny,data->nz);
    int k=0;

    if (data->max_gray > 255)
        for (k = 0; k < data->nz; k++)
            for (i = 0; i < data->ny; i++)
                for (j = 0; j < data->nx; j++)
                {
                    hi = fgetc(pFile);
                    lo = fgetc(pFile);
                    int getValue = (hi << 8) + lo;
                    data->matrix[k][i][j] = (hi << 8) + lo;
                }
    else
    {
        for (k = 0; k < data->nz; k++)
            for (i = 0; i < data->ny; i++)
                for (j = 0; j < data->nx; j++)
                {
                    lo = fgetc(pFile);

                    data->matrix[k][i][j] = lo;

                }
    }
    fclose(pFile);
    return data;
}

void saveImage(CuttingInfos *cuttingInfos,PGMData *data, FILE * pgmFile)
{
    int lo;
    int hi;
    int i,j,k;
    int limit;
    switch(cuttingInfos->typeOfCut)
    {
    case(AXIAL):
        if(cuttingInfos->viwerCode==RADIOLOGISTA)
        {
            for (i = cuttingInfos->pointOfCut; i <= cuttingInfos->pointOfCut; i++)
            {
                for (j = 0; j < data->ny; j++)
                {
                    for (k = 0; k < data->nx; k++)
                    {
                        writeOneColorVoxel(data->matrix[i][j][k],pgmFile);
                    }
                }
            }
        }
        else if(cuttingInfos->viwerCode==CIRURGIAO)
        {
            limit=data->nz-cuttingInfos->pointOfCut;
            for (i = limit; i <= limit; i++)
            {
                for (j = 0; j < data->ny; j++)
                {
                    for (k = data->nx-1; k > -1 ; k--)
                    {
                        writeOneColorVoxel(data->matrix[i][j][k],pgmFile);
                    }
                }
            }
        }
        break;
    case(SAGITAL):
        if(cuttingInfos->viwerCode==RADIOLOGISTA)
        {
            limit=cuttingInfos->pointOfCut;
            for (k = limit; k <= limit; k++)
            {
                for (i = data->nz-1; i > -1 ; i--)
                {
                    for (j = data->ny-1; j > -1 ; j--)
                    {
                        writeOneColorVoxel(data->matrix[i][j][k],pgmFile);
                    }
                }
            }
        }
        else if(cuttingInfos->viwerCode==CIRURGIAO)
        {
            limit=data->nx-1-cuttingInfos->pointOfCut;
            for (k = limit; k <= limit; k++)
            {
                for (i = data->nz-1; i > -1 ; i--)
                {
                    for (j = 0; j < data->ny ; j++)
                    {
                        writeOneColorVoxel(data->matrix[i][j][k],pgmFile);
                    }
                }
            }
        }
        break;
    case(CORONAL):
        limit=cuttingInfos->pointOfCut;
        for (j = limit; j <= limit; j++)
        {
            for (i = data->ny-1; i > -1; i--)
            {
                for (k = 0; k < data->nx ; k++)
                {
                    writeOneColorVoxel(data->matrix[i][j][k],pgmFile);
                }
            }
        }
        break;

    }



}

writeOneColorVoxel(int value,FILE * pgmFile)
{
    int hi, lo;
    hi = HI(value);
    lo = LO(value);
    //Red
    fwrite(&hi, 1,1,pgmFile);
    fwrite(&lo,1,1, pgmFile);
    //Green
    fwrite(&hi, 1,1,pgmFile);
    fwrite(&lo,1,1, pgmFile);
    //Blue
    fwrite(&hi, 1,1,pgmFile);
    fwrite(&lo,1,1, pgmFile);

}





