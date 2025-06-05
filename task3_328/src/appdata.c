#include <stdlib.h>
#include "../include/appdata.h"
#include "../include/flightList.h"
#include <stddef.h>

/*creates the structure for the app, 
and can hold extra meta data in the future*/
APPDATA * initAppData(){
    APPDATA * pAppData = malloc(sizeof(APPDATA));
    if (!pAppData){
        return NULL;
    }
    return pAppData;
}

void freeAppData(APPDATA * pAppData){
    if(pAppData)free(pAppData);
}