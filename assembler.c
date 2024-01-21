#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_OUTPUT 200

char instruction_set[29][9] = {"ADD ", "AND ", "BR ", "JMP ", "JSR ", "JSRR ", "LD ", "LDI ", "LDR ", "LEA ", "NOT ", "RET ", "RTI ", "ST ", "STI ", "STR ", "TRAP ", ".ORIG ", ".END", ".FILL ", ".BLKW ", ".STRINGZ ", "BRN ", "BRZ ", "BRP ", "BRZP ", "BRNP ", "BRNZ ", "BRNZP "};//指令集作为全局变量， 便于比较
// Function prototypes
void read_asm_file(const char *filename, char lines[][MAX_LINE_LENGTH], int *num_lines);
void write_output_file(const char *filename, const char *output[], int num_lines);
void assemble(char lines[][MAX_LINE_LENGTH], int num_lines, char *output[], int* num);
void translate_instruction(char *instruction, char *machine_code, int* locate, char table[][MAX_LINE_LENGTH], int pc);
int findsite(char lines[][MAX_LINE_LENGTH], int num_lines, char* string);//寻找.ORIG出现的位置
int islabel(char* string);//判断当前语句中是否存在label
int getreal(char* string);//返回对应的16进制或者10进制表示的整数，选择不为NULL的字符串,注意！！从x或者#开始
char* get_register(char number);//返回对应的寄存器二进制表示字符串
char* get5(int number);//返回16或10进制对应的5位补码
char* getpcoff9(int pc, char* label, char table[][MAX_LINE_LENGTH], int* locate);//返回当前与label标识的位置的距离的9位补码
char* getpcoff11(int pc, char* label, char table[][MAX_LINE_LENGTH], int* locate);//返回与当前label标识的位置的距离的11位补码
char* getoff6(char* number);//将10进制或16进制转换成对应的6位补码
char* getbinary(char* number, int length);//将TRAP指令中的16进制转换为2进制
// TODO: Define any additional functions you need to implement the assembler, e.g. the symbol table.

int main(int argc, char *argv[])
{
    // Command-line argument parsing
    /*int argc = 3;
    char* argv[3];
    argv[0] = (char*)malloc(sizeof(char) * (strlen("assembler.c") + 1));
    argv[1] = (char*)malloc(sizeof(char) * (strlen("test_in.asm") + 1));
    argv[2] = (char*)malloc(sizeof(char) * (strlen("test_out.txt") + 1));
    strcpy(argv[0], "assembler.c");
    strcpy(argv[1], "test_in.asm");
    strcpy(argv[2], "test_out.txt");*/
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_file.asm> <output_file.txt>\n", argv[0]);
        return 1;
    }

    char input_filename[100];
    char output_filename[100];
    strcpy(input_filename, argv[1]);
    strcpy(output_filename, argv[2]);

    char lines[100][MAX_LINE_LENGTH]; // Assuming a maximum of 100 lines
    int num_lines = 0;
    read_asm_file(input_filename, lines, &num_lines);
    /*for(int i = 0; i < num_lines; i++){
        printf("%s\n", lines[i]);
    }*/char *output[MAX_OUTPUT]; // Output array of strings
    for (int i = 0; i < MAX_OUTPUT; i++)
    {
        output[i] = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
        output[i][0] = '\0';//加上结束标志，便于后续处理
    }assemble(lines, num_lines, output, &num_lines);
    write_output_file(output_filename, (const char **)output, num_lines - 1);

    // Free allocated memory
    for (int i = 0; i < 100; i++)
    {
        free(output[i]);
    }

    return 0;
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
        if(strncmp(lines[*num_lines], "RET", strlen("RET")) == 0 && lines[*num_lines][strlen("RET")] == '\n'){
            lines[*num_lines][strlen("RET")] = ' ';//对于RET后面补全空格，便于后续指令的编译过程
            lines[*num_lines][strlen("RET") + 1] = '\n';
        }else if(strncmp(lines[*num_lines], "RTI", strlen("RTI")) == 0 && lines[*num_lines][strlen("RTI")] == '\n'){
            lines[*num_lines][strlen("RTI") + 1] = '\n';
            lines[*num_lines][strlen("RTI")] = ' ';
        }else if(strncmp(lines[*num_lines], ".END", strlen(".END")) == 0){//对于文件的末尾进行特判
            lines[*num_lines][4] = '\n';
        }for(int i = 0; i < MAX_LINE_LENGTH; i++){
            if(lines[*num_lines][i] == '\n'){
                lines[*num_lines][i] = '\0';
                break;
            }
        }
        //printf("%s\n", lines[*num_lines]);
        (*num_lines)++;
    }

    fclose(file);
}

