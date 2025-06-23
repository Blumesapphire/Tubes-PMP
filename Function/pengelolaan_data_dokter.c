#include "pengelolaan_data_dokter.h"
#include "dokter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <ctype.h> 
void saveDoktersToCSV(ListNode *head) {
    FILE *file = fopen("Data/data_dokter.csv", "w"); // Buka file dalam mode tulis ("w")
    if (file == NULL) {
        g_print("Error: Gagal membuka data_dokter.csv untuk menulis. Pastikan folder 'Data/' ada dan dapat ditulis.\n");
        return;
    }

    fprintf(file, "Id,Nama,MaxShift,Hari,Shift\n"); // Tulis header CSV

    ListNode *temp = head;
    while (temp != NULL) {
        char hariJoin[MAX_HARI_INPUT_LEN] = ""; // Inisialisasi string kosong untuk hari
        for (int i = 0; i < MAX_HARI_ARRAY_SIZE; i++) {
            if (temp->data.hari[i][0] != '\0') { // Periksa apakah string tidak kosong
                if (strlen(hariJoin) > 0) {
                    strcat(hariJoin, ";");
                }
                strcat(hariJoin, temp->data.hari[i]);
            }
        }

        char shiftJoin[MAX_SHIFT_INPUT_LEN] = ""; // Inisialisasi string kosong untuk shift
        for (int i = 0; i < MAX_SHIFT_ARRAY_SIZE; i++) {
            if (temp->data.shift[i][0] != '\0') { // Periksa apakah string tidak kosong
                if (strlen(shiftJoin) > 0) {
                    strcat(shiftJoin, ";");
                }
                strcat(shiftJoin, temp->data.shift[i]);
            }
        }
        fprintf(file, "%d,%s,%d,%s,%s\n", temp->data.id, temp->data.nama, temp->data.maxShift, hariJoin, shiftJoin);
        temp = temp->next;
    }
    fclose(file);
}

// Fungsi untuk menambahkan dokter baru melalui input GUI
int addDokter_gui(ListNode **head, int id, const char *nama, int maxShift, const char *hari_str, const char *shift_str) {
    // Cek apakah ID sudah ada
    if (findDokterById(*head, id) != NULL) {
        g_print("Error: Dokter dengan ID %d sudah ada.\n", id);
        return -1; // ID sudah ada
    }

    ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        g_print("Error: Memory allocation failed.\n");
        return -1; // Gagal alokasi memori
    }

    newNode->data.id = id;
    strncpy(newNode->data.nama, nama, sizeof(newNode->data.nama) - 1);
    newNode->data.nama[sizeof(newNode->data.nama) - 1] = '\0';
    newNode->data.maxShift = maxShift;

    // Inisialisasi hari dan shift ke string kosong
    for (int i = 0; i < MAX_HARI_ARRAY_SIZE; i++) newNode->data.hari[i][0] = '\0';
    for (int i = 0; i < MAX_SHIFT_ARRAY_SIZE; i++) newNode->data.shift[i][0] = '\0';

    // Parsing hari_str
    char *hari_str_copy = g_strdup(hari_str); // Buat salinan string agar strtok tidak mengubah aslinya
    char *hari_token = strtok(hari_str_copy, ",");
    int i = 0;
    while (hari_token != NULL && i < MAX_HARI_ARRAY_SIZE) {
        gchar *cleaned_hari = g_strstrip(g_strdup(hari_token)); // Membersihkan spasi
        strncpy(newNode->data.hari[i], cleaned_hari, sizeof(newNode->data.hari[i]) - 1);
        newNode->data.hari[i][sizeof(newNode->data.hari[i]) - 1] = '\0';
        g_free(cleaned_hari);
        hari_token = strtok(NULL, ",");
        i++;
    }
    g_free(hari_str_copy); // Bebaskan memori salinan

    // Parsing shift_str
    char *shift_str_copy = g_strdup(shift_str); // Buat salinan string
    char *shift_token = strtok(shift_str_copy, ",");
    int j = 0;
    while (shift_token != NULL && j < MAX_SHIFT_ARRAY_SIZE) {
        gchar *cleaned_shift = g_strstrip(g_strdup(shift_token)); // Membersihkan spasi
        strncpy(newNode->data.shift[j], cleaned_shift, sizeof(newNode->data.shift[j]) - 1);
        newNode->data.shift[j][sizeof(newNode->data.shift[j]) - 1] = '\0';
        g_free(cleaned_shift);
        shift_token = strtok(NULL, ",");
        j++;
    }
    g_free(shift_str_copy); // Bebaskan memori salinan

    newNode->next = NULL; // Node baru selalu ditambahkan di akhir

    // Tambahkan node baru ke linked list
    if (*head == NULL) {
        *head = newNode; // Jika list kosong, node baru menjadi head
    } else {
        ListNode *last = *head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = newNode; // Tambahkan ke akhir list
    }

    saveDoktersToCSV(*head); // Simpan daftar yang diperbarui ke CSV
    return 0; // Berhasil
}

// Fungsi untuk menghapus dokter melalui input GUI
int hapusDokter_gui(ListNode **head, int targetId) {
    ListNode *current = *head;
    ListNode *prev = NULL;

    // Cari dokter yang akan dihapus
    while (current != NULL && current->data.id != targetId) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return -1; // Dokter tidak ditemukan
    }

    // Hapus dokter dari list
    if (prev == NULL) {
        *head = current->next; // Head adalah yang akan dihapus
    } else {
        prev->next = current->next;
    }
    free(current); // Bebaskan memori node yang dihapus
    saveDoktersToCSV(*head); // Simpan daftar yang diperbarui ke CSV
    return 0; // Berhasil
}
