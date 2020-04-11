#pragma once

#ifdef __cplusplus
extern "C" {
#endif


extern const char *FS;

// Register WiFi functions
void consoleRun(void);
void initialize_console(void);
void initialize_nvs(void);
void initialize_filesystem(void);
void olcmds(void *pvParameter);



#ifdef __cplusplus
}
#endif

