#define MaxNodeLength 1024
#define MaxAllNodeLength 100000
typedef struct cJSON cJSON;
struct cJSON {
    char nodeString[MaxNodeLength];
    int type;//nonsense(0) or name(1) or content(2)
    cJSON *nextNode;
};

cJSON *cJSON_CreateObject();

void cJSON_AddStringToObject(cJSON *, char *, char *);

char *cJSON_Print(cJSON *);

cJSON *cJSON_CreateArray();

void cJSON_AddItemToArray(cJSON *, cJSON *);

int stringCompare(char *, char *);

void cJSON_DeleteItemFromArray(cJSON *, int);

int cJSON_GetArraySize(cJSON *);

cJSON *cJSON_CreateString(char *);

void cJSON_ParseLineByLine(char *, int *);

cJSON *cJSON_Parse(char *);

cJSON *cJSON_GetObjectItem(cJSON *, char *);

void cJSON_AddItemToObject(cJSON *, char *, cJSON *);

char *cJSON_PrintO(cJSON *);

cJSON *cJSON_GetArrayItem(cJSON *, int);