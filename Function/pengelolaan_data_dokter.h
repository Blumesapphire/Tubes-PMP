#ifndef PENGELOLAAN_DATA_DOKTER_H
#define PENGELOLAAN_DATA_DOKTER_H

#include "dokter.h"

int addDokter_gui(ListNode **head, int id, const char *nama, int maxShift, const char *hari_str, const char *shift_str);
int hapusDokter_gui(ListNode **head, int targetId);
void saveDoktersToCSV(ListNode *head);

#endif
