/*------------------------------------------------------------------------------------------------------------------ 
-- SOURCE FILE: client.c - This is the client side of the program. It will take the user input for filename, send 
--                              to server, and receive the whole file and print it on stdout.
-- 
-- PROGRAM: assn2
-- 
-- FUNCTIONS: 
--          void startClient(char* inputfname, int priority)
--          int OpenMessageQueue()
--          void *client_thread(void * id)
-- 
-- DATE: January 16, 2014 
-- 
-- DESIGNER: Robin Hsieh A00657820 
-- 
-- PROGRAMMER: Robin Hsieh A00657820 
-- 
----------------------------------------------------------------------------------------------------------------------*/

#include "header.h"

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: startClient 
-- 
-- DATE: January 18, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: void startClient(char* inputfname, int priority)
--              char* inputfname:    The char array of the inputfilename from user.
--              int priority:        The int of priority from user.
-- 
-- RETURNS: Void
-- 
-- NOTES:
-- This function is to start the client program to receive a txt file from the server.
--
------------------------------------------------------------------------------------------------------------------*/
void startClient(char* inputfname, int priority)
{

    int msq_id;
    pid_t pid = getpid();

    pthread_t clientThreadId;

    printf("Client started\n");

    msq_id = OpenMessageQueue();

    Mesg *msg = (Mesg*) malloc(sizeof(Mesg));
    msg->mesg_type = 1;
    sprintf(msg->mesg_data, "%s %d %d", inputfname, pid, priority);
    msg->mesg_len = strlen(msg->mesg_data);
    msg->fileTransferCompleted = 0;
    msg->pid = pid;

    if ((send_message(msq_id, msg)) == -1)
    {
        fprintf(stderr, "client: send_message error\n");
        exit(1);
    }

    pthread_create(&clientThreadId, NULL, client_thread, (void *)&msq_id);
    pthread_join(clientThreadId, NULL);

    free(msg);
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: client_thread 
-- 
-- DATE: January 18, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: void *client_thread(void * id)
--              void * id:    The variable passed in from the create thread function.
-- 
-- RETURNS: Void
-- 
-- NOTES:
-- This function is to create a thread that will continue to read and print the text from server until finished.
--
------------------------------------------------------------------------------------------------------------------*/
void *client_thread(void * id)
{
    int msq_id = *(int*)id;
    pid_t pid = getpid();

    Mesg *readermsg = (Mesg*) malloc(sizeof(Mesg));
    printf("%d\n", readermsg->fileTransferCompleted);
    while(1)
    {
        if(readermsg->fileTransferCompleted == 1)
        {
            break;
        }
        
        read_message(msq_id, pid, readermsg);
        printf("%s", readermsg->mesg_data);

    }
    free(readermsg);

    return NULL;
}
