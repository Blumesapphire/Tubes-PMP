#ifndef DOKTER_H
#define DOKTER_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h> 

#define MAX_HARI_INPUT_LEN 100 //Hari
#define MAX_SHIFT_INPUT_LEN 50 //Shift
#define MAX_HARI_ARRAY_SIZE 7  //Jumlah hari perminggu
#define MAX_SHIFT_ARRAY_SIZE 3 //Jumlah shift

typedef struct {
  int id; // Id dokternya
  char nama[50]; //Nama dokternya
  int maxShift; //Jumlah maksimal shift
  char hari[MAX_HARI_ARRAY_SIZE][10]; //Array untuk simpan hari
  char shift[MAX_SHIFT_ARRAY_SIZE][10]; //Array untuk simpan shift
} Dokter;
typedef struct ListNode {
  Dokter data;
  struct ListNode *next;
} ListNode;
ListNode *createDokterList();
char *formatDoktersToString(ListNode *head); 
ListNode* findDokterById(ListNode* head, int id); 
extern ListNode *doctor_head;


#endif 
