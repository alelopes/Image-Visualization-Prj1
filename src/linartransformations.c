#include "../include/lineartransformations.h"

const float toYCgCoMatrix[4][4]= {{0.25, 0.5, 0.25, 0.5},{-0.25, 0.5, -0.25, 2047.5},{0.5, 0, -0.5,2047.5},{0, 0, 0, 1}};
const float toRGBMatrix[4][4]= {{1, -1, 1, 0},{1, 1, 0, -2047.2},{1, -1, -1, 4095},{0, 0, 0, 1}};

RGB getRGBVoxel(int intensity)
{
    float H=4095;
    float V = (float)intensity/H;
    V=(6-2)*V+1;
    RGB rgbPixel;
    rgbPixel.R=0;
    rgbPixel.G=0;
    rgbPixel.B=0;
    int RValue = (int)(H*(3-fabs(V-4)-fabs(V-5))/2);
    int GValue = (int)(H*(4-fabs(V-2)-fabs(V-4))/2);
    int BValue = (int)(H*(3-fabs(V-1)-fabs(V-2))/2);

    if (RValue>0) rgbPixel.R=RValue;
    if (GValue>0) rgbPixel.G=GValue;
    if (BValue>0) rgbPixel.B=BValue;

    return rgbPixel;

}



RGB ***allocate_dynamic_3D_matrix(int row, int col,int depth)
{
    RGB ***ret_val;
    int i,j;

    ret_val = (RGB ***)malloc(sizeof(RGB **) * depth);
    if (ret_val == NULL)
    {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < depth; ++i)
    {
        ret_val[i] = (RGB **)malloc(sizeof(RGB*) * col);
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
            ret_val[i][j] = (RGB *)malloc(sizeof(RGB) * row);
            if (ret_val[i][j] == NULL)
            {
                perror("memory allocation failure");
                exit(EXIT_FAILURE);
            }
        }
    }

    return ret_val;
}




void linearTransformation(PGMData *data,int brilho,int contraste)
{
    float H;
    int test = data->binary;
    if (data->binary==12)
    {
        H =4095;
    }
    else
    {
        H=255;
    }
    brilho = brilho*H/100;
    contraste = contraste*H/100;
    float l1 = (2*brilho-contraste)/2;
    float l2 = (contraste+2*brilho)/2;

    int i,j,k;
    int loadedValue;
    for (i=0; i < data->nz; i++)
    {
        for (j=0; j < data->ny; j++)
        {
            for (k=0; k < data->nx; k++)
            {
                loadedValue=data->matrix[i][j][k];
                if (loadedValue<l1)
                {
                    data->matrix[i][j][k]=0;
                }
                else if(loadedValue>l1 && loadedValue<l2)
                {
                    data->matrix[i][j][k]=(H/(l2-l1))*(loadedValue-l1);
                }
                else if(loadedValue>l2)
                {
                    data->matrix[i][j][k]=H;
                }
            }
        }
    }



}

void label3DPainting(PGMData *data,PGMData *labelData, int labelToChange, PGMColorData *finalColorData)
{
    int i,j,k;
    finalColorData->RGBMatrix=allocate_dynamic_3D_matrix(data->nx, data->ny,data->nz);

    for (i=0; i<data->nz; i++)
    {
        for (j=0; j<data->ny; j++)
        {
            for (k=0; k<data->nx; k++)
            {
                if (labelData->matrix[i][j][k]==labelToChange)
                {
                    float test = data->matrix[i][j][k];
                    RGB rgbPixel = getRGBVoxel(data->matrix[i][j][k]);
                    float imageMatrix[1][4]= {(float)rgbPixel.R,(float)rgbPixel.G,(float)rgbPixel.B,1};
                    MatrixResult ycgcoResult = matrixMultiplication(toYCgCoMatrix,imageMatrix);
                    ycgcoResult.mult[0][0]=(float)data->matrix[i][j][k];
                    float updateResult[1][4] = {(float)ycgcoResult.mult[0][0],(float)ycgcoResult.mult[1][0],(float)ycgcoResult.mult[2][0],(float)ycgcoResult.mult[3][0]};

                    MatrixResult rgbResult=matrixMultiplication(toRGBMatrix,updateResult);

                    finalColorData->RGBMatrix[i][j][k].R=rgbResult.mult[0][0];
                    finalColorData->RGBMatrix[i][j][k].G=rgbResult.mult[1][0];
                    finalColorData->RGBMatrix[i][j][k].B=rgbResult.mult[2][0];
                }
                else
                {
                    finalColorData->RGBMatrix[i][j][k].R=data->matrix[i][j][k];
                    finalColorData->RGBMatrix[i][j][k].G=data->matrix[i][j][k];
                    finalColorData->RGBMatrix[i][j][k].B=data->matrix[i][j][k];
                }
            }
        }
    }
}


