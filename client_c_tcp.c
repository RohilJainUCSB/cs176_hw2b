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
    
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    connect(client_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //This connects to the server to initiate handshake

    socklen_t server_addr_len = sizeof(server_addr);

    printf("Enter string: ");
    char input[128];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; //This removes the new line

    send(client_socket_fd, input, strlen(input), 0); //This sends the user input to the server

    char buffer[256];
    int total_received = 0; //TCP is a stream-based connection, so this will be used to ensure all the separate messages are separated appropriately
    while (true)
    {
        ssize_t recv_len = recv(client_socket_fd, buffer + total_received, sizeof(buffer) - 1 - total_received, 0);

        total_received += recv_len; //We can update the total amount received so that it can appropriately handle the strings
        buffer[total_received] = '\0'; //Set the end char of the buffer
        
        char *start = buffer; //This will be used as we run through the entire buffer to ensure all of it is handled without assuming just one input is received at a time
        while(true)
        {
            char * end_marker = strstr(start, "<END>"); //This helps by processing the input delimiters from the server
            if(!end_marker) break; //If there is no longer an <END> remaining, then we have processed the whole buffer
            *end_marker = '\0'; //This uses that slice from the above code and sets it to be a message terminator so that the prints don't include it
            if (strlen(start) > 0) //We only want to print if it is nonempty
            {
                printf("%s\n", start);
            }

            if (strcmp(start, "Sorry, cannot compute!") == 0) //If it can't be computed we can terminate this whole process
            {
                return 0;
            }

            int value = atoi(start + strlen("From server: "));
            if (value < 10) //This means the last response has been received so we can terminate the client
            {
                close(client_socket_fd);
                return 0;
            }

            // Move remaining data forward
            start = end_marker + strlen("<END>");
        }
        // Shift leftover data to beginning of buffer now that we are using this buffer flushing method with start
        total_received = strlen(start);
        memmove(buffer, start, total_received);
    }
}