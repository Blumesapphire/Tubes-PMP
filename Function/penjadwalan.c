#include "penjadwalan.h"

int countJadwalNodes(JadwalNode* head) {
    int count = 0;
    JadwalNode* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return (count);
}

static void clean_string(char *str) {
    str[strcspn(str, "\n")] = '\0';
    str[strcspn(str, "\r")] = '\0';
    int i = strlen(str) - 1;
    while (i >= 0 && isspace((unsigned char)str[i])) {
        str[i] = '\0';
        i--;
    }
    int j = 0;
    while (str[j] != '\0' && isspace((unsigned char)str[j])) {
        j++;
    }
    if (j > 0) {
        memmove(str, str + j, strlen(str) - j + 1);
    }
}

int isPrefered(int size, char stringComp[], char arrayPref[][10]){
    char stringCompCleanse[10];
    strncpy(stringCompCleanse, stringComp, sizeof(stringCompCleanse) - 1);
    stringCompCleanse[sizeof(stringCompCleanse) - 1] = '\0';
    clean_string(stringCompCleanse);

    for (int i = 0; i < size; i++){
        char stringCleanse[10];
        strncpy(stringCleanse, arrayPref[i], sizeof(stringCleanse) - 1);
        stringCleanse[sizeof(stringCleanse) - 1] = '\0';
        clean_string(stringCleanse);
        if (strcasecmp(stringCompCleanse, stringCleanse) == 0){
            return 0; 
        }
    }
    return 1; 
}

int findUniqueViolation(DailyData ArrayVio[],int sizeArray,Dokter dicari){
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

void initArray(dynamicArray* array, size_t initialSize) {
    array->array = (DailyData*)malloc(initialSize * sizeof(DailyData));
    array->used = 0;
    array->size = initialSize;
}

void insertArray(dynamicArray* array, DailyData element) {
    if (array->used == array->size) {
        array->size *= 2;
        array->array = (DailyData*)realloc(array->array, array->size * sizeof(DailyData));
        if (array->array == NULL) {
            g_print("Error: Reallocation failed in insertArray.\n");
            exit(EXIT_FAILURE);
        }
    }
    array->array[array->used++] = element;
}

void freeArray(dynamicArray *array){
    free(array->array);
    array->array = NULL;
    array->used = 0;
    array->size = 0;
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

void buatJadwal(HariKalender calendar[61], int* numViolations, ListNode* daftarDokter, dynamicArray* violationArray) {
    time_t now;
    time(&now);
    struct tm *current = localtime(&now);
    current->tm_mday += 1; // Mulai dari besok

    // Handling kasus apabila besok bulan atau tahun baru
    mktime(current);
    
    for (int i = 31; i < 61; i++) { 
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 5; k++) {
                calendar[i].ArrayDokter[k][j].id = -1;
                calendar[i].ArrayDokter[k][j].nama[0] = '\0';
            }
        }
    }

    for (int i = 31; i < 61; i++) { 
        calendar[i].dd = current->tm_mday;
        calendar[i].mm = current->tm_mon + 1;
        calendar[i].yy = current->tm_year + 1900;
        
        switch(current->tm_wday) {
            case 0: strncpy(calendar[i].namaHari, "Minggu", 10); break;
            case 1: strncpy(calendar[i].namaHari, "Senin", 10); break;
            case 2: strncpy(calendar[i].namaHari, "Selasa", 10); break;
            case 3: strncpy(calendar[i].namaHari, "Rabu", 10); break;
            case 4: strncpy(calendar[i].namaHari, "Kamis", 10); break;
            case 5: strncpy(calendar[i].namaHari, "Jumat", 10); break;
            case 6: strncpy(calendar[i].namaHari, "Sabtu", 10); break;
        }
        calendar[i].namaHari[9] = '\0';

        strncpy(calendar[i].shift[0], "Pagi", 10); calendar[i].shift[0][9] = '\0';
        strncpy(calendar[i].shift[1], "Siang", 10); calendar[i].shift[1][9] = '\0';
        strncpy(calendar[i].shift[2], "Malam", 10); calendar[i].shift[2][9] = '\0';

        calendar[i].kebutuhanDokter[0] = 2; //kebutuhan pagi
        calendar[i].kebutuhanDokter[1] = 3; //siang
        calendar[i].kebutuhanDokter[2] = 1; //malam

        for (int j = 0; j < 3; j++) {
            DoctorViolation cari = assignDokter(daftarDokter, calendar[i].namaHari, calendar[i].shift[j], calendar, i+1, j);
            if (cari.violations != 999) {
                calendar[i].ArrayDokter[0][j] = cari.doctor;

                if (cari.violations != 0) {
                    DailyData pelanggaran;
                    pelanggaran.dokter = cari.doctor;
                    pelanggaran.total = 1;
                    for (int l = 0; l < 90; l++) {
                        pelanggaran.indexHari[0][l] = -1;
                        pelanggaran.indexHari[1][l] = -1;
                    }
                    pelanggaran.indexHari[0][0] = i;
                    pelanggaran.indexHari[1][0] = j;

                    int tempIndex = findUniqueViolation(violationArray->array, violationArray->used, cari.doctor);
                    if (tempIndex == -1) {
                        insertArray(violationArray, pelanggaran);
                    } else {
                        int k = 0;
                        while (k < 90 && violationArray->array[tempIndex].indexHari[0][k] != -1) k++;
                        if (k < 90) {
                            violationArray->array[tempIndex].indexHari[0][k] = i;
                            violationArray->array[tempIndex].indexHari[1][k] = j;
                        }
                        violationArray->array[tempIndex].total++;
                    }
                    (*numViolations)++;
                }
            }
        }
        current->tm_mday += 1;
        mktime(current);
    }

    // Round 2 untuk additional dokter
    for (int i = 31; i < 61; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 1; k < calendar[i].kebutuhanDokter[j]; k++) {
                DoctorViolation cari = assignDokter(daftarDokter, calendar[i].namaHari, calendar[i].shift[j], calendar, i+1, j);
                if (cari.violations != 999) {
                    calendar[i].ArrayDokter[k][j] = cari.doctor;

                    if (cari.violations != 0) {
                        DailyData pelanggaran;
                        pelanggaran.dokter = cari.doctor;
                        pelanggaran.total = 1;
                        for (int l = 0; l < 90; l++) {
                            pelanggaran.indexHari[0][l] = -1;
                            pelanggaran.indexHari[1][l] = -1;
                        }
                        pelanggaran.indexHari[0][0] = i;
                        pelanggaran.indexHari[1][0] = j;

                        int tempIndex = findUniqueViolation(violationArray->array, violationArray->used, cari.doctor);
                        if (tempIndex == -1) {
                            insertArray(violationArray, pelanggaran);
                        } else {
                            int m = 0;
                            while (m < 90 && violationArray->array[tempIndex].indexHari[0][m] != -1) m++;
                            if (m < 90) {
                                violationArray->array[tempIndex].indexHari[0][m] = i;
                                violationArray->array[tempIndex].indexHari[1][m] = j;
                            }
                            violationArray->array[tempIndex].total++;
                        }
                        (*numViolations)++;
                    }
                }
            }
        }
    }
}

