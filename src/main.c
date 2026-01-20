#include "main.h"

char const* const SETTINGS = "settings.cfg";
char const* const REGISTRY_PATH_KEY = "registry_path";
char const* const DELIMITER = "=";

LPFNAWEMANAGER  lpAWEManager = 0;
HANDLE hAWEMANDLL = 0;
AWEHANDLE hAWEHandle = 0;

int main(int argc, char* argv[]){
    char *pSoundFont = NULL;
    char *pSoundFontPath = NULL;
    char *pRegisterPath = NULL;
    char *pTypeOverride = NULL;
    BOOL checkDRamBool = FALSE;
    BOOL initBool = FALSE;
    BOOL listSfBool = FALSE;
    BOOL helpBool = FALSE;
    
    // parse startup arguments
    size_t i;
    for (i=1; i<argc; i++){
        if(argv[i][0] == '-'){
            switch (argv[i][1])  {
                case 's':
                    if(argc > i)
                        pSoundFont = argv[i+1];
			    	break;
                case 'p':
				    if(argc > i)
                        pSoundFontPath = argv[i+1];
			    	break;
                case 't':
                    if (argc > i)
                        pTypeOverride = argv[i+1];
                    break;
                case 'r':
				    if(argc > i)
                        pRegisterPath = argv[i+1];
			    	break;
                case 'i':
				    if(argc > i)
                        initBool = TRUE;
			    	break;
                case 'a':
				    if(argc > i)
                        checkDRamBool = TRUE;
			    	break;
                case 'l':
                    if(argc > 1)
                        listSfBool = TRUE;
                    break;
                case 'v':
				    if(argc > i)
                        setVerbose(TRUE);
			    	break;
			    case 'h':
				    helpBool = TRUE;
			    	break;
    		    default:
				    break;
		    }
        }
    }
    
    if(helpBool){
        help();
        return 0;
    }

    if(listSfBool){
        printf("..:: Listing available soundfonts in %s ::..\n", SETTINGS);
        listSoundFonts();
        return 0;
    }

    if(initBool){
        printf("..:: Initializing AWEMAN ::..\n");
        if(initAWEMAN()){
            printf("..:: Closing AWEMAN ::..\n");
            closeAWEMAN();
        }
        return 0;
    }

    if(checkDRamBool){
        printf("..:: Checking available DRAM ::..\n");
        checkDRam();
        return 0;
    }

    if(pSoundFontPath){
        printv("..:: Loading specified soundfont ::..\n");
        loadSoundFont(pSoundFontPath, pRegisterPath, FALSE, pTypeOverride);
        return 0;
    }

    if(pSoundFont){
        printv("..:: Loading soundfont from %s ::..\n", SETTINGS);
        loadSoundFont(pSoundFont, pRegisterPath, TRUE, pTypeOverride);
        return 0;
    }

    help();
    return 0;
}

void help(){
    printf(
        "Usage:\n"
        "	sfloader [options] <arg>\n"
        "\n"
        "Examples:\n"
        "	sfloader -s <key>\n"
        "	sfloader -s <key> -t <synth_type>\n"
        "	sfloader -p <file_path> -r <registry_key_path> -t <synth_type> -v\n"
        "	sfloader -i -v\n"
        "	sfloader -a\n"
        "\n"
        "Options:\n"
        "	-s <key>                Load soundfont <key> from %s\n"
        "	-p <file_path>          Load soundfont <file_path>\n"
        "	-r <registry_key_path>  Override regpath in %s\n"
        "	-t <type>               Set synth type ('GM', 'GS', 'MT', 'USER')\n"
        "	-i                      Test initialization of AWEMAN\n"
        "	-a                      Check available DRAM\n"
        "	-l                      List soundfont <key> in %s\n"
        "	-v                      Verbose\n"
        "	-h                      Help\n"
        "\n"
        "Source:\n"
        "	https://github.com/anderssonbilly/sfloader/ (MIT licensed)\n",
        SETTINGS, SETTINGS, SETTINGS);
}

BOOL initAWEMAN(){
    printv("Init AWEMAN...\n");
	hAWEMANDLL = LoadLibrary("AWEMAN32.DLL");

    if (hAWEMANDLL == NULL) {
        printv("AWEMAN init failed\n"
            "LoadLibrary(\"AWEMAN32.DLL\") returned NULL.\n");
        printv("Windows System Error Code: %lu\n", GetLastError());
		return FALSE;
	} else {
        printv("LoadLibrary found and loaded AWEMAN32.DLL successfully.\n");
    }

	lpAWEManager = (LPFNAWEMANAGER)GetProcAddress(hAWEMANDLL, "AWEManager");

	if( lpAWEManager == NULL ){
        printv("AWEMAN init failed\n"
            "AWEMAN32.DLL load failed\n"
            "GetProcAddress could not find the 'AWEManager' function in the DLL.\n");
        printv("Windows System Error Code: %lu\n", GetLastError());
		FreeLibrary(hAWEMANDLL);
		return FALSE;
	} else{
        printv("AWEMAN32.DLL loaded successfully\n");
    }
	
	if( !lpAWEManager((AWEHANDLE)0, AWE_OPEN, (LPARAM)(LPSTR)&hAWEHandle, 0) == AWE_NO_ERR ){
        printv("AWEMAN init Failed\n"
            "Unable to open AWEMAN\n");
        return FALSE;
    } else{
		printv("AWEMAN opened successfully\n");
    }

    return TRUE;
}

