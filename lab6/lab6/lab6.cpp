#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <malloc.h>


#define PATH "input.txt"

// 增加符号表条目类型
typedef enum { VARIABLE, ARRAY, FUNCTION } SymbolType;

// 修改符号表节点结构以存储更多信息
typedef struct LNode {
    char name[30];
    SymbolType type;
    char dataType[30];
    char scope[30];
    struct LNode* next;
    int location;
    // 对于函数，增加返回类型和参数信息
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

// 尾插
void InsertTail(LinkList L, char name[], SymbolType type, char dataType[], char scope[], char returnType[], char params[]) {
    LNode* first = L;
    while (L->next != NULL) {
        L = L->next;
    } // 找到最后一个
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

// 打印单链表
void print(LinkList LL) {
    LNode* first = LL;
    while (LL->next != NULL) {
        LL = LL->next;
        printf("位置: %d\n", LL->location);
        printf("标识符: %s\n", LL->name);
        printf("作用域: %s\n", LL->scope);
        if (LL->type == VARIABLE) {
            printf("类型: 变量\n");
            printf("数据类型: %s\n", LL->dataType);
        }
        else if (LL->type == ARRAY) {
            printf("类型: 数组\n");
            printf("数据类型: %s\n", LL->dataType);
        }
        else if (LL->type == FUNCTION) {
            printf("类型: 函数\n");
            printf("返回类型: %s\n", LL->returnType);
            printf("参数: %s\n", LL->params);
        }
        printf("----------------\n");
    }
    LL = first;
}

// 判断表里是否已经有了该元素
// 返回几表示在第几个，返回-1表示不存在
int hasElem(LinkList L, char name[]) {
    LNode* first = L;
    while (L->next != NULL) {
        if (strcmp(L->next->name, name) == 0) {
            return L->next->location;
        }
        L = L->next;
    }
    L = first;
    return -1; // 表示没有相同元素
}

// 测试方法
void printHasElem(LinkList L, char name[]) {
    printf(hasElem(L, name) == -1 ? "符号表中没有该标识符\n" : "该标识符已位于符号表的第%d位\n", hasElem(L, name));
}

// 是否是系统关键字，1表示是，-1表示不是
int isExist(char string[]) {
    char exist[40][10] = { {"char"}, {"double"}, {"enum"}, {"float"}, {"int"}, {"long"}, {"short"}, {"signed"},
        {"struct"}, {"union"}, {"unsigned"}, {"void"}, {"for"}, {"do"}, {"while"}, {"break"}, {"continue"},
        {"if"}, {"else"}, {"goto"}, {"switch"}, {"case"}, {"default"}, {"return"}, {"auto"}, {"extern"},
        {"register"}, {"static"}, {"const"}, {"sizeof"}, {"typedef"}, {"volatile"}, {"main"}, {"printf"},
        {"include"}, {"true"}, {"bool"}, {"false"} };
    for (int i = 0; i < 40; i++) {
        if (strcmp(string, exist[i]) == 0) {
            return 1; // 与关键字重复
        }
    }
    return -1;
}

// 检查是否是合法标识符
int isLegal(char string[]) {
    bool flag = false; // 是否是合法标识符
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

// 识别变量、数组、函数等并插入符号表
void recognizeAndInsert(LinkList L, char buf[]) {
    char* ptr;
    char* token = strtok_s(buf, " *;(,)=\n\t", &ptr);
    char dataType[30] = "";
    char scope[30] = "global"; // 默认作用域
    char returnType[30] = "";
    char params[100] = "";
    SymbolType type;

    if (token != NULL && isExist(token) == 1) { // 第一个标识符是数据类型
        strcpy(dataType, token);
        token = strtok_s(NULL, " *;(,)=\n\t", &ptr);
        if (token != NULL) {
            if (strcmp(dataType, "void") == 0 && strcmp(token, "main") == 0) {
                // 特殊处理 main 函数
                strcpy(returnType, "void");
                strcpy(params, "int argc, char *argv[]");
                type = FUNCTION;
                InsertTail(L, token, type, (char*)"", scope, returnType, params);
                return;
            }
            else if (strchr(token, '(') != NULL) { // 检查是否是函数
                type = FUNCTION;
                strcpy(returnType, dataType);
                strcpy(params, ptr); // 获取参数
            }
            else if (strchr(ptr, '[') != NULL) { // 检查是否是数组
                type = ARRAY;
            }
            else {
                type = VARIABLE;
            }
            if (hasElem(L, token) == -1) { // 表中没有该元素
                InsertTail(L, token, type, dataType, scope, returnType, params);
            }
            else {
                printf("表中已存在 %s 元素，在第 %d 个\n", token, hasElem(L, token));
            }
        }
    }
}

// 打印文件内容的方法
int printFile() {
    char buf[100];  // 缓冲区
    FILE* fp;      // 文件指针
    if ((fp = fopen(PATH, "r")) == NULL) // 打开文件
        return -1;
    while (fgets(buf, 100, fp) != NULL) {
        char* ptr;
        char* token = strtok_s(buf, " ;(,)=\n\t", &ptr); // 分割字符串
        while (token != NULL) {
            printf("%s\n", token);
            token = strtok_s(NULL, " ;(,)=\n\t", &ptr);
        }
    }
    fclose(fp); // 关闭文件
    return 0;
}

int lastFunction(LinkList L) {
    char buf[100];  // 缓冲区
    FILE* fp;      // 文件指针
    if ((fp = fopen(PATH, "r")) == NULL) // 打开文件
        return -1;
    while (fgets(buf, 100, fp) != NULL) {
        recognizeAndInsert(L, buf);
    }
    fclose(fp); // 关闭文件
    return 0;
}

int main() {
    LinkList L = Init();
    lastFunction(L);
    printf("----------------\n");
    print(L);
    return 0;
}

