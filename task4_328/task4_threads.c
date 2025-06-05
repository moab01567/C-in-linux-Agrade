#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include "./include/dbj2.h"
#include "./include/tea.h"

#define BUFFER_SIZE 4096
#define NUM_THREADS 2
#define BYTE_RANGE 256
#define FILE_SIZE_NAME 30
/*
there is no debug to terminal, if you want to see that everything work, 
U can delete the .enc and .hash files and the program can create new files 
<name>.enc og <name>.hash for you, 
I recommend running the program with no args to get instructions.
*/

/*here is the struct we will pass to threads*/
typedef struct _ARGS{
   unsigned char buffer[BUFFER_SIZE];
   pthread_mutex_t mutex;
   int semInitBufferFull;
   sem_t  semBufferFull;
   int semInitBufferEmpty;
   sem_t  semBufferEmpty;
   int bytes_in_buffer;
   int iDoneReadingFile;
   FILE * fpHashFile;
   FILE * fpEncFile;
   FILE * fpDataFile;
}ARGS;

/*this function is called from the end of thread B*/
int writeEncFile( FILE* fpEncFile, char cBuffer[8]){
   unsigned int k[4] = {1, 2, 3, 4};
   unsigned int v[2];
   unsigned int w[2];
   int i = 0;

   memcpy(v,cBuffer, 8);
   encipher(v,w, k);

   fseek(fpEncFile, 0, SEEK_END);
   fwrite(w,sizeof(unsigned int),2, fpEncFile);

   return 0;
}
/*this function is called from the end of thread B*/
int writeHashFile(FILE* fpDataFile, FILE * fpHashFile){
   unsigned int iHash = 0;
   rewind(fpDataFile);
   rewind(fpHashFile);
   Task2_SimpleDjb2Hash(fpDataFile,  &iHash);
   fprintf(fpHashFile,"%u", iHash);
   return 0;
}
FILE * getFilePointer(char szfileName[FILE_SIZE_NAME], char* szMode){
   FILE * fpFile = NULL;
   fpFile = fopen(szfileName, szMode);
   if (!fpFile) {
      perror("Failed to open file");
      return NULL;
   }
   return fpFile;
}

void cleanArgsMemory(ARGS* apArgs){
   /*check if not NULL*/
   if(!apArgs)return;
   /*close file if not NULL*/
   if (apArgs->fpDataFile){
      fclose(apArgs->fpDataFile);
      apArgs->fpDataFile = NULL;
   }
   /*close file if not NULL*/
   if (apArgs->fpEncFile){
      fclose(apArgs->fpEncFile);
      apArgs->fpEncFile = NULL;
   }
   /*close file if not NULL*/
   if (apArgs->fpHashFile){
      fclose(apArgs->fpHashFile);
      apArgs->fpHashFile = NULL;
   }
   /*destroy sem if not null*/
   if (apArgs->semInitBufferFull)sem_destroy(&apArgs->semBufferFull);
   /*destroy sem if not null*/
   if (apArgs->semInitBufferEmpty)sem_destroy(&apArgs->semBufferEmpty);
   free(apArgs);
}

ARGS * initializationArgsStruct(char *szFileNameData, char *szFileNameHash, char *szFileNameEnc){
   ARGS* apArgs  = NULL;
   /*allocate args struct */
   apArgs = malloc(sizeof(ARGS));
   if (!apArgs){
      return NULL;
   }
   /*Get the Data File */
   apArgs->fpDataFile = getFilePointer(szFileNameData, "rb");
   if (!apArgs->fpDataFile){
      cleanArgsMemory(apArgs);
      return NULL;
   }
   /*create Hash File*/
   apArgs->fpHashFile = getFilePointer(szFileNameHash,"w");
   if (!apArgs->fpHashFile){
      cleanArgsMemory(apArgs);
      return NULL;
   }
   /*create Enc File*/
   apArgs->fpEncFile = getFilePointer(szFileNameEnc,"wb");
   if (!apArgs->fpEncFile){
      cleanArgsMemory(apArgs);
      return NULL;
   }
   /*init sem for semBufferFull*/
   if (sem_init(&apArgs->semBufferFull,0,0) != 0){
      cleanArgsMemory(apArgs);
      return NULL;
   }
   apArgs->semInitBufferFull = 1; /*set to 1, so I know it  should be destroyed later */
   /*init sem for semBufferEmpty*/
   if (sem_init(&apArgs->semBufferEmpty,0,1) != 0){
      cleanArgsMemory(apArgs);
      return NULL;
   }
   apArgs->semInitBufferEmpty = 1; /*set to 1, so I know it  should be destroyed later */

   return apArgs;
}


