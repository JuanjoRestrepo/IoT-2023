/*******************************************************************************
*
*  CoffeeMachineTrc.c -   A trace enabled program implementing the Coffee Machine
*                         for the use case MakeCoffee for the Coffee Machine model
*
*   Notes:                Error checking omitted...
*                         Uses sockets to communicate with the Coffee Machine
*                         TUI
*
*******************************************************************************/

#include  <stdio.h>
#include  <string.h>
#include  <pthread.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <unistd.h>
#include  <stdlib.h>
#include  <errno.h>

#include  "phtrdsMsgLyr.h"                /* pthreads message layer function
                                              prototypes, constants, structs */

/***( Manifest constants )************************************************/

#define   SERVER_PORT 9734                /* Server TCP port */
#define   MAX_CLIENTS 1                   /* Maximum number of concurrent clients */

/***( Function prototypes )***********************************************/

static void *pComm ( void *arg );
static void *pController ( void *arg );
static void *pHardware ( void *arg );

/***( SDL system creation )***********************************************/
int main ( void )
{
  pthread_t   comm_tid;                   /* Communication tid */
  pthread_t   cntrllr_tid;                /* Controller tid */
  pthread_t   hw_tid;                     /* Hardware tid */

  /* Create queues */
  initialiseQueues ();

  /* Create threads */
  pthread_create ( &comm_tid, NULL, pComm, NULL );
  pthread_create ( &hw_tid, NULL, pHardware, NULL );
  pthread_create ( &cntrllr_tid, NULL, pController, NULL );

  /* Wait for threads to finish */
  pthread_join ( cntrllr_tid, NULL );
  pthread_join ( hw_tid, NULL );

  /* Destroy queues */
  destroyQueues ();

  return ( 0 );
}

/***( Process-to-Process Message Gateway )********************************/

/* Communication thread */
static void *pComm ( void *arg )
{
  int                 server_sockfd,      /* server socket descriptor */
                      client_sockfd;      /* client socket descriptor */
  int                 server_len,
                      client_len;
  struct sockaddr_in  server_address;     /* server address */
  struct sockaddr_in  client_address;     /* client address */

  msg_t               InMsg,
                      OutMsg;

  /* Remove any old socket and create an unnamed TCP/IP socket for the server.  */

  server_sockfd = socket ( AF_INET, SOCK_STREAM, 0 );

  /*  Name the socket allowing connections from any client. Use (htonl)
      “host to network, long” and (htons) “host to network, short” to
      convert 32- and 16-bit integers between native host format and the
      standard network byte ordering */

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl ( INADDR_ANY );
  server_address.sin_port = htons ( SERVER_PORT );
  server_len = sizeof ( server_address );
  bind ( server_sockfd, (struct sockaddr *) &server_address, server_len );

  /* Create a connection queue and wait for up to 5 clients.  */

  listen ( server_sockfd, MAX_CLIENTS );

  /* Clear screen */
  system ( "clear" );
  printf ( "Coffee machine server waiting on port %d\n", SERVER_PORT );

  /* Accept a connection.  */

  client_len = sizeof ( client_address );
  client_sockfd = accept ( server_sockfd, (struct sockaddr *) &client_address,
                            (socklen_t *) &client_len );

  /* Print connection info */
  printf ( "\nConnected from Coffee Machine TUI @ %s, port %d\n\n",
            inet_ntoa (client_address.sin_addr), (int) ntohs (client_address.sin_port) );

  /* We can now read/write to client on client_sockfd.  */
  for ( ; ; )
  {
    read ( client_sockfd, &InMsg, sizeof(InMsg) );        /* wait for message from TUI */
    memcpy ( &OutMsg, &InMsg, sizeof(msg_t) );
    sendMessage ( &(queue [CONTROLLER_Q]), OutMsg );      /* send message to Controller */

    read ( client_sockfd, &InMsg, sizeof(InMsg) );        /* wait for message from TUI */
    memcpy ( &OutMsg, &InMsg, sizeof(msg_t) );
    sendMessage ( &(queue [CONTROLLER_Q]), OutMsg );      /* send message to Controller */

    InMsg = receiveMessage ( &(queue [COMM_Q]) );
    memcpy ( &OutMsg, &InMsg, sizeof(msg_t) );
    write ( client_sockfd, &OutMsg, sizeof(OutMsg) );
  }

  /* Close connection and exit */
  close ( client_sockfd );
  close ( server_sockfd );
  return ( NULL );
}

