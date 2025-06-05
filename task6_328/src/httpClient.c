# include <stdio.h>
# include "debugger.h"
# include <string.h> 
# include <netinet/in.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/socket.h>
# include "../include/httpClient.h"

/*the communication flow in httpClient.c
main.c->httpClient.c
from httpClient.c, main.c only calls method connectToServer(char * ipAddress, int portNumber)
and getHttpRequest(int iClientSocket).

*/

/*There is some comments in httpClient.h, look at them before starting her :)
this method is called from main*/
int connectToServer(char * ipAddress, int portNumber){
    int iClientSocket; 
    struct sockaddr_in saClientAddr;
    socklen_t iSocketClientLen = sizeof(struct sockaddr_in);
    iClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    printf("creating client socket...");
    if (iClientSocket < 0){
        printf("Failed\n");
        errorDebug(__FILE__, __LINE__, "%s",strerror(errno));
        return -1;
    }
    printf("success\n");

    saClientAddr.sin_family = AF_INET;
    saClientAddr.sin_port = htons(portNumber);
    saClientAddr.sin_addr.s_addr = inet_addr(ipAddress);/*https://stackoverflow.com/questions/22530089/passing-ip-in-string-to-htonl*/
    printf("Connecting to  server socket...");
    if (connect(iClientSocket, (struct sockaddr *) &saClientAddr,iSocketClientLen)!= 0){
        printf("Failed\n");
        errorDebug(__FILE__, __LINE__, "%s",strerror(errno));
        close(iClientSocket);
        return -1;
    }
    printf("success\n");

    return iClientSocket;
}