void writeColorPGM(const char *filename, const PGMColorData *data, CuttingInfos *cuttingInfos)
{
    FILE *pgmFile;
    int i, j;
    int hi, lo;
    PGMColorData * data2 = &data;

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
        fprintf(pgmFile, "%d %d ", data->pgmData.nx, data->pgmData.ny);
        break;
    case (SAGITAL):
        fprintf(pgmFile, "%d %d ", data->pgmData.ny, data->pgmData.nz);
        break;
    case (CORONAL):
        fprintf(pgmFile, "%d %d ", data->pgmData.nx, data->pgmData.nz);
        break;
    }
    fprintf(pgmFile, "%d ", 4095);
    int k=0;

    saveColorImage(cuttingInfos,data, pgmFile);

    fclose(pgmFile);
}




void saveColorImage(CuttingInfos *cuttingInfos,PGMColorData *dataFile, FILE * pgmFile)
{
    int lo;
    int hi;
    int i,j,k;
    int limit;
    PGMData * data = &dataFile->pgmData;

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
                        writeRGBVoxel(dataFile->RGBMatrix[i][j][k],pgmFile);
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
                        writeRGBVoxel(dataFile->RGBMatrix[i][j][k],pgmFile);
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
                        writeRGBVoxel(dataFile->RGBMatrix[i][j][k],pgmFile);
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
                        writeRGBVoxel(dataFile->RGBMatrix[i][j][k],pgmFile);

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
                    writeRGBVoxel(dataFile->RGBMatrix[i][j][k],pgmFile);
                }
            }
        }
        break;
    }
}

void writeRGBVoxel(RGB rgbTosave,FILE * pgmFile)
{
    int hi,lo;
    hi = HI(rgbTosave.R);
    lo = LO(rgbTosave.R);
    //Red
    fwrite(&hi, 1,1,pgmFile);
    fwrite(&lo,1,1, pgmFile);

    hi = HI(rgbTosave.G);
    lo = LO(rgbTosave.G);
    //Red
    fwrite(&hi, 1,1,pgmFile);
    fwrite(&lo,1,1, pgmFile);

    hi = HI(rgbTosave.B);
    lo = LO(rgbTosave.B);
    //Red
    fwrite(&hi, 1,1,pgmFile);
    fwrite(&lo,1,1, pgmFile);

}

MatrixResult matrixMultiplication(float a[4][4],float b[4][1])
{
    int r1, c1, r2, c2, i, j, k;
    MatrixResult result;

    r1=4;
    c1=4;
    r2=4;
    c2=1;

    while (c1!=r2)
    {
        printf("Error! column of first matrix not equal to row of second.\n\n");
        return result;
    }
    /* Initializing elements of matrix mult to 0.*/
    for(i=0; i<r1; ++i)
        for(j=0; j<c2; ++j)
        {
            result.mult[i][j]=0;
        }
    float partial;
    /* Multiplying matrix a and b and storing in array mult. */
    for(i=0; i<r1; ++i)
    {
        for(j=0; j<c2; ++j)
        {
            partial=0;
            for(k=0; k<c1; ++k)
            {
                float cry = a[i][k];
                float alot = b[k][j];
                partial+=(a[i][k]*b[k][j]);
            }
            if (partial>0) result.mult[i][j]=(int) partial;
            else result.mult[i][j]=0;


        }
    }

    result.c1=c2;
    result.r1=r1;
    return result;

}




