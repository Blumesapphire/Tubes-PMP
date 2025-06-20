#include "dokter.h"
#include "penjadwalan.h"

int main(void){
    int numViolations;
    ListNode *head = createDokterList();
    displayDokters(head);
    HariKalender Jadwal[31];
    dynamicArray dynArrayViolation;
    dynamicArray dynArrayShift;
    initArray(&dynArrayShift,1);
    initArray(&dynArrayViolation,1);
    buatJadwal (Jadwal,&numViolations,head,&dynArrayViolation,&dynArrayShift);
    printJadwal(Jadwal,30);
    printPelanggaran(dynArrayViolation,Jadwal);
    printShift(dynArrayShift,Jadwal);
    simpanJadwalKeCSV(Jadwal, 30, "Data/jadwal.csv");
    // Membersihkan memori (sederhana)
    while (head != NULL) {
        ListNode* temp = head;
        head = head->next;
        free(temp);
    }
    return (0);
}