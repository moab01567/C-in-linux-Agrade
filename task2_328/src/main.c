# include <stdio.h>
# include "debugger.h"
# include "../include/task2_count.h"
# include "../include/task2_hash.h"
# include "../include/task2_sum.h"
# include <string.h>
# include <stdlib.h>

typedef struct _TASK2_FILE_METADATA {
    char szFileName[32];
    int iFileSize;
    char byHash[4];
    int iSumOfChars;
    char aAlphaCount[26];
} FILE_METADATA;/*The name was to long, typedef to make it shorter*/

FILE_METADATA * InitFileMetadata(char szFileName[32]){
    FILE_METADATA *fmData = malloc(sizeof(FILE_METADATA));
    if(!fmData){
        errorDebug(__FILE__, __LINE__,"Could not allocate memory for FILE_METADATA");
        return NULL;
    }
    normalDebug(__FILE__, __LINE__,  "Allocate memory for FILE_METADATA successfully");
    strcpy(fmData->szFileName,szFileName);
    return fmData;
}
/*returns a file pointer and writes to debug */
FILE * GetFilePointer(char szFileName[32], char szMode[3]){
    FILE * pfile = fopen(szFileName, szMode);
    if(!pfile){
        errorDebug(__FILE__, __LINE__, "Could not open file with Name: %s Mode: %s ", szFileName,szMode);
        return NULL;
    }
    normalDebug(__FILE__, __LINE__,  "File pointer created FileName: %s Mode: %s successfully", szFileName, szMode);
    return pfile;
}
/*returns int  to the binary file and writes to debug */
int WriteToBinaryFile(FILE* fpOutputFile, void *fmpData, int iElementSize, int iTotalElements ){    
    if (iTotalElements != fwrite(fmpData, iElementSize, iTotalElements, fpOutputFile)) {
        errorDebug(__FILE__, __LINE__, "Failed to write all elements to binary file.");
        return 1; 
    }
    normalDebug(__FILE__, __LINE__, "Written to binary file successfully.");
    return 0;
}
/*just does a clean :)*/
void CleanupMemory( FILE *fpInputFile, FILE *fpOutputFile, FILE_METADATA *fmpData){
    if (fpInputFile){
        fclose(fpInputFile);
    }
    if (fpOutputFile){
        fclose(fpOutputFile);
    }
    if (fmpData){
        free(fmpData);
    }
    normalDebug(__FILE__, __LINE__, "Everything was freed successfully.");
}

int main(){
    FILE *fpInputFile = NULL;
    FILE *fpOutputFile = NULL;
    FILE_METADATA *fmpData = NULL;
    activeDebug(ENABLE_DEBUG); /*Turning the debugger ON*/
    
    fmpData = InitFileMetadata("pgexam25_test.txt");
    fpInputFile = GetFilePointer("pgexam25_test.txt","r");
    fpOutputFile = GetFilePointer("pgexam25_output.bin","wb");
    
    /*Checks if we could open file and allocate memory*/
    if(fmpData && fpInputFile && fpOutputFile){
        printf("Allocate Memory For TASK2_FILE_METADATA Struct...OK.\n");
        printf("Input File Found...OK.\n");
        printf("Output File Created...OK.\n");

        printf("Calling Method Task2_SimpleDjb2Hash...");
        Task2_SimpleDjb2Hash(fpInputFile,&fmpData->byHash);
        printf("done.\n");

        printf("Calling Method Task2_SizeAndSumOfCharacters...");
        Task2_SizeAndSumOfCharacters(fpInputFile, &fmpData->iFileSize, &fmpData->iSumOfChars);
        printf("done.\n");

        printf("Calling Method Task2_CountEachCharacter...");
        Task2_CountEachCharacter(fpInputFile, fmpData->aAlphaCount);
        printf("done.\n");
        
        printf("Calling Method WriteToBinaryFile...");
        if (WriteToBinaryFile(fpOutputFile,  fmpData, sizeof(FILE_METADATA), 1)){
            printf("failed. Check ./debug/debugLog.txt\n");
        }else printf("done.\n");
   
    }else{
        printf("Something went wrong, check ./debug/debugLog.txt for more info\n");
    }/*endif */
    CleanupMemory(fpInputFile, fpOutputFile, fmpData);
    fpInputFile = NULL;fpOutputFile = NULL;fmpData = NULL;
    printf("Everything was freed. Check with Valgrind to confirm.\n");
    return 0;
}