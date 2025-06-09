#include "penjadwalan.h"

int isPrefered(int size,char stringComp[],char arrayPref[][10]){
    char stringCleanse[10];
    char stringCompCleanse[10];
    strncpy(stringCompCleanse,stringComp,10);
    stringCompCleanse[strcspn(stringCompCleanse, "\r\t\n")] = '\0';
    for (int i =0;i<size;i++){
        strncpy(stringCleanse,arrayPref[i],10);
        stringCleanse[strcspn(stringCleanse, "\r\t\n")] = '\0'; // Menghapus newline jika ada
        if (strcasecmp(stringCompCleanse, stringCleanse) == 0){
            return (0);
        }
    }
    return (1);
}

int checkUniqueViolation(ViolationData ArrayVio[],int sizeArray,Dokter dicari){
    for (int i=0;i<sizeArray;i++){
        if (ArrayVio[i].dokter.id==dicari.id){
            return (i);
        }
    }
    return (-1);
}

DoctorViolation assignDokter(ListNode* daftarDokter,char hari[], char shift[], HariKalender Jadwal[],int hariLewat,int currentShift){
    ListNode *traversalNode = daftarDokter;
    DoctorViolation tempMatch;
    if (traversalNode!=NULL){
        tempMatch.doctor=traversalNode->data;
    }
    tempMatch.violations=999;//dummy value yang cukup besar
    
    while (traversalNode!=NULL){
        int violations=0;


        if (traversalNode->data.maxShift > findDoctorShift(traversalNode->data.id,Jadwal,hariLewat)){
            int isDuplicate = 0;
            
            for (int i = 0; i < Jadwal[hariLewat-1].kebutuhanDokter[currentShift]; i++) {
                if (traversalNode->data.id == Jadwal[hariLewat-1].ArrayDokter[i][currentShift].id) {
                    isDuplicate = 1;
                    break;
                }
            }

            if (!isDuplicate) {
                if (isPrefered(7,hari,traversalNode->data.hari)!=0){
                    violations+=1;
                }
                if (isPrefered(3,shift,traversalNode->data.shift)!=0){
                    violations+=1;
                }
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
        }
        traversalNode=traversalNode->next;
    }
    return (tempMatch);
}

int findDoctorShift(int ID,HariKalender arrayJadwal[],int hariLewat){
    int hariMingguLalu=0;
    if (hariLewat > 7) {
        hariMingguLalu = (hariLewat - 7);
    }
    int count=0;

    for (int i=(hariMingguLalu);i<hariLewat;i++){ // untuk setiap hari yang lewat
        for (int j=0;j<3;j++){  //untuk setiap shift
            for (int k=0;k<arrayJadwal[i].kebutuhanDokter[j];k++){ //untuk setiap dokter
                if (arrayJadwal[i].ArrayDokter[k][j].id==ID){
                    count+=1;
                }
            }
        } 
    }
    return count;
}

void buatJadwal(HariKalender calendar[31],int* numViolations,ListNode* daftarDokter,dynamicArray* violationArray, dynamicArray* shiftArray) {
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
        
        switch(current->tm_wday) {
            case 0: strncpy(calendar[i].namaHari, "Minggu", 10); break;
            case 1: strncpy(calendar[i].namaHari, "Senin", 10); break;
            case 2: strncpy(calendar[i].namaHari, "Selasa", 10); break;
            case 3: strncpy(calendar[i].namaHari, "Rabu", 10); break;
            case 4: strncpy(calendar[i].namaHari, "Kamis", 10); break;
            case 5: strncpy(calendar[i].namaHari, "Jumat", 10); break;
            case 6: strncpy(calendar[i].namaHari, "Sabtu", 10); break;
        }

        strncpy(calendar[i].shift[0], "Pagi", 10);
        strncpy(calendar[i].shift[1], "Siang", 10);
        strncpy(calendar[i].shift[2], "Malam", 10);

        calendar[i].kebutuhanDokter[0]=2; //kebutuhan pagi
        calendar[i].kebutuhanDokter[1]=3; //siang
        calendar[i].kebutuhanDokter[2]=1; //malam

        for (int j = 0; j < 3; j++) {//untuk setiap shift
            for (int k=0;k<calendar[i].kebutuhanDokter[j];k++){//untuk setiap kebutuhan dokter pada shift
                DoctorViolation cari = assignDokter(daftarDokter, calendar[i].namaHari, calendar[i].shift[j], calendar, i+1,j);
                if (cari.violations!=999) {
                    ViolationData doctorShift;
                    doctorShift.dokter = cari.doctor;
                    for (int l=0;l<31;l++){//setting index yang tidak berisi menjadi default -1
                            doctorShift.indexViolation[0][l]=-1;
                            doctorShift.indexViolation[1][l]=-1;
                    }
                    int tempIndex = checkUniqueViolation(shiftArray->arrayViolation,shiftArray->size,cari.doctor);
                    if (tempIndex==-1){
                        doctorShift.totalViolations=1;
                        doctorShift.indexViolation[0][0]=i;
                        doctorShift.indexViolation[1][0]=j;
                        insertArray(shiftArray,doctorShift);
                    }
                    else {
                        shiftArray->arrayViolation[tempIndex].indexViolation[0][shiftArray->arrayViolation[tempIndex].totalViolations]=i;
                        shiftArray->arrayViolation[tempIndex].indexViolation[1][shiftArray->arrayViolation[tempIndex].totalViolations]=j;
                        shiftArray->arrayViolation[tempIndex].totalViolations+=1;
                    }
                    calendar[i].ArrayDokter[k][j] = cari.doctor;
                    if (cari.violations!=0){
                        ViolationData pelanggaran;
                        pelanggaran.dokter=cari.doctor;
                        for (int l=0;l<31;l++){//setting index yang tidak berisi menjadi default -1
                            pelanggaran.indexViolation[0][l]=-1;
                            pelanggaran.indexViolation[1][l]=-1;
                        }
                        int tempIndex = checkUniqueViolation(violationArray->arrayViolation,violationArray->size,cari.doctor);
                        if (tempIndex==-1){
                            pelanggaran.totalViolations=1;
                            pelanggaran.indexViolation[0][0]=i;
                            pelanggaran.indexViolation[1][0]=j;
                            insertArray(violationArray,pelanggaran);
                        }
                        else {
                            violationArray->arrayViolation[tempIndex].indexViolation[0][violationArray->arrayViolation[tempIndex].totalViolations]=i;
                            violationArray->arrayViolation[tempIndex].indexViolation[1][violationArray->arrayViolation[tempIndex].totalViolations]=j;
                            violationArray->arrayViolation[tempIndex].totalViolations+=1;
                        }
                        *numViolations += 1;
                    }
                } else {
                    // Handling kasus tidak ada dokter
                    calendar[i].ArrayDokter[k][j].id = -1; // tanda ID tidak assigned
                }
            }
            
        }
        //iterasi ke hari selanjutnya
        current->tm_mday += 1;
        mktime(current);
    }
}

