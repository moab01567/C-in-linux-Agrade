#include "../include/flightList.h"
#include "../include/passengerList.h"
#include "../include/appdata.h"
#include "../include/debugger.h"
#include <string.h>
#include <stdlib.h>

/*
this is flightList.c file and the communication flow for this file is 
service.c->flightList.c
service.c->flightList.c

all the functions in this file will always be called from service.c

NOTE to developer!! 
you should always use the defined values in flightList.h FLIGHT_ID_SIZE 
and FLIGHT_DESTINATION_SIZE When validating userinput, before allocating 
memory by using these functions */

void cleanFlightFromMemory(FLIGHT * fpFlight){
    if(!fpFlight){
        warnDebug(__FILE__,__LINE__,"No Flight removed from memory");
        return;
    }
    if(fpFlight->szFlightId)free(fpFlight->szFlightId);
    fpFlight->szFlightId = NULL;
    if(fpFlight->szDestination)free(fpFlight->szDestination);
    fpFlight->szDestination = NULL;
    free(fpFlight);
    fpFlight = NULL;
    normalDebug(__FILE__,__LINE__,"Flight was successfully removed from memory");
}


FLIGHT * createFlight(char *szFlightId, char *szDestination, int iTotalSeats, int iDepartureTime){
    /*try to allocate memory*/
    FLIGHT * fpFlight = malloc(sizeof(FLIGHT));
    if(!fpFlight){
        errorDebug(__FILE__,__LINE__,"Could Not Allocate Memory For Flight: %s",szFlightId);  
        return NULL;
    }
    /*not safe, if zero terminator not pressent and size is not handle correctly*/
    fpFlight->szFlightId = malloc(strlen(szFlightId) + 1);/*+ 1 to make space for zero terminator*/
    fpFlight->szDestination = malloc(strlen(szDestination) + 1);
    if (!fpFlight->szFlightId || !fpFlight->szDestination){
        errorDebug(__FILE__,__LINE__,"Could Not Allocate Memory For Flight: %s",szFlightId);
        cleanFlightFromMemory(fpFlight);
        return NULL;
    }
    /*init data to flight */
    strcpy(fpFlight->szFlightId, szFlightId);
    strcpy(fpFlight->szDestination, szDestination);
    fpFlight->iTotalSeats = iTotalSeats;
    fpFlight->iDepartureTime = iDepartureTime;
    fpFlight->iTotalPassengers = 0;
    fpFlight->fpNext = NULL;
    fpFlight->fpPrev = NULL;
    fpFlight->ppHead = NULL;
    normalDebug(__FILE__,__LINE__,"Flight: %s was successfully created",szFlightId);
    return fpFlight;
}

FLIGHT * addFlightToEnd(APPDATA* apAppData, FLIGHT * fpFlight){
    FLIGHT * fpCurrentFlight = NULL;
    /*check if this is the first flight in list  */
    if(!apAppData->fpHead){
        apAppData->fpHead = fpFlight;
        apAppData->iTotalFlights = apAppData->iTotalFlights + 1;
        normalDebug(__FILE__, __LINE__, "Fight was added");
        return fpFlight;
    }
     /*loop through list and add flight to the end  */
    fpCurrentFlight = apAppData->fpHead;
    while(fpCurrentFlight->fpNext){
        fpCurrentFlight = fpCurrentFlight->fpNext;
    }
    fpCurrentFlight->fpNext = fpFlight;
    fpFlight->fpPrev = fpCurrentFlight;

    apAppData->iTotalFlights = apAppData->iTotalFlights + 1;/*increase the list size number   */
    normalDebug(__FILE__,__LINE__,"Flight: %s was successfully added to list ",fpFlight->szFlightId);
    return fpFlight;
}

