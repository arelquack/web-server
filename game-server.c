#include "game-server.h"

/*
    Author    : Arel
    Deskripsi : Menyebarkan pesan ke semua pemain yang terhubung.
                Digunakan untuk memberikan update status permainan atau pesan lainnya.
*/

// Definisikan variabel global yang sudah dideklarasikan di header
int shm_id; // ID shared memory
int *shared_data; // Pointer untuk shared memory
int server_fd; // Socket server
int client_sockets[MAX_PLAYERS]; // Array untuk menyimpan socket klien
int secret_number; // Angka yang harus ditebak
char player_names[MAX_PLAYERS][MAX_NAME_LENGTH]; // Array nama pemain

void broadcast(const char *message) {
    for (int i = 0; i < shared_data[1]; i++) { // `shared_data[1]` adalah jumlah pemain saat ini
        if (client_sockets[i] != 0) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
}

/*
    Author    : Dzak
    Deskripsi : Menangani interaksi dengan pemain.
                Fungsi ini akan meminta input tebakan, memvalidasi, dan memberikan respon berdasarkan tebakan.
                Fungsi ini juga mengatur giliran pemain dan mengirim pesan ke semua pemain.
*/
void handle_client(int client_socket, int player_index) {
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    // Meminta username pemain
    send(client_socket, "Masukkan username Anda: ", 25, 0);
    int valread = read(client_socket, buffer, sizeof(buffer));
    if (valread <= 0) {
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    buffer[valread - 1] = '\0'; // Hilangkan newline
    strncpy(player_names[player_index], buffer, sizeof(player_names[player_index]));
    sprintf(message, "Selamat datang, %s! Tunggu giliran Anda.\n", player_names[player_index]);
    send(client_socket, message, strlen(message), 0);

    while (1) {
        if (shared_data[0] == player_index) { // `shared_data[0]` adalah giliran aktif
            sprintf(message, "Giliran %s sekarang. Tebak angka: ", player_names[player_index]);
            send(client_socket, message, strlen(message), 0);

            valread = read(client_socket, buffer, sizeof(buffer));
            if (valread <= 0) {
                printf("%s terputus.\n", player_names[player_index]);
                close(client_socket);
                client_sockets[player_index] = 0;
                exit(EXIT_FAILURE);
            }

            buffer[valread - 1] = '\0'; // Hilangkan newline
            int guess;
            if (sscanf(buffer, "%d", &guess) != 1) {
                sprintf(message, "%s: Input tidak valid! Harus berupa angka.\n", player_names[player_index]);
                send(client_socket, message, strlen(message), 0);
                continue;
            }
            printf("%s menebak: %d\n", player_names[player_index], guess);

            // Memeriksa tebakan pemain
            if (guess == secret_number) {
                sprintf(message, "%s MENANG! Angkanya adalah %d.\n", player_names[player_index], secret_number);
                broadcast(message);
                shared_data[2] = 0; // `shared_data[2]` menandakan status game aktif
                exit(EXIT_SUCCESS);
            } else if (guess < secret_number) {
                sprintf(message, "%s: Salah! Angka terlalu kecil.\n", player_names[player_index]);
            } else {
                sprintf(message, "%s: Salah! Angka terlalu besar.\n", player_names[player_index]);
            }
            broadcast(message);

            // Update giliran pemain berikutnya
            shared_data[0] = (shared_data[0] + 1) % shared_data[1];
        } else {
            usleep(100000); // Tunggu giliran (0.1 detik)
        }
    }
}

/*
    Author    : Arel
    Deskripsi : Fungsi ini membersihkan resource seperti shared memory dan soket server
                Ketika server dihentikan atau ada gangguan.
*/
void cleanup() {
    shmctl(shm_id, IPC_RMID, NULL); // Hapus shared memory
    close(server_fd);               // Tutup soket server
    printf("Server ditutup dan shared memory dibersihkan.\n");
    exit(EXIT_SUCCESS);
}