void write_output_file(const char *filename, const char *output[], int num_lines)
{//向目标文件书写结果
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < num_lines; i++)
    {
        if(output[i][0] == '\0'){
            break;
        }fprintf(file, "%s", output[i]);
        if(output[i + 1][0] == '\0'){
            break;
        }fputc('\n', file);
    }
    fclose(file);
}

void assemble(char lines[][MAX_LINE_LENGTH], int num_lines, char *output[], int* num)
{
    // TODO: Implement the assembly process
    // Implement the 2-pass process described in textbook.
    int start = findsite(lines, num_lines, ".ORIG ");
    int i = 0, startsite = 0, j = 0, count = 0;
    while(lines[start][i] != 'x' && lines[start][i] != '#'){//找到起始地址表示的位置
        i++;
    }startsite = getreal(&lines[start][i]);//获得初始地址
    int end = findsite(lines, num_lines, ".END");
    char table[100][MAX_LINE_LENGTH], *leftstring;//符号表
    int locate[100] = {0}, count1 = 0;//count1对应的是伪操作.BLKW和.STRINGZ带来的行数的变化
    for(i = start; i <= end; i++){//创建符号表
        if(islabel(lines[i])){//存在label，创建对应的符号表
            j = strchr(lines[i], ' ') - lines[i] + 1;//必须将后续的空格也加入
            strncpy(table[count], lines[i], j);//将label存储于符号表中
            table[count][j] = '\0';
            locate[count] = i + startsite - 1 - start + count1;//存储label对应的位置
            if(strncmp(&lines[i][j], ".BLKW ", strlen(".BLKW ")) == 0){
                leftstring = strchr(lines[i], ' ') + 1;
                count1 = count1 + getreal(leftstring) - 1;
            }else if(strncmp(&lines[i][j], ".STRINGZ ", strlen(".STRINGZ ")) == 0){
                leftstring = strchr(lines[i], '"') + 1;
                count1 = count1 + strlen(leftstring) - 1;
            }count++;
        }
    }
    /*for(i = 0; i < count; i++){
        printf("%x %s\n", locate[i], table[i]);
    }*/
    int number = 0, tempt = 0, left;
    count = 0;
    //printf("%d %d\n", start, end);
    for (int i = start; i < end; i++)//第二次扫描并输出机器码
    {
        leftstring = lines[i];
        if(islabel(leftstring)){//判断有无label
            leftstring = strchr(leftstring, ' ') + 1;//跳过label
        }if(strncmp(leftstring, ".BLKW ", strlen(".BLKW ")) == 0){//对于.BLKW伪操作进行特判
            //printf("%s", leftstring);
            leftstring = strchr(leftstring, ' ') + 1;
            number = getreal(leftstring);
            for(int j = 0; j < number; j++){
                    strcpy(output[i - start + count + j], "0000000000000000");
                    tempt++;
                    *num = *num + 1;
                }
            number = 0;
            count += (tempt - 1);
            tempt = 0;
            continue;
        }else if(strncmp(leftstring, ".STRINGZ ", strlen(".STRINGZ ")) == 0){//.STRINGZ伪操作
            leftstring = strchr(leftstring, ' ') + 2;//到达string
            for(int j = 0; leftstring[j] != '\0'; j++){
                number = leftstring[j];
                if(number == '"'){
                    strcpy(output[i - start + count + j], "0000000000000000");//结束标志
                    count++;
                    break;
                }for(int k = 0; k < 16; k++){
                    left = number % 2;
                    if(left == 0){
                        output[i - start + count + j][15 - k] = '0';
                    }else{
                        output[i - start + count + j][15 - k] = '1';
                    }number = number / 2;
                }output[i - start + count + j][16] = '\0';
                tempt++;
                *num = *num + 1;
            }count += (tempt - 1);
            number = 0;
            tempt = 0;
            continue;
        }translate_instruction(lines[i], output[i - start + count], locate, table, i + count + startsite - start);
    }
}

