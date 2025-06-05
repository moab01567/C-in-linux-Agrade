# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include "../include/printStyle.h"
# include "../include/service.h"
# include "../include/flightList.h"
# include "../include/passengerList.h"
# include "../include/appdata.h"

/* 
this is menu.c file and the communication flow for this file is 
menu.c->service.c
menu.c->printStyle.c (printStyle works more like a helper)

all the functions in this file will always be called from main.c
*/



/*
Clean the stdin stream, so we can take a new user input 
is called when we know there is char left in stdin
is also used when we just want the user to hot enter. 
*/
void cleanKeyboardStream(){
    while (fgetc(stdin) != '\n' && !feof(stdin));
}

/*
used when getting a string from user. 
return 1 if user just hit enter 
and 0 if any text was written
*/
int getUserInputString(char* szPrompt, char*szStoreData, int idataSize, int iHeight, int iWidth){
    printPrompt(szPrompt ,iHeight, iWidth);
    if (fgets(szStoreData, idataSize, stdin) == NULL)return 1;/*return 1 it could not read from stdin */
    if (szStoreData[0] == '\n')return 1; /*return 1 if just enter */

    /*just check if \n pressent if so change it with a \0*/
    if(strchr(szStoreData, '\n')){
        szStoreData[strlen(szStoreData) - 1] = '\0';
    }else{
        cleanKeyboardStream();
    }

    return 0;
}
/*
used when getting a number from user. 
return -1 if user just hit enter 
return the number if the user write 
a valid number
*/
int getUserInputNumber(char* szPrompt, char* szErrorMessage, int iMin,  int iMax, int iHeight, int iWidth){
    char szBuffer[12] = "\0";
    int iValidInt = 0;
    int iDigitOnly = 0;
    int i = 0;
    while(!iValidInt){
        if(getUserInputString(szPrompt, szBuffer, sizeof(szBuffer),  iHeight, iWidth) == 1)
            return -1; /*return -1 if just enter */
       
        iDigitOnly = 1;
        for (i = 0; szBuffer[i] != '\0'; i++) { /*check if digit*/
            if ('0' <= szBuffer[i] && szBuffer[i] <= '9') {
                continue;
            }
            iDigitOnly = 0;
            break;
        }/*end for*/

        /*check if digit in range og iMin and iMax */
        if (iDigitOnly && (iMin <= atoi(szBuffer) && atoi(szBuffer) <= iMax)) {
            printErrorMessage(" ", iHeight, iWidth);
            iValidInt = 1;
        }else{
            printErrorMessage(szErrorMessage, iHeight, iWidth);
        }/*end if*/

    }/*end while*/

    return atoi(szBuffer);/*return value if that was entered as a int*/
}

/*
prints the flight on the right side of the frame if any flight in list
just create the flight and you will understand.
return 1 if not printed 
return 0 if printed
*/
int  printFlightInfoAtTheRightSideOfFrame(APPDATA *pAppData, int iWidth){
    int i = 0;
    FLIGHT *fpFlight = NULL;
    /*check if any flight in list */
    if (pAppData->iTotalFlights <= 0){
        return 1;
    }
    moveCursor(1, iWidth+1);/*move cursor to the right of the frame */
    printf("All Avalble Flights(%d):", pAppData->iTotalFlights);
    fpFlight = pAppData->fpHead;
 
    for(i = 0; i < pAppData->iTotalFlights; i++){/*print all the flights */
        moveCursor(i + 2, iWidth+1);
        printf("FlightID: %s | Item Number: %d ", fpFlight->szFlightId, i + 1);
        fpFlight = fpFlight->fpNext;
    }/*end for */
    fpFlight = NULL;
    return 0;
}

