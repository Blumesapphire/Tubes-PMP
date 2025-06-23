#ifndef PENJADWALAN_H
#define PENJADWALAN_H

#include "dokter.h"
#include <time.h>
#include <math.h>
#include <sys/stat.h>

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
    int kebutuhanDokter[3]; // [nDokter][pagi/siang/malam]
    Dokter ArrayDokter[5][3]; //Dokter [N dokter][lokasi shift]Per Shift  maksimum 5 dokter mengisi shift
} HariKalender;

typedef struct {
    Dokter dokter; //data dokter
    int total; //jumlah total violation/shift
    int indexHari[2][90];//lokasi di array kalender dimana violation/shift terjadi [hari][shift] - Increased size to 90
} DailyData;

typedef struct {
    DailyData *array;
    size_t used;
    size_t size;
} dynamicArray;


int isPrefered(int size,char stringComp[],char arrayPref[][10]);
DoctorViolation assignDokter(ListNode* daftarDokter,char hari[], char shift[], HariKalender Jadwal[],int hariLewat,int currentShift);
int findDoctorShift(int ID,HariKalender arrayJadwal[],int hariLewat);
void buatJadwal(HariKalender calendar[61],int* numViolations,ListNode* daftarDokter,dynamicArray* violationArray);
int checkUniqueViolation(DailyData ArrayVio[],int sizeArray,Dokter dicari); 
void freeArray(dynamicArray *array);
void insertArray(dynamicArray* array, DailyData element);
void initArray(dynamicArray* array, size_t ukuranArray);


char *formatScheduleToString(HariKalender calendar[], int size, ListNode* doctorHead);
char *formatViolationsToString(dynamicArray *arrayViolation, HariKalender Jadwal[]);
char *formatSingleDayScheduleToString(HariKalender *daySchedule); 
void simpanJadwalKeCSV(HariKalender calendar[], int size, const char* namaFile);

extern HariKalender global_schedule[61];
extern dynamicArray global_violation_array;
extern int global_num_violations;


#endif 