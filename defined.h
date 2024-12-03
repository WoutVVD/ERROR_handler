#define ADDRESS     "tcp://192.168.1.23:1883"                   // MQTT broker URL
#define CLIENTID    "RaspberryPiClient"                         // Unique client ID
#define TOPIC       "Woutvvd"                                   // MQTT topic
#define PAYLOAD     "Hello from Raspberry Pi!"                  // Message to send
#define QOS         1                                           // Quality of Service level
#define TIMEOUT     10000L                                      // Timeout in milliseconds

#define	sevDefault  "SEV4\0"                                     //default severity
#define	errorCodeDefault    "ERR0000\0"                            //default error code
#define	errorMsgDefault "errorcode is invalid/not found"    //default error message