void translate_instruction(char *instruction, char *machine_code, int* locate, char table[][MAX_LINE_LENGTH], int pc)//pc指向下一条指令的地址
{
    //printf("%x %s\n", pc, instruction);
    char* leftstring;
    int i;
    if(islabel(instruction)){//如果有label，跳过label
        leftstring = strchr(instruction, ' ') + 1;
    }else{
        leftstring = instruction;//保证leftstring存在且一定跳过label
    }for(i = 0; leftstring[i] == ' '; i++);
    leftstring = &leftstring[i];
    if(strncmp(leftstring, "ADD ", strlen("ADD ")) == 0){//指令为ADD的情况
        strcpy(machine_code, "0001");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//DR
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//SR1
        leftstring = strchr(leftstring, ' ') + 1;
        if(leftstring[0] == 'R'){//SR2
            strcat(machine_code, "000");
            strcat(machine_code, get_register(leftstring[1]));
            return ;
        }else{
            strcat(machine_code, "1");
            char* left = get5(getreal(leftstring));
            strcat(machine_code, left);
            free(left);
            return ;
        }
    }else if(strncmp(leftstring, "AND ", strlen("AND ")) == 0){//AND指令
        strcpy(machine_code, "0101");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//DR
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//SR1
        leftstring = strchr(leftstring, ' ') + 1;
        if(leftstring[0] == 'R'){//SR2
            strcat(machine_code, "000");
            strcat(machine_code, get_register(leftstring[1]));
            return ;
        }else{
            strcat(machine_code, "1");
            char* left = get5(getreal(leftstring));
            strcat(machine_code, left);
            free(left);
            return ;
        }
    }else if(strncmp(leftstring, "BR ", strlen("BR ")) == 0 || strncmp(leftstring, "BRN ", strlen("BRN ")) == 0 || strncmp(leftstring, "BRZ ", strlen("BRZ ")) == 0 ||strncmp(leftstring, "BRP ", strlen("BRP ")) == 0 || strncmp(leftstring, "BRZP ", strlen("BRZP ")) == 0 || strncmp(leftstring, "BRNP ", strlen("BRNP ")) == 0 || strncmp(leftstring, "BRNZ ", strlen("BRNZ ")) == 0 || strncmp(leftstring, "BRNZP ", strlen("BRNZP ")) == 0){//BR指令
        strcpy(machine_code, "0000");
        leftstring = strchr(leftstring, 'R') + 1;
        if(leftstring[0] == 'n' || leftstring[0] == 'N'){//N是否被判断
            strcat(machine_code, "1");
            leftstring = leftstring + 1;
        }else{
            strcat(machine_code, "0");
        }if(leftstring[0] == 'z' || leftstring[0] == 'Z'){//Z是否被判断
            strcat(machine_code, "1");
            leftstring = leftstring + 1;
        }else{
            strcat(machine_code, "0");
        }if(leftstring[0] == 'p' || leftstring[0] == 'P'){//判断P是否被判断
            strcat(machine_code, "1");
            leftstring = leftstring + 1;
        }else{
            strcat(machine_code, "0");
        }if(strncmp(machine_code, "0000000", 7) == 0){//对于BR情况的特判
            strcpy(machine_code, "0000111");
        }leftstring++;
        char* dis = getpcoff9(pc, leftstring, table, locate);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "JMP ", strlen("JMP ")) == 0){//JMP指令
        strcpy(machine_code, "1100000");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));
        strcat(machine_code, "000000");
        return ;
    }else if(strncmp(leftstring, "RET ", strlen("RET ")) == 0){//RET指令
        strcpy(machine_code, "1100000111000000");
        return ;
    }else if(strncmp(leftstring, "JSRR ", strlen("JSRR ")) == 0){//先判断JSRR，以防和JSR弄混
        strcpy(machine_code, "0100000");
        leftstring = strchr(leftstring, ' ') + 2;
        strcat(machine_code, get_register(leftstring[0]));
        strcat(machine_code, "000000");
        return ;
    }else if(strncmp(leftstring, "JSR ", strlen("JSR ")) == 0){//JSR指令
        strcpy(machine_code, "01001");
        leftstring = strchr(leftstring, ' ') + 1;//此时指向label部分
        char* dis = getpcoff11(pc, leftstring, table, locate);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "LDI ", strlen("LDI ")) == 0){//LDI指令
        strcpy(machine_code, "1010");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));
        leftstring = strchr(leftstring, ' ') + 1;//指向label
        char* dis = getpcoff9(pc, leftstring, table, locate);
        //printf("%s", leftstring);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "LDR ", strlen("LDR ")) == 0){//LDR指令
        strcpy(machine_code, "0110");
        leftstring = strchr(leftstring, ' ') + 2;
        strcat(machine_code, get_register(leftstring[0]));//DR
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//BaseR
        leftstring = strchr(leftstring, ' ') + 1;//offset6
        char* dis = getoff6(leftstring);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "LD ", strlen("LD ")) == 0){//LD指令
        strcpy(machine_code, "0010");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));
        leftstring = strchr(leftstring, ' ') + 1;//指向label
        char* dis = getpcoff9(pc, leftstring, table, locate);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "LEA ", strlen("LEA ")) == 0){//LEA指令
        strcpy(machine_code, "1110");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//DR
        leftstring = strchr(leftstring, ' ') + 1;//label
        char* dis = getpcoff9(pc, leftstring, table, locate);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "NOT ", strlen("NOT ")) == 0){//NOT指令
        strcpy(machine_code, "1001");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//DR
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//SR
        strcat(machine_code, "111111");
        return ;
    }else if(strncmp(leftstring, "RTI ", strlen("RTI ")) == 0){//RTI指令
        strcat(machine_code, "1000");
        strcat(machine_code, "000000000000");
        return ;
    }else if(strncmp(leftstring, "STI ", strlen("STI ")) == 0){//STI指令
        strcat(machine_code, "1011");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//DR
        leftstring = strchr(leftstring, ' ') + 1;
        char* dis = getpcoff9(pc, leftstring, table, locate);//
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "STR ", strlen("STR ")) == 0){//STR指令
        strcat(machine_code, "0111");
        leftstring = strchr(leftstring, ' ') + 2;//SR
        strcat(machine_code, get_register(leftstring[0]));
        leftstring = strchr(leftstring, 'R') + 1;//BaseR
        strcat(machine_code, get_register(leftstring[0]));
        leftstring = strchr(leftstring, ' ') + 1;//offset6
        char* dis =  getoff6(leftstring);
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "ST ", strlen("ST ")) == 0){//ST指令
        strcpy(machine_code, "0011");
        leftstring = strchr(leftstring, 'R') + 1;
        strcat(machine_code, get_register(leftstring[0]));//SR
        leftstring = strchr(leftstring, ' ') + 1;
        char* dis = getpcoff9(pc, leftstring, table, locate);//pcoffset9
        strcat(machine_code, dis);
        free(dis);
        return ;
    }else if(strncmp(leftstring, "TRAP ", strlen("TRAP ")) == 0){//TRAP指令
        strcpy(machine_code, "11110000");
        leftstring = strchr(leftstring, 'x');
        char* binary;
        binary = getbinary(leftstring, 2);
        strcat(machine_code, binary);
        free(binary);
        return ;
    }else if(strncmp(leftstring, ".ORIG ", strlen(".ORIG ")) == 0){//.ORIG伪操作
        leftstring = strchr(leftstring, ' ') + 1;
        int startsite = 0, left;
        startsite = getreal(leftstring);
        for(int i = 0; i < 16; i++){
            left = startsite % 2;
            if(left == 0){
                machine_code[15 - i] = '0';
            }else{
                machine_code[15 - i] = '1';
            }startsite = startsite / 2;
        }machine_code[16] = '\0';
        return ;
    }else if(strncmp(leftstring, ".END ", strlen(".END ")) == 0){//.END伪操作
        machine_code[0] = '\0';
        return ;
    }else if(strncmp(leftstring, ".FILL ", strlen(".FILL ")) == 0){//.FILL伪操作
        leftstring = strchr(leftstring, ' ') + 1;
        char* dis;
        dis = getbinary(leftstring, 4);
        strcat(machine_code, dis);
        return ;
    }
}

