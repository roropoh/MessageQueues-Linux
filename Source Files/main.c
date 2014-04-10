/*------------------------------------------------------------------------------------------------------------------ 
-- SOURCE FILE: main.c - This application is an application to start a server, or client to access the message
--                          queues to be able to pass information through the queues. The client will send a
--                          message over the queue to ask the server for a file, and have the server send back
--                          the file over the message queue.
-- 
-- PROGRAM: assn2
-- 
-- FUNCTIONS: 
--      int main (int argc , char *argv[])
--      int send_message( int msg_qid, Mesg *qbuf )
--      int read_message (int qid, long type, Mesg *qbuf )
--      void exit_message()
--      int OpenMessageQueue()
-- 
-- DATE: January 16, 2014 
-- 
-- DESIGNER: Robin Hsieh A00657820 
-- 
-- PROGRAMMER: Robin Hsieh A00657820 
-- 
-- NOTES: 
-- This program will take user command to start server or client, and depending on which one, will follow the
-- each respective functionality. Server will send messages from queue to specific clients, and client will 
-- receive mesages from the queue determined by the pid number.
-- 
----------------------------------------------------------------------------------------------------------------------*/

#include "header.h"

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: main
-- 
-- DATE: January 15, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: int main (int argc , char *argv[])
--              int argc:       The number of the arguments put in from the command line.
--              char *argv[]:   The array of char* of the arguments 
-- 
-- RETURNS: int
--              Returns an int when program terminates.
-- 
-- NOTES:
-- This function is to start the server program to be able to send text files to clients requesting for them.
-- Starting a new process for each client connected to server.
--
------------------------------------------------------------------------------------------------------------------*/
int main (int argc , char *argv[])
{

    char    inputfname[128];
    int     priority;

    if ( argc < 2 ) 
    {
        exit_message();
    }
    if ( argc == 2 ) 
    {
        if ( strcmp( argv[1], "server" ) == 0) 
        {
            startServer();
        }
        else 
        {
            exit_message();
        }
    }
    else if ( argc == 4 ) 
    {
        if ( strcmp( argv[1], "client" ) == 0) 
        {
            strcpy(inputfname, argv[2]);
            priority = atoi(argv[3]);
            if(priority > 5 || priority < 1)
            {
                exit_message();
            }
            startClient(inputfname, priority);
        }
    }
    else
    {
        exit_message();
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: send_message
-- 
-- DATE: January 13, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: int send_message( int msg_qid, Mesg *qbuf )
--              int msg_qid:    The msg_qid to identify the message queue.
--              Mesg *qbuf:     The qbuf Mesg struct wanting to send.
-- 
-- RETURNS: int
--              Returns result after msg has been sent.
-- 
-- NOTES:
-- This function is to send a message struct over the message queue.
--
------------------------------------------------------------------------------------------------------------------*/
int send_message( int msg_qid, Mesg *qbuf )
{
    int length, result;

    length = sizeof(Mesg) - sizeof(long);
    if ((result = msgsnd(msg_qid, qbuf, length, 0)) == -1)
    {
        return (-1);
    }
    return (result);
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: read_message
-- 
-- DATE: January 13, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: int read_message (int qid, long type, Mesg *qbuf )
--              int qid:        The msg_qid to identify the message queue.
--              long type:      The type that the message queue will be written to.
--              Mesg *qbuf:     The qbuf Mesg struct wanting to send.
-- 
-- RETURNS: int
--              Returns result after msg has been sent.
-- 
-- NOTES:
-- This function is to read a message struct over the message queue.
--
------------------------------------------------------------------------------------------------------------------*/
int read_message (int qid, long type, Mesg *qbuf )
{ 
    int result, length; 

    length = sizeof(Mesg) - sizeof(long);
    
    if ((result = msgrcv ( qid, qbuf, length, type, 0)) == -1) 
    { 
        return (-1); 
    } 
    return (result); 
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: exit_message
-- 
-- DATE: January 13, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: void exit_message()
-- 
-- RETURNS: Void
-- 
-- NOTES:
-- This function is to print the error message when user inputs wrong arguments into the command line.
--
------------------------------------------------------------------------------------------------------------------*/
void exit_message()
{
    fprintf(stderr, "Usage:\t./assn2 client [inputfilename] [priority 1-5]\n");
    fprintf(stderr, "Or:\t./assn2 server\n");
    exit(1);
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: OpenMessageQueue 
-- 
-- DATE: January 16, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: int OpenMessageQueue()
-- 
-- RETURNS: int
--              Returns the msq_id after msgget
-- 
-- NOTES:
-- Opens the message queue to read and write to.
--
------------------------------------------------------------------------------------------------------------------*/
int OpenMessageQueue()
{
    int msq_id;
    if ((msq_id = msgget (MESSAGEQUEUEKEY, IPC_CREAT|0660)) < 0)
    {
        fprintf(stderr, "client: msgget error\n");
        exit(1);
    }
    return msq_id;
}