/*
    Author    : Arel
    Deskripsi : Header file untuk server permainan tebak angka
                Berisi deklarasi fungsi dan variabel global yang digunakan
                untuk mengelola game server, komunikasi antar pemain, dan pengaturan shared memory.
*/

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>  // Untuk signal dan SIGINT
#include <sys/ipc.h> // Untuk shmget dan IPC_PRIVATE
#include <sys/shm.h> // Untuk shmget, shmat, shmctl


#define PORT 1234                // Port yang digunakan oleh server
#define BUFFER_SIZE 1024         // Ukuran buffer untuk komunikasi antar klien
#define MAX_PLAYERS 10           // Jumlah maksimal pemain yang bisa bergabung
#define MAX_NAME_LENGTH 50      // Panjang maksimal nama pemain (misalnya 50 karakter)

extern int shm_id;               // ID shared memory
extern int *shared_data;         // Pointer shared memory
extern int server_fd;            // Socket server
extern int client_sockets[MAX_PLAYERS]; // Sockets klien
extern int secret_number;        // Angka yang harus ditebak
extern char player_names[MAX_PLAYERS][MAX_NAME_LENGTH]; // Nama pemain

// Fungsi untuk menyebarkan pesan ke semua pemain
void broadcast(const char *message);

// Fungsi untuk menangani komunikasi dengan setiap pemain
void handle_client(int client_socket, int player_index);

// Fungsi untuk membersihkan resource (socket dan shared memory)
void cleanup();

#endif // GAME_SERVER_H
