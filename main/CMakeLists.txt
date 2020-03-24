idf_component_register(SRCS "lib/cmd_wifi/cmd_wifi.c"
                            "lib/cmd_file/cmd_file.c"
                            "ecm_controls.c"
                            "lib/console/cons.c"
                            "lib/spiffsVFS/spiffsVFS.c"
                            #"sntp/sntp.c"
                            "lib/peripherals/perinit.c"
                    INCLUDE_DIRS ".")

# Create a SPIFFS image from the contents of the 'spiffs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'. 
spiffs_create_partition_image(home ../data FLASH_IN_PROJECT)