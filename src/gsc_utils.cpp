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









void gsc_utils_sendcommandtoclient()
{
	int clientNum;
	char *message;

	if ( !stackGetParams("is", &clientNum, &message) )
	{
		stackError("gsc_utils_sendcommandtoclient() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	SV_GameSendServerCommand(clientNum, 0, message);
	stackPushBool(qtrue);
}



void gsc_utils_sendcommandtoserver()
{
	char *message;

	if ( !stackGetParams("s", &message) )
	{
		stackError("gsc_utils_sendcommandtoserver() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	SV_SendServerCommand(NULL, 1, message);
	stackPushBool(qtrue);
}




void gsc_utils_tolower()
{
	char *str;

	if ( !stackGetParams("s", &str) )
	{
		stackError("gsc_utils_tolower() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( !strlen(str) )
	{
		stackError("gsc_utils_tolower() string length is 0");
		stackPushUndefined();
		return;
	}

	stackPushString(I_strlwr(str));
}










void gsc_utils_replace()
{
    char* orig;
	char* rep;
	char* with;

	if ( !stackGetParams("sss", &orig, &rep, &with) )
	{
		stackError("gsc_utils_replace() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
    char *result; // the return string
	char *ins; // the next insert point
	char *tmp; // varies
	int len_rep; // length of rep
	int len_with; // length of with
	int len_front; // distance between rep and end of last rep
	int count; // number of replacements

	if (!orig)
		return;
	if (!rep || !(len_rep = strlen(rep)))
		return;
	if (!(ins = strstr(orig, rep))) 
		return;
	if (!with)
		with = (char*)"";
	len_with = strlen(with);

	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	tmp = result = (char*)malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return;

	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
    
    stackPushString(result);
}














void gsc_utils_getsystemtime()
{
	time_t timer;
	stackPushInt(time(&timer));
}




void gsc_utils_getconfigstring()
{
	int index;

	if ( !stackGetParams("i", &index) )
	{
		stackError("gsc_utils_getconfigstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
    
    if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		stackError("gsc_utils_getconfigstring() configstring index is out of range");
		stackPushUndefined();
		return;
	}

	const char *string = SV_GetConfigstringConst(index);

	if ( !*string )
		stackPushUndefined();
	else
		stackPushString(string);
}



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