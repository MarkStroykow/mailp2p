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
int sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == -1) {
fprintf(stderr, "Error create socket.\n");
return 1;
}
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(3425);
addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
fprintf(stderr, "Error connect on server.\n");
return 2;
}
char* buf = malloc(10000000);
for (;;) {
const int write_code = send_message_with_keyword(sock);
if (write_code == kEndConnection) {
printf("End session.\n");
break;
} else if (write_code == kTransferComplete) {
/////////////
} else {
fprintf(stderr, "EOF. Exiting!\n");
break;
}
const int read_code = read_message(buf, sock);
if (read_code == kEndConnection) {
printf("Received message: \n%s\n", buf);
printf("End session.\n");
break;
} else if (read_code == kTransferComplete) {
printf("Received message: \n%s\n", buf);
} else {
printf("An error occurred in the connection.\n");
break;
}
}
close(sock);
return 0;
}