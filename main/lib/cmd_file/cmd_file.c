/* Console example — WiFi commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "includes.h"
//SPIFFS
#include "lib/spiffsVFS/spiffsVFS.h" //mounts spiffs partition

static const char* FCMD = "File command";



/** Arguments used by 'join' function */

static struct {
    struct arg_int *press;
    struct arg_int *revs;
    struct arg_end *end;
} ve_args;



//*****************************************************************************************//
//*****************************************************************************************//
//****************************** commands registration ************************************//
//*****************************************************************************************//
//*****************************************************************************************//





static int veDeb(int argc, char **argv)
{

    ve_args.press->ival[0] = -1;
    ve_args.revs->ival[0] = -1;

    int nerrors = arg_parse(argc, argv, (void **) &ve_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ve_args.end, argv[0]);
        return 1;
    }


    if(ve_args.press->ival[0] < 0 && ve_args.revs->ival[0] < 0){
        rdfile();
    }
    else if(ve_args.press->ival[0] < 0 || ve_args.revs->ival[0] < 0){

        printf(""
               "Error: Only one argument is NOT a valid option\n"
               "Please enter both hPa and RPMs\n"
        );
    }else
    {
        printf("Getting VE at: %d hPc & %d RPMs\n",ve_args.press->ival[0],ve_args.revs->ival[0]);
        interpolation(ve_args.press->ival[0],ve_args.revs->ival[0]);
    };
    return 0;
}




//---------- register command ------------//
void register_file(void)
{

    ve_args.press = arg_int0(NULL, NULL, "<hPa>", "Pressure");
    ve_args.revs = arg_int0(NULL, NULL, "<RMP>", "Revolutions Per Minute");
    ve_args.end = arg_end(2);

    const esp_console_cmd_t ve_cmd = {
        .command = "ve",
        .help = "Prints VE table or\n"
        "feeds data into interpolation\n\n"
        "Example:\n"
        "ve ------ prints VE table\n\n"
        "ve 350 5000 --- feeds data into interpolation",
        .hint = NULL,
        .func = &veDeb,
        .argtable = &ve_args
    };


    ESP_ERROR_CHECK( esp_console_cmd_register(&ve_cmd) );

}

//-------------------Read Ve table -----------------//