int findsite(char lines[][MAX_LINE_LENGTH], int num_lines, char* string){//在一个字符串数组中寻找第一个前缀匹配的字符串的位置
    for(int i = 0; i < num_lines; i++){
        if(strncmp(lines[i], string, strlen(string)) == 0){
            return i;
        }
    }printf("Didn't find string \"%s\" \n", string);
    return -1;
}

int islabel(char* string){//判断当前语句中是否有label
    for(int i = 0; i < 29; i++){
        if(strncmp(instruction_set[i], string, (int)strlen(instruction_set[i])) == 0){
            return 0;
        }
    }return 1;
}

char* get_register(char number){
    switch(number){
        case '0':
        return "000";
        case '1':
        return "001";
        case '2':
        return "010";
        case '3':
        return "011";
        case '4':
        return "100";
        case '5':
        return "101";
        case '6':
        return "110";
        case '7':
        return "111";
        default:
        return "error";
    }
}

char* get5(int number){
    char* result;
    int left;
    result = (char*)malloc(sizeof(char) * 6);
    int sign = 0;
    if(number < 0){
        sign = 1;
        number = 16 + number;
    }for(int i = 0; i < 4; i++){
        left = number % 2;
        if(left == 0){
            result[4 - i] = '0';
        }else{
            result[4 - i] = '1';
        }number = number / 2;
    }if(sign == 0){
        result[0] = '0';
    }else{
        result[0] = '1';
    }result[5] = '\0';//补上字符串结尾的标志
    return result;
}

