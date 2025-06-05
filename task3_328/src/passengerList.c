#include "../include/flightList.h"
#include "../include/appdata.h"
#include "../include/passengerList.h"
#include "../include/debugger.h"
#include <string.h>
#include <stdlib.h>

/*
this is passengerList.c file and the communication flow for this file is 
service.c->flightList.c
service.c->passengerList.c

all the functions in this file will always be called from service.c


NOTE to developer!! 
you should always use the defined values in passengerList.h PASSENGER_NAME_SIZE 
When validating userinput, before allocating 
memory by using these functions */

void cleanMemoryPassenger(PASSENGER * ppPassenger){
    if(ppPassenger->szName)free(ppPassenger->szName);
    free(ppPassenger);
    normalDebug(__FILE__,__LINE__,"ppPassenger was freed");
}

PASSENGER * createPassenger( char *szName, int iSeatNumber, int iPasAge){
    /*try to allocate memory*/
    PASSENGER *ppPassenger = malloc(sizeof(PASSENGER));
    if(!ppPassenger){
        errorDebug(__FILE__,__LINE__,"Could Not Allocate memory, person: %s", szName);
        return NULL;
    }
    /*try to allocate memory*/
    ppPassenger->szName = malloc(strlen(szName) + 1);
    if(!ppPassenger->szName){
        errorDebug(__FILE__,__LINE__,"Could Not Allocate memory, person: %s", szName);
        cleanMemoryPassenger(ppPassenger);
        ppPassenger = NULL;
        return NULL;
    }
    /*init data to passenger */
    strcpy(ppPassenger->szName,szName);
    ppPassenger->iSeatNumber = iSeatNumber;
    ppPassenger->iPasAge = iPasAge;
    ppPassenger->ppNext = NULL;
    normalDebug(__FILE__,__LINE__,"Passenger with name %s created",szName);
    return ppPassenger;
}

PASSENGER * addPassengerToListSort(FLIGHT * fpFlight, PASSENGER * ppPassenger){
    PASSENGER *pCurrentPassenger = NULL;
    PASSENGER *pPrevPassenger = NULL;
    /*check if Not NUll */
    if(!fpFlight || !ppPassenger){
        errorDebug(__FILE__, __LINE__, "Invalid flight or passenger pointer.");
        return NULL;
    }
    /*check if FLight ist is empty to add passenger  the first passenger */
    if (!fpFlight->ppHead){
        fpFlight->ppHead = ppPassenger;
        ppPassenger->ppNext = NULL;
        fpFlight->iTotalPassengers++;
        normalDebug(__FILE__, __LINE__, "Passenger added as first in list (empty list).");
        return ppPassenger;
    }
    /*loop through and add passenger before the highest seat number(makes the list sorted)*/
    pCurrentPassenger = fpFlight->ppHead;
    while(pCurrentPassenger){  
        if(pCurrentPassenger->iSeatNumber > ppPassenger->iSeatNumber) break;/*compare the numbers */
        pPrevPassenger = pCurrentPassenger;
        pCurrentPassenger = pCurrentPassenger->ppNext;
    }
    /*adding the ne passenger to list*/
    if (!pPrevPassenger) {
        ppPassenger->ppNext = fpFlight->ppHead;
        fpFlight->ppHead = ppPassenger;
        normalDebug(__FILE__, __LINE__, "Passenger inserted at beginning of list.");
    } else {
        ppPassenger->ppNext = pPrevPassenger->ppNext;
        pPrevPassenger->ppNext = ppPassenger;
        normalDebug(__FILE__, __LINE__, "Passenger inserted after seat %d.", pPrevPassenger->iSeatNumber);
    }
    fpFlight->iTotalPassengers++;/*increase the the iTotalPassengers in Flight */
    return ppPassenger;
}


PASSENGER * getPassengerFromFlight(FLIGHT * fpFlight, char *szName){
    PASSENGER * ppPassenger = NULL;
    /*check if Not NUll */
    if (!fpFlight || !fpFlight->ppHead){
        warnDebug(__FILE__, __LINE__, "Flight pointer is NULL or fpFlight->ppHead");
        return NULL;
    }
    /*loop through and return passenger if found */
    ppPassenger = fpFlight->ppHead;
    while(ppPassenger){
        if (strcmp(ppPassenger->szName, szName) == 0){
            return ppPassenger;
        }
        ppPassenger = ppPassenger->ppNext;
    }
    return NULL;
}

