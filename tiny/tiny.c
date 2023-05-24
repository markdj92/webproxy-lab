/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize, char *method);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);
// void echo(int connfd); 에코서버 테스트                

int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);   // line:netp:tiny:doit
    // echo(connfd); 에코서버 테스트
    Close(connfd);  // line:netp:tiny:close
  }
}

void doit(int fd)
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;
    
    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET") && strcmp(method, "HEAD")) {
        clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this mehtod");
        return;
    }
    read_requesthdrs(&rio);

    is_static = parse_uri(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
        return;
    }
    
    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            //S_ISREG(sbuf.st_mode):S_ISREG매크로는 파일이 일반파일인지 확인하는 데 사용된다. sbuf.st_mode는 st_mode멤버로부터 파일의 속성을 나타내는 비트 마스크이다.
            //S_ISREG매크로는 이 비트 마스크를 검사하여 파일이 일반파일인지 확인한다. 만약 일반 파일인 경우, 이 조건은 참으로 평가된다.
            //S_IRUSR은 사용자에게 읽기 권한을 나타내는 매크로이다. sbuf.st_mode와 S_IRUSR를 비트 AND 연산한 결과를 검사한다. 이 연산은 사용자의 읽기 권한을 확인한다.
            //사용자가 읽기 권한을 가진 경우, 이 조건은 참으로 평가된다.
            //!연산자:논리NOT연산자이다. 이 연산자는 피연산자의 논리 값을 반전 시킨다. 따라서 !(S_ISREG(sbuf.st_mode))은 사용자가 읽기 권한을 갖지 않은 경우에 참이된다.
            //|| 연산자:논리 OR 연산자이다. 두 개의 조건 중 하나라도 참이라면 전체 표현식은 참으로 평가된다.

            //요약 주어진 조건문은 일반 파일이 아니거나 사용자가 읽기 권한을 갖지 않은 경우에 조건이 참이 된다. 이 조건문은 파일이 정규파일이 아니거나 사용자가 읽기 권한을 갖지 않은 경우에 실행되는 코드블록을 나타낸다.
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size, method);
    }
    else {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            clienterror(fd, filename,"403", "Forbidden", "Tiny couldn't run the CGI program");
            return;
        }
        serve_dynamic(fd, filename, cgiargs);
    }

}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXBUF];
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0%s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: test/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);        
    }
    return;
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "home.html");
        return 1;            
    }
    else {
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else
            strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}

void serve_static(int fd, char *filename, int filesize, char *method)
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));
    printf("Response headers:\n");
    printf("%s", buf);

if (!strcmp(method, "GET")) {
    srcfd = Open(filename, O_RDONLY, 0);
    //srcp = Mmap (0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); mmap은 unix와 unix-like 시스템에서 사용하는 메모리 매핑 함수로, 파일을 메모리에 매핑하여 파일을 메모리로 읽거나 쓸 수 있게 해주는 역할을 한다. malloc을 사용하게 되면 더 이상 매핑이 필요하지 않으므로 munmap 함수를 통한 매핑을 해제 해 주어야 한다.
    srcp = (char *)malloc(filesize);//sizeof는 자료형의 크기만큼 들어간다는 말이다.
    Rio_readn(srcfd, srcp, filesize);
    Close(srcfd);
    Rio_writen(fd, srcp, filesize);
    free(srcp);//malloc은 mmap과 달리 그냥 메모리 공간만을 비워줄 뿐이다. 따라서 rio_readn을 통해 포인터를 매핑해주는 작업을 추가해 주어야한다. 또 munmap을 통해서 매핑 해제를 free로 바꿔주어야 한다.
    }
}

void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".mp4"))
        strcpy(filetype, "video/mp4");   
    else
        strcpy(filetype, "text/plain");                
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
    char buf[MAXLINE], *emptylist = { NULL };

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    if (Fork() == 0) {
        setenv("QUERY_STRING", cgiargs, 1);
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
    }
    Wait(NULL);
}

// void echo(int connfd)
// {
//     size_t n;
//     char buf[MAXLINE];
//     rio_t rio;

//     Rio_readinitb(&rio, connfd);
//     while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
//         printf("server received %d bytes\n", (int)n);
//         Rio_writen(connfd, buf, n) ;
//     }
// } 에코서버 테스트