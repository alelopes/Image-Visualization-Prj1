#ifndef LINEARTRANSFORMATIONS_H_INCLUDED
#define LINEARTRANSFORMATIONS_H_INCLUDED
#include "image2dSlicer.h"
#include <math.h>



typedef struct _MatrixResult
{
    int mult[4][1];
    int r1, c1;
} MatrixResult;

typedef struct _PGMColorData {

    PGMData pgmData;
    RGB *** RGBMatrix;

} PGMColorData;


void writeRGBVoxel(RGB rgbTosave,FILE * pgmFile);

void linearTransformation(PGMData *data,int brilho,int contraste);

void writeColorPGM(const char *filename, const PGMColorData *data, CuttingInfos *cuttingInfos);

MatrixResult matrixMultiplication(float a[4][4],float b[4][1]);

RGB getRGBVoxel(int intensity);

void labelColorPainting(PGMData *data,PGMData *labelData, int labelToChange, PGMColorData *finalColorData);
RGB ***allocate_dynamic_3D_matrix(int row, int col,int depth);



#endif // LINEARTRANSFORMATIONS_H_INCLUDED
