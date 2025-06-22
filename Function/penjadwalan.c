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

int checkUniqueViolation(DailyData ArrayVio[],int sizeArray,Dokter dicari){
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
            DoctorViolation cari = assignDokter(daftarDokter, calendar[i].namaHari, calendar[i].shift[j], calendar, i+1,j);
            if (cari.violations!=999) {
                DailyData doctorShift;
                doctorShift.dokter = cari.doctor;
                for (int l=0;l<31;l++){//setting index yang tidak berisi menjadi default -1
                        doctorShift.indexHari[0][l]=-1;
                        doctorShift.indexHari[1][l]=-1;
                }
                int tempIndex = checkUniqueViolation(shiftArray->array,shiftArray->size,cari.doctor);
                if (tempIndex==-1){
                    doctorShift.total=1;
                    doctorShift.indexHari[0][0]=i;
                    doctorShift.indexHari[1][0]=j;
                    insertArray(shiftArray,doctorShift);
                }
                else {
                    shiftArray->array[tempIndex].indexHari[0][shiftArray->array[tempIndex].total]=i;
                    shiftArray->array[tempIndex].indexHari[1][shiftArray->array[tempIndex].total]=j;
                    shiftArray->array[tempIndex].total+=1;
                }
                calendar[i].ArrayDokter[0][j] = cari.doctor;
                if (cari.violations!=0){
                    DailyData pelanggaran;
                    pelanggaran.dokter=cari.doctor;
                    for (int l=0;l<31;l++){//setting index yang tidak berisi menjadi default -1
                        pelanggaran.indexHari[0][l]=-1;
                        pelanggaran.indexHari[1][l]=-1;
                    }
                    int tempIndex = checkUniqueViolation(violationArray->array,violationArray->size,cari.doctor);
                    if (tempIndex==-1){
                        pelanggaran.total=1;
                        pelanggaran.indexHari[0][0]=i;
                        pelanggaran.indexHari[1][0]=j;
                        insertArray(violationArray,pelanggaran);
                    }
                    else {
                        violationArray->array[tempIndex].indexHari[0][violationArray->array[tempIndex].total]=i;
                        violationArray->array[tempIndex].indexHari[1][violationArray->array[tempIndex].total]=j;
                        violationArray->array[tempIndex].total+=1;
                    }
                    *numViolations += 1;
                }
            } else {
                // Handling kasus tidak ada dokter
                calendar[i].ArrayDokter[0][j].id = -1; // tanda ID tidak assigned
            }
        }
        //iterasi ke hari selanjutnya
        current->tm_mday += 1;
        mktime(current);
    }
    for (int i = 0; i < 31; i++) {
        for (int j = 0; j < 3; j++) {//untuk setiap shift
            for (int k=1;k<calendar[i].kebutuhanDokter[j];k++){//untuk setiap kebutuhan dokter pada shift
                DoctorViolation cari = assignDokter(daftarDokter, calendar[i].namaHari, calendar[i].shift[j], calendar, i+1,j);
                if (cari.violations!=999) {
                    DailyData doctorShift;
                    doctorShift.dokter = cari.doctor;
                    for (int l=0;l<31;l++){//setting index yang tidak berisi menjadi default -1
                            doctorShift.indexHari[0][l]=-1;
                            doctorShift.indexHari[1][l]=-1;
                    }
                    int tempIndex = checkUniqueViolation(shiftArray->array,shiftArray->size,cari.doctor);
                    if (tempIndex==-1){
                        doctorShift.total=1;
                        doctorShift.indexHari[0][0]=i;
                        doctorShift.indexHari[1][0]=j;
                        insertArray(shiftArray,doctorShift);
                    }
                    else {
                        shiftArray->array[tempIndex].indexHari[0][shiftArray->array[tempIndex].total]=i;
                        shiftArray->array[tempIndex].indexHari[1][shiftArray->array[tempIndex].total]=j;
                        shiftArray->array[tempIndex].total+=1;
                    }
                    calendar[i].ArrayDokter[k][j] = cari.doctor;
                    if (cari.violations!=0){
                        DailyData pelanggaran;
                        pelanggaran.dokter=cari.doctor;
                        for (int l=0;l<31;l++){//setting index yang tidak berisi menjadi default -1
                            pelanggaran.indexHari[0][l]=-1;
                            pelanggaran.indexHari[1][l]=-1;
                        }
                        int tempIndex = checkUniqueViolation(violationArray->array,violationArray->size,cari.doctor);
                        if (tempIndex==-1){
                            pelanggaran.total=1;
                            pelanggaran.indexHari[0][0]=i;
                            pelanggaran.indexHari[1][0]=j;
                            insertArray(violationArray,pelanggaran);
                        }
                        else {
                            violationArray->array[tempIndex].indexHari[0][violationArray->array[tempIndex].total]=i;
                            violationArray->array[tempIndex].indexHari[1][violationArray->array[tempIndex].total]=j;
                            violationArray->array[tempIndex].total+=1;
                        }
                        *numViolations += 1;
                    }
                } else {
                    // Handling kasus tidak ada dokter
                    calendar[i].ArrayDokter[k][j].id = -1; // tanda ID tidak assigned
                }
            }
        }
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
        DailyData *current = &arrayViolation.array[i];
        
        printf("Dokter: %s\n", current->dokter.nama);
        printf("  Total pelanggaran: %d\n", current->total);
        printf("  Hari:\n");
        
        for (int j = 0; j < 31; j++) {
            if (current->indexHari[0][j] != -1) {
                int dayIndex = current->indexHari[0][j];
                printf("    -%s %s (%d/%d/%d)\n", 
                    Jadwal[dayIndex].namaHari, 
                    Jadwal[dayIndex].shift[current->indexHari[1][j]],
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
        DailyData *current = &arrayViolation.array[i];
        
        printf("Dokter: %s\n", current->dokter.nama);
        printf("  Total shift: %d\n", current->total);
        printf("  Hari:\n");
        
        for (int j = 0; j < 31; j++) {
            if (current->indexHari[0][j] != -1) {
                int dayIndex = current->indexHari[0][j];
                printf("    -%s %s (%d/%d/%d)\n", 
                    Jadwal[dayIndex].namaHari, 
                    Jadwal[dayIndex].shift[current->indexHari[1][j]],
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
    array->array= (DailyData*)malloc(ukuranArray * sizeof(DailyData));
    array->used = 0;
    array->size = ukuranArray;
}

void insertArray(dynamicArray* array, DailyData element) {
    if (array->used == array->size) {
        array->size *= 2;  // doubling size
        array->array = (DailyData*)realloc(array->array, array->size * sizeof(DailyData));
    }
    array->array[array->used++] = element;
}

void freeArray(dynamicArray *array) {
    free(array->array);
    array->array = NULL;
    array->used = array->size = 0;
}

int tanggalSudahAda(const char* namaFile, const char* tanggal) {
    FILE* file = fopen(namaFile, "r");
    if (!file) return 0;

    char baris[256];
    while (fgets(baris, sizeof(baris), file)) {
        if (strncmp(baris, tanggal, strlen(tanggal)) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void simpanJadwalKeCSV(HariKalender calendar[], int size, const char* namaFile) {
    FILE *file;
    struct stat st;
    int fileKosong = (stat(namaFile, &st) != 0 || st.st_size == 0);

    file = fopen(namaFile, "a");
    if (!file) {
        perror("Gagal membuka file CSV");
        return;
    }

    // Tulis header jika file kosong
    if (fileKosong) {
        fprintf(file, "Tanggal,Pagi,Siang,Malam\n");
    }

    for (int i = 0; i < size; i++) {
        char tanggal[20];
        snprintf(tanggal, sizeof(tanggal), "%02d/%02d/%04d", calendar[i].dd, calendar[i].mm, calendar[i].yy);

        if (tanggalSudahAda(namaFile, tanggal)) {
            continue;
        }

        fprintf(file, "%s", tanggal); //tulis tanggal
        for (int shift = 0; shift < 3; shift++) { //tulis pagi siang malam
            fprintf(file, ",");
            int kosong = 1;
            for (int k = 0; k < calendar[i].kebutuhanDokter[shift]; k++) {
                int id = calendar[i].ArrayDokter[k][shift].id;
                if (id != -1) {
                    if (!kosong) fprintf(file, ";");
                    fprintf(file, "%d", id);
                    kosong = 0;
                }
            }
            if (kosong) {
                fprintf(file, "(Kosong)");
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);
    printf("Jadwal berhasil ditambahkan ke file: %s\n", namaFile);
}

ListNode2* findDokterById(ListNode2* head, int id) {
    ListNode2* current = head;
    while (current != NULL) {
        if (current->data.id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// ngebuat linked list dari jadwal.csv
JadwalNode *createJadwalList() {
    JadwalNode *head = NULL;
    JadwalNode *current = NULL;
    JadwalNode *newNode = NULL;

    FILE *file = fopen("Data/jadwal.csv", "r");
    if (file == NULL) {
        printf("Gagal membuka file Data/jadwal.csv\n");
        return NULL;
    }

    char line[100];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char tempPagi[50], tempSiang[50], tempMalam[50];
        char tanggal[11];

        sscanf(line, "%10[^,],%49[^,],%49[^,],%49[^,]", tanggal, tempPagi, tempSiang, tempMalam);

        // dihitung dokter masing-masing sesi
        newNode = (JadwalNode*)malloc(sizeof(JadwalNode));
        strcpy(newNode->data.tanggal, tanggal);
        newNode->data.pagiCount = 0;
        newNode->data.siangCount = 0;
        newNode->data.malamCount = 0;

        // pagi
        char* token = strtok(tempPagi, ";");
        while (token != NULL && newNode->data.pagiCount < 5) {
            newNode->data.pagi[newNode->data.pagiCount++] = atoi(token);
            token = strtok(NULL, ";");
        }

        // siang
        token = strtok(tempSiang, ";");
        while (token != NULL && newNode->data.siangCount < 5) {
            newNode->data.siang[newNode->data.siangCount++] = atoi(token);
            token = strtok(NULL, ";");
        }

        // malam
        token = strtok(tempMalam, ";");
        while (token != NULL && newNode->data.malamCount < 5) {
            newNode->data.malam[newNode->data.malamCount++] = atoi(token);
            token = strtok(NULL, ";");
        }

        newNode->next = NULL;
        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
    }

    fclose(file);
    return (head);
}

// buat menghitung isi di jadwal.csvnya ada berapa
int countJadwalNodes(JadwalNode* head) {
    int count = 0;
    JadwalNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return (count);
}

void displaySchedule(ListNode2 *dokterHead, JadwalNode* jadwalHead) {
    // ngambil data untuk sekarang (real time)
    time_t t = time(NULL);
    struct tm* currentTime = localtime(&t);
    int currentDay = currentTime->tm_mday;
    int currentMonth = currentTime->tm_mon + 1;
    int currentYear = currentTime->tm_year + 1900;
    char currentDate[11];

    sprintf(currentDate, "%02d/%02d/%04d", currentDay, currentMonth, currentYear);

    // hitung total data di jadwal.csv ada berapa
    // kalau ada 0 maka tidak ada isinya, langsung saja keluar dari program
    int totalJadwal = countJadwalNodes(jadwalHead);
    if (totalJadwal == 0) {
        return;
    }

    // program ini menampilkan: 
    // 10 hari sebelum
    // sekarang
    // 10 hari dari sekarang
    // Nyari posisi (indeks) tanggal saat ini agar bisa menampilkan harapan output program
    JadwalNode* currentJadwal = jadwalHead;
    int currentIndex = 0;
    int currentPosition = -1;
    while (currentJadwal != NULL) {
        if (strcmp(currentJadwal->data.tanggal, currentDate) == 0) {
            currentPosition = currentIndex;
            break;
        }
        currentJadwal = currentJadwal->next;
        currentIndex++;
    }

    int startIndex, endIndex;
    // kalau total jadwal kurang dari (10 sebelum + sekarang + 10 setelah) = 21 data
    // tampilkan seadanya
    if (currentPosition == -1 || totalJadwal < 21) {
        startIndex = 0;
        endIndex = totalJadwal - 1;
    } else {
        startIndex = currentPosition - 10;
        endIndex = currentPosition + 10;
        if (startIndex < 0) startIndex = 0;
        if (endIndex >= totalJadwal) endIndex = totalJadwal - 1;
    }

    currentJadwal = jadwalHead;
    currentIndex = 0;
    while (currentJadwal != NULL) {
        if (currentIndex >= startIndex && currentIndex <= endIndex) {
            printf("%s\n", currentJadwal->data.tanggal);
            printf("Pagi\n");
            for (int i = 0; i < currentJadwal->data.pagiCount; i++) {
                ListNode* dokter = findDokterById(dokterHead, currentJadwal->data.pagi[i]);
                if (dokter != NULL) {
                    printf("%s\n", dokter->data.nama);
                }
            }
            printf("Siang\n");
            for (int i = 0; i < currentJadwal->data.siangCount; i++) {
                ListNode* dokter = findDokterById(dokterHead, currentJadwal->data.siang[i]);
                if (dokter != NULL) {
                    printf("%s\n", dokter->data.nama);
                }
            }
            printf("Malam\n");
            for (int i = 0; i < currentJadwal->data.malamCount; i++) {
                ListNode* dokter = findDokterById(dokterHead, currentJadwal->data.malam[i]);
                if (dokter != NULL) {
                    printf("%s\n", dokter->data.nama);
                }
            }
            printf("\n");
        }
        currentJadwal = currentJadwal->next;
        currentIndex++;
    }
}

// fungsi untuk ngari dokter pada tanggal tertentu
void displayDoctorsByDate(ListNode2 *dokterHead, JadwalNode *jadwalHead) {
    char inputDate[11];
    printf("Masukkan tanggal (dd/mm/yyyy): ");
    scanf("%s", inputDate);

    // cek apakah ada tanggal yang sama dengan yang diinput
    JadwalNode *currentJadwal = jadwalHead;
    while (currentJadwal != NULL) {
        if (strcmp(currentJadwal->data.tanggal, inputDate) == 0) {
            break;
        }
        currentJadwal = currentJadwal->next;
    }

    printf("%s\n", inputDate);
    printf("Pagi:\n");
    if (currentJadwal != NULL) {
        for (int i = 0; i < currentJadwal->data.pagiCount; i++) {
            ListNode* dokter = findDokterById(dokterHead, currentJadwal->data.pagi[i]);
            if (dokter != NULL) {
                printf("%s\n", dokter->data.nama);
            }
        }
    }

    printf("Siang:\n");
    if (currentJadwal != NULL) {
        for (int i = 0; i < currentJadwal->data.siangCount; i++) {
            ListNode* dokter = findDokterById(dokterHead, currentJadwal->data.siang[i]);
            if (dokter != NULL) {
                printf("%s\n", dokter->data.nama);
            }
        }
    }

    printf("Malam:\n");
    if (currentJadwal != NULL) {
        for (int i = 0; i < currentJadwal->data.malamCount; i++) {
            ListNode* dokter = findDokterById(dokterHead, currentJadwal->data.malam[i]);
            if (dokter != NULL) {
                printf("%s\n", dokter->data.nama);
            }
        }
    }
}