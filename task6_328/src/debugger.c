# include "debugger.h"
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>


int  activeDebug(eDebug debugType){
    static int iDebuggerActive = 0;

    if (debugType == ENABLE_DEBUG){
        system("rm -rf ./debug");
        system("mkdir ./debug");
        system("touch ./debug/debugLog.txt");
        iDebuggerActive = 1;
    }else if (debugType == DISABLE_DEBUG){
        iDebuggerActive = 0;
    }

    return iDebuggerActive;
}

void normalDebug( char *cpzFile, int iLine, char *cpzFormat, ...){
    FILE *fpDEBUG;
    va_list args;
    va_start(args, cpzFormat);
    if (!activeDebug(GET_DEBUG)) return;
    fpDEBUG = fopen("./debug/debugLog.txt", "a");
    if(!fpDEBUG){
        printf("Could not append to debugger file.");
    }

    fprintf(fpDEBUG,"[Normal Debug] File: %s:%d  Message: ", cpzFile, iLine);
    vfprintf(fpDEBUG,cpzFormat, args);
    va_end(args);
    fprintf(fpDEBUG,"\n");
    fclose(fpDEBUG);
}

void warnDebug( char *cpzFile, int iLine, char *cpzFormat, ...){
    FILE *fpDEBUG;
    va_list args;
    va_start(args, cpzFormat);
    if (!activeDebug(GET_DEBUG)) return;
    fpDEBUG = fopen("./debug/debugLog.txt", "a");
    if(!fpDEBUG){
        printf("Could not append to debugger file.");
    }

    fprintf(fpDEBUG,"[Warn Debug] File: %s:%d  Message: ",cpzFile, iLine);
    vfprintf(fpDEBUG,cpzFormat, args);
    va_end(args);
    fprintf(fpDEBUG,"\n");
    fclose(fpDEBUG);
}

void errorDebug( char *cpzFile, int iLine, char *cpzFormat, ...){
    FILE *fpDEBUG;
    va_list args;
    va_start(args, cpzFormat);
    if (!activeDebug(GET_DEBUG)) return;
    fpDEBUG = fopen("./debug/debugLog.txt", "a");
    if(!fpDEBUG){
        printf("Could not append to debugger file.");
    }

    fprintf(fpDEBUG,"[Error Debug] File: %s:%d  Message: ",cpzFile, iLine);
    vfprintf(fpDEBUG,cpzFormat, args);
    va_end(args);
    fprintf(fpDEBUG,"\n");
    fclose(fpDEBUG);
}
      
