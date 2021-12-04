#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int year,month,days,start;
    int** page;
} PAGE;

PAGE* pages[211][12];
int days_of_month[]={31,28,31,30,31,30,31,31,30,31,30,31};

int IsLeap(int year) {
    return ((year%4==0)&&(year%100!=0))||(year%400==0);
}

int GetDays(int year, int month) {
    if (IsLeap(year)) {
        days_of_month[1]++;
        return days_of_month[month-1];
    } else {
        return days_of_month[month-1];
    }
}

int GetDayOfWeek(int year, int month, int day) {
    int d=0;
    for (int i=1970;i<year;i++) d+=IsLeap(i)?366:365;
    for (int i=1;i<month;i++) d+=GetDays(year,i);
    d+=(--day);
    return ((d%7+4)%7);
}

void MakePage(PAGE* p) {
    p->page=(int**)malloc(sizeof(int*)*6);
    for (int i=0;i<7;i++) (p->page)[i]=malloc(sizeof(int)*7);

    int year_before,month_before,day_before;
    if (p->month==1) {
        year_before=p->year-1;
        month_before=12;
    } else {
        year_before=p->year;
        month_before=p->month-1;
    }
    day_before=GetDays(year_before,month_before);
    for (int i=GetDayOfWeek(year_before,month_before,day_before);i>=0;i--) {
        (p->page)[0][i]=day_before;
        day_before--;
    }
    for (int i=p->start,j=1;i<p->days+p->start;i++,j++)(p->page)[i/7][i%7]=j;
    for (int i=p->days+p->start,j=1;i<42;i++,j++) (p->page)[i/7][i%7]=j;
}

void FreePage(int** p) {
    for (int i=0;i<6;i++) free(p[i]);
    free(p);
}

void PrintCalender(int year, int month) {
    char title[10];
    sprintf(title,"%d-%d",year,month);
    printf("\n%21s\n\n",title);
    printf("%5s%5s%5s%5s%5s%5s%5s\n","Sun","Mon","Tue","Wed","Thu","Fri","Sat");
    pages[year-1970][month-1]->year=year;
    pages[year-1970][month-1]->month=month;
    pages[year-1970][month-1]->days=GetDays(year,month);
    pages[year-1970][month-1]->start=GetDayOfWeek(year,month,1);
    MakePage(pages[year-1970][month-1]);
    for (int i=0;i<6;i++) {
        for (int j=0;j<7;j++) printf("%5d",(pages[year-1970][month-1]->page)[i][j]);
        printf("\n");
    } printf("\n");
    FreePage(pages[year-1970][month-1]->page);
}
// void AddSchedule();
// void RemoveSchedule();
// void Save();
// void Load();

int main() {
    for (int i=0;i<sizeof(pages)/sizeof(pages[0]);i++) {
        for (int j=0;j<sizeof(pages[0])/sizeof(PAGE*);j++) pages[i][j]=malloc(sizeof(PAGE));
    }
    char input[10]={0,}; //* LUT or by user input?
    char* commands[]={"print","add","remove","save","load"};
    do {
        printf("input command ==> ");
        if (scanf("%s",input)!=EOF) {
            if (!strcmp(input,commands[0])) {
                int year,month;
                if (scanf("%d %d",&year,&month)!=EOF) {
                    PrintCalender(year,month);
                } else {
                    rewind(stdin);
                }
            }
        }
    } while (strcmp(input,"exit"));
    for (int i=0;i<sizeof(pages)/sizeof(pages[0]);i++) {
        for (int j=0;j<sizeof(pages[0])/sizeof(PAGE*);j++) free(pages[i][j]);
    }
    return 0;
}