BOOL closeAWEMAN(){
        printv("Closing AWEMAN...\n");
	if ( lpAWEManager(hAWEHandle, AWE_CLOSE, 0, 0 ) == AWE_NO_ERR ){
        printv("AWEMAN closed successfully\n");
        return TRUE;
    } else{
        printv("Unable to close AWEMAN\n");
    }
    return FALSE;
}

BOOL checkDRam(){
    DWORD dwMaxDRAM, dwAvailableDRAM,
    maxDRAMB, maxDRAMKB, maxDRAMMB,
    availableDRAMB, availableDRAMKB, availableDRAMMB;

    if(initAWEMAN()){
        if ( lpAWEManager(hAWEHandle,
				    AWE_QUERY_DRAM_SIZE,
				    (LPARAM)(LPSTR)&dwMaxDRAM,
				    (LPARAM)(LPSTR)&dwAvailableDRAM) == AWE_NO_ERR ){
                        maxDRAMB = dwMaxDRAM * 2;
                        maxDRAMKB = maxDRAMB / 1024;
                        maxDRAMMB = maxDRAMKB / 1000;
                        availableDRAMB = dwAvailableDRAM * 2;
                        availableDRAMKB = availableDRAMB / 1024;
                        availableDRAMMB = availableDRAMKB / 1000;

                        printf(
                            "Total DRAM:\n"
                            "   B  = %lu\n"
                            "   kB = %lu\n"
                            "   MB = %lu\n"
                            "\n"
                            "Available DRAM:\n"
                            "   B  = %lu\n"
                            "   kB = %lu\n"
                            "   MB = %lu\n",
                            maxDRAMB, maxDRAMKB, maxDRAMMB,
                            availableDRAMB, availableDRAMKB, availableDRAMMB);

                        return TRUE;
                    }else{
                        printf("Unable to check available DRAM\n");
                    }
    } else {
        printf("Unable to check available DRAM\n");
    }

    return FALSE;
}

BOOL loadSoundFont(char *sf, char *r, BOOL c, char *typeOverride){
    char sfPath[_MAX_PATH]       = { 0 };
    char registerPath[_MAX_PATH] = { 0 };
    char *b;
    char *pType;

    if (initAWEMAN()) {

        if (c) {
            SFENTRY entry = getSfEntry(sf);

            if (entry.path[0] == 0) {
                printv("invalid soundfont entry\n");
                return FALSE;
            }

            strcpy(sfPath, entry.path);

            if (typeOverride != NULL)
                pType = typeOverride;
            else
                pType = entry.type;

        } else {
            strcpy(sfPath, sf);

            if (typeOverride != NULL)
                pType = typeOverride;
            else
                pType = "GM";   /* default type when using -p */
        }

        if (r == NULL) {
            printv("getting registry path from %s\n", SETTINGS);
            b = getSetting((char*)REGISTRY_PATH_KEY);
            if (b == NULL) {
                printv("registry path not found\n");
                return FALSE;
            }
            strcpy(registerPath, b);
        } else {
            strcpy(registerPath, r);
        }

        printv("updating registry path \"%s\"\n"
               "with\n"
               "soundfont \"%s\"\n",
               registerPath, sfPath);

        if (updateRegister(registerPath, sfPath, pType)) {

            if (lpAWEManager(hAWEHandle,
                             AWE_SELECT_SYN_BANK,
                             GENERAL_MIDI,
                             0) == AWE_NO_ERR) {

                printv("general midi reloaded successfully\n");

            } else {
                printv("general midi reload failed\n");
                closeAWEMAN();
                return FALSE;
            }

            return closeAWEMAN();
        }
    }

    return FALSE;
}

char *getSetting(char *key){
    FILE *file;
    char line[256];
    char keyInFile[_MAX_PATH] = { '\0' };
    static char valueBuf[_MAX_PATH];

    strncpy(keyInFile, key, strlen(key));
    strcat(keyInFile, DELIMITER);

    printv("opening %s...\n", SETTINGS);

    if ((file = fopen(SETTINGS, "r"))){
        printv("getting value of key \"%s\"...\n", key);

        while (fgets(line, sizeof(line), file)) {
            char *b = strchr(line, *DELIMITER);
            if(strncmp(line, keyInFile, strlen(keyInFile)) == 0){
                removeCitation(b);
                b[strcspn(b, "\r\n")] = 0;
                fclose(file);

                strncpy(valueBuf, b+1, sizeof(valueBuf)-1);
                valueBuf[sizeof(valueBuf)-1] = '\0';

                printv("value of \"%s\" = \"%s\"\n", key, valueBuf);
                return valueBuf;
            }
        }
        printv("key \"%s\" not found\nclosing %s...\n", key, SETTINGS);
        fclose(file);
        return NULL;
    }

    printv("%s not found\n", SETTINGS);
    return NULL;
}

