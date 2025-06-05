#ifndef HTTP_H
#define HTTP_H
/* HTTP-forespørselen vi får fra serveren er liten, 
så for å simulere en klient som ikke allokerer mer minne enn nødvendig,
leser vi inn i små blokker på 64 bytes. Det blir en for enkel oppgave 
dersom jeg leser blokker på 4096 bytes og en max HTTP forespørsell på 1MB , som er det vanlige i http server som nginx.
Derfor har jeg valgt å sette en maksgrense på header og body.
Jeg programmerer dette slik at du dynamisk kan endre på disse veridene*/
# define MAX_ALLOWED_HEADER_SIZE 256 /*value in bytes*/
# define MAX_ALLOWED_BODY_SIZE 1024 /*value in bytes*/
# define BLOCK_SIZE 64/*value in bytes*/
# define SEPARATOR_SIZE 4/*value in bytes*/

typedef struct _HTTP{
    int iHeaderSize;
    char *cpzHeader;
    int iBodySize;
    char *cpzBody;
}HTTP;
int connectToServer(char * ipAddress, int portNumber);
int recvAllData(int iClientSocket, char *hpHttpBuffer, int iMaxSize);
void cleanUpHttp(HTTP *hpHttp);
char * getHeader(char * cpzRawHttp, int iRawHttpSize);
int getContentLength(char *cpzHeader, int iHeaderSize);
char * getBody(char * cpzRawHttp, int iRawHttpSize, int iHeadersize, int iBodySize);
HTTP * getHttpRequest(int iClientSocket);
#endif