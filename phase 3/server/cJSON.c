#include <stdio.h>
#include <string.h>

#define MaxNodeLength 1024
#define MaxAllNodeLength 100000

typedef struct cJSON cJSON;
/*struct cJSON {
    char nodeString[MaxNodeLength];
    int type;//nonsense(0) or name(1) or content(2)
    cJSON *nextNode;
};*/
cJSON allNodes[100000];
static int l=0;
cJSON *cJSON_CreateObject() {
    strcpy(allNodes[l].nodeString, "{\n");
    strcpy(allNodes[l+1].nodeString, "}\n");
    allNodes[l].nextNode = &allNodes[l+1];
    allNodes[l].type = 0;
    allNodes[l+1].type = 0;
    l+=2;
    return &allNodes[l-2];
}

void cJSON_AddStringToObject(cJSON *object, char *name, char *content) {
    cJSON *tempPtr;
    tempPtr = object->nextNode;
    while (tempPtr->nextNode != NULL) {
        object = object->nextNode;
        tempPtr = object->nextNode;
    }
    object->nextNode = &allNodes[l];
    allNodes[l].nextNode = &allNodes[l+1];
    allNodes[l+1].nextNode = &allNodes[l+2];
    allNodes[l+2].nextNode = tempPtr;
    allNodes[l].type = 1;
    allNodes[l+1].type = 0;
    allNodes[l+2].type = 2;
    sprintf(allNodes[l].nodeString, "%s", name);
    sprintf(allNodes[l+2].nodeString, "%s\n", content);
    sprintf(allNodes[l+1].nodeString, ":");
    l+=3;
}
char *cJSON_Print(cJSON *nodeAddress) {
    static char string[MaxAllNodeLength];
    strcpy(string, "");
    while (nodeAddress != NULL) {
        strcat(string, nodeAddress->nodeString);
        nodeAddress = nodeAddress->nextNode;
    }
    return string;
}
cJSON* cJSON_CreateArray(){
    strcpy(allNodes[l].nodeString, "[\n");
    strcpy(allNodes[l+1].nodeString, "]\n");
    allNodes[l].nextNode = &allNodes[l+1];
    allNodes[l].type = 0;
    allNodes[l+1].type = 0;
    l+=2;
    return &allNodes[l-2];
}
void cJSON_AddItemToArray(cJSON* node, cJSON* item ){
    printf("%s", cJSON_Print(node));
    cJSON *tempPtr;
    tempPtr = node->nextNode;
    while (tempPtr->nextNode != NULL) {
        node = node->nextNode;
        tempPtr = node->nextNode;
    }
    node->nextNode = item;
    while (item->nextNode != NULL)
        item = item->nextNode;
    item->nextNode=tempPtr;
}
int stringCompare(char *firstString, char *secondString) {
    int l = 0;
    for (; *(firstString + l) != '\0' || *(secondString + l) != '\0'; ++l) {
        if (*(firstString + l) != *(secondString + l))
            return 0;
    }
    if (*(firstString + l) != *(secondString + l))
        return 0;
    else
        return 1;
}
void cJSON_DeleteItemFromArray(cJSON* array,int l){
    int check=-1;
    cJSON* nextNode=array->nextNode;
    while (nextNode != NULL) {
        if(stringCompare(nextNode->nodeString,"{\n")){
            check++;
        }
        if (check == l) {
            while (!stringCompare(nextNode->nodeString, "}\n")) {
                nextNode=nextNode->nextNode;
            }
            array->nextNode=nextNode->nextNode;
            break;
        }
        array=nextNode;
        nextNode=nextNode->nextNode;
    }
}
int cJSON_GetArraySize(cJSON* array){
    int l=0;
    cJSON* nextNode=array->nextNode;
    while (!stringCompare(nextNode->nodeString,"]\n")) {
        if(stringCompare(nextNode->nodeString,"{\n")){
            l++;
        }
        nextNode=nextNode->nextNode;
    }
    return l;
}
cJSON* cJSON_CreateString(char* string){
    allNodes[l].nextNode = &allNodes[l+1];
    allNodes[l+1].nextNode = &allNodes[l+2];
    sprintf(allNodes[l].nodeString, "{\n");
    sprintf(allNodes[l+1].nodeString, "%s", string);
    sprintf(allNodes[l+2].nodeString, "\n}\n");
    l+=3;
    return &allNodes[l-3];
}
void cJSON_ParseLineByLine(char* lineString,int* nodeCounter){
    if (stringCompare(lineString, "{")) {
        strcpy(allNodes[l + *nodeCounter].nodeString, "{\n");
    }else if(stringCompare(lineString,"}")){
        strcpy(allNodes[l + *nodeCounter].nodeString, "}\n");
    }else if(stringCompare(lineString,"[")){
        strcpy(allNodes[l + *nodeCounter].nodeString, "[\n");
    }else if(stringCompare(lineString,"]")){
        strcpy(allNodes[l + *nodeCounter].nodeString, "]\n");
    } else {
        int k=0;
        for (int j = 0; *(lineString+j)!='\0' ; ++j) {
            if (*(lineString + j) == ':') {
                k=j;
                *(lineString + j) = '\0';
                break;
            }
        }
        if (k) {
            strcpy(allNodes[l + *nodeCounter].nodeString, lineString);
            strcpy(allNodes[l+1 + *nodeCounter].nodeString, ":");
            strcpy(allNodes[l+2 + *nodeCounter].nodeString, lineString+k+1);
            strcat(allNodes[l+2 + *nodeCounter].nodeString, "\n");
            *nodeCounter=*nodeCounter+2;
        }else{
            strcpy(allNodes[l + *nodeCounter].nodeString, lineString);
            strcat(allNodes[l + *nodeCounter].nodeString, "\n");
        }
    }
}
cJSON* cJSON_Parse(char* string){
    cJSON* firstNode=&allNodes[l];
    int nodeCounter=1;
    for (int j = 0; *(string+j)!='\0' ; ++j) {
        if(*(string+j)=='\n') {
            *(string + j) = '\0';
            cJSON_ParseLineByLine(string, &nodeCounter);
            nodeCounter++;
            string+=j+1;
            j=0;
        }
    }
    for (int k = 0; k <nodeCounter-1 ; ++k) {
        allNodes[l + k].nextNode = &allNodes[l + k + 1];
    }
    l += nodeCounter;
    return firstNode;
}
cJSON* cJSON_GetObjectItem(cJSON* object,char* string){
    while (object->nextNode != NULL) {
        if(stringCompare(object->nodeString,string)) {
            object=object->nextNode;
            return (object->nextNode);
        }
        object=object->nextNode;
    }
}
void cJSON_AddItemToObject(cJSON* object,char* string,cJSON* item){
    cJSON *tempPtr;
    tempPtr = object->nextNode;
    while (tempPtr->nextNode != NULL) {
        object = object->nextNode;
        tempPtr = object->nextNode;
    }
    object->nextNode = &allNodes[l];
    allNodes[l].nextNode = &allNodes[l+1];
    allNodes[l+1].nextNode = item;
    while (item->nextNode != NULL) {
        item=item->nextNode;
    }
    item->nextNode=tempPtr;
    sprintf(allNodes[l].nodeString, "%s", string);
    sprintf(allNodes[l+1].nodeString, ":");
    l+=2;
}
char* cJSON_PrintO(cJSON* node){
    static char string[MaxAllNodeLength]={};
    strcpy(string, node->nodeString);
    if(stringCompare(node->nodeString,"[")){
        while (!stringCompare(node->nodeString, "]")) {
            node = node->nextNode;
            strcat(string, node->nodeString);
        }
    }
    return string;
}


