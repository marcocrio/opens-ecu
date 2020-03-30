#include "includes.h" //include all standard libraries
#include "spiffsVFS.h"

//spiffs
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_spiffs.h"
#include "spiffs_config.h"


const char *SPFS = "SPIFFS";


//-----------------VE Table-----------------//  

//VE Headers
int rpm[12]={0};
int pre[16]={0};

//VE[row][col]
//VE [j][i]
float VE[12][16]={0};

float VE_Value =0; //the value retunrned from Interpolation



//*****************************************************************************************//
//*****************************************************************************************//
//************************* SPIFFS Virtual File System Init ********************************//
//*****************************************************************************************//
//*****************************************************************************************//
/* Description: This function mounts the SPIFFS VSF system to 
                enable the real time use of data stored in the
                SPIFFS partition "home".
                To flash new data, files must be palced inside the 
                "/data" directory located under the projec's directory ("/ecm").
                Flashing command is located in the "CMakeList.txt" file located
                under "/main" under the project's directory ("/ecm")

*/

void vfsSetup(){
    ESP_LOGI(SPFS, "\n\nInitializing SPIFFS");
    
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(SPFS, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(SPFS, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(SPFS, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }

        rdfile();//initializes VE table arrays
        
        return;
    }
    

    vTaskDelay(3000/portTICK_PERIOD_MS);

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(SPFS, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(SPFS, "Partition size: total: %d, used: %d", total, used);
    }
    
}

//*****************************************************************************************//
//*****************************************************************************************//
//*********************************** INTERPOLATION ***************************************//
//*****************************************************************************************//
//*****************************************************************************************//
/* Description: This function takes the current # of RPMs and the 
                pressure in the intake manifold to interpolate 
                the Volumetric Efficieny of the cylinder
                Based on the mapped (pre-populated) VE table provided
                by Ecotrons.com for their
                Honda GX35 Engine Fuel Injection kit.
                *Mapping functionality to be added in the future.*
*/


float power(float num){
    return num*num;
}


void interpolation(int hpa, int revs){

    // vTaskDelay(3000/portTICK_PERIOD_MS);
    // ESP_LOGI(SPFS, "Initializing Interpoltion\n");
    // printf("Pressure: %d RPMS: %d\n",hpa,revs);
    int i=0,j=0;
    
    float x,y;
    int xl,xh,yl,yh;


    do{i++;}while(hpa >= pre[i] && pre[i] != 1050);
    xl = pre[i-1];
    xh = pre[i];

    do{j++;}while(revs >= rpm[j] && rpm[j] != 8000);
    yl = rpm[j-1];
    yh = rpm[j];




    // printf("\n%.2f   %.2f\n",VE[j-1][i-1], VE[j-1][i]);
    // printf("%.2f   %.2f\n",VE[j][i-1], VE[j][i]);


    x = ( 100*(hpa - pre[i-1]) / (xh - xl) ) * ( 100*( VE[j-1][i] - VE[j-1][i-1] ) );

    y = ( 100*(revs - rpm[j-1]) / (yh - yl) ) * ( 100*( VE[j][i-1] - VE[j-1][i-1] ) ); 

    float dist=0;
    if(VE[j][i] >= VE[j-1][i-1] ){ 
        dist = ( sqrt( pow(x,2) + pow(y,2) )/ 10000 ) + VE[j-1][i-1]; 
    }else {
        dist = VE[j-1][i-1] - ( sqrt( pow(x,2) + pow(y,2) )/ 10000 );
    };

    VE_Value = dist;
    // printf("\ndist= %.4f\n",dist);

    // float VolEff = VE[j-1][i-1]+ sqrt(  power( x-VE[j-1][i-1] ) + power(y-VE[j-1][i-1])   );

    // VE_Value=VolEff;
    
    // printf("\n");

    return;

};


//*****************************************************************************************//
//*****************************************************************************************//
//********************************** READ VE TABLE ****************************************//
//*****************************************************************************************//
//*****************************************************************************************//
/* Description: This function is used to read the contents of the VE table
                for debbuging purposes
*/


void rdfile(){
    char* token;

    int i,j=0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // Open renamed file for reading
    ESP_LOGI(SPFS, "Reading file");

    // FILE* f = fopen("/spiffs/hello.txt", "r");
    FILE* f = fopen("/spiffs/vetable.csv", "r");
    if (f == NULL) {
        ESP_LOGE(SPFS, "Failed to open file for reading");
        return;
    }
    char line[150];


    while(fgets(line, sizeof(line), f)){
        i=0;
        

        token = strtok(line, ",");
        //printf("%s\n", token);
        if(! strcmp (token, "hPa")){
            token = strtok(NULL, ",");
            while (token != NULL) { 
                pre[i]=atoi(token);
                i++;
                token = strtok(NULL, ",");  
            }            
        }else if (! strcmp (token, "RPM")){
            token = strtok(NULL, ",");
            while (token != NULL) { 
                //printf("%s ",token);
                rpm[i]=atoi(token);
                i++;
                token = strtok(NULL, ","); 
            } 
        }else{
            while (token != NULL) { 
                //printf("%s ",token);
                VE[j][i]=atof(token);
                // printf("Row: %d | Col %d | Value: %f \n",j,i,VE[j][i]);
                token = strtok(NULL, ","); 
                i++;
            } 
            // printf("\n");
            j++;
        }   

    }
    fclose(f);
    return;


}



void prntVE(){

    //Print VE Table
    int i,j;

    ESP_LOGI(SPFS, "Pressure:");
    for(i=0; i< 16; i++){
        printf("%d ",pre[i]);
    }printf("\n");

    ESP_LOGI(SPFS,"RPM:");
    for(i=0; i< 12; i++){
        printf("%d ",rpm[i]);
    }printf("\n");

    ESP_LOGI(SPFS,"Volumetric Efficiency:");
    
    j=0;
    while(j<12){
        for(i=0; i< 16; i++){
            printf("%.2f ",VE[j][i]);
            
        }
        printf("\n");
        j++;
    }

    printf("\n"); 
    return;
}