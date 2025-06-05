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
#include <time.h>


/*Remember errorDebug happens where error can be possible*/


/*this method make sure that we get all the data from socket. 
if something goes wrong. return -1 
else return the total data received
and  Remember always validate the data received from this method 
*/
int recvAllData(int iClientSocket, void * vpBuffer, int iDataSize) {
    int iTotalBytesReceived = 0;
    int iBytesReceived = 0;
    int iTry = 0;
    char *cpBuffer = (char*) vpBuffer;
    while (iTotalBytesReceived < iDataSize && iTry < 3) {
        iBytesReceived = recv(iClientSocket, cpBuffer, iDataSize - iTotalBytesReceived, MSG_DONTWAIT);

        if (iBytesReceived > 0) {
            /*adding to total bytes and to cpBuffer*/
            iTotalBytesReceived += iBytesReceived;
            cpBuffer += iBytesReceived;
            iTry = 0;  
        } else if (iBytesReceived == 0) {
            errorDebug(__FILE__, __LINE__, "Connection closed");
            return -1;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* if data is late max try is 3 times*/
                sleep(1);
                iTry++;
                warnDebug(__FILE__, __LINE__, "%s Try: %d", strerror(errno),iTry);
                continue;
            } else {
                /*if socket error*/
                errorDebug(__FILE__, __LINE__, "%s", strerror(errno));
                return -1;
            }
        }
    }

    return iTotalBytesReceived;
}

/*this method use the method above to get the data from socket, 
and then validate it with methods in validator.c  */
char * receiveClientMessage(int iClientSocket, int *ipDataSize) {
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER seHeader;
    int iHeaderSize = sizeof(seHeader);
    int iDataSize = 0;
    char *cpDataBuffer = NULL;
    char *cpBodyBuffer = NULL;
    int iBodySize = 0;
    normalDebug(__FILE__, __LINE__, "Preparing server receive data");
    
    /*make sure we at least have the header*/
    if (recvAllData(iClientSocket, &seHeader, iHeaderSize) != iHeaderSize){
        errorDebug(__FILE__, __LINE__, "Received Wrong data size. expected size: %d", iHeaderSize);
        return NULL;
    }
    /*validate the header content validator.c and get the iDatasize max is 9999*/
    if (validateHeader(seHeader, &iDataSize))
        return NULL;
    
     /*Now we can allocate memory because the header was validated */
    cpDataBuffer = malloc(iDataSize);
    if (!cpDataBuffer){
        errorDebug(__FILE__, __LINE__, "Could not allocate memory, ");
        return NULL;
    }
    /*copy header to new memory*/
    memcpy(cpDataBuffer, &seHeader, iHeaderSize);

    /*get body*/
    cpBodyBuffer = cpDataBuffer + iHeaderSize;
    iBodySize = iDataSize - iHeaderSize;
    /*validate body size important!! so we make sure everything there.*/
    if (recvAllData(iClientSocket, cpBodyBuffer, iBodySize) != iBodySize) {
        errorDebug(__FILE__, __LINE__, "Received Wrong data size. expected size: %d", iBodySize);
        free(cpDataBuffer);
        return NULL;
    }
    /* important to validate body to make sure wi have zero teminator, */
    if (validBody(cpBodyBuffer, iBodySize)){
        free(cpDataBuffer);
        return NULL;
    }
    if (ipDataSize) *ipDataSize = iDataSize;
    return cpDataBuffer;
}

/*this method trys to find the command, if command not found return 1
else return 0
Make sure, before you use this method. that the body size is bigger than header 
use the receiveClientMessage() method above when getting the data. Because then the body and header is validated */
int findClientCommend(char* szCommandBuffer, char * cpDataBuffer){
    int iHeaderSize = sizeof(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER);
    int i = 0;
    char *validCommands[] = {CMD_HELO, CMD_MAIL_FROM, CMD_RCPT_TO, CMD_DATA, CMD_QUIT};
    normalDebug(__FILE__, __LINE__, "Try to find  command");
    memset(szCommandBuffer, 0, MAX_COMMAND_LENGTH);/*max comand length if defined in headerfile */
    for(i = 0; i < 5; i++){
        if (strncmp(cpDataBuffer + iHeaderSize, validCommands[i], strlen(validCommands[i])) == 0 ){
            strncpy (szCommandBuffer,validCommands[i],MAX_COMMAND_LENGTH);
            normalDebug(__FILE__, __LINE__, "Command Found: %s", szCommandBuffer);
            return 0;
        }
    }
    errorDebug(__FILE__, __LINE__, "Command Not Found");
    return 1;
}

