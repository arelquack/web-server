#include "game-server.h"

/*
    Author    : Arel
    Deskripsi : Fungsi utama untuk menjalankan server, menerima koneksi pemain,
                mengelola permainan, dan mengatur shared memory.
                Server menunggu pemain untuk bergabung dan mengelola giliran.
*/
int main() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int max_players;

    signal(SIGINT, cleanup); // Tangkap Ctrl+C untuk pembersihan

    printf("=== MENU SERVER ===\n");
    printf("Masukkan jumlah pemain (2-10): ");
    scanf("%d", &max_players);
    if (max_players < 2 || max_players > MAX_PLAYERS) {
        printf("Jumlah pemain tidak valid! Harus antara 2 hingga 10.\n");
        exit(EXIT_FAILURE);
    }

    // Inisialisasi shared memory
    shm_id = shmget(IPC_PRIVATE, 3 * sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("Shared memory gagal dibuat");
        exit(EXIT_FAILURE);
    }
    shared_data = (int *)shmat(shm_id, NULL, 0);
    shared_data[0] = 0;          // Giliran aktif
    shared_data[1] = 0;          // Jumlah pemain saat ini
    shared_data[2] = 1;          // Status game aktif

    // Inisialisasi server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket gagal dibuat");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind gagal");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen gagal");
        exit(EXIT_FAILURE);
    }

    printf("Server Tebak Angka siap di port %d\n", PORT);
    secret_number = rand() % 100 + 1;  // Angka rahasia yang harus ditebak
    printf("Angka rahasia: %d\n", secret_number);

    while (shared_data[2]) {  // Loop utama server selama game aktif
        printf("Menunggu pemain...\n");
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("Accept gagal");
            exit(EXIT_FAILURE);
        }

        if (shared_data[1] >= max_players) {
            const char *full_msg = "Room sudah penuh. Silakan coba lagi nanti.\n";
            send(new_socket, full_msg, strlen(full_msg), 0);
            close(new_socket);
            continue;
        }

        client_sockets[shared_data[1]] = new_socket; // Tambah pemain
        shared_data[1]++;

        if (fork() == 0) { // Proses anak untuk menangani tiap pemain
            handle_client(new_socket, shared_data[1] - 1);
        }
    }

    cleanup();
    return 0;
}