FLIGHT * getFlightByFlightId(APPDATA* apAppData, char *szFlightId) {
    FLIGHT * fpCurrentFlight = NULL;
    FLIGHT * fpNextFlight = NULL;
    /* check if list empty*/
    normalDebug(__FILE__, __LINE__, "Searching for flight with ID: %s", szFlightId);
    if (!apAppData->fpHead) {
        warnDebug(__FILE__, __LINE__, "Flight list is empty. Cannot search for ID: %s", szFlightId);
        return NULL;
    }
    /*loop through and find flight */
    fpCurrentFlight = apAppData->fpHead;
    while (fpCurrentFlight) {
        fpNextFlight = fpCurrentFlight->fpNext;

        if (strcmp(fpCurrentFlight->szFlightId, szFlightId) == 0) {
            normalDebug(__FILE__, __LINE__, "Flight with ID %s found.", szFlightId);
            return fpCurrentFlight;
        }

        fpCurrentFlight = fpNextFlight;
    }
    warnDebug(__FILE__, __LINE__, "Flight with ID %s not found.", szFlightId);
    return NULL;/*if not found return null*/
}
int deleteFlightByFlightId(APPDATA* apAppData, char * szFlightId){
    int iTotalDeletedFlights = 0;
    FLIGHT * fpCurrentFlight = NULL;
    FLIGHT * fpNextFlight = NULL;
    /* check if list empty*/
    normalDebug(__FILE__, __LINE__, "Starting deletion of flight(s) with ID: %s", szFlightId);
    if (!apAppData->fpHead){
        warnDebug(__FILE__, __LINE__, "Flight list is empty. Nothing to delete.");
        return iTotalDeletedFlights; 
    }

    fpCurrentFlight = apAppData->fpHead;
    while(fpCurrentFlight){
        fpNextFlight = fpCurrentFlight->fpNext;
        
        /*check if flight have same flight id if not continue the loop */
        if(strcmp(fpCurrentFlight->szFlightId, szFlightId) != 0){
            fpCurrentFlight = fpNextFlight;
            continue;
        }
        /*removing the flight and clean from memory*/
        normalDebug(__FILE__, __LINE__, "Deleting flight with ID: %s", fpCurrentFlight->szFlightId);
        if(!fpCurrentFlight->fpPrev){
            normalDebug(__FILE__, __LINE__, "Deleting  flight with ID: %s", fpCurrentFlight->szFlightId);
            apAppData->fpHead = fpNextFlight;
            if (fpNextFlight){
                fpNextFlight->fpPrev = NULL;
            }
            cleanFlightFromMemory(fpCurrentFlight);/*clean from memory */
        }else{
            normalDebug(__FILE__, __LINE__, "Deleting  flight with ID: %s", fpCurrentFlight->szFlightId);
            fpCurrentFlight->fpPrev->fpNext = fpNextFlight;
            if (fpNextFlight){
                fpNextFlight->fpPrev =  fpCurrentFlight->fpPrev;
            }
            cleanFlightFromMemory(fpCurrentFlight);/*clean from memory */
        }
        fpCurrentFlight = fpNextFlight;
        
        apAppData->iTotalFlights = apAppData->iTotalFlights - 1;
        iTotalDeletedFlights++;
    }/*end of while loop*/

    return iTotalDeletedFlights;/*return the total flight deleted flight*/
}


int getFlightNumberByDestination(struct _APPDATA* apAppData, char *szDestination){
    int iFlightNumber = 1;
    FLIGHT * fpCurrentFlight = NULL;
    FLIGHT * fpNextFlight = NULL;
    /* check if list empty*/
    normalDebug(__FILE__, __LINE__, "Searching for flight with Destination: %s", szDestination);
    if (!apAppData->fpHead) {
        warnDebug(__FILE__, __LINE__, "Flight list is empty. Cannot search for Destination: %s", szDestination);
        return 0;
    }
    /*loop through and return counter when find the counter/iFlightNumber*/
    fpCurrentFlight = apAppData->fpHead;
    while (fpCurrentFlight) {
        fpNextFlight = fpCurrentFlight->fpNext;
        
        if (strcmp(fpCurrentFlight->szDestination, szDestination) == 0) {
            normalDebug(__FILE__, __LINE__, "Flight with Destination %s found.", szDestination);
            return iFlightNumber;
        }

        fpCurrentFlight = fpNextFlight;
        iFlightNumber++;
    }
    warnDebug(__FILE__, __LINE__, "Flight with Destination %s not found.", szDestination);
    return 0;/*not found return 0, because flight number starts at 1*/
}
/*delete all flights*/
void deleteAllFlight(APPDATA* apAppData) {
    FLIGHT *fpCurrentFlight = apAppData->fpHead;
    FLIGHT *fpNextFlight = NULL;

    while (fpCurrentFlight) {
        fpNextFlight = fpCurrentFlight->fpNext;
        deleteAllPassengers(fpCurrentFlight);
        cleanFlightFromMemory(fpCurrentFlight);
        fpCurrentFlight = fpNextFlight;
    }
    apAppData->fpHead = NULL;
    apAppData->iTotalFlights = 0;
    normalDebug(__FILE__, __LINE__, "All flights deleted successfully.");
}



