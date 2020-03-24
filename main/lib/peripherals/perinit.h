#pragma once

#ifdef __cplusplus
extern "C" {
#endif


// Register WiFi functions
void setADC(); //Set up ADC at 10 bit
void vfsSetup(); //initializes Virtual File System
void set_injPWM(); //Sets up PWM
void rdfile();


 //-----------------VE Table-----------------//  

    //VE Headers
    int rpm[12]={0};
    int pre[16]={0};

    //VE[row][col]
    //VE [j][i]
    float VE[12][16]={0};

    float VE_Value =0; //the value retunrned from Interpolation



#ifdef __cplusplus
}
#endif

