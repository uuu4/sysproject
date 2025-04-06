#include "../include/project.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

extern ShmBuf *g_shm;

void controller_parse_input(const char *input){
    if(!input)return;

    if(strncmp(input,"@msg",4)==0){
        const char *message = input+4;
        while(*message == ' '){
            message++;
        }
    
    if(model_send_message(g_shm,message)==0){
        view_display_message(message);
    }
    else{
        fprintf(stderr,"Error while sending message");
    }}
    else{
        int status = execute_command(input);
        printf("Running command with %d status code", status);
    }
   
}

void controller_handle_command(const char *command){
    if(!command){
        fprintf(stderr,"Need a valid command");
        return;
    }
    int status = execute_command(command);
    char resultMsg[256];
    snprintf(resultMsg,sizeof(resultMsg),"Running command with %d status code",command , status);
    view_update_terminal(resultMsg);
}

void controller_handle_message(const char *message){
    if(!message)return;
}