cJSON* cJSON_GetArrayItem(cJSON* array, int l){

    int check = -1;
    cJSON *nextNode = array->nextNode;
    while (nextNode != NULL) {
        if (stringCompare(nextNode->nodeString, "{\n")) {
            check++;
        }
        if (check == l) {
            return nextNode;
        }
        array = nextNode;
        nextNode = nextNode->nextNode;
    }
}
/*
int main() {
    char string[]="[\n"
                  "{\n"
                  "type:Error\n"
                  "typqe:Errore\n"
                  "}\n"
                  "{\n"
                  "type:Error\n"
                  "typqe:Errore1\n"
                  "}\n"
                  "{\n"
                  "hello\n"
                  "}\n"
                  "]\n";

    cJSON *x = cJSON_CreateObject();
    cJSON_AddStringToObject(x, "type", "Error");
    cJSON_AddStringToObject(x, "typqe", "Errore");
    cJSON *z = cJSON_CreateObject();
    cJSON_AddStringToObject(z, "type", "Error");
    cJSON_AddStringToObject(z, "typqe", "Errore1");
    cJSON *y =cJSON_CreateArray();
    cJSON_AddItemToArray(y, x);
    cJSON_AddItemToArray(y, z);
    cJSON_AddItemToArray(y, cJSON_CreateString("hello"));
    //cJSON_DeleteItemFromArray(y, 0);


    cJSON *n = cJSON_Parse(string);
    x = cJSON_GetObjectItem(n, "type");

    printf("%s\n", cJSON_Print(x));


    return 0;
}
*/
