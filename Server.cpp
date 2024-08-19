#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "ClAndSer.h"
int main()
{
int listener;
listener = socket(AF_INET, SOCK_STREAM, 0);
if (listener == -1) {
fprintf(stderr, "Ошибка создания сокета\n");
return 1;
}
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(3425);
addr.sin_addr.s_addr = htonl(INADDR_ANY);
if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
fprintf(stderr, "Ошибка привязки сокета\n");
return 2;
}
listen(listener, 1);
fprintf(stderr, "Start server.\n");
for (;;) {
int sock = accept(listener, NULL, NULL);
fprintf(stderr, "connect new client.\n");
if (sock < 0) {
fprintf(stderr, "Error accept connect.\n");
return 3;
}
char* buf = malloc(10000000);
int stop = 0;
while (!stop) {
const int read_code = read_message(buf, sock);
if (read_code == kEndConnection) {
printf("Received message: \n%s\n", buf);
fprintf(stderr, "End session.\n");
break;
} else if (read_code == kTransferComplete) {
printf("Received message: \n%s\n", buf);
} else {
fprintf(stderr, "An error occurred in the connection.\n");
break;
}
const int write_code = send_message_with_keyword(sock);
if (write_code == kEndConnection) {
fprintf(stderr, "End session.\n");
break;
} else if (write_code == kTransferComplete) {
/////////////
} else if (write_code == kEOF) {
fprintf(stderr, "EOF. Exiting!\n");
stop = 1;
break;
} else {
fprintf(stderr, "An error occurred in the connection.\n");
break;
}
}
shutdown(sock, 2);
close(sock);
}
return 0;
}