#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<time.h>
#include "cJSON.h"
#include "cJSON.c"

#define PORT 1111
#define maxMessageLength 100000
#define authTokenLength 25
#define maxNamesLength 30
#define totalMembers 10000
#define totalChannel 100
#define totalChannelMembers 500


char *clientMessage;//this string will use to be send to the server
char *serverAnswer;//this string will use to receive answer form server


void makeConnectionRecvSend();

char* authToken();
int stringCompare(char *, char *);
void parsingClientMessage();

void command(char[2][maxMessageLength]);
void registerMember(char[]);
void logIn(char[]);
void logOut(char[]);
void joinChannel(char[]);
void refresh(char[]);
void channelMember(char[]);
void leaveChannel(char[]);
void createChannel(char[]);
void sendMessage(char[]);

void erroR(char*);
void member();
void addMember(char*);
int findMemberByName(char[]);
int findMemberByToken(char[]);

typedef struct {
    char authToken[authTokenLength + 1];
    char channel[maxNamesLength];
    char name[maxNamesLength];
    int id;
    int position;//0 when member logged out | 1 when member logged in | 2 when member is in a channel
    int lastSeen;
} members;
typedef struct{
    char channelName[maxNamesLength];
    char channelMembers[totalChannelMembers][maxNamesLength];
    int numberOfMessage;
}channels;
members allMembers[totalMembers];
channels allChannels[totalChannel];
int i=0;
int channelCounter=0;


int main() {
    clientMessage = (char *) malloc(maxMessageLength * sizeof(char));
    serverAnswer = (char *) malloc(maxMessageLength * sizeof(char));
    member();
    makeConnectionRecvSend();

    return 0;
}


