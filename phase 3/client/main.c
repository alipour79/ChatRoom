#define maxMessageLength 100000
#define PORT 1111
#define tokenLength 25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "cJSON.h"
#include "cJSON.c"


char *combineString(const char *, const char *, const char *);//this function will combine 3 string to send to the server as command
char *combineString2(const char *, const char *);//this function will combine 2 string to send to the server as command

void makingConnectionSendReceive();//this function will make a socket and make a connection to the server and send a message and receive the answer

void clearScreen();//this function will clear the console screen

//below functions will show the considered menu on console screen
int accountMenu();
void registerMenu();
void loginMenu();
int mainMenu();
int channelMenu();

int errorCheck();//check that if server send an error or not

char *searchInAnswer(char *);//this function will search in answer with cJSON and find the part we want

void createChannel();
void joinChannel();
int logout();

void sendMessageToChannel();
void refresh();
void channelMembers();
int leaveChannel();


char *message;//this string will use to be send to the server
char *answer;//this string will use to receive answer form server
char *authToken;//this string will keep Auth Token


int main() {
    message = (char *) malloc(maxMessageLength * sizeof(char));
    answer = (char *) malloc(maxMessageLength * sizeof(char));
    authToken = malloc(30);
    int i = 0;
    while (1) {
        clearScreen();
        i = accountMenu();
        if (i == -1)
            break;
    }//end while
    return 0;
}//end main

//this function will combine 3 string to send to the server as command
char *combineString(const char *str1, const char *str2, const char *str3) {
    char *result = malloc(
            (strlen(str1) + strlen(str2) + strlen(str3) + 5) * sizeof(char)); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, str1);
    strcat(result, " ");
    strcat(result, str2);
    strcat(result, ", ");
    strcat(result, str3);
    //strcat(result, "\n");
    return result;
}//end combine String

//this function will combine 2 string to send to the server as command
char *combineString2(const char *str1, const char *str2) {
    char *result = malloc((strlen(str1) + strlen(str2) + 2) * sizeof(char)); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, str1);
    strcat(result, " ");
    strcat(result, str2);
    //strcat(result, "\n");
    return result;
}//end combine String 2

//this function will make a socket and make a connection to the server and send a message and receive the answer
void makingConnectionSendReceive() {
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);//making socket


    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;


    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {//check if the socket is connected correctly or not
        printf("There was an error with making connection to the remote socket.\n\n");
        return;
    }//end if

    char server_response[maxMessageLength];
    strcpy(server_response, message);//this function will copy the message to a string to send to the server
    char sAnswer[maxMessageLength];
    send(network_socket, &server_response, sizeof(server_response), 0);
    recv(network_socket, &sAnswer, sizeof(sAnswer), 0);
    strcpy(answer, sAnswer);//this function will copy the server answer to answer string
}//end makingConnectionSendReceive

//this function will show the account menu on the console screen
int accountMenu() {
    printf("Account Menu:\n1:Register\n2:Login\n3:Exit\n");
    int i = 0;
    scanf("%d%*c", &i);
    clearScreen();

    switch (i) {
        case 1:
            registerMenu();
            break;
        case 2:
            loginMenu();
            break;
        case 3:
            return -1;
        default:
            //clearScreen();
            printf("Enter a valid number\nPress any key to go to the account menu...");
            scanf("%*c");
            clearScreen();
    }//end switch
}//end accountMenu

//this function will show the register menu on the console screen
void registerMenu() {
    char username[100], password[100];
    printf("Enter Username:(without space)\n");
    scanf("%s", username);

    printf("Enter Password:(without space)\n");
    scanf("%s", password);

    message = combineString("register", username, password);
    makingConnectionSendReceive();//the last two line will send register command to the server

    if (errorCheck())//check if there is an error or not
        registerMenu();

    clearScreen();
    printf("Now please login with your username and password.\n");
    loginMenu();
}//end register menu

//this function will show the login menu on the console screen
void loginMenu() {
    char username[100], password[100],*auth;
    printf("Enter Username:(without space)\n");
    scanf("%s", username);
    printf("Enter Password:(without space)\n");
    scanf("%s", password);

    message = combineString("login", username, password);
    makingConnectionSendReceive();//the last two line will send login command to the server

    if (errorCheck())//check if there is an error or not
        loginMenu();
    auth = searchInAnswer("content");//this line will save the auth token in intended string
    *(auth + tokenLength ) = '\0';
    strcpy(authToken, auth);
    clearScreen();
    if(mainMenu())
        return;
}//end login menu

//this function will search in answer with cJSON and find the part we want
char *searchInAnswer(char *str) {
    cJSON *answerJson = cJSON_Parse(answer);
    cJSON *type = cJSON_GetObjectItem(answerJson, str);
    char *string = cJSON_PrintO(type);
    return string;
}//end searchInAnswer

//this function will show the main menu on the console screen
int mainMenu() {
    while (1) {
        printf("1:Create Channel\n2:Join Channel\n3:Logout\n");
        int i = 0;
        scanf("%d%*c", &i);
        clearScreen();
        switch (i) {
            case 1:
                createChannel();
                break;
            case 2:
                joinChannel();
                break;
            case 3:
                if(logout())
                    break;
                return 1;
            default:
                printf("Enter a valid number\nPress any key to go to the main menu...");
                scanf("%*c");
                clearScreen();
                break;
        }//end switch
    }//end while

}//end main menu

