//Used help from the Google generated response for "how to initialize a socket server in c with udp"
#include <sys/socket.h>
#include <arpa/inet.h> //Needed for the struct sockaddr_in to work
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> // Needed for isdigit()
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    int serverPort = atoi(argv[1]); //Command line-provided port number of server to listen on

    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0); //Uses Socket_STREAM instead of SOCK_DGRAM so this is TCP
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(serverPort); //Set the server port number to the provided one to listen on
    address.sin_addr.s_addr = htonl(INADDR_ANY); //Set the server IP to my localhost IP (provided in the lab instructions)
    
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    bind(server_socket_fd, (const struct sockaddr *)&address, sizeof(address)); //Bind the socket with the IP and port
    listen(server_socket_fd, 5); // Sets up the listening with a backlog of size 5

    while (true)
    {
        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len); //This does the handhsake and stores the client data
        char buffer[256];
        ssize_t data_received = recv(client_socket_fd, buffer, sizeof(buffer) - 1, 0);
        buffer[data_received] = '\0';

        bool isValid = true;
        for (int i = 0; buffer[i] != '\0'; ++i)
        {
            if (!isdigit(buffer[i]))
            {
                isValid = false;
                break;
            }
        }

        if (!isValid)
        {
            send(client_socket_fd, "From server: Sorry, cannot compute!<END>", 41, 0);
            close(client_socket_fd);
            continue; //Skip this input if it is invalid
        }

        //This will now loop to send the new sum until it is a single digit value
        int sum = 0;
        for (int i = 0; buffer[i] != '\0'; ++i)
        {
            sum += buffer[i] - '0';
        }
        snprintf(buffer, sizeof(buffer), "From server: %d<END>", sum); //The use of <END> here is as a delimiter. It helps ensure appropriate output is displayed with some additional processing on the <END> chars on the client end
        send(client_socket_fd, buffer, strlen(buffer), 0);

        snprintf(buffer, sizeof(buffer), "%d", sum); //Reset the buffer
        while (sum >= 10)
        {
            sum = 0;
            for (int i = 0; buffer[i] != '\0'; ++i)
            {
                sum += buffer[i] - '0';
            }
            snprintf(buffer, sizeof(buffer), "From server: %d<END>", sum); //The use of <END> here is as a delimiter. It helps ensure appropriate output is displayed with some additional processing on the <END> chars on the client end
            send(client_socket_fd, buffer, strlen(buffer), 0);

            snprintf(buffer, sizeof(buffer), "%d", sum); //Reset the buffer
        }
    }
    return 0;
}