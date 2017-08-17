#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtalaris.h"

int main(int argc, char **argv){
    if(argc < 3) {fprintf(stderr,"Usage: %s str1 str2\n",argv[0]); return 1;}
    int l = (int)(strlen(argv[1]) + strlen(argv[2]));
    //printf("l = %d\n",l);
    for(int i = l; i > 0; i --){
        printf("argv[1] = %s, argv[2] = %s\n",argv[1],argv[2]);
        printf("maxlen = %d\n",i);
        char temp[l];
        temp[0] = '\0';
        strcpy(temp, argv[1]);
        printf("temp = %s, argv[2] = %s\n",temp, argv[2]);
        insert_at_end(temp,argv[2],i);
        printf("for i = %d, temp = "C_R"%s"C_W", argv[1] = %s, argv[2] = %s\n\n",i, temp, argv[1],argv[2]);
    }
    return 0;
}
