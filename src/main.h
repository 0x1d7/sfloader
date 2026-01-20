#include <stdio.h>
#include <Windows.h>
#include <string.h>

#include "../include/sfloader/awe_dll.h"
#include "printv.h"

void help();
BOOL checkDRam();
BOOL initAWEMAN();
BOOL closeAWEMAN();
BOOL loadSoundFont(char*, char*, int, char*);
typedef struct {
    char path[_MAX_PATH];
    char type[32];
} SFENTRY;

SFENTRY getSfEntry(char *key);
char *getSetting(char *key); 
void listSoundFonts();
BOOL updateRegister(const char*, const char*, const char*);
char removeCitation(char*);
