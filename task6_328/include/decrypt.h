# ifndef DECRYPT_H
# define DECRYPT_H
int decrypt(char *cpEncBody, unsigned char *cpDecBody, int bodySize, int tryNumber);
int startDecrypting(char *cpEncBody, int bodySize);
FILE * getFilePointer(char* szFileName, char *szMode);
# endif