/*display the add Flight menu */
void addFlightMenu(APPDATA *pAppData){
    int iWidth = 50;
    int iHeight = 12;
    char szFlightId[FLIGHT_ID_SIZE] = {0};
    char szDestination[FLIGHT_DESTINATION_SIZE] = {0};
    char szStatusMessage[100] = {0};
    int iTotalSeats = 0;
    int iDepartureTime = 0;
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Add Flight Menu", "(Hit Return Anytime To Cancel)");
    printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth);/*prints all the flights right side of frame*/

    /*ask for  Flight Id */
    if(getUserInputString("\t Flight Id: ",szFlightId,FLIGHT_ID_SIZE,iHeight,iWidth) == 1)return;
    printStart(4,"\t Flight Id: ");
    printf(szFlightId);

    /*ask for  Destination */
    if(getUserInputString("\t Destination:  ",szDestination, FLIGHT_DESTINATION_SIZE, iHeight,iWidth) == 1)return;
    printStart(5,"\t Destination: ");
    printf(szDestination);

    /*ask for  TotalSeats */
    iTotalSeats= getUserInputNumber("\t TotalSeats: ", "Not valid input, range(1-999)", 1,  999, iHeight,  iWidth);
    if(iTotalSeats == -1) return;
    printStart(6,"\t TotalSeats: ");
    printf("%d", iTotalSeats);

    /*ask for  DepartureTime */
    iDepartureTime = getUserInputNumber("\t DepartureTime: ", "Not valid input, range(1-999999999)", 1,  999999999, iHeight,  iWidth);
    if(iDepartureTime == -1) return;
    printStart(7,"\t DepartureTime: ");
    printf("%d", iDepartureTime);
    
    /*does the task of adding flight, if susses print status, if not print error*/
    if(serviceAddFlightToList(pAppData, szFlightId, szDestination, iTotalSeats, iDepartureTime, szStatusMessage)){
        printErrorMessage(szStatusMessage,iHeight, iWidth);
    }else{
        printStatusMessage(szStatusMessage,iHeight, iWidth);
    }
    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
    cleanKeyboardStream();
}

