# include <stdio.h>
# include "debugger.h"
# include <string.h> 
# include <stdlib.h>
# include "../include/TEA.h"

/*the communication flow in decrypt.c
main.c->decrypt.c
from decrypt.c, main.c only calls method startDecrypting(char *cpEncBody, int iBodySize)

*/

/* write to file, method is call from startDecrypt() when done decrypting*/
void writeToFile(char* szFileName, unsigned char * szText, int iSize){
    FILE * fpFile = NULL;
    fpFile = fopen(szFileName, "w");
    if (!fpFile){
        errorDebug(__FILE__, __LINE__, "failed to create file");
        return;
    }
    fwrite(szText,sizeof(char),iSize,fpFile);
    fclose(fpFile);
}
/*try to decrypt file with key and then validate it 
return -1 if failed, else 0*/
int decrypt(char *cpEncBody, unsigned char *cpDecBody, int iBodySize,  int tryNumber){
    unsigned int iaKey[4] = {0};
    unsigned int v[2] = {0};  
    unsigned int w[2] = {0};
    int i = 0;
    /*Set the key*/
    memset(iaKey, tryNumber, sizeof(iaKey));
    /*decrypting the data*/
    for (i = 0; i < iBodySize / 8; i++) {
        memcpy(v, cpEncBody + (i * 8),  8); 
        decipher(v, w, iaKey);/*method in TEA.c*/
        memcpy(cpDecBody + (i * 8), w, 8);
    }
    /*validate the decrypted data*/
    for (i = 0; i < iBodySize; i++) {
        if (cpDecBody[i] > 127) {
            return -1;
        }
    }
    printf("\ntryNumber: %d, key[0]: 0x%x\n", tryNumber, iaKey[0]);
    printf("tryNumber: %d, key[1]: 0x%x\n", tryNumber, iaKey[1]);
    printf("tryNumber: %d, key[2]: 0x%x\n", tryNumber, iaKey[2]);
    printf("tryNumber: %d, key[3]: 0x%x\n", tryNumber, iaKey[3]);
    return 0;
}

/*start decrypting return -1 if failed else 0*/
int startDecrypting(char *cpEncBody, int iBodySize){
    int i = 0;
    unsigned char * cpDecBody = NULL;/*the decrypted data will be in her :)*/
    int iIsDecrypted = 0;

     /*check if body divideble by 8*/
    if (iBodySize % 8 != 0) {
        errorDebug(__FILE__, __LINE__, "body size must be a multiple of 8");
        return -1;
    }
    /*allocate memory*/
    cpDecBody = malloc(iBodySize + 1);
    if (!cpDecBody){
        errorDebug(__FILE__, __LINE__, "Could not allocate memory");
        return -1;
    }
    /*loop through the possible combinations*/
    printf("Try Numbers: \n");
    for(i = 0; i < 256; i++){
        printf("0x%02x ",i);
        memset(cpDecBody,0,iBodySize + 1);/*reset cpDecBody*/
        if(decrypt(cpEncBody,cpDecBody,iBodySize, i) != -1){/*if found break*/
            iIsDecrypted = 1;
            break;
        }
    }
    /*check if it could decrypt message */
    printf("\n");
    if (!iIsDecrypted){
        printf("Could no find, please try again. the program work :)\n");
        free(cpDecBody);
        return -1;
    }
    
    printf("%s\n", cpDecBody);
    writeToFile("decrypted.txt",cpDecBody,iBodySize);/*write to file*/
    free(cpDecBody);
    return 0;
}

