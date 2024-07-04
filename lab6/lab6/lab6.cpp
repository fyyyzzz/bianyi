#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <malloc.h>


#define PATH "input.txt"

// ���ӷ��ű���Ŀ����
typedef enum { VARIABLE, ARRAY, FUNCTION } SymbolType;

// �޸ķ��ű�ڵ�ṹ�Դ洢������Ϣ
typedef struct LNode {
    char name[30];
    SymbolType type;
    char dataType[30];
    char scope[30];
    struct LNode* next;
    int location;
    // ���ں��������ӷ������ͺͲ�����Ϣ
    char returnType[30];
    char params[100];
} LNode, * LinkList;

LinkList Init() {
    LinkList L;
    L = (LNode*)malloc(sizeof(LNode));
    L->next = NULL;
    L->location = 0;
    return L;
}

// β��
void InsertTail(LinkList L, char name[], SymbolType type, char dataType[], char scope[], char returnType[], char params[]) {
    LNode* first = L;
    while (L->next != NULL) {
        L = L->next;
    } // �ҵ����һ��
    L->next = (LNode*)malloc(sizeof(LNode));
    if (!L->next) return;
    L->next->location = L->location + 1;
    L = L->next;
    strcpy(L->name, name);
    L->type = type;
    strcpy(L->dataType, dataType);
    strcpy(L->scope, scope);
    if (type == FUNCTION) {
        strcpy(L->returnType, returnType);
        strcpy(L->params, params);
    }
    L->next = NULL;
    L = first;
}

// ��ӡ������
void print(LinkList LL) {
    LNode* first = LL;
    while (LL->next != NULL) {
        LL = LL->next;
        printf("λ��: %d\n", LL->location);
        printf("��ʶ��: %s\n", LL->name);
        printf("������: %s\n", LL->scope);
        if (LL->type == VARIABLE) {
            printf("����: ����\n");
            printf("��������: %s\n", LL->dataType);
        }
        else if (LL->type == ARRAY) {
            printf("����: ����\n");
            printf("��������: %s\n", LL->dataType);
        }
        else if (LL->type == FUNCTION) {
            printf("����: ����\n");
            printf("��������: %s\n", LL->returnType);
            printf("����: %s\n", LL->params);
        }
        printf("----------------\n");
    }
    LL = first;
}

// �жϱ����Ƿ��Ѿ����˸�Ԫ��
// ���ؼ���ʾ�ڵڼ���������-1��ʾ������
int hasElem(LinkList L, char name[]) {
    LNode* first = L;
    while (L->next != NULL) {
        if (strcmp(L->next->name, name) == 0) {
            return L->next->location;
        }
        L = L->next;
    }
    L = first;
    return -1; // ��ʾû����ͬԪ��
}

// ���Է���
void printHasElem(LinkList L, char name[]) {
    printf(hasElem(L, name) == -1 ? "���ű���û�иñ�ʶ��\n" : "�ñ�ʶ����λ�ڷ��ű�ĵ�%dλ\n", hasElem(L, name));
}

// �Ƿ���ϵͳ�ؼ��֣�1��ʾ�ǣ�-1��ʾ����
int isExist(char string[]) {
    char exist[40][10] = { {"char"}, {"double"}, {"enum"}, {"float"}, {"int"}, {"long"}, {"short"}, {"signed"},
        {"struct"}, {"union"}, {"unsigned"}, {"void"}, {"for"}, {"do"}, {"while"}, {"break"}, {"continue"},
        {"if"}, {"else"}, {"goto"}, {"switch"}, {"case"}, {"default"}, {"return"}, {"auto"}, {"extern"},
        {"register"}, {"static"}, {"const"}, {"sizeof"}, {"typedef"}, {"volatile"}, {"main"}, {"printf"},
        {"include"}, {"true"}, {"bool"}, {"false"} };
    for (int i = 0; i < 40; i++) {
        if (strcmp(string, exist[i]) == 0) {
            return 1; // ��ؼ����ظ�
        }
    }
    return -1;
}

// ����Ƿ��ǺϷ���ʶ��
int isLegal(char string[]) {
    bool flag = false; // �Ƿ��ǺϷ���ʶ��
    int i = 0;
    if (string[0] <= '9' && string[0] >= '0')
        return -1;
    while (string[i] != '\0') {
        if (string[i] == '_' || (string[i] >= 'a' && string[i] <= 'z') ||
            (string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= '0' && string[i] <= '9')) {
            i++;
            if (string[i] == '\0') {
                flag = true;
                break;
            }
        }
        else {
            break;
        }
    }
    return flag ? 1 : -1;
}

// ʶ����������顢�����Ȳ�������ű�
void recognizeAndInsert(LinkList L, char buf[]) {
    char* ptr;
    char* token = strtok_s(buf, " *;(,)=\n\t", &ptr);
    char dataType[30] = "";
    char scope[30] = "global"; // Ĭ��������
    char returnType[30] = "";
    char params[100] = "";
    SymbolType type;

    if (token != NULL && isExist(token) == 1) { // ��һ����ʶ������������
        strcpy(dataType, token);
        token = strtok_s(NULL, " *;(,)=\n\t", &ptr);
        if (token != NULL) {
            if (strcmp(dataType, "void") == 0 && strcmp(token, "main") == 0) {
                // ���⴦�� main ����
                strcpy(returnType, "void");
                strcpy(params, "int argc, char *argv[]");
                type = FUNCTION;
                InsertTail(L, token, type, (char*)"", scope, returnType, params);
                return;
            }
            else if (strchr(token, '(') != NULL) { // ����Ƿ��Ǻ���
                type = FUNCTION;
                strcpy(returnType, dataType);
                strcpy(params, ptr); // ��ȡ����
            }
            else if (strchr(ptr, '[') != NULL) { // ����Ƿ�������
                type = ARRAY;
            }
            else {
                type = VARIABLE;
            }
            if (hasElem(L, token) == -1) { // ����û�и�Ԫ��
                InsertTail(L, token, type, dataType, scope, returnType, params);
            }
            else {
                printf("�����Ѵ��� %s Ԫ�أ��ڵ� %d ��\n", token, hasElem(L, token));
            }
        }
    }
}

// ��ӡ�ļ����ݵķ���
int printFile() {
    char buf[100];  // ������
    FILE* fp;      // �ļ�ָ��
    if ((fp = fopen(PATH, "r")) == NULL) // ���ļ�
        return -1;
    while (fgets(buf, 100, fp) != NULL) {
        char* ptr;
        char* token = strtok_s(buf, " ;(,)=\n\t", &ptr); // �ָ��ַ���
        while (token != NULL) {
            printf("%s\n", token);
            token = strtok_s(NULL, " ;(,)=\n\t", &ptr);
        }
    }
    fclose(fp); // �ر��ļ�
    return 0;
}

int lastFunction(LinkList L) {
    char buf[100];  // ������
    FILE* fp;      // �ļ�ָ��
    if ((fp = fopen(PATH, "r")) == NULL) // ���ļ�
        return -1;
    while (fgets(buf, 100, fp) != NULL) {
        recognizeAndInsert(L, buf);
    }
    fclose(fp); // �ر��ļ�
    return 0;
}

int main() {
    LinkList L = Init();
    lastFunction(L);
    printf("----------------\n");
    print(L);
    return 0;
}

