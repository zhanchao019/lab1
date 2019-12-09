/*这个程序需要在更改文件读取路径之后才可以使用，否则会出现源程序文件读取错误*/
#include <bits/stdc++.h>

using namespace std;
#define RESERVEWORDNUM 33
#define IDENTIFY_TABLE_LENGTH (10000)
#define NUMBER_TABLE_LENGTH (10000)


struct ID{
    char id[50];
    char type[50];
    int pos;
};

struct NUM{
    char num[50];
    char type[50];
};


static char reserveWord[RESERVEWORDNUM][20] = {
        "auto", "break", "case", "char", "const", "continue",
        "default", "do", "double", "else", "enum", "extern",
        "float", "for", "goto", "if", "int", "long",
        "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void",
        "volatile", "while","main"};
//界符运算符
static char operatorOrSeg[36][10] = {
        "+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==",
        "!=", ";", "(", ")", "^", ",", "\"", "\'", "#", "&",
        "&&", "|", "||", "%", "~", "<<", ">>", "[", "]", "{",
        "}", "\\", ".", "\?", ":", "!"};

static ID Identify_List[IDENTIFY_TABLE_LENGTH] ; //标识符表
static NUM Number_List[NUMBER_TABLE_LENGTH];

bool check_in_single_seg(char ch){
    if( ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ';' || ch == '(' || ch == ')' || ch == '^' ||
        ch == ',' || ch == '\"' || ch == '\'' || ch == '~' || ch == '#' || ch == '%' || ch == '[' || ch == ']' ||
        ch == '{' || ch == '}' || ch == '\\' || ch == '.' || ch == '\?' || ch == ':')
        return true;
        else return false;

}
//寻找保留字
int searchReserve(char reserveWord[][20], char s[]) {
    for (int i = 0; i < RESERVEWORDNUM; i++) {
        if (strcmp(reserveWord[i], s) == 0) {
            return i + 1; //返回
        }
    }
    return -1; //否则返回-1，代表查找不成功，即为标识符,而不是保留字
}


bool IsLetter(char letter) {
    if (letter >= 'a' && letter <= 'z' || letter >= 'A' && letter <= 'Z' || letter == '_') {
        return true;
    } else {
        return false;
    }
}

/*是否为保留字*/
bool IsDigit(char digit) {
    if (digit >= '0' && digit <= '9') {
        return true;
    } else {
        return false;
    }
}

/*pre processing*/
void preprocessing(char *r, int ptrProject) {
    char tempString[1000000];
    int count = 0;
    //cout << ptrProject << "???" << endl;
    for (int i = 0; i <= ptrProject; i++) {
        if (r[i] == '/' && r[i + 1] == '/') { //若为单行注释“//”,则去除注释后面的东西，直至遇到回车换行
            while (r[i] != '\n') {
                i++; //向后扫描
            }
        }
        if (r[i] == '/' && r[i + 1] == '*') { //若为多行注释“/* 。。。*/”则去除该内容
            i += 2;
            while (r[i] != '*' || r[i + 1] != '/') {
                i++; //继续扫描
                if (r[i] == '$') {
                    printf("没有找到 */\n");
                    exit(0);
                }
            }
            i += 2; //跨过“*/”
        }
        if (r[i] != '\n' && r[i] != '\t' && r[i] != '\v' && r[i] != '\r') { //若出现无用字符，则过滤；否则加载
            tempString[count++] = r[i];
           // cout << count << endl;
        }
    }
    tempString[count] = '\0';
    strcpy(r, tempString); //产生净化之后的源程序

    cout << "\n源程序为:" << endl;
    //cout << count << endl;
    // cout<<sourceCode<<endl;
    for (int i = 0; i < count; ++i) {
      //  cout << tempString[i];
    }
    cout << "***********end of pre************" << endl;
}
/*endp of pre processing */



