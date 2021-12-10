#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int days_of_month[]={31,28,31,30,31,30,31,31,30,31,30,31};
int schedule_count=0; // 일정 개수

typedef struct {
    int year,month,days,start;
    int** page; // 출력할 때 사용할 2차원 배열
} PAGE; // 실제 달력과 같이 한 페이지 구조체

PAGE* pages[131][12]; // 년-월 별로 페이지 생성(구조체 포인터 배열)

typedef struct _SCHEHDULE{
    int type;
    char content[1000];
    struct _SCHEDULE* next;
} SCHEDULE; // 일정 연결 리스트

SCHEDULE* schedules[131][12][31]={NULL,}; // 2차원 배열의 각 요소에 2중 포인터 할당(달별로 일 수만큼) -> 3차원 구조체 포인터 배열처럼 사용해서 각 일마다 리스트의 head 포인터 할당 하려 했으나 오류 발생.

int IsLeap(int year) { // 윤년인지 확인
    return ((year%4==0)&&(year%100!=0))||(year%400==0);
}

int GetDays(int year, int month) { // 입력받은 달의 일 수 확인
    if (IsLeap(year)) {
        days_of_month[1]++; // 윤년이면 2월의 일 수 ++
        return days_of_month[month-1];
    } else {
        return days_of_month[month-1]; // 윤년이 아니면 그대로 리턴
    }
}

int IsValid(int year, int month, int day, int type) { // 입력값 검증
    int a=(1970<=year)&&(year<=2100); // 년
    int b=(1<=month)&&(month<=12); // 월
    int c=(1<=day)&&(day<=GetDays(year,month)); // 일
    int d=(type==0)||(type==1); // type
    return (a&b&c&d); // bitwise is faster
}

int GetDayOfWeek(int year, int month, int day) { // 입력받은 날짜의 요일 확인
    int d=0; // 1970-1-1 Thu 와의 일수 차이
    for (int i=1970;i<year;i++) d+=IsLeap(i)?366:365; // 윤년이면 +366, 평년이면 +365 - 입력받은 연도 직전까지
    for (int i=1;i<month;i++) d+=GetDays(year,i); // 입력받은 연도에는 입력받은 달 까지만 일수 계산
    d+=(--day); // 차이이므로 --
    return ((d%7+4)%7); // 1970-1-1 Thu와의 요일 차이 리턴
}

void MakePage(PAGE* p) { // print에 사용되는 한 페이지 생성
    p->page=(int**)malloc(sizeof(int*)*6); // 행 할당(6개)
    for (int i=0;i<7;i++) (p->page)[i]=(int*)malloc(sizeof(int)*7); // 열 할당(7개)

    int year_before,month_before,day_before; // 해당 페이지의 이전 연도, 이전 달, 이전 일
    if (p->month==1) { // 1월이면
        year_before=p->year-1; // 연도--
        month_before=12; // 전월은 12월
    } else { 
        year_before=p->year; // 1월이 아니면
        month_before=p->month-1; // 전월은 month-1월
    }
    day_before=GetDays(year_before,month_before); // 전월의 마지막 날은 그 달의 일수와 같음
    for (int i=GetDayOfWeek(year_before,month_before,day_before);i>=0;i--) { // 0행 거꾸로 전월의 날들로 채움
        (p->page)[0][i]=day_before;
        day_before--;
    }
    for (int i=p->start,j=1;i<p->days+p->start;i++,j++)(p->page)[i/7][i%7]=j; // 해당 월의 시작일~끝나는날까지 7로 나눈 몫=행, 나머지=열
    for (int i=p->days+p->start,j=1;i<42;i++,j++) (p->page)[i/7][i%7]=j; // 나머지를 다음달의 날들로 채움
}

void FreePage(int** p) { // 페이지 해제
    for (int i=0;i<6;i++) free(p[i]);
    free(p);
}

