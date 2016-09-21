#ifndef IMAGE2DSLICER_H_INCLUDED
#define IMAGE2DSLICER_H_INCLUDED

#include "math.h"

#include <stdio.h>
#include <png.h>

#define EXIT_FAILURE 10
#define AXIAL 0
#define SAGITAL 1
#define CORONAL 2
#define RADIOLOGISTA 3
#define CIRURGIAO 4

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

typedef struct _2DImage
{
    int ***x;
} Image_2D;


typedef struct _CuttingInfos
{
    int typeOfCut, viwerCode, pointOfCut;

} CuttingInfos;


typedef struct _PGMData
{
    int row;
    int col;
    int  nx,ny,nz;/* dimensoes da imagem */
    int binary;/* size */

    float dx,dy,dz; /* tabela de linhas */
    int max_gray;
    int ***matrix;
} PGMData;

typedef struct RGB
{
    int R;
    int G;
    int B;
} RGB;


int ***allocate_dynamic_matrix(int row, int col,int depth);

void deallocate_dynamic_matrix(int **matrix, int row);

void writePGM(const char *filename, const PGMData *data, CuttingInfos *cuttingInfos);

PGMData* read3DPGM(const char *file_name, PGMData *data);

void getImageSlice(int typeOfCut, int viwerCode, int pointOfCut, char *readFileName, char* writeFileName);

writeOneColorVoxel(int valor,FILE * pgmFile);






#endif // IMAGE2DSLICER_H_INCLUDED
