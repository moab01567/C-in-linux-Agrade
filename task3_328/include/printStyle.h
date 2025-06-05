#ifndef PRINTSTYLE_h
#define PRINTSTYLE_h
enum Color{
    RED, 
    BLUE,
    GREEN,
    NORMAL
};
void moveCursor(int y, int x);
void drawFrame(int iFrameWidth, int iFrameHeight, char *szTitle, char *szSubTitle);
void printPrompt(char *szInputPrompt, int iFrameHeight ,int iFrameWidth);
void printCentered(int y, int width, char *text);
void clearScreen();
void printStart(int y, char *text);
void printByYX(int y, int x,  char *text);
void makeTextColor( enum Color cSetColor);
void makeLine(int y, int width);
void clearLine(int y, int width);
void printErrorMessage(char *szErrorMessage, int iFrameHeight ,int iFrameWidth);
void printStatusMessage(char *szStatusMessage, int iFrameHeight ,int iFrameWidth);
#endif
