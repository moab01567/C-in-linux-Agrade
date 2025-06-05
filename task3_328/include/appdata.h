#ifndef APPDATA_H
#define APPDATA_H

struct _FLIGHT;
typedef struct _APPDATA{
    struct _FLIGHT * fpHead;
    int iTotalFlights;
}APPDATA;

APPDATA * initAppData();
void freeAppData(APPDATA * pAppData);
#endif