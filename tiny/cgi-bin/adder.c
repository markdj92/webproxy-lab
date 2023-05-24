/* $begin adder */
#include "csapp.h"

int main(void) {
  char *buf, *p;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1=0, n2=0;
  char *A; //A,B값 변수명을 지정해줘야한다.
  char *B;

  if ((buf = getenv("QUERY_STRING")) != NULL) {
    A = strchr(buf, 'A');
    B = strchr(buf, 'B');
    //문자열 buf에서 A와B를 찾아서 각가의 첫 번째 발생 위치를 A와 B에 할당한다.
    *A = '\0';
    *B = '\0';
    //*A = '\0' 및 *B = '\0'; A와 B의 위치에 있는 문자를 \0로 바꿈으로써 해당 위치 이후의 문자열을 끊는다. 이로써 buf문자열은 A와B사이의 두 개의 문자열로 분할된다.
    strcpy(arg1, A+2);
    strcpy(arg2, B+2);
    //strcpy(arg1, A+2);및 strcpy(arg2, B+2); A와 B 다음 위치부터 시작하는 문자열을 각각 arg1과 arg2에 복사한다. +2는 A와 B사이의 두 개의 문자열로 분할된다.
    n1 = atoi(arg1);
    n2 = atoi(arg2);
  }

  sprintf(content, "QUERY_STRING=%s", buf);
  sprintf(content, "Welcom to add.com: ");
  sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  printf("Connetion: clse\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(0);
}
