#ifndef VALIDATOR_H
#define VALIDATOR_H
struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER;
int validBody( char * cpBodyBuffer, int iBodySize);
int validateHeader(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER seHeader, int * ipDataSize);
int validateStruct(char * acCommand, char * CMD_TYPE, char *acHardSpace, char *acHardZero);
int validateUsernameAndIPAddress(char *acFormattedString, int iFormattedStringBytes, char szUserName[25], char szIpaddress[13]);
int validateFileName(char *acFormattedString, int iFormattedStringBytes);
int validateMail(char *acFormattedString, int iFormattedStringBytes);
#endif