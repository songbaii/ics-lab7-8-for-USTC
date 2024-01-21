#define MAX_LINE_LENGTH 100
#define LENGTH 3
#define STUDENT_LAST_ID 3

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

void read_asm_file(const char *filename, char lines[][MAX_LINE_LENGTH], int *num_lines);
void writenumber(char* file, int16_t number);
int16_t lab1(char* string);
int16_t lab2(char* string);
int16_t lab3(char* string);
void lab4(char* string, char* file);
void remove1(int16_t n, char* file, int16_t* state);
void writenumberbinary(char* file, int16_t* number);
void put(int16_t n, char* file, int16_t* state);
void writetitle(char* file, int number);
int main(int argc, char* argv[]){
    /*int argc = 3;
    char* argv[3];
    for(int i = 0; i < 3; i++){
        argv[i] = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
    }strcpy(argv[0], "lab.c");
    strcpy(argv[1], "test.txt");
    strcpy(argv[2], "test_out.txt");*/
    
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_file.asm> <output_file.txt>\n", argv[0]);
        return 1;
    }char input_filename[100];
    char output_filename[100];
    strcpy(input_filename, argv[1]);
    strcpy(output_filename, argv[2]);
    char lines[MAX_LINE_LENGTH][MAX_LINE_LENGTH];
    int num_lines = 0, i = 0, count = 0;
    int16_t result1;
    read_asm_file(input_filename, lines, &num_lines);
    writetitle(output_filename, 1);
    for(i = 0; i < LENGTH; i++){
        result1 = lab1(lines[i]);
        writenumber(output_filename, result1);
    }writetitle(output_filename, 2);
    count = count + LENGTH;
    for(i = 0; i < LENGTH; i++){
        result1 = lab2(lines[count + i]);
        writenumber(output_filename, result1);
    }writetitle(output_filename, 3);
    count = count + LENGTH;
    for(i = 0; i < LENGTH; i++){
        result1 = lab3(lines[count + i]);
        writenumber(output_filename, result1);
    }writetitle(output_filename, 4);
    count = count + LENGTH;
    for(i = 0; i < LENGTH; i++){
        lab4(lines[i + count], output_filename);
    }return 0;
}

void read_asm_file(const char *filename, char lines[][MAX_LINE_LENGTH], int *num_lines)
{//阅读并根据换行符分割文件
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file))
    {
        strcpy(lines[*num_lines], line);
        for(int i = 0; i < MAX_LINE_LENGTH; i++){
            if(lines[*num_lines][i] == '\n'){
                lines[*num_lines][i] = '\0';//添加结束标志
                break;
            }
        }
        (*num_lines)++;
    }

    fclose(file);
}

int16_t lab1(char* string){
    int16_t R1 = 0, R2 = 0, R3 = 0;
    for(int i =  0; string[i] != '\0'; i++){
        R3 = R3 & 0;//清空R3
        R1 = string[i] - '0';
        for(int j = 0; j < 10; j++){
            R3 = R3 + R2;//R2十进制下左移一位
        }R2 = R3;
        R2 = R2 + R1;
    }R1 = R2;
    R2 = R2 & 0;
    R3 = R3 & 0;
    R3 = R3 + 8;
    R3 = R3 + 8;
    int16_t R4 = 0;
    R4 = R4 + 1;//用于检测
    int16_t R5 = R1 & R4;
    if(R5 > 0){//如果是奇数
        goto x300B;
    }R1 = ~R1;//如果是偶数
    R1 = R1 + 1;
    x300B: R5 = R1 & R4;
    if(R5 == 0){
        goto x300E;
    }R2 = R2 + 1;//找到了1
    x300E: R4 = R4 + R4;//移动到下一位
    R3 = R3 - 1;
    if(R3 > 0){
        goto x300B;
    }int16_t R6 = R6 & 0;
    R6 = R6 + STUDENT_LAST_ID;
    R2 = ~R2;
    R2 = R2 + 1;
    R2 = R2 + 8;
    R2 = R2 + 8;
    int16_t R7 = R2 + R6;
    return R7;
}

void writenumber(char* file, int16_t number){
    FILE* fp;
    fp = fopen(file, "a+");
    fprintf(fp, "%d\n", number);
    fclose(fp);
    return ;
}