/*all methods under this comment was made by Ewa and  all method over this comment was made by me :) */
void* thread_A(void* arg) {
   ARGS * apArgs = NULL;
   /*validating args*/
   if(!(apArgs = (ARGS*) arg)) pthread_exit(NULL);
   
   while (1) {
      sem_wait(&apArgs->semBufferEmpty);
      pthread_mutex_lock(&apArgs->mutex);

      int read_bytes = fread(apArgs->buffer + apArgs->bytes_in_buffer, 1, BUFFER_SIZE - apArgs->bytes_in_buffer, apArgs->fpDataFile);
      apArgs->bytes_in_buffer += read_bytes;

      if (read_bytes < BUFFER_SIZE - apArgs->bytes_in_buffer) {
         apArgs->iDoneReadingFile = 1; /*set to 1 to tell thread B that we are done reading the file*/
         pthread_mutex_unlock(&apArgs->mutex);
         sem_post(&apArgs->semBufferFull);
         break;
      }
      pthread_mutex_unlock(&apArgs->mutex);
      sem_post(&apArgs->semBufferFull);
   }
   pthread_exit(NULL);
}

void* thread_B(void* arg) {
   char caBuffer8Bytes[8];/*Buffer that holds 8 bytes*/
   int iPadding = 0;
   ARGS * apArgs = NULL;
   int i = 0;
   if(!(apArgs = (ARGS*) arg)){
      pthread_exit(NULL);
   }
   
   while (1) {
      sem_wait(&apArgs->semBufferFull);
      pthread_mutex_lock(&apArgs->mutex);
      iPadding = apArgs->bytes_in_buffer % 8;/*calculate the padding for the last 8 bytes */
      
      /*adding 8 bytes to caBuffer8Bytes until modulus is 0, then writes to file
      and do it again until done looping through 'apArgs->buffer'.*/
      /*i starts at 1 because 0 % 8 is 0*/      
      for(i = 1; i <= apArgs->bytes_in_buffer  / 8; i++){
            /*adding to bytes to caBuffer8Bytes until modulus is 0 then writes to file*/
            caBuffer8Bytes[( i - 1 ) % 8] =  apArgs->buffer[i - 1];
            if (i % 8 == 0){
               writeEncFile(apArgs->fpEncFile,caBuffer8Bytes);
            }
      }
      /*add the padding to buffer  */
      for(i = 0; i < iPadding; i++ ){
         caBuffer8Bytes[( i - 1 ) % 8] = 48 + iPadding;/*char '0' in ascii is  48*/
         if (i % 8 == 0){
            writeEncFile(apArgs->fpEncFile,caBuffer8Bytes);
         }
      }

      apArgs->bytes_in_buffer = 0;
      pthread_mutex_unlock(&apArgs->mutex);
      sem_post(&apArgs->semBufferEmpty);

      if (apArgs->iDoneReadingFile == 1 && apArgs->bytes_in_buffer == 0)
         break;
   }
   
   writeHashFile(apArgs->fpDataFile, apArgs->fpHashFile);
   pthread_exit(NULL);
}


int main(int iCArgs, char* capArgs[]) {
   pthread_t threadA, threadB;
   ARGS* apArgs  = NULL;

   if (iCArgs <= 1){
      printf("\n\t **Missing args**\n");
      printf("\n\t You can provide one arg <program> <DataFile>\n");
      printf("\t default file names will be task4_pg2265.hash and task4_pg2265.enc\n");
      printf("\n\t But I also support multiple args <program> <DataFile> <HashFile> <EncFile>\n\n");
      return 1;
   }else if  (iCArgs == 2){
      apArgs = initializationArgsStruct(capArgs[1], "task4_pg2265.hash", "task4_pg2265.enc");
   }else{
      apArgs = initializationArgsStruct(capArgs[1], capArgs[2], capArgs[3]);
   }

   if (!apArgs){
      perror("Could not allocate memory");
      exit(1);
   }

   if (pthread_create(&threadA, NULL, thread_A, (void*)apArgs) != 0) {
      perror("Could not create thread A");
      cleanArgsMemory(apArgs);
      exit(1);
   }

   if (pthread_create(&threadB, NULL, thread_B, (void*)apArgs) != 0) {
      perror("Could not create thread B");
      cleanArgsMemory(apArgs);
      exit(1);
   }

   if (pthread_join(threadA, NULL) != 0) {
      perror("Could not join thread A");
      cleanArgsMemory(apArgs);
      exit(1);
   }

   if (pthread_join(threadB, NULL) != 0) {
      perror("Could not join thread B");
      cleanArgsMemory(apArgs);
      exit(1);
   }

   cleanArgsMemory(apArgs);

   return 0;
}










