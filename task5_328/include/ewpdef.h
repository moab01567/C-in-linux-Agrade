/* This file has been created by EWA, and is part of task 5 on the exam for PG3401 2025*/
#ifndef EWA_EXAM25_TASK5_PROTOCOL_DEFINED
#define EWA_EXAM25_TASK5_PROTOCOL_DEFINED

#define EWA_EXAM25_TASK5_PROTOCOL_MAGIC "EWP"
struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER {
   char acMagicNumber[3]; /* = EWP */
   char acDataSize[4]; /* = sizeof(structure below), as ASCII, always base 10, ignore leading 0 */
   char acDelimeter[1]; /* = | */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acStatusCode[3]; /* Set to 220 if server is ready */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[51]; /* Set to SERVERIP PROTOCOL SERVERNAME TIMESTAMP */
                               /* For instance; 127.0.0.1 SMTP SmtpTest 27 okt 2020, 12:42:42 */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTHELO {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[4]; /* Set to HELO */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[50]; /* Set to USERNAME.CLIENTIP */
                               /* For instance; bengt.127.0.0.1 */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acStatusCode[3]; /* Set to 250 if HELO received successfully */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[51]; /* Set to CLIENTIP GREATING */
                               /* For instance; 127.0.0.1 Hello bengt */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

#define EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK "250"
#define EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_CLOSED "221"
#define EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY "354"
struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acStatusCode[3]; /* Set to 250 if command is supported */
                         /* As a response to a DATA command field should be 354 if server is ready, */
                         /*  or for instance 501 if filename not valid or acceptable */
                         /* Set to 221 if the server is closing */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[51]; /* Set to TEXTMESSAGE */
                               /* For instance; Sender address ok | Ready for message */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_MAILFROM {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[10]; /* Set to MAIL FROM: */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[44]; /* Set to EMAILADDRESS */
                               /* For instance; <bengt@test.com> */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_RCPTTO {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[8]; /* Set to RCPT TO: */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[46]; /* Set to EMAILADDRESS */
                               /* For instance; <student@kristiania.no> */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is 0064 */
   char acCommand[4]; /* Set to DATA: */
   char acHardSpace[1]; /* Set to a space (ascii 0x20) */
   char acFormattedString[50]; /* Set to FILENAME*/
                               /* For instance; myfile.eml */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATAFILE {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; /* Size is dynamic */
   char acFileContent[1]; /* Dynamic size array, will be stHead.acDataSize - 1 bytes, results in max size 9998 bytes*/
   /* char acHardZero[1]; */ /* Ensure a zero-termination character is part of data, all though not part of this struct */
}; 

struct EWA_EXAM25_TASK5_PROTOCOL_CLOSECOMMAND {
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHead; 
   char acCommand[4]; /* Set to QUIT */
   char acFormattedString[51]; /* Reserved for future use */
   char acHardZero[1]; /* Set to binary 0, ensures the received data is a zero-terminated string */
}; 

#endif

