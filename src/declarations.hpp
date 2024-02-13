#ifndef _DECLARATIONS_HPP_
#define _DECLARATIONS_HPP_

#include <setjmp.h>

#define QDECL __attribute__((cdecl))

#define qboolean int
#define qtrue 1
#define qfalse 0

#define ARCHIVEDSSBUF_SIZE          0x2000000
#define BIG_INFO_STRING             0x2000
#define ENTFIELD_MASK               0xC000
#define FLOAT_INT_BITS              13
#define FLOAT_INT_BIAS              ( 1 << ( FLOAT_INT_BITS - 1 ) ) // 0x1000
#define FRAMETIME                   50


#define MAX_CLIENTS                 64
#define MAX_STRINGLENGTH            1024




#define CVAR_NOFLAG             0               // 0x0000
#define CVAR_ARCHIVE            (1 << 0)        // 0x0001
#define CVAR_USERINFO           (1 << 1)        // 0x0002
#define CVAR_SERVERINFO         (1 << 2)        // 0x0004
#define CVAR_SYSTEMINFO         (1 << 3)        // 0x0008
#define CVAR_INIT               (1 << 4)        // 0x0010
#define CVAR_LATCH              (1 << 5)        // 0x0020
#define CVAR_ROM                (1 << 6)        // 0x0040
/*
#define CVAR_CHEAT              (1 << 7)        // 0x0080
#define CVAR_CODINFO            (1 << 8)        // 0x0100
#define CVAR_SAVED              (1 << 9)        // 0x0200
#define CVAR_NORESTART          (1 << 10)       // 0x0400
#define CVAR_CHANGEABLE_RESET   (1 << 12)       // 0x1000
#define CVAR_EXTERNAL           (1 << 14)       // 0x4000
#define CVAR_AUTOEXEC           (1 << 15)       // 0x8000
*/





#define PMF_PRONE           0x1
#define PMF_CROUCH          0x2 // PMF_DUCKED





typedef unsigned char byte;
typedef signed char sbyte;

typedef int clipHandle_t;


typedef struct scr_entref_s
{
	uint16_t entnum;
	uint16_t classnum;
} scr_entref_t;



typedef float vec_t;
typedef vec_t vec3_t[3];





//CODEXTENDED
typedef struct cvar_s
{
	char *name;
	char *string;
	char *resetString;
	char *latchedString;
	int flags;
	qboolean modified;
	int modificationCount;
	float value;
	int integer;
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

/*
typedef struct dvar_s
{
	const char *name;
	unsigned short flags;
	DvarType type;
	bool modified;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarLimits domain;
	dvar_s *next;
	dvar_s *hashNext;
} ;
*/







typedef void (*xfunction_t)();
typedef void (*xmethod_t)(scr_entref_t);


typedef struct scr_function_s
{
	const char      *name;
	xfunction_t     call;
	qboolean        developer;
} scr_function_t;


typedef struct scr_method_s
{
	const char     *name;
	xmethod_t      call;
	qboolean       developer;
} scr_method_t;












//CODEXTENDED
typedef struct playerState_s
{
	int commandTime;
	int pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	char gap_2C[20];
	float leanf;
	int speed;
	char gap_48[12];
	int groundEntityNum;
	char gap_58[12];
	int jumpTime;
	int field_68;
	int legsTime;
	int legsAnim;
	int torsoTime;
	int torsoAnim;
	int movementDir;
	int eFlags;
	char gap_84[24];
	int field_9C;
	int field_A0;
	int field_A4;
	int field_A8;
	int clientNum;
	int weapon;
	int field_B4;
	char gap_B8;
	char gap_B9;
	char gap_BA[2];
	int field_BC;
	vec3_t viewangles;
	char gap_CC[40];
	int health;
	char gap_F8[556];
	vec3_t mins;
	vec3_t maxs;
	float viewheight_prone;
	int viewheight_crouched;
	float viewheight_standing;
	int field_348;
	float runSpeedScale;
	float sprintSpeedScale;
	char gap_354[40];
	float friction;
	char gap_380[68];
	float fTorsoHeight;
	float fTorsoPitch;
	float fWaistPitch;
	char rest[7416];
	int end;
} playerState_t;











#if COD_VERSION == COD1_1_1
//static const int gentities_offset = 0x08665480;
#elif COD_VERSION == COD1_1_5
//static const int gentities_offset = 0x08716400;
#endif

#if COD_VERSION == COD1_1_1
//static const int gclients_offset = 0x086F1480;
#elif COD_VERSION == COD1_1_5
//static const int gclients_offset = 0x087A2500;
#endif

#if COD_VERSION == COD1_1_1 // Not tested
//static const int gameInitialized_offset = 0x0;
#elif COD_VERSION == COD1_1_5
//static const int gameInitialized_offset = 0x083E2F80;
#endif




//#define gameInitialized (*((int*)( gameInitialized_offset )))



// Check for critical structure sizes and fail if not match
#if __GNUC__ >= 6

#if COD_VERSION == COD1_1_1
 //static_assert((sizeof(client_t) == 0x78F14), "ERROR: client_t size is invalid!");
#elif COD_VERSION == COD1_1_5
 //static_assert((sizeof(client_t) == 0xB1064), "ERROR: client_t size is invalid!");
 
#endif
 
 //static_assert((sizeof(gentity_t) == 560), "ERROR: gentity_t size is invalid!");
 //static_assert((sizeof(gclient_t) == 0x28A4), "ERROR: gclient_t size is invalid!");
 //static_assert((sizeof(scr_data_t) == 14072), "ERROR: scr_data_t size is invalid!");

#endif

#endif

// Custom data types

#define MAX_ERROR_BUFFER 64

typedef struct src_error_s
{
	char internal_function[64];
	char message[1024];
} scr_error_t;





typedef struct callback_s
{
	int *pos;
	const char *name;
} callback_t;