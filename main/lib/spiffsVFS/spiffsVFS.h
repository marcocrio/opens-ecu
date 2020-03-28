#pragma once

extern const char *SPFS;


//-----------------VE Table-----------------//  

/* Note: variables are declared and deifned in spiffsVFS.h file
         they are only declared here to be worked with.
*/


//VE Headers
extern int rpm[12];
extern int pre[16];

//VE[row][col]
//VE [j][i]
extern float VE[12][16];

extern float VE_Value; //the value retunrned from Interpolation



#ifdef __cplusplus
extern "C" {
#endif





void vfsSetup();
void rdfile();
void prntVE();
void interpolation(int hpa, int revs);



#ifdef __cplusplus
}
#endif

