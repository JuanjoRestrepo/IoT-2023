/*******************************************************************************
*
*  CoffeeMachine_TUI.c -  A program implementing a Text User Interface for
*                         the use case MakeCoffee for the Coffee Machine model
*
*   Notes:                Error checking omitted...
*                         Uses sockets to communicate with the Coffee Machine
*                         pthreads
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
#define   SERVER_ADDR "127.0.0.1"         /* Server IP */

/***( Preprocessor macros )***********************************************/

#define   STR_(X)     #X
#define   STR(X)      STR_(X)

/***( Function prototypes )***********************************************/

static void *pCustomer ( void *arg );

/***( RTDS_ENV creation )*************************************************/
int main( void )
{
  pthread_t   customr_tid;                /* Customer tid */

  /* Create threads */
  pthread_create ( &customr_tid, NULL, pCustomer, NULL );

  /* Wait for threads to finish */
  pthread_join ( customr_tid, NULL );

  return ( 0 );
}

/* Customer thread */
static void *pCustomer ( void *arg )
{
  int                 coinValue;
  char                cupType;
  msg_t               OutMsg,
                      InMsg;

  int                 sockfd;       /* socket descriptor */
  int                 len;
  struct sockaddr_in  address;      /* server address */
  int                 result;

  char                line[100];    /* fgets buffer */

  unsigned int        NbrOfCoffeeCups;

  /* Create a TCP/IP socket for the client */

  sockfd = socket ( AF_INET, SOCK_STREAM, 0 );

  /* Name the socket, as agreed with the server. */

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr ( SERVER_ADDR );
  address.sin_port = htons ( SERVER_PORT );
  len = sizeof ( address );

  /* Now connect our socket to the server's socket. */

  result = connect ( sockfd, (struct sockaddr *) &address, len );

  /* Clear screen */
  system ( "clear" );
  if ( result == -1 )
  {
    perror ( "Aaaargh!: CoffeeMachine_TUI attempt to connect to Coffee Machine server on port " STR(SERVER_PORT) " @ " STR(SERVER_ADDR) " failed" );
    exit ( result );
  }

  /* Print connection info */
  printf ( "Coffee Machine TUI connected to Coffee Machine server @ %s, port %d\n\n",
            inet_ntoa (address.sin_addr), (int) ntohs (address.sin_port) );

  NbrOfCoffeeCups = 0;
  for ( ; ; )
  {
    printf ( "Enter coin value (5 or 10): " );
    fflush ( stdout );
    fflush ( stdin );
    fgets ( line, sizeof (line), stdin );
    sscanf ( line, "%d", &coinValue );

    OutMsg.signal = (int) sCoin;
    OutMsg.value = coinValue;
    write ( sockfd, &OutMsg, sizeof(OutMsg) );        /* send message to Controller */

    printf ( "Select (C)offee or (T)ea: " );
    fflush ( stdout );
    fflush ( stdin );
    fgets ( line, sizeof (line), stdin );
    sscanf ( line, "%c", &cupType );

    if ( cupType == 'C' )
      OutMsg.signal = (int) sCoffee;
    else
      OutMsg.signal = (int) sTea;
    OutMsg.value = 0;
    write ( sockfd, &OutMsg, sizeof(OutMsg) );        /* send message to Controller */

    read ( sockfd, &InMsg, sizeof(InMsg) );
    NbrOfCoffeeCups += InMsg.value;
    printf ( "%d Cup(s) of Coffee served!\n\n", NbrOfCoffeeCups );
    fflush ( stdout );
  }

  /* Close connection and exit. */
  close ( sockfd );
  return ( NULL );
}
