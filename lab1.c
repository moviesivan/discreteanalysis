#include <stdio.h>
#include <stdlib.h>

int NUM;

typedef struct TVector_ { 
    short day, month, year;
    char d[3], m[3], y[5]; 
    unsigned long long val;
    struct TVector_ *next; 
} TVector;

short Str2short(char a[]){
    short n = 0;
    for (int c = 0; a[c] != '\0'; c++) {
        n = n * 10 + a[c] - '0';
    }
    return n;
}

void Strcpy(char a[], char b[]){
    for (int c = 0; b[c] != '\0'; c++) {
        a[c] = b[c];
    }
}

TVector *Insert (TVector *v, char d[3], char m[3], char y[5], unsigned long long val) {
    TVector *new = malloc(sizeof(TVector));
    new->val = val;
    Strcpy(new->d, d);
    Strcpy(new->m, m);
    Strcpy(new->y, y);
    new->day = Str2short(d);
    new->month = Str2short(m);
    new->year = Str2short(y);
    new->next = NULL;
    v->next = new;
    NUM+=1;
    v = v->next;
    return v;
}

void PrintVector(TVector *v){
    TVector *temp;
    temp = v;
    for (int i=1; i<=NUM; i++) {
        printf("%s.%s.%s\t%llu\n",temp->d,temp->m,temp->y,temp->val);
        temp = temp->next;
    }
}

TVector *RadixInternal(TVector *v, short k, short type) {
    int i, j, d, m=1;
    TVector *temp, *out, *head[10], *tail[10];
    out=v;

    for (j=1; j<=k; j++) { 
        for (i=0; i<=9; i++){
            head[i] = (tail[i]=NULL);
        }
        
        while ( v != NULL ) {
            switch (type) {
                case 1 : 
                    d = ((short)(v->day/m))%10;
                    break;
                case 2 :
                    d = ((short)(v->month/m))%10;
                    break;
                case 3 : 
                    d = ((short)(v->year/m))%10;
                    break;
            }
            if ( head[d]==NULL ) { 
                head[d] = v;
                tail[d] = v;
                temp = tail[d];
                v = v-> next;
                temp->next = NULL;
            }
            else {
                temp = tail[d];
                temp->next = v;
                tail[d] = temp->next;
                temp = temp->next;
                v = v->next;
                temp->next = NULL;
            }
        }
        for (i=0; i<=9; i++)
            if ( head[i] != NULL ) break;
        v = head[i];
        temp = tail[i];
        for (d=i+1; d<=9; d++) {
            if ( head[d] != NULL) { 
                temp->next = head[d];
                temp = tail[d];
            }
        }
    out = v;
    m*=10;
    }
    return out;
}

TVector *RadixSort(TVector *v) {
    v = RadixInternal(v, 2, 1);
    v = RadixInternal(v, 2, 2);
    v = RadixInternal(v, 4, 3);
    return v;
}

void ReadVector(TVector *v) {
    unsigned long long val;
    char d[3], m[3], y[5]; 
    TVector *temp;
    temp = v;
    scanf("%*[ \n\t]");
    if (scanf("%[0-9].%[0-9].%[0-9] %llu", d, m, y, &val) == 4) {
        temp->val = val;
        Strcpy(temp->d, d);
        Strcpy(temp->m, m);
        Strcpy(temp->y, y);
        temp->day = Str2short(d);
        temp->month = Str2short(m);
        temp->year = Str2short(y);
        temp->next = NULL;
        NUM+=1;

    }
    while (1) {
        scanf("%*[ \n\t]");
        if (scanf("%[0-9].%[0-9].%[0-9] %llu", d, m, y, &val) != 4){
            break;
        }
        temp = Insert(temp, d, m, y, val);

    }
}


int main(int argc, char **argv) {
    TVector *start = malloc(sizeof(TVector));
    NUM=0;
    ReadVector(start);
    if (NUM>1){
        start = RadixSort(start);
    }
    PrintVector(start);
    return 0;
}