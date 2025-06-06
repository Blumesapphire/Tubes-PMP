#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dokter.h"

void displayDokters(ListNode *head) {
  ListNode *current = head;
  while (current != NULL) {
    printf("ID: %d, Nama: %s, Max Shift: %d\n", current->data.id, current->data.nama, current->data.maxShift);
    printf("Hari: ");
    for (int i = 0; i < 7; i++) {
      if (strlen(current->data.hari[i]) > 0) {
        printf("%s", current->data.hari[i]);
        if (i < 6 && strlen(current->data.hari[i+1]) > 0) printf(", ");
      }
    }
    printf("\nShift: ");
    for (int i = 0; i < 3; i++) {
      if (strlen(current->data.shift[i]) > 0) {
        printf("%s", current->data.shift[i]);
        if (i < 2 && strlen(current->data.shift[i+1]) > 0) printf(", ");
      }
    }
    printf("\n\n");
    current = current->next;
  }
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