#include "dokter.h"
#include "penjadwalan.h"

int main(void){
    int numViolations;
    ListNode *head = createDokterList();
    displayDokters(head);
    HariKalender Jadwal[31];
    dynamicArray dynArrayViolation;
    initArray(&dynArrayViolation,1);
    buatJadwal (Jadwal,&numViolations,head,&dynArrayViolation);
    printJadwal(Jadwal,30);
    printPelanggaran(dynArrayViolation,Jadwal);
    printf ("Jumlah pelanggaran shift dan hari :%d",numViolations);
    // Membersihkan memori (sederhana)
    while (head != NULL) {
        ListNode* temp = head;
        head = head->next;
        free(temp);
    }
    return (0);
}