/* HTTP-forespørselen vi får fra serveren er liten, 
så for å simulere en klient som ikke allokerer mer minne enn nødvendig,
leser vi inn i små blokker på 64 bytes. Det blir en for enkel oppgave 
dersom jeg leser blokker på 4096 bytes.
Derfor har jeg valgt å sette en maksgrense på header og body.
Jeg programmerer dette slik at du dynamisk kan endre på disse veridene se httpClient.h*/
/*recycled from task 5 and made better, if i you read this comment 
it means i did not have time to upgrade the same method in task5 hahah :)
return iTotalBytesReceived or -1 if something goes wrong */
int recvAllData(int iClientSocket, char *hpHttpBuffer, int iMaxSize) {
    int iTotalBytesReceived = 0;
    int iBytesReceived = 0;
    int iTry = 0;
    int iReadSize = BLOCK_SIZE;/*the amount of bytes to read from socket per block*/
    
    /*if max data recv or to many trys,  break*/
    while (iTotalBytesReceived < iMaxSize && iTry < 3) {
        iBytesReceived = recv(iClientSocket, hpHttpBuffer + iTotalBytesReceived, iReadSize, MSG_DONTWAIT);
        
        if (iBytesReceived > 0) {/*make sure that bytes recv is larger then 0*/
            iTotalBytesReceived += iBytesReceived;
            /*this if statement, checks if we should reed less from socket so we dont get overflow*/
            if (iTotalBytesReceived + BLOCK_SIZE > iMaxSize){
                iReadSize = iMaxSize - iTotalBytesReceived;
            }
            iTry = 0;
        } else if (iBytesReceived == 0) {/*check if zero connection closed*/
            warnDebug(__FILE__, __LINE__, "connection closed by server");
            break;
        } else {/*if -1, check if the problem will be resolved, maybe bad connection*/
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                warnDebug(__FILE__, __LINE__, "%s", strerror(errno));
                sleep(1);
                iTry++;
                continue;
            } else {
                errorDebug(__FILE__, __LINE__, "%s", strerror(errno));
                return -1;
            }
        }
    }
    return iTotalBytesReceived;
}
/*this method is called from main.c*/
HTTP * getHttpRequest(int iClientSocket){
    int iRecvData = 0;
    int iMaxRequestSize = MAX_ALLOWED_HEADER_SIZE + MAX_ALLOWED_BODY_SIZE;
    char * cpzRawHttp = NULL;/*note this will contains all the data received from server. and later allocated to HTTP struct.*/
    HTTP * hpHttp = NULL;
    /*allocate memory for the hole request, */
    cpzRawHttp = malloc(iMaxRequestSize + 1);/*adding +1 for zero terminator*/
    if (!cpzRawHttp){
        errorDebug(__FILE__, __LINE__, "Could No allocate memory");
        return NULL;
    }
    memset(cpzRawHttp,0, iMaxRequestSize + 1); /*adding +1 for zero terminator*/

    /*get the data from the server, this method is right above*/
    iRecvData = recvAllData(iClientSocket, cpzRawHttp, iMaxRequestSize);/*no +1 so we always have zero terminator */
    if (iRecvData <= 0){
        free(cpzRawHttp);
        return NULL;
    }
    /*allocate memory for the http struct in httpClient.h*/
    hpHttp = malloc(sizeof(HTTP));
    if (!hpHttp){
        errorDebug(__FILE__, __LINE__, "Could No allocate memory");
        free(cpzRawHttp);
        return NULL;
    }
    /*get the header and validate it*/
    hpHttp->cpzHeader = getHeader(cpzRawHttp, iRecvData);
    if (!hpHttp->cpzHeader){
        free(cpzRawHttp);
        cleanUpHttp(hpHttp);
        return NULL;
    }
    /*sett header size, strlen is safe here because cpzHeader is zeroterminated check getHeader() */
    hpHttp->iHeaderSize = strlen(hpHttp->cpzHeader);

    hpHttp->iBodySize = getContentLength(hpHttp->cpzHeader, hpHttp->iHeaderSize);
    /*here is the validation if ContentLength in Header == 0 
    in this case i don't see recane to continue if ContentLength is 0*/
    if (hpHttp->iBodySize <= 0){
        free(cpzRawHttp);
        cleanUpHttp(hpHttp);
        return NULL;
    }
    /*just get the body if everything is okey*/
    hpHttp->cpzBody = getBody(cpzRawHttp, iRecvData, hpHttp->iHeaderSize, hpHttp->iBodySize);
    if (!hpHttp->iBodySize){
        free(cpzRawHttp);
        cleanUpHttp(hpHttp);
        return NULL;
    }

    free(cpzRawHttp);
    return hpHttp;
}
/*is called from getHttpRequest(int iClientSocket)
return NULL if Header not valid og nor found, else return a cpBufferHeader of the header*/
char * getHeader(char * cpzRawHttp, int iRawHttpSize){
    int iPos = 0;
    int isPressent = 0;
    char * cpzHeader = NULL;
    /*try to find "\r\n\r\n" in header */
    for (iPos = 0; iPos < iRawHttpSize - SEPARATOR_SIZE; iPos++){
        if(cpzRawHttp[iPos] != '\r')continue;
        if(cpzRawHttp[iPos+1] != '\n')continue;
        if(cpzRawHttp[iPos+2] != '\r')continue;
        if(cpzRawHttp[iPos+3] != '\n')continue;
        isPressent = 1;
        break;
    }
    /*if separators not found write to debug and return null */
    if (!isPressent){
        errorDebug(__FILE__, __LINE__, "Could not find separator");
        return NULL;
    }
    /*iPos is the postion where the first char in separator 
    where found '\r' */
    cpzHeader = malloc(iPos + 1);/*add zero terminator*/
    if (!cpzHeader){
        errorDebug(__FILE__, __LINE__, "Could not allocate memory");
        return NULL;
    }
    /*make everything zero terminated and then copy data over to cpzHeader*/
    memset(cpzHeader,0,iPos + 1);
    memcpy(cpzHeader,cpzRawHttp,iPos);
    return cpzHeader;
}
/*is called after getHeader() in getHttpRequest(int iClientSocket)
get the content length from header, return -1 if content length not found in header 
else return 0, if the value in content length is not a number, just return 0
and the body will just be ignored. later in getHttpRequest(int iClientSocket)*/
int getContentLength(char *cpzHeader, int iHeaderSize){
    char * cpzContentLength = "Content-Length: ";
    int iContentLengthLen = strlen(cpzContentLength);
    char * cpzContentLengthCourser = cpzHeader; /*pint to the header*/
    int iMatchFound = 0;
    char iBodySizeBuffer[5] = {0};/*max size is is set her. xxxx from 0..9999. can be change if needed*/
    int i = 0;
    int j = 0;
    
    /*loop through the header and try to find content length */
    for (i = 0; i <= iHeaderSize - iContentLengthLen; i++) {
        for (j = 0; j < iContentLengthLen; j++) {
            if (cpzHeader[i + j] != cpzContentLength[j])
                break;
        }
        if (j == iContentLengthLen){
            iMatchFound = 1;
            /*just point to the end of content length in header if matched*/
            cpzContentLengthCourser = &cpzHeader[i + iContentLengthLen];
            break;
        }
    }
    /*no matched return -1*/
    if (!iMatchFound){
        errorDebug(__FILE__, __LINE__, "Could not Find Content Length");
        return -1;
    }
    /*get the content length and puts it in to cpzContentLengthCourser*/
    for (i = 0; i <  ((int) sizeof(iBodySizeBuffer))- 1; i++){
        if(cpzContentLengthCourser[i] == '\r' || cpzContentLengthCourser[i] == '\n')break;
        if(cpzContentLengthCourser[i] < '0' || cpzContentLengthCourser[i] > '9')break;
        iBodySizeBuffer[i] = cpzContentLengthCourser[i];
    }
    /*will return 0, if not valid input and body will just be ignore later in getHttpRequest(int iClientSocket)*/ 
    return atoi(iBodySizeBuffer);
}

/*is called after getContentLength() in getHttpRequest(int iClientSocket)
return NULL if failed else a pointer of the body(cpzBody) */
char * getBody(char * cpzRawHttp, int iRawHttpSize, int iHeaderSize, int iBodySize){
    char *cpzBody = NULL;
    int iRemaining = iRawHttpSize - iHeaderSize - SEPARATOR_SIZE;/*calculate the Remaining data from the request*/
    
    /*important, the iBodySize variable is the ContentLength specfied in HEADER. 
    if the iBodySize is not the same as iRemaining, the BODY data in the  HTTP request is not valid */
    if (iRemaining != iBodySize) {
        errorDebug(__FILE__, __LINE__, "Invalid remaining buffer size");
        return NULL;
    }
    /*allocate memory*/
    cpzBody= malloc(iBodySize + 1);/*make space for zero terminator */
    if (!cpzBody){
        errorDebug(__FILE__, __LINE__, "Could not allocate memory");
        return NULL;
    }
    /*fill cpzBody with '\0' and allocate memory*/
    memset(cpzBody, 0, iBodySize + 1);
    memcpy(cpzBody, cpzRawHttp + iHeaderSize + SEPARATOR_SIZE, iBodySize);
    return cpzBody;
}

/*clean memory from struct, struct in httpClient.h*/
void cleanUpHttp(HTTP *hpHttp){
    if (!hpHttp)return;
    if (hpHttp->cpzHeader)free(hpHttp->cpzHeader);
    if (hpHttp->cpzBody)free(hpHttp->cpzBody);
    free(hpHttp);
}
