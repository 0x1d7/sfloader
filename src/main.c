#include "main.h"

char const* const SETTINGS = "settings.cfg";
char const* const REGISTRY_PATH_KEY = "register_path";
char const* const GENERAL_MIDI_KEY = "GM";
char const* const DELIMITER = "=";

LPFNAWEMANAGER  lpAWEManager = 0;
HANDLE hAWEMANDLL = 0;
AWEHANDLE hAWEHandle = 0;

int main(int argc, char* argv[]){
    char *pSoundFont = NULL;
    char *pSoundFontPath = NULL;
    char *pRegisterPath = NULL;
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
        printf("..:: Help ::..\n");
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
        loadSoundFont(pSoundFontPath, pRegisterPath, FALSE);
        return 0;
    }

    if(pSoundFont){
        printv("..:: Loading soundfont from %s ::..\n", SETTINGS);
        loadSoundFont(pSoundFont, pRegisterPath, TRUE);
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
        "Example:\n"
        "	sfloader -s <key>\n"
        "	sfloader -p <key> -r <path> -v\n"
        "	sfloader -i -v\n"
        "	sfloader -a\n"
        "\n"
        "Options:\n"
        "	-s <key>	Load soundfont <key> from %s\n"
        "	-p <path>	Load soundfont <path>\n"
        "	-r <path>	Override regpath in %s\n"
        "	-i		Test initialization of AWEMAN\n"
        "	-a		Check available DRAM\n"
        "	-l		List soundfont <key> in %s\n"
        "	-v		Verbose\n"
        "	-h		Help\n",
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
    } else
        printf("Unable to check available DRAM\n");
    return FALSE;
}

BOOL loadSoundFont(char *sf, char *r, BOOL c){
    char sfPath[_MAX_PATH] = { '\0' };
    char registerPath[_MAX_PATH] = { '\0 '};

    char *b; // tmp buffer

    if(initAWEMAN()){
        if(c){
            printv("getting soundfont path of key \"%s\" in %s\n", sf, SETTINGS);
            b = getSetting(sf);
            strcpy(sfPath, b);
            if(sfPath == NULL)
                return FALSE;
        }
        else{
            printv("soundfont to load: %s\n", sf);
            strcpy(sfPath, sf);
        }

        if(r == NULL){
            printv("getting register path from %s\n", SETTINGS);
            b = getSetting((char*)REGISTRY_PATH_KEY);
            strcpy(registerPath, b);
            if(registerPath == NULL)
                return FALSE;
        } else{
            strcpy(registerPath, r);
        }
        printv("updating register path \"%s\"\n"
            "with\n"
            "soundfont \"%s\"\n", 
        registerPath, sfPath);
        
        if(updateRegister(registerPath, sfPath)){
            if ( lpAWEManager(hAWEHandle, AWE_SELECT_SYN_BANK, GENERAL_MIDI, 0) == AWE_NO_ERR ){
                printv("general midi reloaded successfully");
            }else{
                printv("general midi reload failed");
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
    strncpy(keyInFile, key, strlen(key));
    strcat(keyInFile, DELIMITER);

    printv("opening %s...\n", SETTINGS);

    // if file is found get value of key
    if ((file = fopen(SETTINGS, "r"))){
        printv("getting value of key \"%s\"...\n", key);

        // loop each line in settings.cfg
        while (fgets(line, sizeof(line), file)) {
			// split line by delimiter
			char *b = strchr(line, *DELIMITER);
			// check if start of line matches key
			if(strncmp(line, keyInFile, strlen(keyInFile)) == 0){
				// if match, remote citation from string
                removeCitation(b);
				// remove trailing newline
				b[strcspn(b, "\r\n")] = 0;
                printv("closing %s...\n", SETTINGS);
                // close file
				fclose(file);
				// return value of key
                printv("value of \"%s\" = \"%s\"\n", key, b+1);
				return b+1;
			}
		}
        printv("key \"%s\" not found\n"
            "closing %s...\n", key, SETTINGS);
        fclose(file);
        return '\0';
    }

    // file not fount
    printv("%s not found\n", SETTINGS);
    return '\0';
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

BOOL updateRegister(char *registerPath, char *sfPath){
    HKEY hKey;
    HKEY phKey;
    wchar_t pRegisterPath[_MAX_PATH];

    if(strncmp("HKEY_CLASSES_ROOT", registerPath, strlen("HKEY_CLASSES_ROOT")) == 0){
        hKey = HKEY_CLASSES_ROOT;
        mbstowcs(pRegisterPath, registerPath+18, _MAX_PATH);
    }
    else if(strncmp("HKEY_CURRENT_USER", registerPath, strlen("HKEY_CURRENT_USER")) == 0){
        hKey = HKEY_CURRENT_USER;
        mbstowcs(pRegisterPath, registerPath+18, _MAX_PATH);
    }
    else if(strncmp("HKEY_LOCAL_MACHINE", registerPath, strlen("HKEY_LOCAL_MACHINE")) == 0){
        hKey = HKEY_LOCAL_MACHINE;
        mbstowcs(pRegisterPath, registerPath+19, _MAX_PATH);
    }
    else if(strncmp("HKEY_USERS", registerPath, strlen("HKEY_USERS")) == 0){
        hKey = HKEY_USERS;
        mbstowcs(pRegisterPath, registerPath+11, _MAX_PATH);
    }
    else{
		printv("registry path error, could not parse hkey\n");
        return FALSE;
    }

	if(RegOpenKeyExW(hKey, pRegisterPath, 0, KEY_READ, &phKey) == ERROR_SUCCESS){
		if(RegSetValueExA(phKey, GENERAL_MIDI_KEY, 0, REG_SZ, (PBYTE)sfPath, lstrlenA(sfPath)+1) == ERROR_SUCCESS){
            printv("registry was updated successfully\n");
            return TRUE;
		}else{
            printv("registry update failed\n");
		}
		RegCloseKey(phKey);
	}else{
        printv("unable to open registry path\n");
    }
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
