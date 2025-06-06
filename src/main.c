#include <stdlib.h>
#include "dokter.h"

int main() {
  ListNode *head = createDokterList();
  displayDokters(head);

  // Membersihkan memori (sederhana)
  while (head != NULL) {
    ListNode* temp = head;
    head = head->next;
    free(temp);
  }

  return (0);
}