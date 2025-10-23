//Used help from the Google generated response for "how to initialize a client in c with udp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    char * server_ip = argv[1];
    int server_port = atoi(argv[2]);
    
    int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    socklen_t server_addr_len = sizeof(server_addr);

    printf("Enter string: ");
    char input[512];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; //This removes the new line

    sendto(client_socket_fd, input, strlen(input), 0, (const struct sockaddr *)&server_addr, server_addr_len); //This sends the user input to the server

    char buffer[2048];
    while (true)
    {
        ssize_t recv_len = recvfrom(client_socket_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server_addr, &server_addr_len);
        buffer[recv_len] = '\0'; //Set the end char of the buffer

        //Remove the <END> delimiter
        char *end_marker = strstr(buffer, "<END>");
        if (end_marker) {
            *end_marker = '\0'; // Truncate the string at <END>
        }

        printf("%s\n", buffer);

        if (strcmp(buffer, "From server: Sorry, cannot compute!") == 0)
        {
            return 0;
        }

        int value = atoi(buffer + strlen("From server: "));
        if (value < 10) //This means the last response has been received so we can terminate the client
        {
            return 0;
        }
    }
}