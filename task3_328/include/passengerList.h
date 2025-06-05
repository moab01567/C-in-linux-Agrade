#ifndef PASSENGER_LIST_H
#define PASSENGER_LIST_H

#define PASSENGER_NAME_SIZE 30
struct _FLIGHT;
struct _APPDATA;



typedef struct _PASSENGER{
    struct _PASSENGER *ppNext;
    int iSeatNumber;
    char *szName;
    int iPasAge;
}PASSENGER;

PASSENGER * createPassenger(char *szName, int iSeatNumber, int iPasAge);
PASSENGER * addPassengerToListSort(struct _FLIGHT * fpFlight, PASSENGER * ppPassenger);
PASSENGER * getPassengerFromFlight(struct _FLIGHT * fpFlight, char *szName);
int deletePassengerByName(struct _FLIGHT * fpFlight, char *szPassengerName);
void deleteAllPassengers(struct _FLIGHT * fpFlight);
void cleanMemoryPassenger(PASSENGER * ppPassenger);
int  searchFlightsByPassengerAmount(struct _APPDATA* pAppData, char *szPassengerName);


typedef struct _PASSNAMELIST{
    struct _PASSNAME *ppHead;
}PASSNAMELIST;
typedef struct _PASSNAME{
    struct _PASSNAME *ppNext;
    char szName[PASSENGER_NAME_SIZE];
    int count;
}PASSNAME;
void addToPassList(PASSNAMELIST *plPassNameList, char *szPassengerName);
PASSNAMELIST* getPassengersBookedOnMultipleFlights(struct _APPDATA *pAppData);
void cleanPassNameListFromMemory(PASSNAMELIST *plPassNameList);
#endif