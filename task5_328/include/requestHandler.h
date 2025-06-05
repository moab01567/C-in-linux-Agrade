#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#define MAX_COMMAND_LENGTH 11
#define CMD_HELO "HELO"
#define CMD_MAIL_FROM "MAIL FROM:"
#define CMD_RCPT_TO "RCPT TO:"
#define CMD_DATA "DATA"
#define CMD_QUIT "QUIT"

int findClientCommend(char* szCommandBuffer, char * cpDataBuffer);
char * receiveClientMessage(int iClientSocket, int *ipDataSize);
int waitOnData( int iClientSocket, void *vpBuffer, int iDataSize, int iFlags);
int sendServerReplay(int iClientSocket, char * szMessage , char *statusCode );

int handleServerAccept(int iSocketClient,char * szId);
int handleClientHelo(int iClientSocket, char * cpDataBuffer, int iDataSize );
int handleMailFrom(int iClientSocket, char * cpDataBuffer, int iDataSize);
int handleRcptTo(int iClientSocket, char * cpDataBuffer, int iDataSize);
int handleData(int iClientSocket, char * cpDataBuffer, int iDataSize);
int recvDataStreamFromClient(int iClientSocket, char *szFileName);
int handleQuit(int iClientSocket, char * cpDataBuffer, int iDataSize);
#endif