void jadwalBentukArray(int size, ListNode *dokterHead,HariKalender* kalender) {
    JadwalNode* jadwalHead = createJadwalList();
    if (jadwalHead == NULL) {
        size = 0;
        return;
    }

    int totalDays = countJadwalNodes(jadwalHead);
    if (totalDays == 0) {
        size = 0;
        return;
    }

    // ngambil data untuk sekarang (real time)
    time_t t = time(NULL);
    struct tm* currentTime = localtime(&t);
    int currentDay = currentTime->tm_mday;
    int currentMonth = currentTime->tm_mon + 1;
    int currentYear = currentTime->tm_year + 1900;
    char currentDate[11];

    sprintf(currentDate, "%02d/%02d/%04d", currentDay, currentMonth, currentYear);

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
    if (currentPosition == -1 || currentPosition < 30) {
        startIndex = 0;
    } else {
        startIndex = currentPosition - 30;
    }

    if (currentPosition == -1) {
        endIndex = totalDays - 1;
    } else {
        if (currentPosition + 30 < totalDays) {
            endIndex = currentPosition + 30;
        } else {
            endIndex = totalDays - 1;
        }
    }

    int range = endIndex - startIndex + 1;

    currentJadwal = jadwalHead;
    currentIndex = 0;
    int arrayIndex = 0;

    while (currentJadwal != NULL && arrayIndex < range) {
        if (currentIndex >= startIndex && currentIndex <= endIndex) {

            sscanf(currentJadwal->data.tanggal, "%d/%d/%d", &kalender[arrayIndex].dd, &kalender[arrayIndex].mm, &kalender[arrayIndex].yy);

            for (int shift = 0; shift < 3; shift++) {
                kalender[arrayIndex].kebutuhanDokter[shift] = 0;
                for (int k = 0; k < 5; k++) {
                    kalender[arrayIndex].ArrayDokter[k][shift].id = -1;
                }
            }

            // Isi data dokter dari linked list
            for (int shift = 0; shift < 3; shift++) {
                int* count = NULL;
                int* id = NULL;
                if (shift == 0) {
                    count = &currentJadwal->data.pagiCount;
                    id = currentJadwal->data.pagi;
                } else if (shift == 1) {
                    count = &currentJadwal->data.siangCount;
                    id = currentJadwal->data.siang;
                } else if (shift == 2) {
                    count = &currentJadwal->data.malamCount;
                    id = currentJadwal->data.malam;
                }

                for (int i = 0; i < *count && i < 5; i++) {
                    ListNode* dokter = findDokterById(dokterHead, id[i]);
                    if (dokter != NULL) {
                        kalender[arrayIndex].ArrayDokter[i][shift] = dokter->data;
                        kalender[arrayIndex].kebutuhanDokter[shift]++;
                    }
                }
            }

            arrayIndex++;
        }
        currentJadwal = currentJadwal->next;
        currentIndex++;
    }

    while (jadwalHead != NULL) {
        JadwalNode* temp = jadwalHead;
        jadwalHead = jadwalHead->next;
        free(temp);
    }

    size = range;
}