char* getpcoff9(int pc, char* label, char table[][MAX_LINE_LENGTH], int* locate){
    char* dis = (char*)malloc(sizeof(char) * 10);
    int left;
    /*for(int i = 0; i < MAX_LINE_LENGTH; i++){
        printf("%s\n", table[i]);
    }*/
    for(int i = 0; i < 100; i++){
        if(strncmp(label, table[i], strlen(label)) == 0 && table[i][strlen(label)] ==' ' && label[strlen(label)] == '\0'){//找到了label
            int des = locate[i];
            int distance = des - pc, sign = 0;
            dis[0] = '0';
            if(distance < 0){
                sign = 1;
                dis[0] = '1';
                distance = 256 + distance;//寻找对应的二进制补码对应的数
            }//printf("%d %d\n", pc, distance);
            for(int i = 0; i < 8; i++){
                left = distance % 2;
                if(left == 0){
                    dis[8 - i] = '0';
                }else{
                    dis[8 - i] = '1';
                }distance = distance / 2;
            }dis[9] = '\0';
            return dis;
        }
    }
}

char* getpcoff11(int pc, char* label, char table[][MAX_LINE_LENGTH], int* locate){
    char* dis = (char*)malloc(sizeof(char) * 12);
    int left;
    for(int i = 0; i < 100; i++){
        if(strncmp(label, table[i], strlen(label)) == 0 && table[i][strlen(label)] ==' '){//找到label
            int des = locate[i];
            int distance = des - pc, sign = 0;
            dis[0] = '0';
            if(distance < 0){
                sign = 1;
                dis[0] = '1';
                distance = 1024 + distance;//求的对应的补码对应的数值
            }for(int i = 0; i < 10; i++){
                left = distance % 2;
                if(left == 0){
                    dis[10 - i] = '0';
                }else{
                    dis[10 - i] = '1';
                }distance = distance / 2;
            }dis[11] = '\0';//字符串结束标志
            return dis;
        }
    }
}

char* getoff6(char* number){
    int sign = 0, wait = 0, start = 1, left;//start对应的是数字部分的开始地址
    char* dis = (char*)malloc(sizeof(char) * 7);
    dis[0] = '0';
    wait = getreal(number);
    if(wait < 0){
        wait = 32 + wait;
        dis[0] ='1';
    }for(int i = 0; i < 5; i++){//书写0，1
            left = wait % 2;
            if(left == 0){
                dis[5 - i] = '0';
            }else{
                dis[5 - i] = '1';
            }wait = wait / 2;
        }dis[6] = '\0';
    return dis;
}

char* getbinary(char* number, int length){
    int real, left, bits;
    real = getreal(number);
    bits = 4 * length;
    char* dis = (char*)malloc(sizeof(char) * (bits + 1));
    for(int i = 0; i < bits; i++){
        left = real % 2;
        if(left == 0){
            dis[bits - 1 - i] = '0';
        }else{
            dis[bits - 1 - i] = '1';
        }real = real / 2;
    }dis[bits] = '\0';
    return dis;
}

int getreal(char* string){
    int result = 0, sign = 0, i = 0;
    if(string[0] == 'x' || string[0] == 'X'){//16进制
        i++;
        if(string[i] == '-'){
            sign = 1;
            i++;
        }for(; string[i] != '\0'; i++){
            result *= 16;
            if(string[i] >= '0' && string[i] <= '9'){
                result = result + string[i] - '0';
            }else if(string[i] >= 'a' && string[i] <= 'z'){
                result = result + string[i] - 'a' + 10;
            }else{
                result = result + string[i] - 'A' + 10;
            }
        }
    }else{//10进制
        i++;
        if(string[i] == '-'){
            i++;
            sign = 1;
        }for(; string[i] != '\0'; i++){
            result *= 10;
            result = result + string[i] - '0';
        }
    }
    if(sign == 1){
        result = -result;
    }return result;
}
