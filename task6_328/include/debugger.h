typedef enum {
    ENABLE_DEBUG,
    DISABLE_DEBUG,
    GET_DEBUG
} eDebug;
int activeDebug(eDebug debugType);
void normalDebug(char *cpzFile, int iLine, char *cpzFormat, ...);
void warnDebug(char *cpzFile, int iLine, char *cpzFormat, ...);
void errorDebug(char *cpzFile, int iLine, char *cpzFormat, ...);

