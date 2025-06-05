# include <stdio.h>
# include <string.h>
# include "../include/printStyle.h"
#include <stdlib.h>
/* 
this is printStyle.c file and the communication flow for this file is 
menu.c->printStyle.c

all the functions in this file will always be called from menu.c
the functions her is small and simple. i will not use time to explain  
*/
void moveCursor(int y, int x) {
    printf("\033[%d;%dH", y, x);
}

void clearScreen() {
    printf("\033[2J\033[H");
}
void drawFrame(int iFrameWidth, int iFrameHeight, char *szTitle, char *szSubTitle) {
    int i = 0;
    moveCursor(1, 1);
    printf("╔");
    for ( i = 1; i < iFrameWidth - 2; i++) printf("═");
    printf("╗");
    for ( i = 2; i < iFrameHeight; i++) {
        moveCursor(i, 1);
        printf("║");
        moveCursor( i, iFrameWidth - 1);
        printf("║");
    }
    moveCursor(iFrameHeight, 1);
    printf("╚");
    for (i = 1; i < iFrameWidth - 2; i++) printf("═");
    printf("╝");

    printCentered(2,iFrameWidth, szTitle);
    makeLine(3, iFrameWidth);
    if(strcmp(szSubTitle, " ")){
        printCentered(3, iFrameWidth, szSubTitle);
    }
}
void printErrorMessage(char *szErrorMessage, int iFrameHeight ,int iFrameWidth){
    clearLine(iFrameHeight-3,iFrameWidth);
    makeTextColor(RED);
    printCentered(iFrameHeight-3,iFrameWidth,szErrorMessage);
    makeTextColor(NORMAL);
}
void printPrompt(char *szInputPrompt, int iFrameHeight ,int iFrameWidth){
    makeLine(iFrameHeight-2, iFrameWidth);
    clearLine(iFrameHeight-1, iFrameWidth);
    printStart(iFrameHeight-1, szInputPrompt);
}
void printStatusMessage(char *szStatusMessage, int iFrameHeight ,int iFrameWidth){
    makeTextColor(BLUE);
    printCentered(iFrameHeight-3,iFrameWidth, szStatusMessage);
    makeTextColor(NORMAL);
}
void makeLine(int y, int width){
    int i = 0;
    moveCursor(y, 2);
    for (i = 1; i < width - 2; i++) printf("═");
}

void clearLine(int y, int width){
    int i = 0;
    moveCursor(y, 2);
    for (i = 1; i < width - 2; i++) printf(" ");
}

void printCentered(int y, int width, char *text) {
    int len = strlen(text);
    int x = (width - len) / 2;
    moveCursor(y, x);
    printf("%s", text);
}


void printStart(int y, char *text){
    moveCursor(y, 2);
    printf("%s", text);
}

void printByYX(int y, int x, char *text){
    moveCursor(y, x);
    printf("%s", text);
}

void makeTextColor(enum Color cSetColor){
    switch(cSetColor){
        case RED:
            printf("\033[0;31m");
            break;
        case BLUE:
            printf("\033[0;34m");
            break;
        case GREEN:
            printf("\033[0;32m");
            break;
        case NORMAL:
            printf("\033[0m");
            break;
    }
}