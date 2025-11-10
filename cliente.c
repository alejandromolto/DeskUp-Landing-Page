#include <sys/socket.h> // socket()
#include <netinet/in.h>
#include <sys/types.h> // helpers for socket()

#include <arpa/inet.h> //inet_addr() && inet_ntoa()
#include <string.h> // strcmp()
#include <stdio.h> // fprintf()
#include <unistd.h> //write()
#include <fcntl.h> // open()

static const in_port_t port = 9999;

static const char * google_file_path_copy = "Google_copy.html";

int main(int argc, char ** argv){
    if(argc != 2){
        fprintf(stderr, "Invalid arguments!\n");
        return 1;
    }

    char * server_ip = argv[1];
    if(strcmp(server_ip, "") == 0){
        fprintf(stderr, "Invalid IP!\n");
        return 1;
    }

    /*Add checking for valid ip*/ 
    
    short family = AF_INET;

    struct sockaddr_in sock;
    memset(&sock, 0, sizeof sock);
    sock.sin_addr.s_addr = inet_addr(server_ip);
    sock.sin_port = htons(port);
    sock.sin_family = family;
    

    int fd_connector;
    
    if((fd_connector = socket((int) family,SOCK_STREAM, IPPROTO_TCP)) == -1){
        fprintf(stderr, "The socket could not be created!\n");
        return 1;
    }

    if(connect(fd_connector, (struct sockaddr *)&sock, (socklen_t) sizeof sock) == -1){
        perror("connect");
        return 1;
    }

    char buff[512];

    int fd_google_copy;
    if((fd_google_copy = open(google_file_path_copy, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1){
        fprintf(stderr, "Could not be opened: %s!\n", google_file_path_copy);
        return 0;
    }


    ssize_t size;
    while((size = read(fd_connector, buff, sizeof buff)) > 0){
        if(write(fd_google_copy, buff, size) == -1){
            fprintf(stderr, "The file could not be sent: %s!\n", google_file_path_copy);
            break;
        }
    }

    if(size == -1){
        fprintf(stderr, "There was an error parsing %s!\n", google_file_path_copy);
    }

    shutdown(fd_connector, SHUT_WR);
    close(fd_connector);

    return 1;
}