/*lex analysis*/
void Scanner(int &syn, char sourceCode[], char token[], int &ptrProject)//一定是引用，否则会产生数据不同步
{
    //根据DFA的状态转换图设计
    int i, count = 0; //count用来做token[]的指示器，收集有用字符
    char ch;          //作为判断使用
    ch = sourceCode[ptrProject];
    while (ch == ' ') { //过滤空格，防止程序因识别不了空格而结束
        ptrProject++;
        ch = sourceCode[ptrProject];
    }

    //每次收集前先清零
    memset(token, 0, sizeof(token));


    if (IsLetter(sourceCode[ptrProject])) { //开头为字母
        token[count++] = sourceCode[ptrProject]; //收集
        ptrProject++;                            //下移
        while (IsLetter(sourceCode[ptrProject]) || IsDigit(sourceCode[ptrProject])) {//后跟字母或数字
            token[count++] = sourceCode[ptrProject]; //收集
            ptrProject++;                            //下移
        }                                            //多读了一个字符既是下次将要开始的指针位置
        token[count] = '\0';
        syn = searchReserve(reserveWord, token); //查表找到种别码
        if (syn == -1) {              //若不是保留字则是标识符
            syn = 100; //标识符种别码--变量
        }
        return;
    } else if (IsDigit(sourceCode[ptrProject])) { //首字符为数字
        while (IsDigit(sourceCode[ptrProject])) {//后跟数字
            token[count++] = sourceCode[ptrProject]; //收集
            ptrProject++;
        } //多读了一个字符既是下次将要开始的指针位置，一定要加上！！！！！！！！
        token[count] = '\0';
        syn = 99; //常数种别码
    } else if (check_in_single_seg(ch)) { //这个是单一运算符形成的单字符串
        token[0] = sourceCode[ptrProject];
        token[1] = '\0'; //形成单字符串

        for (i = 0; i < 36; i++) { //查运算符界符表
            if (strcmp(token, operatorOrSeg[i]) == 0) {
                syn = 33 + i; //获得种别码，使用了一点技巧，使之呈线性映射
                break;        //查到即推出
            }
        }
        ptrProject++; //指针下移，为下一扫描做准备
        return;
    }
    //下面开始进行对比多位运算符和分隔符
    else if (sourceCode[ptrProject] == '<') {                 //<,<=,<<
        ptrProject++; //后移
        if (sourceCode[ptrProject] == '=') {
            syn = 38;
        } else if (sourceCode[ptrProject] == '<') { //左移1位
            ptrProject--;
            syn = 58;
        } else {
            ptrProject--;
            syn = 37;
        }
        ptrProject++; //指针下移
        return;
    } else if (sourceCode[ptrProject] == '>') { //>,>=,>>
        ptrProject++;
        if (sourceCode[ptrProject] == '=') {
            syn = 40;
        } else if (sourceCode[ptrProject] == '>') {
            syn = 59;
        } else {
            ptrProject--;
            syn = 39;
        }
        ptrProject++;
        return;
    } else if (sourceCode[ptrProject] == '=') { //=
        ptrProject++;
        if (sourceCode[ptrProject] == '=') {//==
            syn = 42;
        } else {
            ptrProject--;
            syn = 41;
        }
        ptrProject++;
        return;
    } else if (sourceCode[ptrProject] == '!') { //!,!=
        ptrProject++;
        if (sourceCode[ptrProject] == '=') {
            syn = 43;
        } else {
            syn = 68;
            ptrProject--;
        }
        ptrProject++;
        return;
    } else if (sourceCode[ptrProject] == '&') { //&,&&
        ptrProject++;
        if (sourceCode[ptrProject] == '&') {
            syn = 53;
        } else {
            ptrProject--;
            syn = 52;
        }
        ptrProject++;
        return;
    } else if (sourceCode[ptrProject] == '|') { //|,||
        ptrProject++;
        if (sourceCode[ptrProject] == '|') {
            syn = 55;
        } else {
            ptrProject--;
            syn = 54;
        }
        ptrProject++;
        return;
    } else if (sourceCode[ptrProject] == '$') {            //结束符  EOF
        syn = 0; //种别码为0
    } else if (sourceCode[ptrProject] == '\0' ){
        syn = 0; //种别码为0
        cout << "end of program" << endl;
        return;
    } else { //rise ERROR
        printf("error：there is no exist %c \n", ch);
        //  goto end_program;
        exit(0);
    }
}

void read(char *sourceCode, int &ptrProject, FILE *fp) {
    //load soursecode into memory
    sourceCode[ptrProject] = fgetc(fp);
    while (sourceCode[ptrProject] != EOF) { //将源程序读入sourceCode
        ptrProject++;
      //  cout << ptrProject << endl;
        sourceCode[ptrProject] = fgetc(fp);
    }
    sourceCode[ptrProject] = '\0';

    fclose(fp);
    cout << "\n源程序为:" << endl;
    // cout<<sourceCode<<endl;
    for (int i = 0; i < ptrProject; ++i) {
        cout << sourceCode[i];
    }
    cout << "***********end of read************" << endl;
    //end of read
}

