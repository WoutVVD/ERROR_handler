#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>
#include "defined.h"
#include <time.h>

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
void insert_first(char *errorcode, char *errormsg)
{
    struct tbl *lk = (struct tbl *)malloc(sizeof(struct tbl));

    strcpy(lk->errcode, errorcode);
    strcpy(lk->errmsg, errormsg);

    lk->next = NULL;

    head = lk;
}

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
    while (clock() < start_time + milli_seconds);
}

//get date and time, returned in a char array "year-month-day hour:minute:second"
char date_time()
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

    char datetime[20];
    datetime = "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second;
    return datetime;
}













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
