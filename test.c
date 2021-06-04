#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(){
    char line[20] = "mai ta di choi190";
    for(int i=0;strlen(line)>i;i++){
        if(atoi(line+i)||line[i]-'0'==0) printf("vi pham\n");
    }
}