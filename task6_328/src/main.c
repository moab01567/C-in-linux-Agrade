# include <stdio.h>
# include "debugger.h"
# include <string.h> 
# include <stdlib.h>
# include "../include/decrypt.h"
# include "../include/httpClient.h"
# include <unistd.h>
# include <sys/socket.h>

/*the communication flow in main.c

main.c->httpClient.c
main.c->decrypt.c->TEA.c

I recommend reding the report first, to understand the bug in this code. 
*/


/*this code is recycled from task 5 in main.c :)*/
/*validating flag and args if failed return 1 else 0*/
int validateArgs(int iArgC, char * sazArgV[], char ** ipAddress, int * iPort){
    int i = 0;
    if (iArgC < 5){
        printf("Missing argv\n");
        printf("Format: <program> -server <value> -port <value>\n");
        return 1;
    }
    /* make sure that valid input, mainly port number, */
    for(i = 1; i < iArgC - 1; i++){
        if (strcmp(sazArgV[i] ,"-server") == 0 ){
            *ipAddress = sazArgV[i + 1];
        }
        if (strcmp(sazArgV[i] ,"-port") == 0){
            if(atoi(sazArgV[i + 1]) < 5000 || atoi(sazArgV[i + 1]) > 65353){
                printf("port range is [5000...65353]\n");
            }else{
                *iPort = atoi(sazArgV[i + 1]);
            }
        }
    }
    /*validate if the values was set*/
    if (*ipAddress == NULL || *iPort == 0){
        printf("no valid ipaddress or port\n");
        return 1;
    }
    return 0;
}
/*this method is call from main() and is called when args are valid
return 1 if failed else 0*/
int startProgram(char * ipAddress, int portNumber){
    HTTP * hpHttp = NULL;

    /*connet to the server and get client */
    int iClientSocket = connectToServer(ipAddress,portNumber);
    if (iClientSocket == -1){
        printf("Something went wrong see debug log\n");
        return 1;
    }

    /*get the http request see httpClient.h for struct */
    hpHttp = getHttpRequest(iClientSocket);
    if (!hpHttp){
        printf("Something went wrong see debug log\n");
        return 1;
    }

    /*start decrypting the data*/
    startDecrypting(hpHttp->cpzBody, hpHttp->iBodySize);


    /*clean up everything, no leak possible */
    close(iClientSocket);
    cleanUpHttp(hpHttp);
    return 0;
}

int main(int iArgC, char * sazArgV[]){
    int iPort = 0;
    char * ipAddress = NULL;
    activeDebug(ENABLE_DEBUG); 
    if (validateArgs(iArgC, sazArgV, &ipAddress, &iPort)){
        return 1;
    }
    startProgram(ipAddress,iPort);
    return 0;
}