#pragma once
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#define kEndConnection 0
#define kTitleError 1
#define kConnectionError 2
#define kTransferComplete 3
#define kEOF 4
const int size_head = 1;
typedef void (*sighandler)(int);
int flag = 0;
void brake(int c) { flag = 1; }
// signal(SIGQUIT, (sighandler)brake);
int read_message(char* buf, int sock)
{
char tmp[0xff];
char* buf_ptr = buf;
do {
int bytes_read = recv(sock, tmp, 0xff - 2, 0);
if (bytes_read <= 0) {
printf("dd\n");
return kConnectionError;
}
const int mes_size = bytes_read - size_head;
memcpy(buf_ptr, tmp + size_head, mes_size);
buf_ptr += mes_size;
} while (tmp[0] == 'n');
(*buf_ptr) = '\0';
switch (tmp[0]) {
case 'q':
return kTransferComplete;
case 'z':
return kEndConnection;
default:
// printf("dd2 ------------ %c --\n", tmp[0]);
return kTitleError;
}
return 0;
}
int send_message_with_keyword(int sock)
{
signal(SIGQUIT, (sighandler)brake);
// printf("начало передачи")
char buf[0xff];
int size_mes = 0;
// const char* end_mes = "end_mes!\n";
const char* end = "end!\n";
buf[0] = 'n';
printf("begin\n");
while ((size_mes = read(1, buf + size_head, 0xff - 2)) > 0) {
buf[size_head + size_mes] = '\0'; // для работы strcmp
if (flag == 1) {
flag = 0;
buf[0] = 'q';
const int send_res = send(sock, buf, size_head, 0);
if (send_res == -1)
return kConnectionError;
return kTransferComplete;
} else if (strcmp(end, buf + size_head) == 0) {
buf[0] = 'z';
const int send_res = send(sock, buf, size_head, 0);
if (send_res == -1)
return kConnectionError;
return kEndConnection;
} else {
buf[0] = 'n';
const int send_res = send(sock, buf, size_mes + size_head, 0);
if (send_res == -1)
return kConnectionError;
}
}
printf("end\n");
return kEOF;
}