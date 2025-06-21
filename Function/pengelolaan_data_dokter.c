#include "dokter.h"

void displayDokters(ListNode *head) {
  ListNode *current = head;
  while (current != NULL) {
    printf("ID: %d, Nama: %s, Max Shift: %d\n", current->data.id, current->data.nama, current->data.maxShift);
    printf("Hari: ");
    for (int i = 0; i < 7; i++) {
      if (strlen(current->data.hari[i]) > 0) {
        printf("%s", current->data.hari[i]);
        if (i < 6 && strlen(current->data.hari[i+1]) > 0) {
          printf(", "); 
        }
      }
    }
    printf("\nShift: ");
    for (int i = 0; i < 3; i++) {
      if (strlen(current->data.shift[i]) > 0) {
        printf("%s", current->data.shift[i]);
        if (i < 2 && strlen(current->data.shift[i+1]) > 0) {
          printf(", ");
        }
      }
    }
    printf("\n\n");
    
    current = current->next;
  }
}

void tambahDokter(ListNode **head) {
  Dokter newDokter;
  printf("Masukkan ID: ");
  scanf("%d", &newDokter.id);
  getchar();

  printf("Masukkan Nama: ");
  fgets(newDokter.nama, sizeof(newDokter.nama), stdin);
  newDokter.nama[strcspn(newDokter.nama, "\n")] = '\0';

  printf("Masukkan Max Shift: ");
  scanf("%d", &newDokter.maxShift);
  getchar();

  printf("Masukkan Hari: ");
  char hariStr[100];
  fgets(hariStr, sizeof(hariStr), stdin);
  hariStr[strcspn(hariStr, "\n")] = '\0';

  printf("Masukkan Shift: ");
  char shiftStr[100];
  fgets(shiftStr, sizeof(shiftStr), stdin);
  shiftStr[strcspn(shiftStr, "\n")] = '\0';

  // Parse hari
  char *token = strtok(hariStr, " ");
  int i = 0;
  char hariJoin[100] = "";

  while (token && i < 7) {
    strcpy(newDokter.hari[i++], token);

    strcat(hariJoin, token);
    token = strtok(NULL, " ");
    if (token != NULL) strcat(hariJoin, ";");
  }
  while (i < 7) strcpy(newDokter.hari[i++], "");

  // Parse shift
  token = strtok(shiftStr, " ");
  int j = 0;
  char shiftJoin[100] = "";

  while (token && j < 3) {
    strcpy(newDokter.shift[j++], token);

    strcat(shiftJoin, token);
    token = strtok(NULL, " ");
    if (token != NULL) strcat(shiftJoin, ";");
  }
  while (j < 3) strcpy(newDokter.shift[j++], "");

  // Tambah ke linked list
  ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
  newNode->data = newDokter;
  newNode->next = NULL;

  if (*head == NULL) {
    *head = newNode;
  } else {
    ListNode *temp = *head;
    while (temp->next != NULL) temp = temp->next;
    temp->next = newNode;
  }

  // 🔄 Re-write seluruh file dengan linked list terbaru
  FILE *file = fopen("../Data/data_dokter.csv", "w");
  if (file == NULL) {
    perror("Gagal membuka file CSV");
    return;
  }

  fprintf(file, "Id,Nama,MaxShift,Hari,Shift\n");

  ListNode *temp = *head;
  while (temp != NULL) {
    char hariJoin[100] = "", shiftJoin[100] = "";

    for (int i = 0; i < 7 && strlen(temp->data.hari[i]) > 0; i++) {
      strcat(hariJoin, temp->data.hari[i]);
      if (i < 6 && strlen(temp->data.hari[i + 1]) > 0) strcat(hariJoin, ";");
    }

    for (int i = 0; i < 3 && strlen(temp->data.shift[i]) > 0; i++) {
      strcat(shiftJoin, temp->data.shift[i]);
      if (i < 2 && strlen(temp->data.shift[i + 1]) > 0) strcat(shiftJoin, ";");
    }

    fprintf(file, "%d,%s,%d,%s,%s\n",
            temp->data.id,
            temp->data.nama,
            temp->data.maxShift,
            hariJoin,
            shiftJoin);

    temp = temp->next;
  }

  fclose(file);
  printf("Dokter berhasil ditambahkan dan file CSV diperbarui.\n");
}

