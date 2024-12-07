#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Define port-nya dulu bos :D
#define PORT 8080

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    /*
        Author    : Arel
        Deskripsi :
        Membuat socket menggunakan protokol TCP/IP (SOCK_STREAM).
        Jika gagal, program akan keluar dengan status error.
    */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket has been created.\n");

    /*
        Author    : Arel
        Deskripsi :
        Mengatur opsi pada socket agar dapat menggunakan kembali alamat/port.
        Ini berguna untuk menghindari "address already in use" saat restart server.
    */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Set socket options failed");
        exit(EXIT_FAILURE);
    }

    /*
        Author    : Arel
        Deskripsi :
        Mengonfigurasi properti socket:
        - Keluarga alamat: IPv4 (AF_INET)
        - Alamat IP: INADDR_ANY (menerima koneksi dari semua antarmuka jaringan)
        - Port: 8080
    */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    printf("Socket config has been done :D.\n");

    return 0;
}