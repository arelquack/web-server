#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 45\r\n"
        "\r\n"
        "<html><body><h1>Ohayou Sekai!</h1></body></html>";

        // Bikin socket
        if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket failure");
            exit(EXIT_FAILURE);
        }

        // Konfigurasi alamat server
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Bind socket ke alamat
        if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failure");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Listen koneksi
        if(listen(server_fd, 3) < 0) {
            perror("Listen failure");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Server is running on http://localhost:%d\n", PORT);

        // Terima koneksi klien
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failure");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Baca request dan kirim response
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Request received: \n%s\n", buffer);
        write(new_socket, response, strlen(response));

        // Tutup koneksi
        close(new_socket);
        close(server_fd);
        return 0;
}
