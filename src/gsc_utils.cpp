#include "gsc_utils.hpp"
#include "libcod.hpp"



#if COMPILE_UTILS == 1

/*
=================
Sys_AnsiColorPrint
Transform Q3 colour codes to ANSI escape sequences
=================
*/
//#define MAXPRINTMSG 1024
//#define ColorIndex(c)	(((c) - '0') & 0x07)
//#define Q_COLOR_ESCAPE	'^'
//#define Q_IsColorString(p)	((p) && *(p) == Q_COLOR_ESCAPE && *((p)+1) && isdigit(*((p)+1))) // ^[0-9]





void gsc_utils_makelocalizedstring()
{
    char *str;

    if ( !stackGetParams("s", &str) )
    {
        stackError("gsc_utils_makelocalizedstring() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    stackPushString(str);

    VariableValue *var;
    int param = 0;

    var = &scrVmPub.top[-param];
    var->type = STACK_LOCALIZED_STRING;
}



#endif