/*building the header */
struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER buildHeader(int iDataSize){
    int i = 0;
    int j = 0;
    char caDataBufferSize[12];
    char acDataSize[4] = {'0', '0', '0', '0'};
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER head;

    normalDebug(__FILE__, __LINE__, "Building header for data size: %d", iDataSize);
    
    memcpy(head.acMagicNumber,EWA_EXAM25_TASK5_PROTOCOL_MAGIC, strlen(EWA_EXAM25_TASK5_PROTOCOL_MAGIC));
    sprintf(caDataBufferSize,"%d", iDataSize);
    
    /*ble stolt av denne algoritmen xD hahaha, legger til padding*/
    j = strlen(caDataBufferSize) - 1;
    if (j > 3)j = 3;
    for(i = 3; i >= 0; i--){
        if (j < 0 )break;
        acDataSize[i] = caDataBufferSize[j];
        j--;
    }
    memcpy(head.acDataSize, acDataSize, 4);
    head.acDelimeter[0] ='|';

    return head;
}

/*this method is use by 
handleData, handleQuit, handleMailFrom and handleRcptTo
all of them use the same structure when sending data 
return 1 if client not received all the data or if the message was to big to  for struct 
else return 0*/
int sendServerReplay(int iClientSocket, char * szMessage , char *statusCode ){
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY seServerReplay;
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER header = buildHeader(sizeof(seServerReplay));
    char szFormattedString[51] = {0};
    normalDebug(__FILE__, __LINE__, "Preparing server reply: statusCode=%s, message=%s", statusCode, szMessage);
    printf("Preparing server reply: \nstatusCode=%s \nmessage=%s\n", statusCode, szMessage);
    if (sizeof(szFormattedString) <= sizeof(szMessage)) {
        errorDebug(__FILE__, __LINE__, "Message was too large");
        return 1;
    }else{
        sprintf(szFormattedString, szMessage);
    }   
    
    memcpy(&seServerReplay.stHead, &header, sizeof(header));
    memcpy(seServerReplay.acStatusCode, statusCode, 3);
    memcpy(seServerReplay.acHardSpace, " ", 1);
    memcpy(seServerReplay.acFormattedString, szFormattedString, 51);
    memcpy(seServerReplay.acHardZero, "\0", 1);
    /*validating that the message was send*/
    if (send(iClientSocket, &seServerReplay, sizeof(seServerReplay), 0) != sizeof(seServerReplay)){
        errorDebug(__FILE__, __LINE__, "%s", strerror(errno));
        return 1;
    }
    
    normalDebug(__FILE__, __LINE__, "send server reply: statusCode=%s, message=%s", statusCode, szMessage);
    return 0;
}

/*handle server accept, return 1 if failed and else 0*/
int  handleServerAccept(int iClientSocket,char * szId){
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT seServerAcceptRequest;
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER header = buildHeader(sizeof(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT));
    /*https://stackoverflow.com/questions/5141960/get-the-current-time-in-c*/
    char  cBufferFormatting[46] = {0};   
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    if(strlen(szId) > 10){
        szId[9] = '\0'; 
    }
    sprintf(cBufferFormatting, "127.0.0.1 %s %s", szId, asctime(timeinfo));
    normalDebug(__FILE__, __LINE__, "Handle ServerAccept");
    memset(&seServerAcceptRequest, 0, sizeof(seServerAcceptRequest));
    /*setting up SERVERACCEPT struct*/
    memcpy(&seServerAcceptRequest.stHead, &header, sizeof(header));
    memcpy(seServerAcceptRequest.acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY, strlen(EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY));
    seServerAcceptRequest.acHardSpace[0] = ' ';
    memcpy(seServerAcceptRequest.acFormattedString,cBufferFormatting, 46);
    seServerAcceptRequest.acHardZero[0] = '\0';
   
    /*validating that the message was send*/
    if (send(iClientSocket, &seServerAcceptRequest, sizeof(seServerAcceptRequest), 0) != sizeof(seServerAcceptRequest) ){
        errorDebug(__FILE__, __LINE__, "%s", strerror(errno));
        return 1;
    }
    
    return 0;
}