void makeConnectionRecvSend() {

    int server_socket, client_socket;
    struct sockaddr_in server, client;

    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Socket creation failed...\n");
        return;
    }

    // Assign IP and port
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (struct sockaddr *) &server, sizeof(server))) != 0) {
        printf("Socket binding failed...\n");
        return;
    }
    //server listens and verify
    if ((listen(server_socket, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    socklen_t len = sizeof(client);
    while(1) {
        client_socket = accept(server_socket, (struct sockaddr *) &client, &len);
        if (client_socket < 0) {
            printf("Server accceptance failed...\n");
            exit(0);
        }

        char clientMessage2[maxMessageLength];
        char serverAnswer2[maxMessageLength];
        recv(client_socket, &clientMessage2, sizeof(clientMessage2), 0);
        strcpy(clientMessage, clientMessage2);//this function will copy the message to a string to send to the server
        parsingClientMessage();
        strcpy(serverAnswer2, serverAnswer);//this function will copy the server answer to answer string
        send(client_socket, &serverAnswer2, sizeof(serverAnswer2), 0);
        printf("%s\n", serverAnswer);
        // Close the socket

        shutdown(client_socket, SHUT_RDWR);
    }
}

char *authToken() {
    static char authToken[authTokenLength + 1];
    authToken[authTokenLength + 1] = '\0';
    srand(time(0));
    for (int i = 0; i < authTokenLength; ++i) {
        authToken[i] = rand() % 79 + 48;
    }
    return authToken;
}

/*
int stringCompare(char *firstString, char *secondString) {
    int i = 0;
    for (; *(firstString + i) != '\0' || *(secondString + i) != '\0'; ++i) {
        if (*(firstString + i) != *(secondString + i))
            return 0;
    }
    if (*(firstString + i) != *(secondString + i))
        return 0;
    else
        return 1;
}
*/

void parsingClientMessage() {
    char parsedMessage[2][maxMessageLength];
    int i, j = 0;
    for (i = 0; *(clientMessage + i) != ' '; ++i) {
        parsedMessage[0][i] = *(clientMessage + i);
    }
    parsedMessage[0][i] = '\0';
    for (i += 1; *(clientMessage + i) != '\0'; ++i) {
        parsedMessage[1][j] = *(clientMessage + i);
        j++;
    }
    parsedMessage[1][j] = '\0';
    command(parsedMessage);
}


void command(char parsedMessage[2][maxMessageLength]) {
    if (stringCompare("register", parsedMessage[0]))
        registerMember(parsedMessage[1]);
    else if (stringCompare("login", parsedMessage[0]))
        logIn(parsedMessage[1]);
    else if (stringCompare("createChannel", parsedMessage[0]))
        createChannel(parsedMessage[1]);
    else if (stringCompare("joinChannel", parsedMessage[0]))
        joinChannel(parsedMessage[1]);
    else if (stringCompare("send", parsedMessage[0]))
        sendMessage(parsedMessage[1]);
    else if (stringCompare("refresh", parsedMessage[0]))
        refresh(parsedMessage[1]);
    else if (stringCompare("channelMember", parsedMessage[0]))
        channelMember(parsedMessage[1]);
    else if (stringCompare("leave", parsedMessage[0]))
        leaveChannel(parsedMessage[1]);
    else if (stringCompare("logout", parsedMessage[0]))
        logOut(parsedMessage[1]);
    else
        erroR("wrong command");
}
void erroR(char* errorMessage){
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x, "type", "Error");
    cJSON_AddStringToObject(x, "content", errorMessage);
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}
void member(){
    FILE* membersFile;
    membersFile=fopen("members.json","r");
    for (i = 0; fscanf(membersFile,"%s",allMembers[i].name)==1 ; ++i) {
        allMembers[i].id=i;
    }

}





void addMember(char* name){
    FILE* memberFile=fopen("members.json","a");
    fprintf(memberFile, "%s\n", name);
    fclose(memberFile);
}

void registerMember(char message[]){
    char *pass = strrchr(message, ',');
    *pass = '\0';
    pass+=2;
    char location[60];
    sprintf(location, "./members/%s.json", message);
    FILE* member=fopen(location,"r");
    if (member != NULL) {
        erroR("there is a user with this username");
        fclose(member);
        return;
    }
    member = fopen(location, "w");
    fprintf(member, "%s\n%s", pass, message);
    fclose(member);
    addMember(message);
    strcpy(allMembers[i].name,message);
    allMembers[i].id=i;
    i++;
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","");
    cJSON_AddStringToObject(x,"content","the user created");
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}
void logIn(char message[]){
    char *pass = strrchr(message, ',');
    *pass = '\0';
    pass+=2;
    char location[60];
    sprintf(location, "./members/%s.json", message);
    FILE* member=fopen(location,"r");
    if (member == NULL) {
        erroR("This user doesn't exist!!");
        return;
    }
    char password[100];
    fscanf(member, "%s\n", password);
    fclose(member);

    if(!stringCompare(password,pass)) {
        erroR("incorrect password!!");
        return;
    }
    int i=findMemberByName(message);
    if(allMembers[i].position!=0) {
        erroR("you are already logged in");
        return;
    }
    char *authTkn = authToken();
    allMembers[i].position=1;
    strcpy(allMembers[i].authToken, authTkn);
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","AuthToken");
    cJSON_AddStringToObject(x,"content",authTkn);
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}
void createChannel(char message[]){
    char *token = strrchr(message, ',');
    *token = '\0';
    token+=2;
    char location[60];
    sprintf(location, "./channels/%s.json", message);
    int i = findMemberByToken(token);
    if (i == -1) {
        erroR("wrong token");
        return;
    } else if (allMembers[i].position == 0) {
        erroR("you should log in first!!");
        return;
    } else if (allMembers[i].position == 2) {
        erroR("you are already in a channel");
        return;
    }
    FILE* channel=fopen(location,"r");
    if (channel != NULL) {
        erroR("there is a channel with this name");
        fclose(channel);
        return;
    }
    allMembers[i].position = 2;
    cJSON* channelMessage=cJSON_CreateObject();
    cJSON_AddStringToObject(channelMessage, "sender", "server");
    char joinMessage[200];
    sprintf(joinMessage, "%s joined the channel!!", allMembers[i].name);
    cJSON_AddStringToObject(channelMessage,"content",joinMessage);
    cJSON* array=cJSON_CreateArray();
    cJSON_AddItemToArray(array, channelMessage);
    channel = fopen(location, "w");
    fprintf(channel, cJSON_Print(array));
    fclose(channel);
    strcpy(allMembers[i].channel,message);
    allChannels[channelCounter].numberOfMessage = 1;
    strcpy(allChannels[channelCounter].channelName,message);
    for (int j = 0; j <totalChannelMembers ; ++j) {
        if(stringCompare(allChannels[channelCounter].channelMembers[j],"")||stringCompare(allChannels[channelCounter].channelMembers[j],"-1")) {
            strcpy(allChannels[channelCounter].channelMembers[j] ,allMembers[i].name);
            cJSON *x = cJSON_CreateObject();
            cJSON_AddStringToObject(x,"type","");
            cJSON_AddStringToObject(x,"content","channel created");
            sprintf(serverAnswer,"%s", cJSON_Print(x));
            channelCounter++;
            allMembers[i].position=2;
            return;
        }
    }
}
void sendMessage(char message[]){
    char *token = strrchr(message, ',');
    *token = '\0';
    token+=2;
    int i = findMemberByToken(token);
    if (i == -1) {
        erroR("wrong token");
        return;
    } else if (allMembers[i].position == 0) {
        erroR("you should log in first!!");
        return;
    } else if (allMembers[i].position == 1) {
        erroR("you should join a channel first");
        return;
    }
    char location[60];
    sprintf(location, "./channels/%s.json", allMembers[i].channel);
    FILE* channel=fopen(location,"r");
    if (channel == NULL) {
        erroR("This channel doesn't exist!!");
        return;
    }
    char* cjsonArray=calloc(maxMessageLength*100,sizeof(char));
    char temp[maxMessageLength];
    while(fgets(temp, 100, channel)) {
        strcat(cjsonArray, temp);
    }
    cJSON *cjsonArr = cJSON_Parse(cjsonArray);
    fclose(channel);
    cJSON* channelMessage=cJSON_CreateObject();
    cJSON_AddStringToObject(channelMessage, "sender", allMembers[i].name);
    cJSON_AddStringToObject(channelMessage,"content",message);
    cJSON_AddItemToArray(cjsonArr, channelMessage);
    channel = fopen(location, "w");
    fprintf(channel, cJSON_Print(cjsonArr));
    fclose(channel);
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","");
    cJSON_AddStringToObject(x,"content","message send");
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}
void joinChannel(char message []){
    char *token = strrchr(message, ',');
    *token = '\0';
    token+=2;
    int i = findMemberByToken(token);
    if (i == -1) {
        erroR("wrong token");
        return;
    } else if (allMembers[i].position == 0) {
        erroR("you should log in first!!");
        return;
    } else if (allMembers[i].position == 2) {
        erroR("you are already in a channel");
        return;
    }
    char location[60];
    sprintf(location, "./channels/%s.json", message);
    FILE* channel=fopen(location,"r");
    if (channel == NULL) {
        erroR("This channel doesn't exist!!");
        return;
    }
    allMembers[i].position = 2;
    char* cjsonArray=calloc(maxMessageLength*100,sizeof(char));
    char temp[maxMessageLength];

    while(fgets(temp, 100, channel)) {
        strcat(cjsonArray, temp);
    }
    cJSON *cjsonArr = cJSON_Parse(cjsonArray);
    fclose(channel);
    cJSON* channelMessage=cJSON_CreateObject();
    cJSON_AddStringToObject(channelMessage, "sender", "server");
    char joinMessage[200];
    sprintf(joinMessage, "%s joined the channel!!", allMembers[i].name);
    cJSON_AddStringToObject(channelMessage,"content",joinMessage);
    cJSON_AddItemToArray(cjsonArr, channelMessage);
    channel = fopen(location, "w");
    fprintf(channel, cJSON_Print(cjsonArr));
    fclose(channel);
    strcpy(allMembers[i].channel,message);
    strcpy(allChannels[channelCounter].channelName,message);
    allMembers[i].position=2;
    int k;
    for (k = 0; k <totalChannel ; ++k) {
        if (stringCompare(allChannels[k].channelName, message)) {
            break;
        }
    }
    allChannels[k].numberOfMessage++;
    for (int j = 0; j <totalChannelMembers ; ++j) {
        if(stringCompare(allChannels[k].channelMembers[j],"")||stringCompare(allChannels[k].channelMembers[j],"-1")) {
            strcpy(allChannels[k].channelMembers[j] ,allMembers[i].name);
            cJSON *x = cJSON_CreateObject();
            cJSON_AddStringToObject(x,"type","");
            cJSON_AddStringToObject(x,"content","user successfully joined the channel");
            sprintf(serverAnswer,"%s", cJSON_Print(x));
            return;
        }
    }

}
void refresh(char message[]){
    int i = findMemberByToken(message);
    if (i == -1) {
        erroR("wrong token");
        return;
    }
    if (allMembers[i].position == 0) {
        erroR("please log in first!!");
        return;
    } else if (allMembers[i].position == 1) {
        erroR("please join a channel first!!");
        return;
    }
    char location[60];
    sprintf(location, "./channels/%s.json", allMembers[i].channel);
    FILE* channel=fopen(location,"r");
    char* string=calloc(maxMessageLength*100,sizeof(char));
    char temp[maxMessageLength];
    while(fgets(temp, 100, channel)) {
        strcat(string, temp);
    }
    cJSON *cjsonArr = cJSON_Parse(string);

    for (int j = 0; j <allMembers[i].lastSeen ; ++j) {
        cJSON_DeleteItemFromArray(cjsonArr, 0);
    }
    allMembers[i].lastSeen += cJSON_GetArraySize(cjsonArr);
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","List");
    cJSON_AddItemToObject(x, "content", cjsonArr);
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}
void channelMember(char message[]){
    int i = findMemberByToken(message);
    if (i == -1) {
        erroR("wrong token");
        return;
    }
    if (allMembers[i].position == 0) {
        erroR("you should log in first!!");
        return;
    } else if (allMembers[i].position == 1) {
        erroR("you should join a channel first!!");
        return;
    }
    int k=0;
    for (k = 0; k <totalChannel ; ++k) {
        if(stringCompare(allMembers[i].channel,allChannels[k].channelName))
            break;
    }
    cJSON *memberArray = cJSON_CreateArray();
    for (int l = 0; l <totalChannelMembers && !stringCompare(allChannels[k].channelMembers[l],"") ; ++l) {
        if (!stringCompare(allChannels[k].channelMembers[l] , "-1")) {
            cJSON_AddItemToArray(memberArray, cJSON_CreateString(allChannels[k].channelMembers[l]));
        }
    }
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","");
    cJSON_AddItemToObject(x,"content",memberArray);
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}

void logOut(char message[]){
    int i = findMemberByToken(message);
    if (i == -1) {
        erroR("wrong token");
        return;
    }
    if (allMembers[i].position == 2) {
        leaveChannel(message);
    }
    allMembers[i].lastSeen=0;
    allMembers[i].position=0;
//    strcpy(allMembers[i].name, "");
    strcpy(allMembers[i].authToken, "");
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","");
    cJSON_AddStringToObject(x,"content","logged out");
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}
void leaveChannel(char message[]){
    int i = findMemberByToken(message);
    if (i == -1) {
        erroR("wrong token");
        return;
    }
    if (allMembers[i].position == 0) {
        erroR("please log in first!!");
        return;
    } else if (allMembers[i].position == 1) {
        erroR("you aren't in any channel yet!!");
        return;
    }
    int k=0;
    for (k = 0; k <totalChannel ; ++k) {
        if(stringCompare(allMembers[i].channel,allChannels[k].channelName))
            break;
    }
    for (int j = 0; j <totalChannelMembers ; ++j) {
        if (stringCompare(allChannels[k].channelMembers[j], allMembers[i].name)) {
            strcpy(allChannels[k].channelMembers[j], "-1");
            break;
        }
    }
    char location[60];
    sprintf(location, "./channels/%s.json", allChannels[k].channelName);
    FILE* channel=fopen(location,"r");
    char* cjsonArray=calloc(maxMessageLength*100,sizeof(char));
    char temp[maxMessageLength];
    while(fgets(temp, 100, channel)) {
        strcat(cjsonArray, temp);
    }
    cJSON *cjsonArr = cJSON_Parse(cjsonArray);
    fclose(channel);
    cJSON* channelMessage=cJSON_CreateObject();
    cJSON_AddStringToObject(channelMessage, "sender", "server");
    char leaveMessage[200];
    sprintf(leaveMessage, "%s left the channel!!", allMembers[i].name);
    cJSON_AddStringToObject(channelMessage,"content",leaveMessage);
    cJSON_AddItemToArray(cjsonArr, channelMessage);
    channel = fopen(location, "w");
    fprintf(channel, cJSON_Print(cjsonArr));
    fclose(channel);

    strcpy(allMembers[i].channel, "");
    allMembers[i].position=1;
    allMembers[i].lastSeen=0;
    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x,"type","");
    cJSON_AddStringToObject(x,"content","user left the channel");
    sprintf(serverAnswer,"%s", cJSON_Print(x));
}




int findMemberByName(char name[]){
    for (int j = 0; j <totalMembers && !stringCompare(allMembers[j].name,"") ; ++j) {
        if(stringCompare(name,allMembers[j].name))
            return j;
    }
    return -1;
}
int findMemberByToken(char token[]){
    for (int j = 0; j <totalMembers && !stringCompare(allMembers[j].name,"") ; ++j) {
        if(stringCompare(token,allMembers[j].authToken))
            return j;
    }
    return -1;
}