void ResetSchedule() { // 일정 초기화(load 파일 변경되어 데이터 변조 시)
    for (int i=0;i<131;i++) {
        for (int j=0;j<12;j++) {
            for (int k=0;k<31;k++) free(schedules[i][j][k]);
        }
    }
    for (int i=0;i<131;i++) {
        for (int j=0;j<12;j++) {
            for (int k=0;k<31;k++) {
                schedules[i][j][k]=(SCHEDULE*)malloc(sizeof(SCHEDULE));
                schedules[i][j][k]->next=NULL;
            }
        }
    }
}

void PrintCalender(int year, int month) { // print 기능
    char title[10];
    sprintf(title,"%d-%d",year,month);
    printf("\n%21s\n\n",title); // XXXX-XX 출력
    printf("%5s%5s%5s%5s%5s%5s%5s\n","Sun","Mon","Tue","Wed","Thu","Fri","Sat"); // 요일
    pages[year-1970][month-1]->year=year; // 초기값 설정(연도)
    pages[year-1970][month-1]->month=month; // 초기값 설정(달)
    pages[year-1970][month-1]->days=GetDays(year,month); // 초기값 설정(일수)
    pages[year-1970][month-1]->start=GetDayOfWeek(year,month,1); // 초기값 설정(시작 요일)
    MakePage(pages[year-1970][month-1]); // 페이지 생성, 초기화
    for (int i=0;i<6;i++) {
        for (int j=0;j<7;j++) printf("%5d",(pages[year-1970][month-1]->page)[i][j]); // 6x7 배열 출력
        printf("\n");
    } printf("\n");
    FreePage(pages[year-1970][month-1]->page); // 페이지 해제

    printf("<Private>\n"); // 일정 출력(private)
    for (int day=1;day<=GetDays(year,month);day++) { // 해당 달의 일수만큼 반복
        if (schedules[year-1970][month-1][day-1]->next!=NULL) { // 리스트가 비어있지 않을 때
            SCHEDULE* curr=schedules[year-1970][month-1][day-1];
            while (curr->next!=NULL) { // 탐색하면서 type=0이라면 출력
                curr=curr->next;
                if (curr->type==0) printf("%d: %s\n",day,curr->content);
            }
        }
    }
    printf("<Public>\n"); // 일정 출력(public)
    for (int day=1;day<=GetDays(year,month);day++) { // 해당 달의 일수만큼 반복
        if (schedules[year-1970][month-1][day-1]->next!=NULL) { // 리스트가 비어있지 않을 때
            SCHEDULE* curr=schedules[year-1970][month-1][day-1];
            while (curr->next!=NULL) { // 탐색하면서 type=1이라면 출력
                curr=curr->next;
                if (curr->type==1) printf("%d: %s\n",day,curr->content);
            }
        }
    }
}

void AddSchedule(int year, int month, int day, int type, char* content) { // add 기능
    schedule_count++;
    SCHEDULE* curr=schedules[year-1970][month-1][day-1]; // 리스트 원소 탐색 포인터
    while (curr->next!=NULL) { // 끝까지 탐색
        curr=curr->next;
    }
    SCHEDULE* new=(SCHEDULE*)malloc(sizeof(SCHEDULE));
    curr->next=new;
    new->type=type;
    new->next=NULL;
    strcpy(new->content,content); // 리스트의 마지막에 원소 추가 및 초기화
}

void RemoveSchedule(int year, int month, int day) { // remove 기능
    schedule_count--;
    SCHEDULE *curr=schedules[year-1970][month-1][day-1],*prev=NULL; // 리스트 원소 탐색 포인터, 이전 원소 포인터
    if (curr->next==NULL) { // 리스트가 비어 있다면
        printf("No schedule to remove\n");
        return;
    }
    while (curr->next!=NULL) { // 끝까지 탐색
        prev=curr;
        curr=curr->next;
    }
    prev->next=NULL; // 마지막 원소 전의 원소를 tail로 만듬
    free(curr); // 마지막 원소 삭제
}