/*display the add Passenger menu */
void addPassengerMenu(APPDATA *pAppData){
    int iWidth = 50;
    int iHeight = 12;
    char szFlightId[FLIGHT_ID_SIZE] = {0};
    char szName[PASSENGER_NAME_SIZE] = {0};
    int iSeatNumber = 0;
    int iPasAge = 0;
    char szStatusMessage[50] = {0};
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Add Passenger To Flight Menu","(Hit Return Anytime To Cancel)");
    /*if not any flight in system prompt error to user*/
    if (printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth)){
        printErrorMessage( "No Flights where found." , iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
        return;
    }

    /*ask for  Flight Id */
    if(getUserInputString("\t Flight Id: ",szFlightId,FLIGHT_ID_SIZE,iHeight,iWidth) == 1)return;
    printStart(4,"\t Flight Id: ");
    printf(szFlightId);

    /*ask for Passenger Name */
    if(getUserInputString("\t Passenger Name: ",szName,PASSENGER_NAME_SIZE,iHeight,iWidth) == 1)return;
    printStart(5,"\t Passenger Name: ");
    printf(szName);

    /*ask for  SeatNumber */
    iSeatNumber = getUserInputNumber("\t Seat Number: ", "Not valid input, range[1...999]", 1,  999, iHeight,  iWidth);
    if(iSeatNumber == -1) return;
    printStart(6,"\t Seat Number: ");
    printf("%d", iSeatNumber);

    /*ask for  Age */
    iPasAge = getUserInputNumber("\t Passenger Age: ", "Not valid input, range[1...120]", 1,  120, iHeight,  iWidth);
    if(iPasAge == -1) return;
    printStart(7,"\t Passenger Age: ");
    printf("%d", iPasAge);

    /*does the task of adding Passenger, if susses print status, if not print error*/
    if(serviceAddPassengerToFlight(pAppData, szFlightId, iSeatNumber,szName,iPasAge, szStatusMessage)){
        printErrorMessage(szStatusMessage,iHeight, iWidth);
    }else{
        printStatusMessage(szStatusMessage,iHeight, iWidth);
    }
    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
    cleanKeyboardStream();
}
/*
this function just display a flight and there passenger on the screen 
it do not call service in any way to manipulate data, flightList and passengerList*/
void viewFlightDetails(FLIGHT* fpFlight){
    int counter = 0;
    int iLinesPerFlight = 3;
    int iLinesPerPassenger = 5;
    int iWidth = 75 ;
    int iHeight = 7 + iLinesPerFlight;
    char szBufferInfo[100] = {0};
    PASSENGER *ppPassenger = NULL;
    
    /*return if flight NULL*/
    if(!fpFlight)return;
    ppPassenger = fpFlight->ppHead;
    iHeight = iHeight + (fpFlight->iTotalPassengers * iLinesPerPassenger);
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Flight Details Menu"," ");
    /*print flight info first */
    sprintf(szBufferInfo, "FLIGHT ID: %s | Destination: %s",fpFlight->szFlightId, fpFlight->szDestination);
    printCentered(4, iWidth, szBufferInfo);
    sprintf(szBufferInfo, "Total Seats: %d | Departure Time: %d", fpFlight->iTotalSeats, fpFlight->iDepartureTime);
    printCentered(5, iWidth,szBufferInfo);
    sprintf(szBufferInfo, "Passengers: %d", fpFlight->iTotalPassengers);
    printCentered(6, iWidth,szBufferInfo);
    /*loop through Passengers and print there info info first */
    while(ppPassenger){
        makeLine(7 + (counter * iLinesPerPassenger), iWidth);
        printCentered(8 + (counter * iLinesPerPassenger), iWidth, "PASSENGER:");
        sprintf(szBufferInfo, "Seat Number: %d", ppPassenger->iSeatNumber);
        printCentered(9 + (counter * iLinesPerPassenger), iWidth, szBufferInfo);
        sprintf(szBufferInfo, "Name: %s | Age: %d", ppPassenger->szName, ppPassenger->iPasAge);
        printCentered(10 + (counter * iLinesPerPassenger), iWidth, szBufferInfo);
        ppPassenger = ppPassenger->ppNext;
        counter++;
    }/*while end */
    ppPassenger = NULL;

    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight, iWidth);
    cleanKeyboardStream();
}
/*display the Flight By Number menu */
void viewFlightByNumber(APPDATA *pAppData){
    int  iFlightNumber = 0;
    char szInfoMessage[100] = {0};
    int iWidth = 50;
    int iHeight = 8;
    FLIGHT *fpFlight = NULL;
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"View Flight By Number Menu", "(Hit Return Anytime To Cancel)");
    /*check if there is any flight, if not any flight in system prompt error to user*/ 
    if(printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth)){
        printErrorMessage( "No Flights where found." , iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
        return;
    }else{
        sprintf(szInfoMessage,"Total Flights: %d",pAppData->iTotalFlights);
        printStatusMessage(szInfoMessage, iHeight, iWidth);
    }/*endif*/
    /*ask for Flight Number */
    sprintf(szInfoMessage,"Not valid Number, range is [1..%d]",pAppData->iTotalFlights);
    iFlightNumber = getUserInputNumber("\t Flight Number: ", szInfoMessage, 1,  pAppData->iTotalFlights, iHeight,  iWidth);
    
    /*-1 means just return */
    if(iFlightNumber == -1) return;
    /*get the flight by number */
    fpFlight = serviceGetFlightByNumber(pAppData,iFlightNumber);
    /*call the function above to print the flight*/
    viewFlightDetails(fpFlight);
    fpFlight = NULL;
}
/*display the find Flight Number By Destination Menu */
void findFlightNumberByDestinationMenu(APPDATA *pAppData){
    int iWidth = 50;
    int iHeight = 8;
    char szDestination[FLIGHT_DESTINATION_SIZE] = {0};
    char szStatusMessage[50] =  {0};
    int iFlightNumber = 0;
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Find Flight Number By Destination Menu", "(Hit Return Anytime To Cancel)");
     /*check if there is any flight, if not any flight in system prompt error to user*/ 
    if (printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth)){
        printErrorMessage( "No Flights where found." , iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
        return;
    }
    /*ask for Destination */
    if (getUserInputString("\tDestination: ",szDestination,FLIGHT_DESTINATION_SIZE,iHeight, iWidth))
        return;
    
    /*get the flight number that is on destination*/
    iFlightNumber = serviceGetFlightNumberByDestination(pAppData, szDestination, szStatusMessage);
    if (iFlightNumber){
        printStatusMessage(szStatusMessage,iHeight, iWidth);
    }else{
        printErrorMessage(szStatusMessage, iHeight, iWidth);
    }

    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight, iWidth);
    cleanKeyboardStream();
}
/*display the delete Flight Menu */
void deleteFlightMenu(APPDATA *pAppData){
    int iWidth = 50;
    int iHeight = 8;
    char szFlightId[FLIGHT_ID_SIZE] = {0};
    char szStatusMessage[50] = {0};
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Delete Flight Menu", "(Hit Return Anytime To Cancel)");
     /*check if there is any flight, if not any flight in system prompt error to user*/ 
    if (printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth)){
        printErrorMessage( "No Flights where found." , iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
        return;
    }
    /*ask for  Flight Id */
    if(getUserInputString("\t Flight Id: ",szFlightId,FLIGHT_ID_SIZE,iHeight,iWidth) == 1)return;
    /*try to delete flight and display status*/
    if(serviceDeleteFlightByFlightId(pAppData,szFlightId,szStatusMessage)){
        printErrorMessage(szStatusMessage,iHeight, iWidth);
    }else{
        printStatusMessage(szStatusMessage,iHeight, iWidth);
    }
    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight, iWidth);
    cleanKeyboardStream();
}
/*display the change Passenger Seat Menu */
void changePassengerSeatMenu(APPDATA *pAppData){
    int iWidth = 50;
    int iHeight = 8;
    char szFlightId[FLIGHT_ID_SIZE] = {0};
    char szPassengerName[PASSENGER_NAME_SIZE] = {0};
    int iSeatNumber = 0;
    char szStatusMessage[50] = {0};
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Change Passenger Seat Menu", "(Hit Return Anytime To Cancel)");
    
    /*check if there is any flight, if not any flight in system prompt error to user*/ 
    if (printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth)){
        printErrorMessage( "No Flights where found." , iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
        return;
    }
    /*ask for  Flight Id */
    if(getUserInputString("\t Flight Id: ",szFlightId,FLIGHT_ID_SIZE,iHeight,iWidth) == 1)return;
    printStart(4,"\t Flight Id: ");
    printf(szFlightId);
    /*ask for Passenger Name*/
    if(getUserInputString("\t Passenger Name: ",szPassengerName,PASSENGER_NAME_SIZE,iHeight,iWidth) == 1)return;
    printStart(5,"\t Passenger Name: ");
    printf(szPassengerName);
    /*ask for New Seat*/
    iSeatNumber = getUserInputNumber("\t Seat Number: ", "Not valid input, range[1...999]", 1,  999, iHeight,  iWidth);
    if(iSeatNumber == -1) return;
    printStart(6,"\t Seat Number: ");
    printf("%d", iSeatNumber);
    /*try to change passenger seat and display status*/
    if(serviceChangePassengerSeat(pAppData,szFlightId,szPassengerName,iSeatNumber, szStatusMessage)){
        printErrorMessage(szStatusMessage,iHeight, iWidth);
    }else{
        printStatusMessage(szStatusMessage,iHeight, iWidth);
    }
    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight, iWidth);
    cleanKeyboardStream();
}
/*display the Flights id after passenger search */
void showSearchedFlights(APPDATA*pAppData,char *szPassengerName, int iTotalFlightFound , char *szStatusMessage){
    int iWidth = 50;
    int counter = 0;
    int iHeight = 9 + iTotalFlightFound ;
    FLIGHT * fpCurrentFlight = pAppData->fpHead; 
     /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Search Passenger In Flights Menu", "(Hit Return Anytime To Cancel)");
    printCentered(4 , iWidth, "Flights Id:");
    
    /*display all flight ids that after passenger name*/
    while(fpCurrentFlight){
        if(getPassengerFromFlight(fpCurrentFlight, szPassengerName)){
            printCentered(5 + counter , iWidth, fpCurrentFlight->szFlightId);
            counter++;
        }
        fpCurrentFlight = fpCurrentFlight->fpNext;
    }/*end while*/
    printStatusMessage(szStatusMessage,iHeight, iWidth);
    /*Waiting on user to hit enter */ 
    printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
    cleanKeyboardStream();
}

