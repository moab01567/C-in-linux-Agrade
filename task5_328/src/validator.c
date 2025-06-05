# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>
# include <sys/socket.h>  
# include <unistd.h>
# include "../include/requestHandler.h"
# include "../include/validator.h"
# include "../include/debugger.h"
# include "../include/ewpdef.h"
/*validate header and set the Datasize (header + body) */
int validateHeader(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER seHeader, int * ipDataSize){
    int iHeaderSize = sizeof(seHeader);
    char szMagicNumberBuffer[4] = {0};
    char szDelimeter[2] = {0};
    char szDataSize[5] = {0};
    memcpy(szMagicNumberBuffer, seHeader.acMagicNumber, 3);
    memcpy(szDelimeter, seHeader.acDelimeter, 1);
    memcpy(szDataSize, seHeader.acDataSize, 4);
    /*check magic number*/
    if (strcmp(szMagicNumberBuffer, EWA_EXAM25_TASK5_PROTOCOL_MAGIC) != 0){
        errorDebug(__FILE__, __LINE__, "Wrong Magic Number expected %s but got %s", EWA_EXAM25_TASK5_PROTOCOL_MAGIC, szMagicNumberBuffer);
        return  1;
    }
    /*check Delimeter*/
    if (szDelimeter[0] != '|'){
        errorDebug(__FILE__, __LINE__, "Wrong Delimeter expected %s but got %s","|",szDelimeter);
        return 1;
    }
    /*check sDataSize and make sure its bigger then header*/
    if (iHeaderSize >= atoi(szDataSize)){
        errorDebug(__FILE__, __LINE__, "Not valid DataSize expected xxxx but got  %s", szDataSize);
        return 1;
    }
    *ipDataSize = atoi(szDataSize);
    normalDebug(__FILE__, __LINE__, "valid header Info");
    return 0;
}
/*check if Zero terminator there 
this is important, we dont handle body with no zero terminators.
this is method is used by receiveClientMessage() in requestHandler.c*/
int validBody( char * cpBodyBuffer, int iBodySize){
    
    /*validating zero terminators */
    if (cpBodyBuffer[iBodySize - 1] != '\0'){
        errorDebug(__FILE__, __LINE__, "missing zero terminator");
        return 1;
    }
    return 0;
}
/*validate the body, after casting is done. 
except StringFormat. string format is validated one the method under */
int validateStruct(char * acCommand, char *CMD_TYPE, char * acHardSpace, char *acHardZero){
    if (strncmp(acCommand, CMD_TYPE, strlen(CMD_TYPE)) != 0){
        errorDebug(__FILE__, __LINE__, "wrong command %s",CMD_TYPE);
        return 1;
    }
    if (acHardSpace[0] != ' '){
        errorDebug(__FILE__, __LINE__, "wrong HardSpace");
        return 1;
    }
    if (acHardZero[0] != '\0'){
        errorDebug(__FILE__, __LINE__, "missing zero terminator at the end");
        return 1;
    }
    return 0;
}
/*validate string body*/
int validateStringFormat(char *acFormattedString, int iFormattedStringBytes){
    int iFoundTerminator = 0;
    int i = 0;
    /*check if terminator pressent */
    for (i = 0; i < iFormattedStringBytes; i++){
        if (acFormattedString[i] == '\0') {
            iFoundTerminator = 1;
            break;
        }
    }
    if (!iFoundTerminator) {
        errorDebug(__FILE__, __LINE__, "missing zero terminator in FormattedString");
        return 1;
    }
    /*check if no chars*/
    if (strlen(acFormattedString) == 0){
        errorDebug(__FILE__, __LINE__, "FormattedString contains no chars");
        return 1;
    }
    printf("%s\n",acFormattedString);
    return 0;
}
/*simple validate user name and ip*/
int validateUsernameAndIPAddress( char *acFormattedString, int iFormattedStringBytes, char szUserName[25], char szIpaddress[13]){
    int iCountDot = 0;
    int i = 0;
    int iFormattedSizeLen = 0;
    char caUserName[25] = {0};
    int ip1, ip2, ip3, ip4;
    
    if (validateStringFormat(acFormattedString,iFormattedStringBytes))return 1;
    /*cout '.' */
    iFormattedSizeLen =  strlen(acFormattedString);
    if (acFormattedString[0] == '.' || acFormattedString[iFormattedSizeLen - 1] == '.')return 1;
    for (i = 0; i < iFormattedSizeLen; i++) {
        if (acFormattedString[i] == '.' &&  iCountDot < 4 ) {
            iCountDot++;
        };
    }
      /*check if we have 4  '.' */
    if(iCountDot != 4){
        errorDebug(__FILE__, __LINE__, "Make sure you have 4 '.' only");
        return 1;
    }

    /*use '.' insted of ' ' space as separators  */
    if (sscanf(acFormattedString, "%24[^.].%3d.%3d.%3d.%3d",caUserName, &ip1, &ip2, &ip3, &ip4) != 5) {
        errorDebug(__FILE__, __LINE__, "Not right format");
        return 1;
        }
    sprintf(szUserName, "%s" ,caUserName);
    sprintf(szIpaddress, "%d.%d.%d.%d" ,ip1, ip2, ip3, ip4);
    return 0;
}
/*simple fileName validator */
int validateFileName(char *acFormattedString, int iFormattedStringBytes){
    char cChar = '\0';
    int i = 0;
    int iFilenameSize = 0;
    normalDebug(__FILE__, __LINE__, "Validating filename");
    
    /*check for zero terminator*/
    if (validateStringFormat(acFormattedString, iFormattedStringBytes))return 1;
    iFilenameSize = strlen(acFormattedString);
    if (strstr(acFormattedString, "..") != NULL){
        errorDebug(__FILE__, __LINE__, "File Name ERROR");
        return 1;
    }

    for ( i = 0; i < iFilenameSize; i++) {
        cChar = acFormattedString[i];
        if (97 <= cChar && cChar <=122 )continue;
        if (65 <= cChar && cChar <=90 )continue;
        if (48 <= cChar && cChar <=57 )continue;
        if (cChar == '_')continue;
        if (cChar == '-')continue;
        if (cChar == '.')continue;
        errorDebug(__FILE__, __LINE__, "File Name ERROR");
        return 1;
    }

    normalDebug(__FILE__, __LINE__, "Done Validating Filename");
    return 0;
}

