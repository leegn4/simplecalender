#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int year,month,days,start;
    int** page;
} PAGE;

PAGE* pages[131][12];
int days_of_month[]={31,28,31,30,31,30,31,31,30,31,30,31};

typedef struct _SCHEHDULE{
    int type;
    char content[1000];
    struct _SCHEDULE* next;
} SCHEDULE;

SCHEDULE* schedules[131][12]={NULL,};

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
    for (int i=0;i<7;i++) (p->page)[i]=(int*)malloc(sizeof(int)*7);

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

    printf("<Private>\n");
    for (int day=1;day<=GetDays(year,month);day++) {
        if (schedules[year-1970][month-1][day-1].next!=NULL) {
            printf("%d-%d\n",year,month);
            SCHEDULE* curr=&schedules[year-1970][month-1][day-1];
            while (curr->next!=NULL) {
                curr=curr->next;
                if (curr->type==0) printf("%s\n",curr->content);
            }
        }
    }
    printf("<Public>\n");
    for (int day=1;day<=GetDays(year,month);day++) {
        if (schedules[year-1970][month-1][day-1].next!=NULL) {
            printf("%d-%d\n",year,month);
            SCHEDULE* curr=&schedules[year-1970][month-1][day-1];
            while (curr->next!=NULL) {
                curr=curr->next;
                if (curr->type==1) printf("%s\n",curr->content);
            }
        }
    }
}

void AddSchedule(int year, int month, int day, int type, char* content) {
    SCHEDULE* curr=&schedules[year-1970][month-1][day-1];
    while (curr!=NULL) {
        curr=curr->next;
    }
    SCHEDULE* new=(SCHEDULE*)malloc(sizeof(SCHEDULE));
    curr->next=new;
    new->type=type;
    new->next=NULL;
    strcpy(new->content,content);   
}

void RemoveSchedule(int year, int month, int day) {
    SCHEDULE *curr=&schedules[year-1970][month-1][day-1],*prev=NULL;
    if (curr->next==NULL) printf("No schedule to remove\n");
    return;
    while (curr!=NULL) {
        prev=curr;
        curr=curr->next;
    }
    prev->next=NULL;
    free(curr);
}
// void Save();
// void Load();

int main() {
    for (int i=0;i<sizeof(pages)/sizeof(pages[0]);i++) {
        for (int j=0;j<sizeof(pages[0])/sizeof(PAGE*);j++) pages[i][j]=(PAGE*)malloc(sizeof(PAGE));
    }
    for (int i=0;i<sizeof(schedules)/sizeof(schedules[0]);i++) {
        for (int j=0;j<sizeof(schedules[0])/sizeof(SCHEDULE*);j++) {
            schedules[i][j]=(SCHEDULE*)malloc(GetDays(i+1970,j+1)*sizeof(SCHEDULE));
            for (int d=0;d<GetDays(i+1970,j+1)-1;d++) schedules[i][j][d].next=NULL;
        }
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
            else if (!strcmp(input,commands[1])) {
                int year,month,day,type;
                char content[1000];
                scanf("%d %d %d %d %s",&year,&month,&day,&type,content);
                AddSchedule(year,month,day,type,content);
            }
        }
    } while (strcmp(input,"exit"));
    for (int i=0;i<sizeof(pages)/sizeof(pages[0]);i++) {
        for (int j=0;j<sizeof(pages[0])/sizeof(PAGE*);j++) free(pages[i][j]);
    }
    for (int i=0;i<sizeof(schedules)/sizeof(schedules[0]);i++) {
        for (int j=0;j<sizeof(schedules[0])/sizeof(SCHEDULE*);j++) {
            free(schedules[i][j]);
        }
    }
    return 0;
}
