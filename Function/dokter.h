#ifndef DOKTER_H
#define DOKTER_H

typedef struct {
  int id; // Id dokternya
  char nama[50]; // Nama dokternya
  int maxShift; // Jumlah maksimal shift yang bisa diambil
  char hari[7][10]; // Array untuk menyimpan hari (Senin sampai Minggu)
  char shift[3][10]; // Array untuk menyimpan shift (Pagi, Siang, Malam)
} Dokter;

typedef struct ListNode {
  Dokter data;
  struct ListNode *next;
} ListNode;

ListNode *createDokterList();
void displayDokters(ListNode *head);

#endif