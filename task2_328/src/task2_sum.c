/* This file has been created by EWA, and is part of task 2 on the exam for PG3401 2025*/
int Task2_SizeAndSumOfCharacters(FILE* fFileDescriptor, int* piSizeOfFile, int* piSumOfChars) {
   fseek(fFileDescriptor, 0, SEEK_END);
   *piSizeOfFile = ftell(fFileDescriptor);
   rewind(fFileDescriptor);
   *piSumOfChars = 0;
   for (int i = 0; i < *piSizeOfFile; i++){ *piSumOfChars += fgetc(fFileDescriptor); }
   rewind(fFileDescriptor);
   return 0;
}