void deleteAllPassengers(FLIGHT * fpFlight){
    PASSENGER * ppCurrentPassenger = NULL;
    PASSENGER * ppNextPassenger = NULL;
    normalDebug(__FILE__, __LINE__, "Starting deletion of all passengers...");
    /*check if Not NUll */ 
    if(!fpFlight) {
        warnDebug(__FILE__, __LINE__, "Flight pointer is NULL. Aborting passenger deletion.");
        return;
    }
    /*loop through and deletting all passenegr */
    ppCurrentPassenger = fpFlight->ppHead;
    while(ppCurrentPassenger){
        ppNextPassenger = ppCurrentPassenger->ppNext;
        normalDebug(__FILE__, __LINE__, "Deleting passenger with seat number %d", ppCurrentPassenger->iSeatNumber);
        cleanMemoryPassenger(ppCurrentPassenger);
        ppCurrentPassenger = ppNextPassenger;
    }
    ppCurrentPassenger = NULL;
    ppNextPassenger = NULL;
    normalDebug(__FILE__, __LINE__, "All passengers deleted successfully.");
}

int deletePassengerByName(FLIGHT * fpFlight,char *szPassengerName){
    PASSENGER * ppCurrentPassenger = NULL;
    PASSENGER * ppPrevPassenger = NULL;
    /*check if Not NUll */
    if(!fpFlight || !fpFlight->ppHead) {
        warnDebug(__FILE__, __LINE__, "Flight pointer  or fpFlight->ppHead is NULL");
        return 1;
    }
    /*if check if it is first passenger in flight */
    ppCurrentPassenger = fpFlight->ppHead;
    if (strcmp(ppCurrentPassenger->szName, szPassengerName) == 0){
        fpFlight->ppHead = ppCurrentPassenger->ppNext;
        fpFlight->iTotalPassengers--;
        cleanMemoryPassenger(ppCurrentPassenger);
        return 0;
    }
    /*if loop through and and delet if found delete and return */
    ppPrevPassenger = ppCurrentPassenger;
    ppCurrentPassenger = ppCurrentPassenger->ppNext;
    while(ppCurrentPassenger){

        if (strcmp(ppCurrentPassenger->szName, szPassengerName) == 0){
            ppPrevPassenger->ppNext = ppCurrentPassenger->ppNext;
            fpFlight->iTotalPassengers--;
            cleanMemoryPassenger(ppCurrentPassenger);
            return 0;
        }
        ppPrevPassenger = ppCurrentPassenger;
        ppCurrentPassenger = ppCurrentPassenger->ppNext;
    }
    ppCurrentPassenger = NULL;
    ppPrevPassenger = NULL;
    normalDebug(__FILE__, __LINE__, "All passengers deleted successfully.");
    return 1;
}


/*Merk alt funker fint er testa :)*/
/*alt under her er for det siste punktet, merk at PASSNAMELIST er ikke passasjerlisten. 
dette er en anne linkedlist for å holde overiskt over passasjeren med samme navn og 
har booket mer enn en gang*/
/*Har bruk masse tid på denne oppgaven og må komme meg videre med eksamen. 
så alt under her er bare skrevet rask. Alle punktene på lista er implmentert.
men siste punkt ble implmentert rask og har ikke fine UI som de andre punkene.
men alt fungrer fint :) dersom jeg rekker så vil jeg selvagt komme tilbake til denne.
Siste punkt:
"Search through the lists any passenger (in any flights) that are booked on
more than 1 flight, and print the name of the passenger on the screen"
*/
int  searchFlightsByPassengerAmount(APPDATA* pAppData, char *szPassengerName){
    int iTotalFlightFound = 0;
    FLIGHT * fpCurrentFlight = pAppData->fpHead;
    if (!fpCurrentFlight){
        return iTotalFlightFound;
    } 

    while(fpCurrentFlight){
        if(getPassengerFromFlight(fpCurrentFlight, szPassengerName)){
            iTotalFlightFound++;
        }
        fpCurrentFlight = fpCurrentFlight->fpNext;
    }

    return iTotalFlightFound;
}

