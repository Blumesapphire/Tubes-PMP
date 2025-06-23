#include "dokter.h"
#include "pengelolaan_data_dokter.h" 
#include <glib.h> 

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

