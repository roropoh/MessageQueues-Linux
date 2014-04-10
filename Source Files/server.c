/*------------------------------------------------------------------------------------------------------------------ 
-- SOURCE FILE: server.c - This is the server side of the program. It will start up the server, and be able to
--                              send text files to multiple client connections.
-- 
-- PROGRAM: assn2
-- 
-- FUNCTIONS: 
--          void output_process(int output_pipe, int c)
-- 
-- DATE: January 16, 2014 
-- 
-- DESIGNER: Robin Hsieh A00657820 
-- 
-- PROGRAMMER: Robin Hsieh A00657820 
-- 
----------------------------------------------------------------------------------------------------------------------*/

#include "header.h"

int msq_id;

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: startServer 
-- 
-- DATE: January 18, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: void startServer()
-- 
-- RETURNS: Void
-- 
-- NOTES:
-- This function is to start the server program to be able to send text files to clients requesting for them.
-- Starting a new process for each client connected to server.
--
------------------------------------------------------------------------------------------------------------------*/
void startServer()
{
    FILE *fp;
    pid_t pid;
    double priority;
    double messagesize;

    signal(SIGINT, catch_int);
    signal(SIGCHLD, catch_cleanup);

    msq_id = OpenMessageQueue();

    printf("Server started\n");

    while(1)
    {
        Mesg * writermsg = (Mesg *) malloc(sizeof(Mesg));
        Mesg * readermsg = (Mesg *) malloc(sizeof(Mesg));

        if (read_message(msq_id, 1, readermsg) >= 0)
        {
            if((pid = fork()) < 0)
            {
                fprintf(stderr, "pid = fork() error\n");
                exit(1);
            }

            if(pid == 0)
            {
                sscanf(readermsg->mesg_data, "%s %ld %lf", writermsg->mesg_data, &writermsg->mesg_type, &priority);
                messagesize = MAXMESSAGEDATA * (priority / 10) * 2;
                writermsg->mesg_type = readermsg->pid;

                printf("Client with pid[%ld] connected, ", writermsg->mesg_type);
                printf("with priority of %.0f. \n", priority);
                printf("Sending with message size: %.0lf\n\n", messagesize);
                

                fp = fopen(writermsg->mesg_data, "r");
                if(fp != NULL)
                {
                    writermsg->fileTransferCompleted = 0;
                    while(fread(writermsg->mesg_data, sizeof(char), messagesize - 1, fp) > 0)
                    {
                        if((send_message(msq_id, writermsg)) == -1)
                        {
                            fprintf(stderr, "server send_message error: fread\n");
                            exit(1);
                        }
                        memset(writermsg->mesg_data, 0, messagesize);
                        usleep(350);
                    }
                    
                    strcpy(writermsg->mesg_data, "File transfer completed.\n");
                    writermsg->fileTransferCompleted = 1;

                    send_message(msq_id, writermsg);
                    memset(writermsg->mesg_data, 0, messagesize);
                    
                    printf("File transfer completed to pid[%ld] with priority %.0lf.\n\n", writermsg->mesg_type, priority);
                    fclose(fp);
                }
                else
                {
                    writermsg->fileTransferCompleted = 1;
                    memset(writermsg, 0, MAXMESSAGEDATA);
                    strcpy(writermsg->mesg_data, "fopen error: file does not exist.\n");

                    if((send_message(msq_id, writermsg)) == -1)
                    {
                        fprintf(stderr, "server send_message error: fopen error\n");
                        exit(1);
                    }
                    memset(writermsg, 0, MAXMESSAGEDATA);
                }
            }
        }
        free(writermsg);
        free(readermsg);
    }
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: catch_int 
-- 
-- DATE: January 19, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: void catch_int(int signo)
--              int signo: The signal number when the user presses ctrl+c
-- 
-- RETURNS: Void
-- 
-- NOTES:
-- This function will catch all the signals being typed/sent by the user. Once a signal is caught, it will
-- close the message queue, and kill children processes.
--
------------------------------------------------------------------------------------------------------------------*/
void catch_int(int signo)
{
    if(signo == SIGINT)
    {
        msgctl(msq_id, IPC_RMID, 0);
        kill(getpid(), SIGTERM);
    }
}

/*------------------------------------------------------------------------------------------------------------------ 
-- FUNCTION: catch_cleanup 
-- 
-- DATE: January 19, 2014 
-- 
-- REVISIONS:  
-- 
-- DESIGNER: Robin Hsieh 
-- 
-- PROGRAMMER: Robin Hsieh 
-- 
-- INTERFACE: void catch_cleanup(int signo)
--              int signo: The signal number when the user presses ctrl+c
-- 
-- RETURNS: Void
-- 
-- NOTES:
-- This function will catch all the signals being typed/sent by the user. Once a signal is caught, it will check
-- if it is signal from child, if so, wait for all processes to be done, so they can be exited.
--
------------------------------------------------------------------------------------------------------------------*/
void catch_cleanup(int signo)
{
    if(signo == SIGCHLD)
    {
        wait(NULL);
    }
}