void printJadwal(HariKalender calendar[], int size) {
    int hariTidakTerisi=0;
    for (int i = 0; i < size; i++) {
        printf("\nHari: %s (%d/%d/%d)\n", 
               calendar[i].namaHari, 
               calendar[i].dd, 
               calendar[i].mm, 
               calendar[i].yy);
        
        for (int j = 0; j < 3; j++) {
            printf("  Shift: %s\n", calendar[i].shift[j]);
            for (int k = 0; k<calendar[i].kebutuhanDokter[j];k++){
                if (calendar[i].ArrayDokter[k][j].id!=-1){
                    printf("  Nama Dokter: %s\n", calendar[i].ArrayDokter[k][j].nama);
                }
                else {
                    hariTidakTerisi+=1;
                    printf("  Tidak Ada Dokter tersedia!\n");
                }
            }
            printf("  ------------------\n");
        }
    }
    printf("\nJumlah shift tidak terisi: %d\n", hariTidakTerisi);
}

void printPelanggaran(dynamicArray arrayViolation, HariKalender Jadwal[]) {
    for (int i = 0; i < arrayViolation.used; i++) {
        ViolationData *current = &arrayViolation.arrayViolation[i];
        
        printf("Dokter: %s\n", current->dokter.nama);
        printf("  Total pelanggaran: %d\n", current->totalViolations);
        printf("  Hari:\n");
        
        for (int j = 0; j < 31; j++) {
            if (current->indexViolation[0][j] != -1) {
                int dayIndex = current->indexViolation[0][j];
                printf("    -%s %s (%d/%d/%d)\n", 
                    Jadwal[dayIndex].namaHari, 
                    Jadwal[dayIndex].shift[current->indexViolation[1][j]],
                    Jadwal[dayIndex].dd, 
                    Jadwal[dayIndex].mm, 
                    Jadwal[dayIndex].yy
                );
            }
        }
        printf("\n");
    }
}

//Sama seperti printPelanggaran, hanya saja ini untuk shift
void printShift(dynamicArray arrayViolation, HariKalender Jadwal[]) {
    for (int i = 0; i < arrayViolation.used; i++) {
        ViolationData *current = &arrayViolation.arrayViolation[i];
        
        printf("Dokter: %s\n", current->dokter.nama);
        printf("  Total shift: %d\n", current->totalViolations);
        printf("  Hari:\n");
        
        for (int j = 0; j < 31; j++) {
            if (current->indexViolation[0][j] != -1) {
                int dayIndex = current->indexViolation[0][j];
                printf("    -%s %s (%d/%d/%d)\n", 
                    Jadwal[dayIndex].namaHari, 
                    Jadwal[dayIndex].shift[current->indexViolation[1][j]],
                    Jadwal[dayIndex].dd, 
                    Jadwal[dayIndex].mm, 
                    Jadwal[dayIndex].yy
                );
            }
        }
        printf("\n");
    }
}

void initArray(dynamicArray* array, size_t ukuranArray) {
    array->arrayViolation = (ViolationData*)malloc(ukuranArray * sizeof(ViolationData));
    array->used = 0;
    array->size = ukuranArray;
}

void insertArray(dynamicArray* array, ViolationData element) {
    if (array->used == array->size) {
        array->size *= 2;  // doubling size
        array->arrayViolation = (ViolationData*)realloc(array->arrayViolation, array->size * sizeof(ViolationData));
    }
    array->arrayViolation[array->used++] = element;
}

void freeArray(dynamicArray *array) {
    free(array->arrayViolation);
    array->arrayViolation = NULL;
    array->used = array->size = 0;
}