/*simple mail validator */
int validateMail(char *acFormattedString, int iFormattedStringBytes){
    char cChar = '\0';
    int i = 0;
    int iMailSize = 0;
    normalDebug(__FILE__, __LINE__, "Validating Mail");
    /*check for zero terminator*/
    if (validateStringFormat(acFormattedString, iFormattedStringBytes))return 1;
    iMailSize = strlen(acFormattedString);

    if (acFormattedString[0] == '.' || acFormattedString[iMailSize - 1] == '.'){
        errorDebug(__FILE__, __LINE__, "Mail ERROR");
        return 1;
    }
    if (strstr(acFormattedString, "..") != NULL){
        errorDebug(__FILE__, __LINE__, "Mail ERROR");
        return 1;
    }

    for ( i = 0; i < iMailSize; i++) {
        cChar = acFormattedString[i];
        if (97 <= cChar && cChar <=122 )continue;
        if (65 <= cChar && cChar <=90 )continue;
        if (48 <= cChar && cChar <=57 )continue;
        if (cChar == '.')continue;
        if (cChar == '@')continue;
        if (cChar == '<')continue;
        if (cChar == '>')continue;
        errorDebug(__FILE__, __LINE__, "Mail ERROR failed on char %d",cChar);
        return 1;
    }
    normalDebug(__FILE__, __LINE__, "Done Validating Mail");
    return 0;
}