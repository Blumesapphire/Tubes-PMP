#include "dokter.h"
#include <time.h>
#include <math.h>

typedef struct DoctorViolation {
    Dokter doctor;
    int violations;
} DoctorViolation;

typedef struct {
    int dd;
    int mm;
    int yy;
    char namaHari[10];
    char shift[3][10]; // shift [slot shift][nama shift (Pagi,siang/malam)]
    int kebutuhanDokter[3]; // [nDokter][pagi/siang/malam]
    Dokter ArrayDokter[5][3]; //Dokter [N dokter][lokasi shift]Per Shift ada maksimum 5 dokter mengisi shift
} HariKalender;

typedef struct {
    Dokter dokter; //data dokter
    int total; //jumlah total violation/shift
    int indexHari[2][31];//lokasi di array kalender dimana violation/shift terjadi [hari][shift]
} DailyData;

typedef struct {
    DailyData *array;
    size_t used;
    size_t size;
} dynamicArray;


int isPrefered(int size,char stringComp[],char arrayPref[][10]);
DoctorViolation assignDokter(ListNode* daftarDokter,char hari[], char shift[], HariKalender Jadwal[],int hariLewat,int currentShift);
int findDoctorShift(int ID,HariKalender arrayJadwal[],int hariLewat);
void buatJadwal(HariKalender calendar[31],int* numViolations,ListNode* daftarDokter,dynamicArray* violationArray, dynamicArray* shiftArray);
void printJadwal(HariKalender calendar[], int size);
int checkUniqueViolation(DailyData ArrayVio[],int sizeArray,Dokter dicari); //Mencari dicari di ArrayVio dengan size sizeArray , mengembalikan -1 bila tidak ditemukan
void freeArray(dynamicArray *array);
void insertArray(dynamicArray* array, DailyData element);
void initArray(dynamicArray* array, size_t ukuranArray);
void printPelanggaran(dynamicArray arrayViolation, HariKalender Jadwal[]);
void printShift(dynamicArray arrayViolation, HariKalender Jadwal[]);