/***( SDL system processes )**********************************************/

/* Controller thread */
static void *pController ( void *arg )
{
  CONTROLLER_STATE_ENUM state,
                        state_next;
  msg_t                 InMsg,
                        OutMsg;
  unsigned int          NbrOfCoffeeCups;

  NbrOfCoffeeCups = 0;

  state_next = IdleC;
  for ( ; ; )
  {
    state = state_next;
    InMsg = receiveMessage ( &(queue [CONTROLLER_Q]) );
    printf ( "\tController received signal [%s], value [%d] in state [%s]\n", TO_CONTROLLER_STRING[InMsg.signal], InMsg.value, CONTROLLER_STATE_STRING[state] );
    fflush ( stdout );
    switch ( state )
    {
      case IdleC:
        switch ( InMsg.signal )
        {
          case sCoin:
            if ( InMsg.value == 10 )
              state_next = PaidTen;
            else
              state_next = IdleC;
            break;
          default:
            break;
        }
        break;
      case PaidTen:
        switch ( InMsg.signal )
        {
          case sCoffee:
            OutMsg.signal = (int) sFillWater;
            OutMsg.value = 0;
            sendMessage ( &(queue [HARDWARE_Q]), OutMsg ); /* send message to Hardware */
            state_next = PouringWater;
            break;
          default:
            break;
        }
        break;
      case PouringWater:
        switch ( InMsg.signal )
        {
          case sWaterOk:
            OutMsg.signal = (int) sFillCoffee;
            OutMsg.value = 0;
            sendMessage ( &(queue [HARDWARE_Q]), OutMsg ); /* send message to Hardware */
            state_next = DispensingCoffee;
            break;
          default:
            break;
        }
        break;
      case DispensingCoffee:
        switch ( InMsg.signal )
        {
          case sCoffeeOk:
            OutMsg.signal = (int) sHeatWater;
            OutMsg.value = 0;
            sendMessage ( &(queue [HARDWARE_Q]), OutMsg ); /* send message to Hardware */
            state_next = BrewingCoffee;
            break;
          default:
            break;
        }
        break;
      case BrewingCoffee:
        switch ( InMsg.signal )
        {
          case sWarm:
            NbrOfCoffeeCups = 1;
            OutMsg.signal = (int) sCoffee;
            OutMsg.value = NbrOfCoffeeCups;
            sendMessage ( &(queue [COMM_Q]), OutMsg ); /* send message to Comm */
            state_next = IdleC;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    printf ( "\tController next state is [%s]\n", CONTROLLER_STATE_STRING[state_next] );
    fflush ( stdout );
  }

  return ( NULL );
}

/* Hardware thread */
static void *pHardware ( void *arg )
{
  HARDWARE_STATE_ENUM state,
                      state_next;
  msg_t               InMsg,
                      OutMsg;

  state_next = IdleH;
  for ( ; ; )
  {
    state = state_next;
    InMsg = receiveMessage ( &(queue [HARDWARE_Q]) );
    printf ( "\t\tHardware received signal [%s], value [%d] in state [%s]\n", TO_HARDWARE_STRING[InMsg.signal], InMsg.value, HARDWARE_STATE_STRING[state] );
    fflush ( stdout );
    switch ( state )
    {
      case IdleH:
        switch ( InMsg.signal )
        {
          case sFillWater:
            OutMsg.signal = (int) sWaterOk;
            break;
          case sFillCoffee:
            OutMsg.signal = (int) sCoffeeOk;
            break;
          case sHeatWater:
            OutMsg.signal = (int) sWarm;
            break;
          default:
            break;
        }
        state_next = IdleH;
        OutMsg.value = 0;
        sendMessage ( &(queue [CONTROLLER_Q]), OutMsg );     /* send message to Controller */
        break;
      default:
        break;
    }
  }

  return ( NULL );
}
