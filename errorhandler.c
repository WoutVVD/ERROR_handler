#include <stdio.h>
#include <string.h>
//#include <MQTTClient.h>
#include "defined.h"
#include <time.h>
#include <stdlib.h>

//set language
char* languageSet(char* lang) 
{
    if (lang == NULL)
    {
        lang = "EN";
    }
    else if (strcmp(lang, "EN") != 0 && strcmp(lang, "NL") != 0 && strcmp(lang, "FR") != 0)
    {
        lang = "EN";
    }
    return lang;
}

//tbl construct for all error msgs
struct tbl
{
    char errcode[8]; //"app####\n"
    char errmsg[80];
    struct tbl *next;
};
struct tbl *head = NULL;
struct tbl *current = NULL;

//insert first error code+msg into linked list
void insert_first(struct tbl **head, char *errorcode, char *errormsg)
{
    struct tbl *lk = (struct tbl *)malloc(sizeof(struct tbl));

    strcpy(lk->errcode, errorcode);
    strcpy(lk->errmsg, errormsg);

    lk->next = NULL;

    *head = lk;
}

//insert next error code+msg into linked list
void insert_next(struct tbl *list, char *errorcode, char *errormsg)
{
    struct tbl *lk = (struct tbl *)malloc(sizeof(struct tbl));

    strcpy(lk->errcode, errorcode);
    strcpy(lk->errmsg, errormsg);

    lk->next = NULL;

    list->next = lk;
}

char searchtbl(struct tbl **list, char errorcode)
{
    struct tbl *temp = head;
    while (temp != NULL)
    {
        if (strcasecmp(temp->errcode, errorcode) == 0)
        {
            *list = temp;
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

//delay function  
//https://stackoverflow.com/questions/56158798/how-do-you-make-a-function-that-waits-an-x-amount-of-seconds-in-c
void delay(int miliseconds)
{
    clock_t start_time = clock();
    //while (clock() < start_time + milli_seconds);
}
    
//get date and time, returned in a char array "year-month-day hour:minute:second"
char* date_time()
{
    time_t currentTime;

    time(&currentTime);

    struct tm *localTime = localtime(&currentTime);

    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;

    //YYYY-MM-DD HH:MM:SS\0;
    char* datetime = malloc(20 * sizeof(char));
    sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
    return datetime;
}

//debug of tbl
void print_list()
{
    struct tbl *p = head;
    int count = 1;
    while (p != NULL)
    {
        printf("%d, %s, %s\n", count, p->errcode, p->errmsg);
        p = p->next;
        count++;
    }
    printf("Einde van de lijst: \n\n");
}

//fill tbl with error codes and messages
void fillTbl(char* languageChoice){
    char errorcode[8]; //"app####\n"
    char errormsg[80];
    char line[256];
    int line_number = 0;

    char filenaam[20];
    sprintf(filenaam, "Error_msg_%s.txt", languageChoice);
    char filepath[50]; 
    sprintf(filepath, "./%s", filenaam);

    FILE *fp = fopen(filepath, "r");
        while (fgets(line, sizeof(line), fp))
    {
        line_number++;
        if (line[0] != '#' && line[0] != '\n' && line[0] != '=')
        {
            if (sscanf(line, "%8[^\t\n]%*c%80[^\t\n]%*c",errorcode, errormsg) == 2)
            {
                if (head == NULL)
                {
                    insert_first(&head, errorcode, errormsg);
                    current = head;
                }
                else
                {
                    insert_next(current, errorcode, errormsg);
                    current = current->next;
                }
            }
            else
            {
                printf("Error op lijn %d: Foute data formaat\n", line_number);
            }
        }
    }

    fclose(fp);
}

//split msg "errorcode";"subsys;"errormsg";extra info"
char splitMsg_Default(char incomingMsg[1024], char datetime[20]){
    char errorcode[8]; //"app####\n"
    char subsys[10];
    char errormsg[1024];
    int sevcode[1];
    char msg[1024];
    struct tbl *found_record = NULL;

    sscanf(incomingMsg, "%d;%s;%s;%s", sevcode, errorcode, subsys, errormsg);

    if (searchtbl(&found_record, errorcode) == 0)
    {
        strcpy(errorcode, errorCodeDefault);
        strcpy(errormsg, errorMsgDefault);
    }
    else
    {
        strcpy(errorcode, found_record->errcode);
        strcpy(errormsg, found_record->errmsg);
    }
    if (sevcode > 4 || sevcode < 0){
        strcpy(sevcode, sevDefault);
    }
    strcpy(msg, formatmsg(sevcode, errorcode, subsys, errormsg, datetime));
    return msg;
}

//format msg to "datetime;sevcode;subsys;errorcode;errormsg"
char formatmsg(char sevcode[1], char errorcode[8], char subsys[10], char errormsg[1024], char datetime[20]){
    char msg[1024];
    sevcode = "sev%d", sevcode;
    sprintf(msg, "%s;%s;%s;%s;%s", datetime, sevcode, subsys, errorcode, errormsg);
    return msg;
}

//send msg to mqtt broker
void sendMsg(){}

//recieve msg from mqtt broker
char receiveMsg(){}

int main(int argc, char* argv[]) {

    char incomingMsg[1024];
    char datetime[20];
    char msg[1024];

    char* languageChoice = languageSet(argv[1]); //set language FR/NL/EN, default = EN
    fillTbl(languageChoice);
    strcpy(incomingMsg, receiveMsg());
    strcpy(datetime, date_time());
    strcpy(msg, splitMsg_Default(incomingMsg, datetime));
    sendMsg(msg);
    return 0;
}