/*display the search Passenger In Flights Menu */
void searchPassengerInFlightsMenu(APPDATA*pAppData){
    int iWidth = 50;
    int iHeight = 8;
    char szStatusMessage[50] = {0};
    char szName[PASSENGER_NAME_SIZE] = {0};
    int iTotalFlightFound = 0;
    /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Search Passenger In Flights Menu", "(Hit Return Anytime To Cancel)");
    if (printFlightInfoAtTheRightSideOfFrame(pAppData,iWidth)){
        printErrorMessage( "No Flights where found." , iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
        return;
    }
    /*ask for Passenger Name */
    if(getUserInputString("\t Passenger Name: ",szName,PASSENGER_NAME_SIZE,iHeight,iWidth) == 1)return;
     /*try to  find passenger by name and display status*/
    iTotalFlightFound = serviceSearchFlightsByPassengerAmount(pAppData,szName, szStatusMessage);
    if(!iTotalFlightFound){
        printErrorMessage(szStatusMessage,iHeight, iWidth);
        printPrompt("\t Press enter, to return to Main Menu ",iHeight,iWidth);
        cleanKeyboardStream();
    }else{
        showSearchedFlights(pAppData,szName,  iTotalFlightFound , szStatusMessage);
    }
   
    
}

/*display exit menu*/
void exitMenu(APPDATA *pAppData){
    int iWidth = 30;
    int iHeight = 5;
    clearScreen();
    drawFrame(iWidth,iHeight,"Exit Menu"," ");
    printCentered(iHeight - 1, iWidth, "Bye");
    serviceDeleteAllFlights(pAppData);
    moveCursor(iHeight+1, 1);
}
/*display Main menu*/
int printMainMenu(){
    int iWidth = 75;
    int iHeight = 30;
    int contentOfY = 5;
    int iSelectedNumber = 0;
     /*clear  screen and draw frame */
    clearScreen();
    drawFrame(iWidth,iHeight,"Main Menu", " ");
    printByYX(4 + contentOfY,0, "\t\t1. Add flight.");
    printByYX(5 + contentOfY,0, "\t\t2. Add passenger (sorted by seat).");
    printByYX(6 + contentOfY,0, "\t\t3. View flight by number.");
    printByYX(7 + contentOfY,0, "\t\t4. Get Flight Number by destination.");
    printByYX(8 + contentOfY,0, "\t\t5. Delete flight and passengers ");
    printByYX(9 + contentOfY,0, "\t\t6. Change passenger seat.");
    printByYX(10 + contentOfY,0,"\t\t7. Search Passenger In Flights.");
    printByYX(11 + contentOfY,0,"\t\t8. Find passengers on multiple flights");
    printByYX(12 + contentOfY,0,"\t\t9. Exit.");
    /*ask for Menu */
    iSelectedNumber = getUserInputNumber("\t Select Number: ", "Not valid input, range[1..9]", 1,  9, iHeight,  iWidth);
    if(iSelectedNumber == -1) return 0;
    return iSelectedNumber;
}

/*Har bruk masse tid på denne oppgaven og må komme meg videre med eksamen. 
så alt under her er bare skrevet rask. Alle punktene på lista er implmentert.
men siste punkt ble implmentert rask og har ikke fine UI som de andre punkene.
men alt fungrer fint :) dersom jeg rekker så vil jeg selvagt komme tilbake til denne.
Siste punkt:
"Search through the lists any passenger (in any flights) that are booked on
more than 1 flight, and print the name of the passenger on the screen"
*/
void passengersBookedOnMultipleFlightsMenu(APPDATA*pAppData){
    PASSNAMELIST * pPassNameList = NULL;
    PASSNAME * pPassName = NULL; 
    pPassNameList = getPassengersBookedOnMultipleFlights(pAppData);
    pPassName =pPassNameList->ppHead;
    clearScreen();
    printf("Hadde ikke nok tid, UI ser Ikke bra ut her derfor. Men alt funker :)\n");
    if (!pPassNameList->ppHead){
        printf("No Passenger On Multiple FLIGHTs, Create some flights and passenger first\n");
        printf("Press enter, to return to Main Menu\n");
        cleanKeyboardStream();
        return;
    }
    printf("Passenger names: \n");

    while(pPassName){
        printf("Name: %s, Flights: %d\n",pPassName->szName, pPassName->count);
        pPassName = pPassName->ppNext;
    }
    cleanPassNameListFromMemory(pPassNameList);
    printf("Press enter, to return to Main Menu\n");
    cleanKeyboardStream();
}