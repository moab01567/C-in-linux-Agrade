# include <stdio.h>
# include "../include/debugger.h"
# include "../include/requestHandler.h"
# include <string.h> 
# include <netinet/in.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/socket.h>

/*the flow of methods in main.c 
main->validateArgs->(return back to main)
main->setupServer->(return back to main)
main->startAcceptingClinets->handleTraffic

I spent a lot of time creating debugger.c, so I’ve decided to use it here.
When something goes wrong, the methods will simply return 1 and display a generic error message on the terminal.
More detailed information about the cause will be logged in debugLog.txt.

The program/server will close after the EWA connection, if you wish to let the program continue running after QUIT command 
and accept new connections, you must scroll down to startAcceptingClinets() and in the while loop remove break. 
*/


/*if threads was involved this method will be used as a second thread */
int handleTraffic(int iClientSocket, char * szId){
    char szCommandBuffer[MAX_COMMAND_LENGTH] = {0};
    int iDataSize = 0;
    char * cpDataBuffer = NULL;
    int iQuit = 0;
    handleServerAccept(iClientSocket,szId);
    
    while(!iQuit){
        iDataSize = 0;
        cpDataBuffer = NULL;
        memset(szCommandBuffer,0,MAX_COMMAND_LENGTH);
        
        cpDataBuffer = receiveClientMessage(iClientSocket,  &iDataSize);
        if (!cpDataBuffer){
            printf("Could not recv Message from client, see debug for more info");
            break;
        }
        /*check if there is any CMD there*/
        if (findClientCommend(szCommandBuffer,cpDataBuffer)){
            printf("Could not find command see debug for more info");
            iQuit = 1;
        }

        printf("command received: %s\n",szCommandBuffer);
        
        /*check if we could recognize the command*/
        if(strcmp(szCommandBuffer, CMD_HELO) == 0){
            if (handleClientHelo(iClientSocket, cpDataBuffer,iDataSize)){
                iQuit = 1;
                printf("Something went wrong handling HELO, check debuglog");
            }

        }else if(strcmp(szCommandBuffer,CMD_MAIL_FROM) == 0){
            if (handleMailFrom(iClientSocket,cpDataBuffer,iDataSize)){
                iQuit = 1;
                printf("Something went wrong handling MAIL FROM:, check debuglog");
            }
        
        }else if(strcmp(szCommandBuffer,CMD_RCPT_TO) == 0){
            if (handleRcptTo(iClientSocket,cpDataBuffer,iDataSize)){
                iQuit = 1;
                printf("Something went wrong handling RCPT TO:, check debuglog");
            }
        
        }else if(strcmp(szCommandBuffer,CMD_DATA) == 0){
            if (handleData(iClientSocket,cpDataBuffer,iDataSize)){
                iQuit = 1;
                printf("Something went wrong handling DATA, check debuglog");
            }
        
        }else if(strcmp(szCommandBuffer, CMD_QUIT) == 0){
            if (handleQuit(iClientSocket,cpDataBuffer,iDataSize)){
                printf("Something went wrong handling QUIT, check debuglog");
            }
            iQuit = 1;
        }/*end big if*/

        free(cpDataBuffer);
        cpDataBuffer = NULL;
    }/*end loop*/
    close(iClientSocket);
    return 0;
}
/*this method accept incoming traffic*/
int startAcceptingClinets(int iServerSocket, char * szId){
    int iClientSocket; 
    struct sockaddr_in saClientAddr;
    socklen_t iSocketClientLen = sizeof(struct sockaddr_in);
    /*waiting to accept clients */
    while(1){
        printf("Ready to accept client\n");
        iClientSocket = accept(iServerSocket, (struct sockaddr *) &saClientAddr, &iSocketClientLen);
        if (iClientSocket < 0){
            errorDebug(__FILE__, __LINE__, "%s",strerror(errno));
            printf("Accept clients...Failed\n");
            close(iServerSocket);
            return 1;
        }
        /*handle client traffic */
        handleTraffic(iClientSocket, szId);
         /*remove this break if you want the server to  accept new request after EWA connection end*/
        break;
    }/*end loop*/
    return 0;
}

/*just binding and setting up everything
return -1 if error 
else socket */
int setupServer(int iPort){
    int iServerSocket;
    struct sockaddr_in saSocketAddress;
    iServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    printf("Creating socket...");
    if(iServerSocket < 0){
        printf("Failed\n");
        errorDebug(__FILE__, __LINE__, "%s",strerror(errno));
        return -1;
    }
    printf("success\n");

    printf("Binding socket to localhost and port %d...", iPort);
    saSocketAddress.sin_family = AF_INET;
    saSocketAddress.sin_port = htons(iPort);
    saSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(iServerSocket, (struct sockaddr *)&saSocketAddress ,sizeof(saSocketAddress)) == -1){
        printf("Failed\n");
        errorDebug(__FILE__, __LINE__, "%s",strerror(errno));
        close(iServerSocket);
        return -1;
    }
    printf("success\n");

    printf("Listening on localhost and port %d...", iPort);
    if (listen(iServerSocket, 1)){ 
        printf("Failed\n");
        errorDebug(__FILE__, __LINE__, "%s",strerror(errno));
        close(iServerSocket);
        return -1;
    }
    printf("success\n");


    return iServerSocket;
}

/*validating flag and args*/
int validateArgs(int iArgC, char * cazArgC[], char ** szId, int * iPort){
    int i = 0;
    if (iArgC < 5){
        printf("Missing argv\n");
        printf("Format: <program> -port <value> -id <value>\n");
        return 1;
    }
    /*just make sure that data valid input, mainly port number, */
    for(i = 1; i < iArgC - 1; i++){
        if (strcmp(cazArgC[i] ,"-id") == 0 ){
            *szId = cazArgC[i + 1];
        }
        if (strcmp(cazArgC[i] ,"-port") == 0){
            if(atoi(cazArgC[i + 1]) < 5000 || atoi(cazArgC[i + 1]) > 65353){
                printf("port range is [5000...65353]\n");
            }else{
                *iPort = atoi(cazArgC[i + 1]);
            }
        }
    }

    if (*szId == NULL || *iPort == 0){
        printf("no valid id or port\n");
        return 1;
    }
    return 0;
}


int main(int iArgC, char * cazArgC[]){
    int iServerSocket = 0;
    int iPort = 0;
    char * szId = NULL;
    activeDebug(ENABLE_DEBUG); /*enable debug mode*/


    if (validateArgs(iArgC,cazArgC, &szId,  &iPort))return 1;/*validating args*/

    iServerSocket = setupServer(iPort);/*binding to socket and listen*/
    if (iServerSocket == -1)return 1;
    startAcceptingClinets(iServerSocket,szId);/*start accept clients*/
    close(iServerSocket);
    return 0;
}