SFENTRY getSfEntry(char *key){
    FILE *file;
    char line[256];
    SFENTRY entry;
    char keyInFile[_MAX_PATH];
    char *value;
    char *colon;

    entry.path[0] = '\0';
    entry.type[0] = '\0';

    keyInFile[0] = '\0';
    strncpy(keyInFile, key, strlen(key));
    keyInFile[strlen(key)] = '\0';
    strcat(keyInFile, DELIMITER);

    printv("opening %s...\n", SETTINGS);

    file = fopen(SETTINGS, "r");
    if (file) {
        printv("getting value of key \"%s\"...\n", key);

        while (fgets(line, sizeof(line), file)) {

            if (strncmp(line, keyInFile, strlen(keyInFile)) == 0) {

                value = strchr(line, '=');
                if (value == NULL) {
                    fclose(file);
                    return entry;
                }

                value++;
                removeCitation(value);
                value[strcspn(value, "\r\n")] = '\0';

                colon = strrchr(value, ':');
                if (colon == NULL) {
                    printv("no type specified for key \"%s\"\n", key);
                    fclose(file);
                    return entry;
                }

                *colon = '\0';

                strncpy(entry.path, value, sizeof(entry.path) - 1);
                entry.path[sizeof(entry.path) - 1] = '\0';

                strncpy(entry.type, colon + 1, sizeof(entry.type) - 1);
                entry.type[sizeof(entry.type) - 1] = '\0';

                printv("value of \"%s\" = \"%s\" (type=%s)\n",
                       key, entry.path, entry.type);

                fclose(file);
                return entry;
            }
        }

        printv("key \"%s\" not found\nclosing %s...\n", key, SETTINGS);
        fclose(file);
        return entry;
    }

    printv("%s not found\n", SETTINGS);
    return entry;
}

void listSoundFonts(){
    FILE *file;
    char line[256];
    char keyInFile[_MAX_PATH] = { '\0' };
    strncpy(keyInFile, REGISTRY_PATH_KEY, strlen(REGISTRY_PATH_KEY));
    strcat(keyInFile, DELIMITER);

    printv("opening %s...\n"
        "listing available soundfonts...\n", SETTINGS);

    if ((file = fopen(SETTINGS, "r"))){
        while (fgets(line, sizeof(line), file)) {
            if(strncmp(line, keyInFile, strlen(keyInFile)) != 0){
                line[strcspn(line, "\r\n")] = 0;
                removeCitation(line);
                printf("%s\n", line);
            }
		}

        printv("closing %s...\n", SETTINGS);
        fclose(file);
        return;
    };

    printv("%s not found\n", SETTINGS);
    return;
}

BOOL updateRegister(const char *registerPath, const char *sfPath, const char *valueName){
    struct {
        const char *name;
        HKEY root;
    } hives[] = {
        { "HKEY_CLASSES_ROOT",  HKEY_CLASSES_ROOT  },
        { "HKEY_CURRENT_USER",  HKEY_CURRENT_USER  },
        { "HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE },
        { "HKEY_USERS",         HKEY_USERS         },
    };

    HKEY root = NULL;
    const char *subkey = NULL;
    size_t prefix_len = 0;
    HKEY phKey;
    int i;

    for (i = 0; i < sizeof(hives) / sizeof(hives[0]); i++) {
        prefix_len = strlen(hives[i].name);

        if (strncmp(registerPath, hives[i].name, prefix_len) == 0) {
            root = hives[i].root;
            break;
        }
    }

    if (!root) {
        printv("registry path error, could not parse hkey\n");
        return FALSE;
    }

    if (registerPath[prefix_len] != '\\') {
        printv("registry path missing backslash\n");
        return FALSE;
    }

    subkey = registerPath + prefix_len + 1;

    if (RegCreateKeyExA(root, subkey, 0, NULL, 0,
                        KEY_ALL_ACCESS, NULL, &phKey, NULL) != ERROR_SUCCESS) {
        printv("unable to open or create registry path\n");
        return FALSE;
    } else {

        LONG res;
        char buf[128];

        res = RegSetValueExA(
            phKey,
            valueName,
            0,
            REG_SZ,
            (const BYTE*)sfPath,
            lstrlenA(sfPath) + 1
        );

        wsprintf(buf, "RegSetValueExA returned %ld\n", res);
        printv(buf);

        if (res == ERROR_SUCCESS) {
            printv("registry was updated successfully\n");
            RegCloseKey(phKey);
            return TRUE;
        } else {
            char buf[64];
            wsprintf(buf, "registry update failed, error=%ld\n", res);
            printv(buf);
        }
    }

    RegCloseKey(phKey);
    return FALSE;
}

char removeCitation(char *string){
    const char citation = '\"';
    char *pr = string, *pw = string;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != citation);
    }
    *pw = '\0';
    return *string;
}
