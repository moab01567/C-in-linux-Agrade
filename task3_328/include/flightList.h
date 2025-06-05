#ifndef FLIGHT_LIST_H
#define FLIGHT_LIST_H

#define FLIGHT_ID_SIZE 11
#define FLIGHT_DESTINATION_SIZE 31

struct _PASSENGER;
struct _APPDATA;

typedef struct _FLIGHT{
    struct _FLIGHT *fpNext;
    struct _FLIGHT *fpPrev;
    char *szFlightId;
    char *szDestination;
    int iTotalSeats;
    int iDepartureTime;
    int iTotalPassengers; 
    struct _PASSENGER* ppHead;
}FLIGHT;

FLIGHT * createFlight(char *szFlightId, char * szDestination, int iTotalSeats, int iDepartureTime);
FLIGHT * addFlightToEnd(struct _APPDATA* apAppData, FLIGHT * fpFlight);
FLIGHT * getFlightByFlightId(struct _APPDATA* apAppData, char *szFlightId);
int  getFlightNumberByDestination(struct _APPDATA* apAppData, char *szDestination);
int deleteFlightByFlightId(struct _APPDATA* apAppData, char * szFlightId);
void deleteAllFlight(struct _APPDATA* apAppData);
void cleanFlightFromMemory(FLIGHT * fpFlight);
#endif