ListNode *createDokterList() {
  ListNode *head = NULL;
  ListNode *current = NULL;
  ListNode *newNode = NULL;

  FILE *file = fopen("../Data/data_dokter.csv", "r");
  if (file == NULL) {
    printf("Error opening file!\n");
    return NULL;
  }

  char tempNama[50], tempHari[100], tempShift[50];
  int tempId, tempMaxShift;

  // Skip baris header karena merupakan nama kolom
  char line[100];
  fgets(line, sizeof(line), file);

  while (fscanf(file, "%d,%49[^,],%d,%99[^,],%49[^\n]", &tempId, tempNama, &tempMaxShift, tempHari, tempShift) == 5) {
    newNode = (ListNode*)malloc(sizeof(ListNode));

    newNode->data.id = tempId;
    strcpy(newNode->data.nama, tempNama);
    newNode->data.maxShift = tempMaxShift;

    // Proses hari
    char* hari = strtok(tempHari, ";");
    int j = 0;
    while (hari != NULL && j < 7) {
      strcpy(newNode->data.hari[j++], hari);
      hari = strtok(NULL, ";");
    }

    // Proses shift
    char* shift = strtok(tempShift, ";");
    int k = 0;
    while (shift != NULL && k < 3) {
      strcpy(newNode->data.shift[k++], shift);
      shift = strtok(NULL, ";");
    }

    newNode->next = NULL;

    if (head == NULL) {
      head = newNode;
      current = newNode;
    } else {
      current->next = newNode;
      current = newNode;
    }
  }

  fclose(file);
  return (head);
}

void hapusDokter(ListNode **head, int targetId) {
  ListNode *current = *head, *prev = NULL;

  // Cari node yang ingin dihapus
  while (current != NULL && current->data.id != targetId) {
    prev = current;
    current = current->next;
  }

  if (current == NULL) {
    printf("Dokter dengan ID %d tidak ditemukan.\n", targetId);
    return;
  }

  // Hapus dari linked list
  if (prev == NULL) {
    *head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);

  // Tulis ulang ke file CSV
  FILE *file = fopen("../Data/data_dokter.csv", "w");
  if (file == NULL) {
    printf("Gagal membuka file.\n");
    return;
  }

  // Header
  fprintf(file, "Id,Nama,MaxShift,Hari,Shift\n");

  ListNode *temp = *head;
  while (temp != NULL) {
    // Gabungkan hari dan shift jadi string CSV
    char hariJoin[100] = "", shiftJoin[100] = "";
    for (int i = 0; i < 7 && strlen(temp->data.hari[i]) > 0; i++) {
      strcat(hariJoin, temp->data.hari[i]);
      if (i < 6 && strlen(temp->data.hari[i + 1]) > 0) strcat(hariJoin, ";");
    }
    for (int i = 0; i < 3 && strlen(temp->data.shift[i]) > 0; i++) {
      strcat(shiftJoin, temp->data.shift[i]);
      if (i < 2 && strlen(temp->data.shift[i + 1]) > 0) strcat(shiftJoin, ";");
    }

    fprintf(file, "%d,%s,%d,%s,%s\n", temp->data.id, temp->data.nama, temp->data.maxShift, hariJoin, shiftJoin);
    temp = temp->next;
  }

  fclose(file);
  printf("Dokter dengan ID %d berhasil dihapus.\n", targetId);
}

int main() {
  ListNode *dokters = createDokterList();
  displayDokters(dokters);

  tambahDokter(&dokters);
  displayDokters(dokters);

  int idHapus;
  printf("Masukkan ID dokter yang ingin dihapus: ");
  scanf("%d", &idHapus);
  hapusDokter(&dokters, idHapus);
  displayDokters(dokters);

  return 0;
}

