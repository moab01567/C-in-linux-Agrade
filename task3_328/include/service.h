
#ifndef SERVER_H
#define SERVER_H

struct _APPDATA;
struct _FLIGHT;
int serviceAddFlightToList(struct _APPDATA* pAppdata, char *szFlightId, char *szDestination, int iTotalSeats, int iDepartureTime, char* szStatusMessage);
int  serviceAddPassengerToFlight(struct _APPDATA* pAppdata, char *szFlightId, int iSeatNumber, char *szName, int iPasAge,char* szStatusMessage);
struct _FLIGHT * serviceGetFlightByNumber(struct _APPDATA* pAppdata, int flightNumber);
int serviceGetFlightNumberByDestination(struct _APPDATA* pAppdata, char *szDestination, char* szStatusMessage);
void serviceDeleteAllFlights(struct _APPDATA *pAppdata);
int serviceDeleteFlightByFlightId(struct _APPDATA *pAppdata, char *szFlightId, char* szStatusMessage);
int serviceChangePassengerSeat(struct _APPDATA*pAppData, char* szFlightId, char *szPassengerName, int iPassengerSeat,  char* szStatusMessage );
int serviceSearchFlightsByPassengerAmount(struct _APPDATA* pAppData, char *szPassengerName, char* szStatusMessage);

#endif
