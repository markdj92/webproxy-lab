#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv)
{
    int listenfd, connfd; //리스닝 소켓과 연결된 소켓의 파일 디스크립터
    socklen_t clientlen; //클라이언트 주소의 길이
    struct sockaddr_storage clientaddr; //클라이언트 주소를 저장하는 구조체
    char client_hostname[MAXLINE], client_port [MAXLINE]; //클라이언트의 호스트명과 포트를 저장하는 문자열
    
    if (argc !=2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]); // 포트가 제공되지 않을 때 올바른 사용법 출력
        exit(0);
    }
    
    listenfd = Open_listenfd(argv[1]); // 지정된 포트에서 리스닝 소켓 열기
    
    while (1) { 
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 클라이언트로부터의 연결 수락

        Getnameinfo((SA *)  &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        // 클라이언트의 호스트명과 포트를 가져옴

        // (클라이언트 호스트명, 포트)에 연결됨을 출력
        printf("Connected to (%s, %s)\n", client_hostname, client_port);

        echo(connfd); // 클라이언트와의 통신을 처리하는 함수 호출

        Close(connfd); // 연결된 소켓 닫기
    }
    exit(0);
}

/*Open_listenfd() 함수를 사용하여 지정된 포트에서 리스닝 소켓을 엽니다.
Accept() 함수를 사용하여 클라이언트로부터의 연결을 수락하고, 이에 대한 파일 디스크립터를 반환합니다.
Getnameinfo() 함수를 사용하여 클라이언트의 호스트명과 포트를 가져옵니다.
클라이언트의 호스트명과 포트를 출력합니다.
echo() 함수를 호출하여 클라이언트와의 통신을 처리합니다.
연결된 소켓을 닫고, 다음 클라이언트의 연결을 기다립니다.*/