int16_t lab2(char* string){
    int16_t R1 = 0, R2 = 0, R3 = 0;
    for(int i =  0; string[i] != '\0'; i++){
        R3 = R3 & 0;//清空R3
        R1 = string[i] - '0';
        for(int j = 0; j < 10; j++){
            R3 = R3 + R2;//R2十进制下左移一位
        }R2 = R3;
        R2 = R2 + R1;
    }R1 = R2;
    R2 = R2 & 0;
    R3 = R3 & 0;
    int16_t R0 = 3;
    int16_t R4 = 4095;
    int16_t R6 = ~R4;
    R1 = R1 - 1;
    if(R1 == 0){
        goto write;
    }begin: if(R2 == 0){
        goto positive;
    }else if(R2 < 0){
        goto negative;
    }positive: R0 = R0 + R0;
    R0 = R0 + 2;
    goto judge;
    negative: R0 = R0 + R0;
    R0 = R0 - 2;
    int16_t R5;
    judge: R5 = R0 & R6;
    if(R5 == 0){
        goto next;
    }R0 = R0 & R4;
    next: R1 = R1 - 1;
    if(R1 == 0){
        goto write;
    }R5 = R0 & 7;
    if(R5 == 0){
        goto operation;
    }R5 = R0;
    minus: R5 = R5 - 10;
    if(R5 >= 0){
        goto minus;
    }R5 = R5 + 10;
    R5 = R5 - 8;
    if(R5 != 0){
        goto last;
    }operation: R2 = ~R2;
    last: R2 = R2;
    goto begin;
    write: return R0;
}

int16_t lab3(char* string){
    char* string1;
    char* string2;
    string2 = strchr(string, ' ') + 1;
    for(int i = 0; i < MAX_LINE_LENGTH; i++){
        if(string2[i] == '\0'){
            string2[i] = NULL;//这里会有warning，但是根据题意这里应该做这个操作，且此处的warning不影响程序的执行
            break;
        }
    }
    string1 = strchr(string, ' ');
    string1[0] = NULL;
    string1 = string;
    int16_t R0 = 0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0;
    next_turn: R3 = string1[R0];
    R4 = string2[R1];
    R5 = ~R3;
    R5 = R5 + 1;
    R5 = R5 + R4;
    if(R5 != 0){
        goto notmatch;
    }R5 = R4 + R3;
    if(R5 == 0){
        goto match;
    }R0 = R0 + 1;
    R1 = R1 + 1;
    goto next_turn;

    notmatch: R5 = ~R5;
    R2 = R5 + 1;
    goto done;
    
    match: R2 = R2 & 0;

    done: return R2;
}

void lab4(char* string, char* file){
    int16_t R1 = 0, R2 = 0, R3 = 0;
    for(int i =  0; string[i] != '\0'; i++){
        R3 = R3 & 0;//清空R3
        R1 = string[i] - '0';
        for(int j = 0; j < 10; j++){
            R3 = R3 + R2;//R2十进制下左移一位
        }R2 = R3;
        R2 = R2 + R1;
    }R1 = R2;//R1中对应的我们需要的
    R2 = R2 & 0;
    R3 = R3 & 0;
    int16_t* number = &R2;
    remove1(R1, file, number);
    FILE* fp;
    fp = fopen(file, "a+");
    fprintf(fp, "\n");
    fclose(fp);
    return ;
}

void remove1(int16_t n, char* file, int16_t* state){
    if(n == 0){
        return ;
    }else if(n == 1){
        *state = *state + 1;
        writenumberbinary(file, state);
        return ;
    }remove1(n - 2, file, state);
    int16_t R0 = 0, R2 = 1;
    for(R0 = 0; R0 < n - 1; R0++){
        R2 = R2 + R2;
    }*state = *state + R2;
    writenumberbinary(file, state);
    put(n - 2, file, state);
    remove1(n - 1, file, state);
    return ;
}

void writenumberbinary(char* file, int16_t* number){
    FILE* fp;
    fp = fopen(file, "a+");
    char* result = (char*)malloc(sizeof(char) * 17);
    int tempt = *number;
    itoa(tempt, result, 2);
    int16_t size = 1, count = 1;
    while(size <= *number){
        size = size + size;//左移一位
        count = count + 1;
    }count--;
    count = 16 - count;
    for(int i = 0; i < count; i++){
        fprintf(fp, "0");
    }
    fprintf(fp, "%s\n", result);
    fclose(fp);
    return ;
}

void put(int16_t n, char* file, int16_t* state){
    if(n == 0){
        return ;
    }else if(n == 1){
        *state = *state - 1;
        writenumberbinary(file, state);
        return ;
    }put(n - 1, file, state);
    remove1(n - 2, file, state);
    int16_t R0 = 0, R2 = 1;
    for(R0 = 0; R0 < n - 1; R0++){
        R2 = R2 + R2;
    }*state = *state - R2;
    writenumberbinary(file, state);
    put(n - 2, file, state);
    return ;
}

void writetitle(char* file, int number){
    FILE* fp;
    if(number == 1){
        fp = fopen(file, "w");
    }else{
        fp = fopen(file, "a+");
    }fprintf(fp, "===== lab%d =====\n", number);
    fclose(fp);
    return ;
}