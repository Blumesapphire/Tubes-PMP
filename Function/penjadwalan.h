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

typedef struct {
    int id;
    char nama[50];
    int maxShift;
    char hari[7][20]; // Array to hold up to 7 days
    char shift[3][20]; // Array to hold up to 3 shifts (Pagi, Siang, Malam)
} Dokter2;

typedef struct ListNode2 {
    Dokter2 data;
    struct ListNode2* next;
} ListNode2;

typedef struct {
    char tanggal[11]; // Format dd/mm/yyyy
    int pagi[5];     // Array untuk ID dokter shift pagi (maks 10)
    int siang[5];    // Array untuk ID dokter shift siang
    int malam[5];    // Array untuk ID dokter shift malam
    int pagiCount;    // Jumlah ID di pagi
    int siangCount;   // Jumlah ID di siang
    int malamCount;   // Jumlah ID di malam
} Jadwal;

typedef struct JadwalNode {
    Jadwal data;
    struct JadwalNode* next;
} JadwalNode;

ListNode2* createDokterList();
JadwalNode* createJadwalList();
ListNode2* findDokterById(ListNode2* head, int id);
void displaySchedule(ListNode2* dokterHead, JadwalNode* jadwalHead);
void displayDoctorsByDate(ListNode2 *dokterHead, JadwalNode* jadwalHead);

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
int tanggalSudahAda(const char* namaFile, const char* tanggal);
void simpanJadwalKeCSV(HariKalender calendar[], int size, const char* namaFile);