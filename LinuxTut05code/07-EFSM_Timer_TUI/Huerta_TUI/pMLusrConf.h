/*******************************************************************************
*
*  pMLusrConf.h -   Manifest Constants and Types for concurrent access to a
*                   circular buffer modelling a message queue
*
*   Notes:          User defined according to application
*
*******************************************************************************/

/***( Manifest constants for user-defined queuing system  )********************/

#define     BUFSIZE       8   /* number of slots in queues */
#define     NUM_QUEUES    4   /* number of queues */
#define     CONTROLLER_Q  0   /* queue 0: controller */
#define     HARDWARE_Q    1   /* queue 1: hardware */
#define     COMM_Q        2   /* queue 2: P2P communications */
#define     DATA_BASE_Q   3   /* queue 3: data base */

/***********( Macros to manage symbolic (literal) enumerated values )******************/

#define     GENERATE_ENUM(ENUM)     ENUM,
#define     GENERATE_STRING(STRING) #STRING,

/***( User-defined message structure )*****************************************/

typedef struct
{
  int   signal;
  int   value; //Valor Cafe
} msg_t;

/************( User-defined signals )****************************************************/

/* Signals sent to pController */

#define FOREACH_TO_CONTROLLER(SIGNAL) \
        SIGNAL(sActivacion) \
        SIGNAL(sVerificacionHumedadOk) \
        SIGNAL(sVerificacionLuminosidadOK) \
        SIGNAL(sHumedadMateraOK) \
        SIGNAL(sLuminosidadOk) \
        SIGNAL(sReleDesactivado) \
        SIGNAL(sID)

typedef enum
{
  FOREACH_TO_CONTROLLER(GENERATE_ENUM)
} TO_CONTROLLER_ENUM;

static const char *TO_CONTROLLER_STRING[] =
{
  FOREACH_TO_CONTROLLER(GENERATE_STRING)
};

/* Signals sent to pHardware */

#define FOREACH_TO_HARDWARE(SIGNAL) \
        SIGNAL(sHumedadMateraOK) \
        SIGNAL(sLuminosidadOk) \
        SIGNAL(sReleDesactivado)

typedef enum
{
  FOREACH_TO_HARDWARE(GENERATE_ENUM)
} TO_HARDWARE_ENUM;

static const char *TO_HARDWARE_STRING[] =
{
  FOREACH_TO_HARDWARE(GENERATE_STRING)
};


/* Signals sent to pControllerDataBase */

#define FOREACH_TO_CONTROLLER_DATABASE(SIGNAL) \
        SIGNAL(sDatoH) \
        SIGNAL(sDatoL) \
        SIGNAL(sLocalizacion) \
        SIGNAL(sHumedadGuardar) \
        SIGNAL(sVerificacionHumedad) \
        SIGNAL(sLuminosidadGuardar) \
        SIGNAL(sVerificacionLuminosidad) \
        SIGNAL(sFechaHora) \
        SIGNAL(sMateraRegada) \
        SIGNAL(sObtenerId) \

typedef enum
{
  FOREACH_TO_CONTROLLER_DATABASE(GENERATE_ENUM)
} TO_CONTROLLER_DATABASE_ENUM;

static const char *TO_CONTROLLER_DATABASE_STRING[] =
{
  FOREACH_TO_CONTROLLER_DATABASE(GENERATE_STRING)
};

/* Signals sent to pDataBase */

#define FOREACH_TO_DATABASE(SIGNAL) \
        SIGNAL(sGuardarLoc) \
        SIGNAL(sAlmacenarHumedad) \
        SIGNAL(sObtenerDatoH) \
        SIGNAL(sAlmacenarLuminosidad) \
        SIGNAL(sObtenerDatoL) \
        SIGNAL(sFechaHoraGuardar) \
        SIGNAL(sGuardarDatoRiego)

typedef enum
{
  FOREACH_TO_DATABASE(GENERATE_ENUM)
} TO_CONTROLLER_DATABASE_ENUM;

static const char *TO_DATABASE_STRING[] =
{
  FOREACH_TO_DATABASE(GENERATE_STRING)
};








/************( User-defined EFSM states )************************************************/

/* EFSM states for pController */

#define FOREACH_CONTROLLER_STATE(STATE) \
        STATE(IdleC)   \
        STATE(Estado1)  \
        STATE(Verificacion1)  \
        STATE(GuardarDatoDeLuminosidad)  \
        STATE(Verificacion2) \
        STATE(ProcesoRiego) \

typedef enum
{
  FOREACH_CONTROLLER_STATE(GENERATE_ENUM)
} CONTROLLER_STATE_ENUM;

static const char *CONTROLLER_STATE_STRING[] =
{
  FOREACH_CONTROLLER_STATE(GENERATE_STRING)
};


/* EFSM states for pControllerDataBase */

#define FOREACH_CONTROLLER_DATABASE_STATE(STATE) \
        STATE(IdleCC)

typedef enum
{
  FOREACH_CONTROLLER_DATABASE_STATE(GENERATE_ENUM)
} CONTROLLER_DATABASE_STATE_ENUM;

static const char *CONTROLLER_DATABASE_STATE_STRING[] =
{
  FOREACH_CONTROLLER_DATABASE_STATE(GENERATE_STRING)
};


/* EFSM states for pDataBase */

#define FOREACH_TO_DATABASE_STATE(STATE) \
        STATE(IdleD)

typedef enum
{
  FOREACH_TO_DATABASE_STATE(GENERATE_ENUM)
} DATABASE_STATE_ENUM;

static const char *DATABASE_STATE_STRING[] =
{
  FOREACH_TO_DATABASE_STATE(GENERATE_STRING)
};


/* EFSM states for pHardware */

#define FOREACH_HARDWARE_STATE(STATE) \
        STATE(IdleH) \
        STATE(TiempoTranscurrido)

typedef enum
{
  FOREACH_HARDWARE_STATE(GENERATE_ENUM)
} HARDWARE_STATE_ENUM;

static const char *HARDWARE_STATE_STRING[] =
{
  FOREACH_HARDWARE_STATE(GENERATE_STRING)
};