void demo() {
    //打开一个文件，读取其中的源程序
    char sourceCode[1000000];
    char token[20] = {0};
    int syn = -1, i;    //初始化
    int ptrProject = 0; //源程序指针
    FILE *fp, *fp1,*fpid,*fpuint,*fpout;

    if ((fp = fopen("C:\\Users\\13431\\Desktop\\CLionProjects\\lab\\demo.txt", "r")) == NULL) { //打开源程序
        cout << "can't open demo";
        exit(0);
    }
    //read in data
    cout << "read end" << endl;
    read(sourceCode, ptrProject, fp);

    //对源程序进行过滤
    preprocessing(sourceCode, ptrProject);
    // cout << "过滤之后的程序:" << endl;
    //cout << sourceCode<< endl;
    ptrProject = 0; //从头开始读

    if ((fp1 = fopen("C:\\Users\\13431\\Desktop\\CLionProjects\\lab\\compile.txt", "w+")) == NULL) { //打开源程序
        cout << "can't open compile";
        exit(0);
    }

    if ((fpout = fopen("C:\\Users\\13431\\Desktop\\CLionProjects\\lab\\out.txt", "w+")) == NULL) { //打开源程序
        cout << "can't open compile";
        exit(0);
    }

    if ((fpid = fopen("C:\\Users\\13431\\Desktop\\CLionProjects\\lab\\id_list.txt", "w+")) == NULL) { //打开源程序
        cout << "can't open fpid";
        exit(0);
    }
    if ((fpuint = fopen("C:\\Users\\13431\\Desktop\\CLionProjects\\lab\\uint.txt", "w+")) == NULL) { //打开源程序
        cout << "can't open fpuint";
        exit(0);
    }

    while (syn != 0) {
        //启动扫描
        Scanner(syn, sourceCode, token, ptrProject);
        if (syn == 100) { //标识符
            for (i = 0; i < IDENTIFY_TABLE_LENGTH; i++) { //插入标识符表中
                if (strcmp(Identify_List[i].id, token) == 0) { //已在表中
                    break;
                }
                if (strcmp(Identify_List[i].id, "") == 0) { //查找空间or 找到了空的位置，结束
                    //新增标识符表项
                    strcpy(Identify_List[i].id, token);
                    Identify_List[i].type[0]='\0';
                    Identify_List[i].pos=0;
                    /*
                    printf("(id  ,%s)\n", token);*/
                    fprintf(fpid, "%s\n", token);
                    fprintf(fpout, "(id   ,%s)\n", token);
                    break;
                }
            }

        } else if (syn >= 1 && syn <= 32) { //保留字
           /* printf("(%s   ,  )\n", reserveWord[syn - 1]);
            fprintf(fp1, "(%s   ,  )\n", reserveWord[syn - 1]);*/
            fprintf(fpout, "(res   , %s )\n", reserveWord[syn - 1]);
        } else if (syn == 99) { //const 常数

            for (i = 0; i < NUMBER_TABLE_LENGTH; i++) { //插入常数表中
                if (strcmp(Number_List[i].num, token) == 0) { //已在表中
                    break;
                }
                if (strcmp(Number_List[i].num, "") == 0) { //查找空间or 找到了空的位置，结束
                    //新增常数表项
                    strcpy(Number_List[i].num, token);
                    Number_List[i].type[0]='\0';


                    //printf("(id  ,%s)\n", token);
                    fprintf(fpuint, "%s\n", token);
                  //  fprintf(fpout, "(id   ,%s)\n", token);
                    break;
                }
            }


            printf("(num   ,   %s)\n", token);

            fprintf(fpout, "(num   ,   %s)\n", token);
        } else if (syn >= 33 && syn <= 68) {
            printf("(  %s ,   )\n", operatorOrSeg[syn - 33]);
            fprintf(fp1, "(  %s ,   )\n", operatorOrSeg[syn - 33]);
            fprintf(fpout, "(%s   ,   )\n", operatorOrSeg[syn - 33]);
        }
    }

    fclose(fp1);
    fclose(fp);
    fclose(fpout);
    fclose(fpid);
    fclose(fpuint);
}

int main() {
    start_program:
        demo();

    end_program:
    //cout << "error occor exit!!" << endl;
    return 0;
}