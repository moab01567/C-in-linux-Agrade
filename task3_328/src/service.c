#include <stdio.h>
#include <stdlib.h>
#include "../include/appdata.h"
#include "../include/flightList.h"
#include "../include/passengerList.h"
/* 
this is service.c file and the communication flow for this file is 
service.c->flightList.c
service.c->passengerList.c

all the functions in this file will always be called from menu.c
*/



int serviceAddFlightToList(APPDATA* pAppdata, char *szFlightId, char *szDestination, int iTotalSeats, int iDepartureTime, char* szStatusMessage){
    FLIGHT *pFlight = NULL;
    FLIGHT *pNewFlight = NULL;
    /*check if flight by that id already exist*/
    pFlight = getFlightByFlightId(pAppdata, szFlightId);
    if (pFlight){
        sprintf(szStatusMessage,"Flight ID %s Already Exist", szFlightId);
        return 1;
    }
    /*create flight*/
    pNewFlight = createFlight(szFlightId,szDestination,iTotalSeats,iDepartureTime);
    if (!pNewFlight){
        sprintf(szStatusMessage,"Cloud Not Create Flight");
        return 1;
    }
    /*add flight to the end */
    pNewFlight = addFlightToEnd(pAppdata, pNewFlight);
    if (!pNewFlight){
        sprintf(szStatusMessage,"Cloud Add Flight To LIST");
        cleanFlightFromMemory(pNewFlight);
        return 1; 
    };
    sprintf(szStatusMessage,"successfully added");
    return 0;
}

int serviceAddPassengerToFlight(APPDATA* pAppdata, char *szFlightId, int iSeatNumber, char *szName, int iPasAge, char* szStatusMessage){
    /*check if flight by that id already exist*/
    FLIGHT *fpFlight = getFlightByFlightId(pAppdata,szFlightId);
    PASSENGER * ppPassenger = NULL;
    PASSENGER * ppNewPassenger = NULL;
    if (!fpFlight){
        sprintf(szStatusMessage,"Could not find Flight Id %s", szFlightId);
        return 1;
    }
    /*check if passenger already exist*/
    ppPassenger = getPassengerFromFlight(fpFlight,szName);
    if (ppPassenger){
        sprintf(szStatusMessage,"Passenger,%s Already Exist", szName);
        return 1;
    }
    /*create passenger */
    ppNewPassenger = createPassenger(szName,iSeatNumber,iPasAge);
    if (!ppNewPassenger){
        sprintf(szStatusMessage,"Could Not Create Passenger");
        return 1;
    }
    /*add passenger to list*/
    if (!addPassengerToListSort(fpFlight, ppNewPassenger)){
        cleanMemoryPassenger(ppNewPassenger);
        sprintf(szStatusMessage,"Could Not Create Passenger");
        return 1;
    }
    sprintf(szStatusMessage,"Passenger added successfully");
    return 0;
}

FLIGHT * serviceGetFlightByNumber(APPDATA* pAppdata, int iFlightNumber){
    int count = 1;
    FLIGHT *fpFlight = pAppdata->fpHead;
    /*just loop through the list and return flight by number*/
    while(fpFlight){
        if (iFlightNumber == count){
            return fpFlight;
        }
        fpFlight = fpFlight->fpNext;
        count++;
    }
    return NULL;
}

/*delete all flights, it call on exit :)*/
void serviceDeleteAllFlights(APPDATA *pAppdata){
    deleteAllFlight(pAppdata);
}

int serviceGetFlightNumberByDestination(APPDATA *pAppdata, char *szDestination, char* szStatusMessage){
    int iFlightNumber = 0;
    /*check is app data not null and pflight not null*/
    if (!pAppdata){
        sprintf(szStatusMessage, "APPDATA was not INIT");
        return iFlightNumber;
    }else if(!pAppdata->fpHead){
        sprintf(szStatusMessage, "No Flights in System");
        return iFlightNumber;
    }
    /*check get the flight number and check if the flight exist*/
    iFlightNumber = getFlightNumberByDestination(pAppdata,szDestination);
    if(iFlightNumber){
        sprintf(szStatusMessage, "Flight to %s is flight number %d", szDestination, iFlightNumber);
        return iFlightNumber;
    }
    sprintf(szStatusMessage, "Flight to %s was not found", szDestination);
    return iFlightNumber;
}
/*delete flight by id */
int serviceDeleteFlightByFlightId(struct _APPDATA *pAppdata, char *szFlightId, char* szStatusMessage){
    FLIGHT * pFlight = getFlightByFlightId(pAppdata, szFlightId);
    /*check if  flight exist */
    if (!pFlight){
        sprintf(szStatusMessage,"Flight ID %s Not Found", szFlightId);
        return 1;
    }
    /*delete passenger */
    deleteAllPassengers(pFlight);
    /*delete Flight */
    deleteFlightByFlightId(pAppdata, szFlightId);
    sprintf(szStatusMessage,"Flight ID %s Was Deleted", szFlightId);
    return 0;

}


int serviceChangePassengerSeat(APPDATA* pAppData, char* szFlightId, char *szPassengerName, int iPassengerSeat,  char* szStatusMessage ){
    FLIGHT *fpFlight = getFlightByFlightId(pAppData,szFlightId);
    PASSENGER * ppPassenger = NULL;
    PASSENGER * ppNewPassenger = NULL;
     /*check if flight exist*/
    if (!fpFlight){
        sprintf(szStatusMessage,"Could not find Flight Id %s", szFlightId);
        return 1;
    }
     /*check if passenger   exist*/
    ppPassenger = getPassengerFromFlight(fpFlight,szPassengerName);
    if (!ppPassenger){
        sprintf(szStatusMessage,"Passenger,%s Not Fund ", szPassengerName);
        return 1;
    }
    /*create new passenger exist*/
    ppNewPassenger = createPassenger(szPassengerName,iPassengerSeat, ppPassenger->iPasAge);
    if (!ppNewPassenger){
        sprintf(szStatusMessage,"Could Not Change Passenger Seat");
        return 1;
    }
     /*delete the old one passenger */
    if (deletePassengerByName(fpFlight,szPassengerName)){
        sprintf(szStatusMessage,"Could Not Change Passenger Seat");
        return 1;
    }

    ppPassenger = NULL;
    /*add new the new one passenger */
    addPassengerToListSort(fpFlight, ppNewPassenger);
    sprintf(szStatusMessage,"Passenger seat Changed");
    return 0;
}
/*find the amount of time a passenger is in flight */
int  serviceSearchFlightsByPassengerAmount(APPDATA* pAppData, char *szPassengerName, char* szStatusMessage){
    int iTotalFlightFound = 0;
    /*check if there is any flights*/
    FLIGHT * fpCurrentFlight = pAppData->fpHead;
    if (!fpCurrentFlight){
        sprintf(szStatusMessage,"No Flights in system");
        return iTotalFlightFound;
    } 
    /*check if passenger  is any flights*/
    while(fpCurrentFlight){
        if(getPassengerFromFlight(fpCurrentFlight, szPassengerName)){
            iTotalFlightFound++;
        }
        fpCurrentFlight = fpCurrentFlight->fpNext;
    }

    sprintf(szStatusMessage,"Total Flights Found is %d",iTotalFlightFound);
    return iTotalFlightFound;
}
