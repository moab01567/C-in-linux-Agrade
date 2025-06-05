/* This file has been created by EWA, and is part of task 4 on the exam for PG3401 2025*/
/* Original code credited to David Wheeler and Roger Needham */
/* Documentation: https://www.schneier.com/wp-content/uploads/2015/03/TEA-2.c */ 
void encipher(unsigned int *const v,unsigned int *const w, const unsigned int *const k){
   register unsigned int y=v[0],z=v[1],sum=0,delta=0x9E3779B9, a=k[0],b=k[1],c=k[2],d=k[3],n=32;
   while(n-->0){
      sum += delta;
      y += (z<<4)+a ^ z+sum ^ (z>>5)+b;
      z += (y<<4)+c ^ y+sum ^ (y>>5)+d;
   }
   w[0]=y; w[1]=z;
}