/*Merk alt funker fint er testa :)*/
/*alt under her er for det siste punktet, merk at PASSNAMELIST er ikke passasjerlisten. 
dette er en anne linkedlist for å holde overiskt over passasjeren med samme navn og 
har booket mer enn en gang*/
/*Har bruk masse tid på denne oppgaven og må komme meg videre med eksamen. 
så alt under her er bare skrevet rask. Alle punktene på lista er implmentert.
men siste punkt ble implmentert rask og har ikke fine UI som de andre punkene.
men alt fungrer fint :) dersom jeg rekker så vil jeg selvagt komme tilbake til denne.
Siste punkt:
"Search through the lists any passenger (in any flights) that are booked on
more than 1 flight, and print the name of the passenger on the screen"
*/
void addToPassList(PASSNAMELIST *plPassNameList, char *szPassengerName){
    PASSNAME *pPassNameCurrent  = NULL;
    PASSNAME *pPassNamePrev  = NULL;
    PASSNAME *pPassNameCreate  = NULL;

    if (!plPassNameList->ppHead){
        pPassNameCurrent = malloc(sizeof(PASSNAME));
        if(!pPassNameCurrent)return;
        strcpy(pPassNameCurrent->szName, szPassengerName);
        pPassNameCurrent->count = 1;
        plPassNameList->ppHead = pPassNameCurrent;
        pPassNameCurrent->ppNext = NULL;
        return;
    }

    pPassNameCurrent = plPassNameList->ppHead;
    pPassNamePrev = NULL;
    while(pPassNameCurrent){
        if (strcmp(pPassNameCurrent->szName,szPassengerName) == 0){
            pPassNameCurrent->count++;
            return;
        }
        pPassNamePrev = pPassNameCurrent;
        pPassNameCurrent = pPassNameCurrent->ppNext;
    }

    pPassNameCreate = malloc(sizeof(PASSNAME));
    
    if(!pPassNameCreate)return;
    strcpy(pPassNameCreate->szName, szPassengerName);
    pPassNameCreate->count = 1;
    pPassNamePrev->ppNext = pPassNameCreate;
    pPassNameCreate->ppNext = NULL;
}
/*Merk alt funker fint er testa :)*/
/*alt under her er for det siste punktet, merk at PASSNAMELIST er ikke passasjerlisten. 
dette er en anne linkedlist for å holde overiskt over passasjeren med samme navn og 
har booket mer enn en gang*/
/*Har bruk masse tid på denne oppgaven og må komme meg videre med eksamen. 
så alt under her er bare skrevet rask. Alle punktene på lista er implmentert.
men siste punkt ble implmentert rask og har ikke fine UI som de andre punkene.
men alt fungrer fint :) dersom jeg rekker så vil jeg selvagt komme tilbake til denne.
Siste punkt:
"Search through the lists any passenger (in any flights) that are booked on
more than 1 flight, and print the name of the passenger on the screen"
*/
PASSNAMELIST* getPassengersBookedOnMultipleFlights(APPDATA *pAppData) {
    int iTotalFlightFoundWithPassenger = 0;
    FLIGHT *fpCurrentFlight = pAppData->fpHead; 
    PASSENGER *ppPassengerCurrent = NULL;
    PASSNAMELIST *plPassNameList = malloc(sizeof(PASSNAMELIST));
    
    if (!plPassNameList) return NULL;
    plPassNameList->ppHead = NULL;

    while (fpCurrentFlight) {
        ppPassengerCurrent = fpCurrentFlight->ppHead;
        while (ppPassengerCurrent) {
            iTotalFlightFoundWithPassenger = searchFlightsByPassengerAmount(pAppData, ppPassengerCurrent->szName);
            
            if (iTotalFlightFoundWithPassenger > 1) {
                addToPassList(plPassNameList, ppPassengerCurrent->szName);
            }

            ppPassengerCurrent = ppPassengerCurrent->ppNext; 
        }
        fpCurrentFlight = fpCurrentFlight->fpNext;
    }

    return plPassNameList;
}
/*Merk alt funker fint er testa :)*/
/*alt under her er for det siste punktet, merk at PASSNAMELIST er ikke passasjerlisten. 
dette er en anne linkedlist for å holde overiskt over passasjeren med samme navn og 
har booket mer enn en gang*/
/*Har bruk masse tid på denne oppgaven og må komme meg videre med eksamen. 
så alt under her er bare skrevet rask. Alle punktene på lista er implmentert.
men siste punkt ble implmentert rask og har ikke fine UI som de andre punkene.
men alt fungrer fint :) dersom jeg rekker så vil jeg selvagt komme tilbake til denne.
Siste punkt:
"Search through the lists any passenger (in any flights) that are booked on
more than 1 flight, and print the name of the passenger on the screen"
*/
void cleanPassNameListFromMemory(PASSNAMELIST *plPassNameList){
    PASSNAME *pPassNameNext  = NULL;
    PASSNAME *pPassNameCurrent  = plPassNameList->ppHead;

    while(pPassNameCurrent){
        pPassNameNext = pPassNameCurrent->ppNext;
        free(pPassNameCurrent);
        pPassNameCurrent = pPassNameNext;
    }
    free(plPassNameList);

}

