#include "dokter.h"
#include "penjadwalan.h"

int main(void){
    int numViolations;
    ListNode *head = createDokterList();
    displayDokters(head);
    HariKalender Jadwal[31];
    buatJadwal (Jadwal,&numViolations,head);
    printJadwal(Jadwal,30);
    printf ("Jumlah pelanggaran shift dan hari :%d",numViolations);
    // Membersihkan memori (sederhana)
    while (head != NULL) {
        ListNode* temp = head;
        head = head->next;
        free(temp);
    }
    return (0);
}