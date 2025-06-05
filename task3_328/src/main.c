# include <stdio.h>
# include "../include/menu.h"
# include "../include/appdata.h"
# include "../include/debugger.h"

/*
målet har vært å vise mest mulig bredde i faget. 
gjør små mye jeg rekker før jeg starter på neste oppgave.
jeg kommenterer der jeg følger det trengs, og har prøvd å skrive
enkel metode nav for å gjøre det lett leslig
jeg har også resirklert tidligere kode fra tidligere oppgaver gjort i emnet selv :). derfor mye innhold her.  

kan være at du må ha stor skjerm på terminal vinudet for å se UI best mulig :)
*/

/* 
this is main.c file and the communication flow for this file is 
main.c->menu.c
main.c->appdata.c (just to init appdata) 

main.c is a good place to describe the overall comuniaction flow
main.c->menu.c->printStyle.c
main.c->menu.c->service.c->flightList.c
main.c->menu.c->service.c->passengerFlight.c

look at each *.c file to understand it better :)
*/

int main(){
    int iNumberSelected = 0;
    int quit = 1;
    APPDATA *pAppData = NULL;
    activeDebug(ENABLE_DEBUG);/*activating debugger*/
    pAppData = initAppData();
    if (!pAppData){
        printf("Could not allocate memory for pAppData\n");
        freeAppData(pAppData);
        return 1;
    }
    while(quit){
        iNumberSelected = printMainMenu();/*display main menu*/
        switch(iNumberSelected){
            case 1:
                addFlightMenu(pAppData);/*display add flight menu*/
                break;
            case 2:
                addPassengerMenu(pAppData);/*display add passenger menu*/
                break;
            case 3:
                viewFlightByNumber(pAppData);/*display view flight  menu*/
                break;
            case 4:
                findFlightNumberByDestinationMenu(pAppData);/*display find flight menu*/
                break;
            case 5:
                deleteFlightMenu(pAppData);/*display delete flight menu*/
                break;
            case 6:
                changePassengerSeatMenu(pAppData);/*display change passenger seat menu*/
                break;
            case 7:
                searchPassengerInFlightsMenu(pAppData);/*display search passenger menu */
                break;
            case 8:
                passengersBookedOnMultipleFlightsMenu(pAppData);/* display passenger on more then on flight menu
                                                                    note, the menu here is no pretty like the others.
                                                                    I simply did not have time to continue.
                                                                    But the good thing is, the functionality is 
                                                                    still there. the UI is just not pretty like the other menus. 
                                                                    */
                break;
            case 9:
                exitMenu(pAppData);
                quit = 0;
                break;
            default:
                break;
        }
    }
    freeAppData(pAppData);
    return 0;
}