#include <stdio.h>
#include <string.h>
//#include <MQTTClient.h>
#include "defined.h"
#include <time.h>
#include <stdlib.h>

//language
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

int main(int argc, char* argv[]) {
    char errorcode[8]; //"app####\n"
    char errormsg[80];
    char line[256];
    int line_number = 0;
    
    char* languageChoice = languageSet(argv[1]); //set language FR/NL/EN, default = EN
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
    print_list();

    }

    return 0;
}









/*
int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    // Initialize the MQTT client
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    // Set connection options
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Connect to the broker
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        return -1;
    }

    // Create the message
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    // Publish the message
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    // Disconnect from the broker
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return rc;
}
*/