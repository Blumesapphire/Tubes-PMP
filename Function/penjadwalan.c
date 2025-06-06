#include "penjadwalan.h"

int isPrefered(int size,char stringComp[],char arrayPref[][10]){
    for (int i =0;i<size;i++){
        if (strcmp(stringComp,arrayPref[i])==0){
            return (0);
        }
    }
    return (1);
}

DoctorViolation assignDokter(ListNode* daftarDokter,char hari[], char shift[], HariKalender Jadwal[],int hariLewat){
    ListNode *traversalNode = daftarDokter;
    DoctorViolation tempMatch;
    if (traversalNode!=NULL){
        tempMatch.doctor=traversalNode->data;
    }
    tempMatch.violations=999;//dummy value yang cukup besar
    
    while (traversalNode!=NULL){
        int violations=0;

        if (isPrefered(7,shift,traversalNode->data.hari)!=0){
            violations+=1;
        }
        if (isPrefered(3,shift,traversalNode->data.shift)!=0){
            violations+=1;
        }

        if (traversalNode->data.maxShift > findDoctorShift(traversalNode->data.id,Jadwal,hariLewat)){
            if (violations==0){
                tempMatch.doctor=traversalNode->data;
                tempMatch.violations=violations;
                break;
            }
            else if (violations<tempMatch.violations){
                tempMatch.doctor=traversalNode->data;
                tempMatch.violations=violations;
            }
        }
        traversalNode=traversalNode->next;
    }
    return (tempMatch);
}

int findDoctorShift(int ID,HariKalender arrayJadwal[],int hariLewat){
    int count=0;
    for (int i=0;i<hariLewat;i++){
        for (int j=0;j<3;j++){
            if (arrayJadwal[i].dokter[j].id==ID){
            count+=1;
            }
        } 
    }
    return count;
}

void buatJadwal(HariKalender calendar[31],int* numViolations,ListNode* daftarDokter) {
    time_t now;
    time(&now);
    struct tm *current = localtime(&now);
    current->tm_mday += 1; // Mulai dari besok

    // Handling kasus apabila besok bulan atau tahun baru
    mktime(current);
    
    for (int i = 0; i < 31; i++) {
        calendar[i].dd = current->tm_mday;
        calendar[i].mm = current->tm_mon + 1; // time h mulai 0-11
        calendar[i].yy = current->tm_year + 1900; // time.h mulai dai 1900
        
        // Day name
        switch(current->tm_wday) {
            case 0: strncpy(calendar[i].namaHari, "Minggu", 10); break;
            case 1: strncpy(calendar[i].namaHari, "Senin", 10); break;
            case 2: strncpy(calendar[i].namaHari, "Selasa", 10); break;
            case 3: strncpy(calendar[i].namaHari, "Rabu", 10); break;
            case 4: strncpy(calendar[i].namaHari, "Kamis", 10); break;
            case 5: strncpy(calendar[i].namaHari, "Jumat", 10); break;
            case 6: strncpy(calendar[i].namaHari, "Sabtu", 10); break;
        }

        // Shifts
        strncpy(calendar[i].shift[0], "Pagi", 10);
        strncpy(calendar[i].shift[1], "Siang", 10);
        strncpy(calendar[i].shift[2], "Malam", 10);

        for (int j = 0; j < 3; j++) {
            DoctorViolation cari = assignDokter(daftarDokter, calendar[i].namaHari, calendar[i].shift[j], calendar, i+1);
            if (cari.violations!=999) {
                calendar[i].dokter[j] = cari.doctor;
                *numViolations += cari.violations;
            } else {
                // Handling kasus tidak ada dokter
                calendar[i].dokter[j].id = -1; // tanda ID tidak assigned
                *numViolations += -0;
            }
        }
        //iterasi ke hari selanjutnya
        current->tm_mday += 1;
        mktime(current);
    }
}

void printJadwal(HariKalender calendar[], int size) {
    for (int i = 0; i < size; i++) {
        printf("\nHari: %s (%d/%d/%d)\n", 
               calendar[i].namaHari, 
               calendar[i].dd, 
               calendar[i].mm, 
               calendar[i].yy);
        
        for (int j = 0; j < 3; j++) {
            printf("  Shift: %s\n", calendar[i].shift[j]);
            if (calendar[i].dokter[j].id!=-1){
                printf("  Nama Dokter: %s\n", calendar[i].dokter[j].nama);
            }
            else {
                printf("  Tidak Ada Dokter tersedia!\n");
            }
            printf("  ------------------\n");
        }
    }
}