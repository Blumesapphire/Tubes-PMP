#include "dokter.h"

char *formatDoktersToString(ListNode *head) {
  GString *buffer = g_string_new("");
  ListNode *current = head;
  if (head == NULL) {
      g_string_append(buffer, "No doctors registered yet.\n");
  } else {
      g_string_append(buffer, "--- Registered Doctors ---\n");
  }

  while (current != NULL) {
    g_string_append_printf(buffer, "ID: %d\n", current->data.id);
    g_string_append_printf(buffer, "Nama: %s\n", current->data.nama);
    g_string_append_printf(buffer, "Max Shift per week: %d\n", current->data.maxShift); 

    g_string_append(buffer, "Hari Preferensi: ");
    int first_day = 1;
    for (int i = 0; i < MAX_HARI_ARRAY_SIZE; i++) {
      if (strlen(current->data.hari[i]) > 0) {
        if (!first_day) g_string_append(buffer, ", ");
        g_string_append(buffer, current->data.hari[i]);
        first_day = 0;
      }
    }
    g_string_append(buffer, "\n");

    g_string_append(buffer, "Shift Preferensi: ");
    int first_shift = 1;
    for (int i = 0; i < MAX_SHIFT_ARRAY_SIZE; i++) {
      if (strlen(current->data.shift[i]) > 0) {
        if (!first_shift) g_string_append(buffer, ", ");
        g_string_append(buffer, current->data.shift[i]);
        first_shift = 0;
      }
    }
    g_string_append(buffer, "\n\n");
    current = current->next;
  }
  return g_string_free(buffer, FALSE);
}

ListNode *createDokterList() {
    ListNode *head = NULL;
    FILE *file = fopen("Data/data_dokter.csv", "r");
    if (file == NULL) {
        g_print("Warning: data_dokter.csv not found or could not be opened. Starting with an empty doctor list.\n");
        return NULL;
    }

    char line[256]; 
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return NULL; 
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Dokter newDokter;
        for(int i = 0; i < MAX_HARI_ARRAY_SIZE; i++) newDokter.hari[i][0] = '\0';
        for(int i = 0; i < MAX_SHIFT_ARRAY_SIZE; i++) newDokter.shift[i][0] = '\0';

        char *token;
        char *rest = line;

        token = strtok_r(rest, ",", &rest);
        if (token) newDokter.id = atoi(token); else continue;

        token = strtok_r(rest, ",", &rest);
        if (token) strncpy(newDokter.nama, token, sizeof(newDokter.nama) - 1);
        newDokter.nama[sizeof(newDokter.nama) - 1] = '\0';

        token = strtok_r(rest, ",", &rest);
        if (token) newDokter.maxShift = atoi(token); else continue;

        token = strtok_r(rest, ",", &rest);
        if (token) {
            char *hari_copy = g_strdup(token);
            char *hari_rest = hari_copy;
            char *hari_token;
            int i = 0;
            while ((hari_token = strtok_r(hari_rest, ";", &hari_rest)) != NULL && i < MAX_HARI_ARRAY_SIZE) {
                gchar *cleaned_hari = g_strstrip(g_strdup(hari_token));
                strncpy(newDokter.hari[i], cleaned_hari, sizeof(newDokter.hari[i]) - 1);
                newDokter.hari[i][sizeof(newDokter.hari[i]) - 1] = '\0';
                g_free(cleaned_hari);
                i++;
            }
            g_free(hari_copy);
        }

        token = strtok_r(rest, "\n", &rest);
        if (token) {
            char *shift_copy = g_strdup(token); 
            char *shift_rest = shift_copy;
            char *shift_token;
            int i = 0;
            while ((shift_token = strtok_r(shift_rest, ";", &shift_rest)) != NULL && i < MAX_SHIFT_ARRAY_SIZE) {
                gchar *cleaned_shift = g_strstrip(g_strdup(shift_token));
                strncpy(newDokter.shift[i], cleaned_shift, sizeof(newDokter.shift[i]) - 1);
                newDokter.shift[i][sizeof(newDokter.shift[i]) - 1] = '\0';
                g_free(cleaned_shift);
                i++;
            }
            g_free(shift_copy);
        }

        ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
        if (newNode == NULL) {
            g_print("Error: Memory allocation failed for new doctor node.\n");
            fclose(file);
            return head;
        }
        newNode->data = newDokter;
        newNode->next = NULL;

        if (head == NULL) {
            head = newNode;
        } else {
            ListNode *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }
    fclose(file);
    return head;
}


ListNode* findDokterById(ListNode* head, int id) {
    ListNode* current = head;
    while (current != NULL) {
        if (current->data.id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL; 
}

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