char *formatScheduleToString(HariKalender calendar[], int size, ListNode* doctorHead) {
    GString *buffer = g_string_new("");
    g_string_append(buffer, "--- 30-Day Schedule ---\n\n");
    char *shifts_names[] = {"Pagi", "Siang", "Malam"};

    for (int i = 31; i < size; i++) {
        g_string_append_printf(buffer, "Date: %02d/%02d/%04d (%s)\n",
                               calendar[i].dd, calendar[i].mm, calendar[i].yy, calendar[i].namaHari);
        g_string_append(buffer, "  Shifts:\n");
        for (int s = 0; s < 3; s++) { 
            g_string_append_printf(buffer, "    %s: ", shifts_names[s]);
            int doctors_assigned = 0;
            for (int d = 0; d < 5; d++) { 
                if (calendar[i].ArrayDokter[d][s].id != -1) {
                    if (doctors_assigned > 0) g_string_append(buffer, ", ");
                    g_string_append_printf(buffer, "%s (ID: %d)",
                                           calendar[i].ArrayDokter[d][s].nama,
                                           calendar[i].ArrayDokter[d][s].id);
                    doctors_assigned++;
                }
            }
            if (doctors_assigned == 0) {
                g_string_append(buffer, "No doctor assigned");
            }
            g_string_append(buffer, "\n");
        }
        g_string_append(buffer, "\n");
    }
    return g_string_free(buffer, FALSE);
}

gchar* formatSingleDayScheduleToString(HariKalender *day) {
    GString *result = g_string_new(NULL);
    
    g_string_append_printf(result, "Schedule for %s, %02d/%02d/%04d\n\n",
                         day->namaHari, day->dd, day->mm, day->yy);
    
    for (int s = 0; s < 3; s++) {
        g_string_append_printf(result, "%s Shift:\n", day->shift[s]);
        
        int doctors_found = 0;
        for (int d = 0; d < day->kebutuhanDokter[s]; d++) {
            if (day->ArrayDokter[d][s].id != -1) {
                g_string_append_printf(result, "  - %s (ID: %d)\n", 
                                     day->ArrayDokter[d][s].nama, 
                                     day->ArrayDokter[d][s].id);
                doctors_found++;
            }
        }
        
        if (doctors_found == 0) {
            g_string_append(result, "  No doctors assigned\n");
        }
        
        g_string_append(result, "\n");
    }
    
    return g_string_free(result, FALSE);
}

char *formatViolationsToString(dynamicArray *arrayViolation, HariKalender Jadwal[]) {
    GString *buffer = g_string_new("");
    g_string_append(buffer, "--- Doctor Shift Summary and Violations ---\n\n");
    char *shifts_names[] = {"Pagi", "Siang", "Malam"};


    if (arrayViolation->used == 0) {
        g_string_append(buffer, "No violations recorded.\n");
    } else {
        g_string_append(buffer, "Violations:\n");
        for (size_t i = 0; i < arrayViolation->used; i++) {
            DailyData *ds = &(arrayViolation->array[i]);
            g_string_append_printf(buffer, "  Doctor: %s (ID: %d)\n", ds->dokter.nama, ds->dokter.id);
            g_string_append_printf(buffer, "    Total Violations: %d\n", ds->total);
            g_string_append(buffer, "    Violation Details:\n");
            for (int j = 0; j < 90; j++) {
                int day_idx = ds->indexHari[0][j];
                int shift_idx = ds->indexHari[1][j];
                if (day_idx != -1 && shift_idx != -1) {
                    g_string_append_printf(buffer, "      - %02d/%02d/%04d (%s) - %s (Violation)\n",
                                           Jadwal[day_idx].dd, Jadwal[day_idx].mm, Jadwal[day_idx].yy,
                                           Jadwal[day_idx].namaHari, shifts_names[shift_idx]);
                }
            }
            g_string_append(buffer, "\n");
        }
    }
      
    return g_string_free(buffer, FALSE);
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

    file = fopen(namaFile, "w");
    if (!file) {
        perror("Gagal membuka file CSV");
        return;
    }

    fprintf(file, "Tanggal,Pagi,Siang,Malam\n");
    
    for (int i = 0; i < size; i++) {
        char tanggal[20];
        snprintf(tanggal, sizeof(tanggal), "%02d/%02d/%04d", calendar[i].dd, calendar[i].mm, calendar[i].yy);

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