void Save() { // save 기능
    FILE* fp=fopen("data.txt","w+t");
    int count=0; // load 마지막줄 \n에 의해 EOF로 예외 발생하는 경우 방지하기 위해 개수 셈
    if (fp!=NULL) {
        for (int i=0;i<131;i++) {
            for (int j=0;j<12;j++) {
                for (int k=0;k<31;k++) {
                    SCHEDULE* curr=schedules[i][j][k]; // 3차원 구조체 포인터 배열의 각 원소에 대하여 head 포인터 설정
                    while (curr->next!=NULL) { // 끝까지 탐색
                        curr=curr->next;
                        count++;
                        if (count!=schedule_count) fprintf(fp,"%d-%d-%d-%d-%s\n",i+1970,j+1,k+1,curr->type,curr->content); // 존재하는 모든 데이터 서식화하여 저장
                        else fprintf(fp,"%d-%d-%d-%d-%s",i+1970,j+1,k+1,curr->type,curr->content); // 마지막줄은 \n 없이 입력(EOF 빙지)
                    }
                }
            }
        }
    } else printf("File save failed\n"); // 파일 예외처리
    fclose(fp);
}

void Load() { // load 기능
    FILE* fp=fopen("data.txt","rt");
    if (fp!=NULL) {
        int year,month,day,type;
        char buffer[1001];
        while (!feof(fp)) { // 파일 안에 있을때 - 데이터 바뀌었을 때 예외처리를 위하여 while (fscanf) 가 아니라 feof 사용하고 fscanf는 반복문 안으로 집어넣음
            int e=fscanf(fp,"%d-%d-%d-%d-%[^\n]s",&year,&month,&day,&type,buffer); // 읽어들인 값 개수
            if ((e!=5)||(!IsValid(year,month,day,type))) { // 데이터 입력 검증(개수, 범위)
                printf("Data File corrupted\n");
                ResetSchedule(); // 복원 불가능 시 초기화
                break;
            } else AddSchedule(year,month,day,type,buffer); // 정상적인 경우 일정 추가
        }
    } else printf("Data file does not exist\n"); // 파일 예외처리
    fclose(fp);
}

int main() {
    for (int i=0;i<sizeof(pages)/sizeof(pages[0]);i++) {
        for (int j=0;j<sizeof(pages[0])/sizeof(PAGE*);j++) pages[i][j]=(PAGE*)malloc(sizeof(PAGE)); // page 포인터 배열 메모리 할당
    }
    for (int i=0;i<131;i++) {
        for (int j=0;j<12;j++) {
            for (int k=0;k<31;k++) {
                schedules[i][j][k]=(SCHEDULE*)malloc(sizeof(SCHEDULE)); // schedule 포인터 배열 메모리 할당 및 초기화
                schedules[i][j][k]->next=NULL;
            }
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
                } else rewind(stdin);
            } else if (!strcmp(input,commands[1])) {
                int year,month,day,type;
                char content[1000];
                if (scanf("%d %d %d %d",&year,&month,&day,&type)!=EOF) {
                    scanf(" %[^\n]s",content);
                    AddSchedule(year,month,day,type,content);
                } else rewind(stdin);
            } else if (!strcmp(input,commands[2])) {
                int year,month,day;
                if (scanf("%d %d %d",&year,&month,&day)!=EOF) {
                    RemoveSchedule(year,month,day);
                } else rewind(stdin);
            } else if (!strcmp(input,commands[3])) {
                Save();
            } else if (!strcmp(input,commands[4])) {
                Load();
            } else rewind(stdin);
        }
    } while (strcmp(input,"exit"));

    for (int i=0;i<sizeof(pages)/sizeof(pages[0]);i++) {
        for (int j=0;j<sizeof(pages[0])/sizeof(PAGE*);j++) free(pages[i][j]); // page 포인터 배열 해제
    }
    for (int i=0;i<131;i++) {
        for (int j=0;j<12;j++) {
            for (int k=0;k<31;k++) free(schedules[i][j][k]); // schedule 포인터 배열 해제
        }
    }
    return 0;
}
