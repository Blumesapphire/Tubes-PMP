#include "dokter.h"
#include <time.h>

typedef struct DoctorViolation {
    Dokter doctor;
    int violations;
} DoctorViolation;


typedef struct {
    int dd;
    int mm;
    int yy;
    char namaHari[10];
    char shift[3][10];
    Dokter dokter[3];
} HariKalender;

int isPrefered(int size,char stringComp[],char arrayPref[][10]);
DoctorViolation assignDokter(ListNode* daftarDokter,char hari[], char shift[], HariKalender Jadwal[],int hariLewat);
int findDoctorShift(int ID,HariKalender arrayJadwal[],int hariLewat);
void buatJadwal(HariKalender calendar[31],int* numViolations,ListNode* daftarDokter);
void printJadwal(HariKalender calendar[], int size);