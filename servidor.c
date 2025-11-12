#include <sys/socket.h> // socket()
#include <netinet/in.h>
#include <sys/types.h> // helpers for socket()

#include <arpa/inet.h> //inet_addr() && inet_ntoa()
#include <string.h> // strcmp()
#include <stdio.h> // fprintf()
#include <unistd.h>
#include <fcntl.h>


static const in_port_t port = 9999;
static const char * google_file_path = "Google.html";

int main(){

    short family = AF_INET;

    struct sockaddr_in sc;
    memset(&sc, 0, sizeof sc);
    sc.sin_port = htons(port);
    sc.sin_family = family;

    int fd_link;
    
    if((fd_link = socket((int) family,SOCK_STREAM, IPPROTO_TCP)) == -1){
        fprintf(stderr, "The socket could not be created!\n");
        return 1;
    }

    bind(fd_link, (struct sockaddr *) &sc, sizeof sc);

    listen(fd_link, 1);

    struct sockaddr_in cli;
    socklen_t cli_len = sizeof(cli);

    int fd_conn = accept(fd_link, (struct sockaddr *)&cli, &cli_len);
    if (fd_conn == -1) {
        perror("accept");
        return 1;
    }

    char buff[512];

    int fd_google;
    if((fd_google = open(google_file_path, O_RDWR)) == -1){
        fprintf(stderr, "Could not be opened: %s\n", google_file_path);
        return 0;
    }

    ssize_t size;
    while((size = read(fd_google, buff, sizeof buff)) > 0){
        if(write(fd_conn, buff, size) == -1){
            fprintf(stderr, "The file could not be sent: %s!\n", google_file_path);
            break;
        }
    }

    if(size == -1){
        fprintf(stderr, "There was an error parsing %s!\n", google_file_path);
    }

    close(fd_google);

    shutdown(fd_conn, SHUT_WR);
    shutdown(fd_link, SHUT_WR);
    close(fd_conn);
    close(fd_link);

    return 1;
}