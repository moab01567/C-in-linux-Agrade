#ifndef MENU_H
#define MENU_H
struct _APPDATA;
int validateUserInputNumber(char * czInput,  int iMin,  int iMax);
int  printFlightInfoAtTheRightSideOfFrame(struct _APPDATA *pAppData, int iWidth);
void deleteFlightMenu(struct _APPDATA *pAppData);
void cleanKeyboardStream();
void changePassengerSeatMenu(struct _APPDATA *pAppData);
void addFlightMenu(struct _APPDATA *pAppData);
void addPassengerMenu(struct _APPDATA *pAppData);
void viewFlightByNumber(struct _APPDATA *pAppData);
int getUserInputString(char* szPrompt, char*szStoreData, int idataSize, int iHeight, int iWidth);
void findFlightNumberByDestinationMenu(struct _APPDATA *pAppData);
void searchPassengerInFlightsMenu(struct _APPDATA *pAppData);
void showSearchedFlights(struct _APPDATA*pAppData, char *szPassengerName,int iTotalFlightFound , char *szStatusMessage);
int printMainMenu();
void passengersBookedOnMultipleFlightsMenu(struct _APPDATA *pAppData);
void exitMenu(struct _APPDATA *pAppData);
#endif