//this function will create channel
void createChannel() {
    printf("Enter the channel name(without space)\n");
    char channel[200];
    scanf("%s", channel);
    clearScreen();

    message = combineString("createChannel", channel, authToken);
    makingConnectionSendReceive();//the last two line will send create channel command to the server

    if (errorCheck())//this statement will check if server will send an error or not
        return;
    channelMenu();
}//end createChannel

//this function will join channel
void joinChannel() {
    printf("Enter the channel name(without space)\n");
    char channel[200];
    scanf("%s", channel);
    clearScreen();

    message = combineString("joinChannel", channel, authToken);
    makingConnectionSendReceive();//the last two line will send join channel command to the server

    if (errorCheck())//this statement will check if server will send an error or not
        return;
    channelMenu();
}//end joinChannel

//this function will logout from the account
int logout() {
    message = combineString2("logout", authToken);
    makingConnectionSendReceive();//the last two line will send logout command to the server

    if (errorCheck())//this statement will check if server will send an error or not
        return 1;
    return 0;
}//end logout

//this function will show the channel menu on the console screen
int channelMenu() {
    while (1) {
        printf("1:Send Message\n2:Refresh\n3:Channel Members\n4:Leave Channel\n");
        int i = 0;
        scanf("%d%*c", &i);
        clearScreen();

        switch (i) {
            case 1:
                sendMessageToChannel();
                break;
            case 2:
                refresh();
                break;
            case 3:
                channelMembers();
                break;
            case 4:
                if(leaveChannel())
                    break;
                return 1;
            default:
                printf("Enter a valid number\nPress any key to go to the channel menu...");
                scanf("%*c");
                clearScreen();
        }//end of switch
    }//end while
}//end channelMenu

//this function will use to a user send a message to a group
void sendMessageToChannel() {
    printf("Enter the message:\n");
    char msg[maxMessageLength];

    //the below statement will get message from user
    fgets(msg, sizeof(msg), stdin);
    for (int i = 0; i < maxMessageLength; ++i) {
        if (msg[i] == '\n') {
            msg[i] = '\0';
            break;
        }//end if
    }//end for

    clearScreen();

    message = combineString("send", msg, authToken);
    makingConnectionSendReceive();//the last two line will send a message that user entered to the program to the server


    if (errorCheck())//this statement will check if server will send an error or not
        return;

}//end sendMessageToChannel

//this function will receive the last changes in the channel refresh the messages and show them on the console screen
void refresh() {
    message = combineString2("refresh", authToken);
    makingConnectionSendReceive();//the last two line will send refresh command to the server

    if (errorCheck())//this statement will check if server will send an error or not
        return;

    cJSON *ansJson = cJSON_Parse(answer);
    cJSON *content = NULL;
    content = cJSON_GetObjectItem(ansJson, "content");
    int n = cJSON_GetArraySize(content);
    if (n == 0)
        printf("No new message!!");

    cJSON *element;
    cJSON *sender;
    cJSON *message;
    for (int i = 0; i < n; ++i) {
        element = cJSON_GetArrayItem(content, i);
        sender = cJSON_GetObjectItem(element, "sender");
        message = cJSON_GetObjectItem(element, "content");
        printf("%s", cJSON_PrintO(sender));
        printf("%s\n\n", cJSON_PrintO(message));
    }//end for


    printf("Press any key to go to the channel menu...");
    scanf("%*c");
    clearScreen();
}//end refresh function

//this channel use to show the channel members
void channelMembers() {
    message = combineString2("channelMember", authToken);
    makingConnectionSendReceive();//the last two line will send the channel members command to the server

    if (errorCheck())//this statement will check if server will send an error or not
        return;

    cJSON *ansJson = cJSON_Parse(answer);
    cJSON *content = NULL;
    content = cJSON_GetObjectItem(ansJson, "content");
    int n = cJSON_GetArraySize(content);
    if (n == 0)
        printf("No member!!");

    cJSON *member;
    for (int i = 1; i <= n; ++i) {
        member = cJSON_GetArrayItem(content, i-1);
        printf("%d:", i);
        printf("%s\n", cJSON_PrintO(member->nextNode));
    }//end for

    printf("Press any key to go to the channel menu...");
    scanf("%*c");
    clearScreen();
}//end channelMembers

//this function will leave the channel
int leaveChannel() {
    message = combineString2("leave", authToken);
    makingConnectionSendReceive();//the last two line will send leave channel command to the server

    if (errorCheck())//this statement will check if server will send an error or not
        return 1;
    return 0;
}//end leave channel

//this function will clear the console screen
void clearScreen() {
    system("clear||cls");
}//end clear screen

//this function will the receive message from server contain a error or not and print it
int errorCheck() {
    char *string = searchInAnswer("type");
    if (strcmp(string, "Error\n") == 0) {
        clearScreen();
        char *err = searchInAnswer("content");
        printf("%s\n", err);
        return 1;
    }//end if
    return 0;
}//end error check


