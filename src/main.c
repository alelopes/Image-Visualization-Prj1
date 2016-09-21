#include <stdio.h>
#include <stdlib.h>
#include "../include/lineartransformations.h"



void getImageSlice(int typeOfCut, int viwerCode, int pointOfCut, char *readFileName, char* writeFileName)
{
    PGMData datatest;

    CuttingInfos cuttingInfos;
    cuttingInfos.typeOfCut=typeOfCut;
    cuttingInfos.viwerCode=viwerCode;
    cuttingInfos.pointOfCut=pointOfCut;

    read3DPGM(readFileName,&datatest);

    linearTransformation(&datatest,0,50);

    writePGM(writeFileName,&datatest,&cuttingInfos);

}

void getPaintedSlice(int typeOfCut, int viwerCode, int pointOfCut, char *readFileName, char* writeFileName)
{

    PGMData datatest;

    CuttingInfos cuttingInfos;
    cuttingInfos.typeOfCut=typeOfCut;
    cuttingInfos.viwerCode=viwerCode;
    cuttingInfos.pointOfCut=pointOfCut;

    read3DPGM(readFileName,&datatest);

    linearTransformation(&datatest,15,70);

    PGMData dataLabel;
    PGMColorData finalColorData;
    finalColorData.pgmData=datatest;

    read3DPGM("brain_label.scn",&dataLabel);

    label3DPainting(&datatest,&dataLabel, 3,&finalColorData);
    //int color = finalColorData.RGBMatrix[0][0][0].B;
    //writePGM(writeFileName,&datatest,&cuttingInfos);
    // printf("color: %d",color);
    writeColorPGM(writeFileName,&finalColorData,&cuttingInfos);

}

int main()
{



    //getImageSlice(SAGITAL, RADIOLOGISTA, 95, "brain.scn", "cabeloComContraste2.ppm");
    getPaintedSlice(SAGITAL, RADIOLOGISTA, 95, "brain.scn", "cabeloT.ppm");

    return 0;
}