/*handle client helo, return 1 if error else 0 */
int handleClientHelo(int iClientSocket, char * cpDataBuffer, int iDataSize){
    char szUserName[25] = {0};
    char szIpaddress[13] = {0};
    char szFormattedString[51] = {0};
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTHELO seClientHelo;
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO seServerHelo;
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER header = buildHeader(sizeof(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO));
    
    normalDebug(__FILE__, __LINE__, "Handhandle Client Helo");
     /*making sure size fit structure before copy memory*/
    if (sizeof(seClientHelo) != iDataSize){
        errorDebug(__FILE__, __LINE__, "Received Wrong data size. iDataSize: %d expected size: %d", iDataSize, sizeof(seClientHelo));
        return 1;
    }
    memcpy(&seClientHelo,cpDataBuffer, sizeof(seClientHelo));
    /*validate struct, dose not hurt to be sure, but its  allready be validated in reciveClientMessage ()  */
    if(validateStruct(seClientHelo.acCommand, CMD_HELO, seClientHelo.acHardSpace, seClientHelo.acHardZero))
        return 1;
    /*validate USER AND Ipaddress*/
    if(validateUsernameAndIPAddress(seClientHelo.acFormattedString, sizeof(seClientHelo.acFormattedString),szUserName,szIpaddress))
        return 1;
     /*setting up seServerHelo struct*/
    sprintf(szFormattedString,"%s Hello %s", szIpaddress, szUserName);
    memcpy(&seServerHelo.stHead, &header, sizeof(header));
    memcpy(seServerHelo.acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK, 3);
    memcpy(seServerHelo.acHardSpace, " ", 1);
    memcpy(seServerHelo.acFormattedString, szFormattedString, 51);
    memcpy(seServerHelo.acHardZero, "\0", 1);
    /*validating that the message was send*/
    if (send(iClientSocket, &seServerHelo, sizeof(seServerHelo), 0) != sizeof(seServerHelo) ){
        errorDebug(__FILE__, __LINE__, "%s", strerror(errno));
        return 1;
    }
    return 0;
}
/*handle client MAIL FROM, return 1 if error else 0 */
int handleMailFrom(int iClientSocket, char * cpDataBuffer, int iDataSize){
    struct EWA_EXAM25_TASK5_PROTOCOL_MAILFROM seClientMailFrom;
    
    normalDebug(__FILE__, __LINE__, "Handhandle Mail From");
     /*making sure size fit structure before copy memory*/
    if (sizeof(seClientMailFrom) != iDataSize){
        errorDebug(__FILE__, __LINE__, "Received Wrong data size. iDataSize: %d expected size: %d", iDataSize, sizeof(seClientMailFrom));
        return 1;
    }
    memcpy(&seClientMailFrom,cpDataBuffer, sizeof(seClientMailFrom));
    /*validate struct, dose not hurt to be sure, but its  allready be validated in reciveClientMessage ()  */
    if(validateStruct(seClientMailFrom.acCommand, CMD_MAIL_FROM, seClientMailFrom.acHardSpace, seClientMailFrom.acHardZero))
        return 1;
    /*validate mail*/
    if (validateMail(seClientMailFrom.acFormattedString, sizeof(seClientMailFrom.acFormattedString)))return 1;


    if (sendServerReplay(iClientSocket, "Sender address ok", EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK))
        return 1;
    
    
    return 0;
}
/*handle client RCPT TO, return 1 if error else 0 */
int handleRcptTo(int iClientSocket, char * cpDataBuffer, int iDataSize){
    struct EWA_EXAM25_TASK5_PROTOCOL_RCPTTO seClientRcptTo;
    normalDebug(__FILE__, __LINE__, "Handhandle Recpt TO");
    /*making sure size fit structure before copy memory*/
    if (sizeof(seClientRcptTo) != iDataSize){
        errorDebug(__FILE__, __LINE__, "Received Wrong data size. iDataSize: %d expected size: %d", iDataSize, sizeof(seClientRcptTo));
        return 1;
    }
    memcpy(&seClientRcptTo,cpDataBuffer, sizeof(seClientRcptTo));
     /*validate struct, dose not hurt to be sure, but its  allready be validated in reciveClientMessage ()  */
     if(validateStruct(seClientRcptTo.acCommand, CMD_RCPT_TO, seClientRcptTo.acHardSpace, seClientRcptTo.acHardZero))
     return 1;

    /*validate mail*/
    if (validateMail(seClientRcptTo.acFormattedString, sizeof(seClientRcptTo.acFormattedString)))return 1;
    
    /*validating that the message was send*/
    if (sendServerReplay(iClientSocket, "Received address ok",EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK))
        return 1;
    
    return 0;
}
/*this method is called from the function under(handleData). 
and handle the stream of data
this method is only called if FileName was valid*/
int recvDataStreamFromClient(int iClientSocket, char *szFileName){
    struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER seHeader;
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATAFILE *seDataFile = NULL;
    int iHeaderSize = sizeof(seHeader);
    int iDataSize = 0;
    int iBodySize = 0;
    int iStreamEndLen = 8; 
    int i = 0;
    FILE *fpFile = NULL;

    /*Important, we are no using receiveClientMessage(). so we have to validate everything before using the data.
    we can not use receiveClientMessage() because it can be multiple data streams*/
    if (recvAllData(iClientSocket, &seHeader, iHeaderSize) != iHeaderSize ){/*validating the header size first*/
        errorDebug(__FILE__, __LINE__, "Received Wrong data size. HeaderSize: %d", iHeaderSize);
        return 1;
    }
    /*validate header content */
    if (validateHeader(seHeader, &iDataSize)){
        return 1;
    }
    /*allocate memeroy for the data */
    seDataFile = malloc(iDataSize + iStreamEndLen);
    if (!seDataFile){
        return 1;
    }
    memcpy(seDataFile, &seHeader, iHeaderSize);
   /*calculate body size*/
    iBodySize = iDataSize - iHeaderSize + iStreamEndLen;
    if (recvAllData(iClientSocket, &seDataFile->acFileContent, iBodySize) != iBodySize){
        free(seDataFile);
        return 1;
    }

    fpFile = fopen(szFileName,"w");
    if (!fpFile){
        free(seDataFile);
        return 1;
    }
    /*put char to file */
    for(i = 0; i < iDataSize - iHeaderSize; i++){
        fputc(seDataFile->acFileContent[i],fpFile);
    }
    
    fclose(fpFile);
    free(seDataFile);

    if(sendServerReplay(iClientSocket, "ALL data received", EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK)){
        return 1;
    }
    
    return 0;
}
/*handle client QOUT , return 1 if error else 0 */
int handleData(int iClientSocket, char * cpDataBuffer, int iDataSize){
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD seClientDataCMD;
    normalDebug(__FILE__, __LINE__, "Handhandle DATA");
    /*making sure size fit structure before copy memory*/
    if (sizeof(seClientDataCMD) != iDataSize){
        return 1;
    }
    memcpy(&seClientDataCMD,cpDataBuffer, sizeof(seClientDataCMD));
    /*validate struct, dose not hurt to be sure, but its  allready be validated in reciveClientMessage ()  */
    if(validateStruct(seClientDataCMD.acCommand, CMD_DATA, seClientDataCMD.acHardSpace, seClientDataCMD.acHardZero))
        return 1;
    /*validating file name */
    if(validateFileName(seClientDataCMD.acFormattedString, sizeof(seClientDataCMD.acFormattedString)))
        return 1;
    if (sendServerReplay(iClientSocket, "File Name OK",EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY))
        return 1;
    recvDataStreamFromClient(iClientSocket, seClientDataCMD.acFormattedString);
    return 0;
}

/*handle client QOUT , return 1 if error else 0 */
int handleQuit(int iClientSocket, char * cpDataBuffer, int iDataSize){
    struct EWA_EXAM25_TASK5_PROTOCOL_CLOSECOMMAND seClientQuit;
    normalDebug(__FILE__, __LINE__, "Handhandle QUIT");
    /*making sure size fit structure before copy memory*/
    if (sizeof(seClientQuit) != iDataSize){
        return 1;
    }
    memcpy(&seClientQuit,cpDataBuffer, sizeof(seClientQuit));
    /*validate struct, dose not hurt to be sure, but its  allready be validated in reciveClientMessage ()  */
    if(validateStruct(seClientQuit.acCommand, CMD_QUIT, " ", seClientQuit.acHardZero))
        return 1;
    printf("%s\n", seClientQuit.acFormattedString);
    
    if (sendServerReplay(iClientSocket, "Bye",EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_CLOSED))
        return 1;
    return 0;
}
