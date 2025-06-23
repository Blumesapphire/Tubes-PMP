#include "penjadwalan.h"
#include "dokter.h" 
#include <time.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <ctype.h>

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

int findDoctorShift(int ID, HariKalender arrayJadwal[], int hariLewat){
    for (int j = 0; j < 3; j++){ 
        for (int k = 0; k < 5; k++){
            if (arrayJadwal[hariLewat].ArrayDokter[k][j].id == ID){
                return j;
            }
        }
    }
    return -1; 
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

void buatJadwal(HariKalender calendar[61], int* numViolations, ListNode* daftarDokter, dynamicArray* violationArray, dynamicArray* shiftArray) {
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
                DailyData doctorShift;
                doctorShift.dokter = cari.doctor;
                doctorShift.total = 1;
                for (int l = 0; l < 90; l++) {
                    doctorShift.indexHari[0][l] = -1;
                    doctorShift.indexHari[1][l] = -1;
                }
                doctorShift.indexHari[0][0] = i;
                doctorShift.indexHari[1][0] = j;

                int tempIndex = findUniqueViolation(shiftArray->array, shiftArray->used, cari.doctor);
                if (tempIndex == -1) {
                    insertArray(shiftArray, doctorShift);
                } else {
                    int k = 0;
                    while (k < 90 && shiftArray->array[tempIndex].indexHari[0][k] != -1) k++;
                    if (k < 90) {
                        shiftArray->array[tempIndex].indexHari[0][k] = i;
                        shiftArray->array[tempIndex].indexHari[1][k] = j;
                    }
                    shiftArray->array[tempIndex].total++;
                }

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
                    DailyData doctorShift;
                    doctorShift.dokter = cari.doctor;
                    doctorShift.total = 1;
                    for (int l = 0; l < 90; l++) {
                        doctorShift.indexHari[0][l] = -1;
                        doctorShift.indexHari[1][l] = -1;
                    }
                    doctorShift.indexHari[0][0] = i;
                    doctorShift.indexHari[1][0] = j;

                    int tempIndex = findUniqueViolation(shiftArray->array, shiftArray->used, cari.doctor);
                    if (tempIndex == -1) {
                        insertArray(shiftArray, doctorShift);
                    } else {
                        int m = 0;
                        while (m < 90 && shiftArray->array[tempIndex].indexHari[0][m] != -1) m++;
                        if (m < 90) {
                            shiftArray->array[tempIndex].indexHari[0][m] = i;
                            shiftArray->array[tempIndex].indexHari[1][m] = j;
                        }
                        shiftArray->array[tempIndex].total++;
                    }

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
    
    g_string_append(buffer, "\n--- Doctor Shift Totals ---\n\n");
    extern dynamicArray global_shift_array; 
    if (global_shift_array.used == 0) {
        g_string_append(buffer, "No shifts assigned yet or no doctors registered.\n");
    } else {
        for (size_t i = 0; i < global_shift_array.used; i++) {
            DailyData *ds = &(global_shift_array.array[i]);
            g_string_append_printf(buffer, "  Doctor: %s (ID: %d)\n", ds->dokter.nama, ds->dokter.id);
            g_string_append_printf(buffer, "    Total Shifts Assigned: %d\n", ds->total);
            g_string_append(buffer, "    Assigned Shifts:\n");
            
            int shifts_this_week = 0;
            int current_week_start_day_idx = 0;
            typedef struct {
                int day_idx;
                int shift_idx;
            } ShiftDetail;

            ShiftDetail temp_shift_details[90];
            int num_shifts_to_display = 0;
            for (int j = 0; j < 90; j++) {
                if (ds->indexHari[0][j] != -1 && ds->indexHari[1][j] != -1) {
                    temp_shift_details[num_shifts_to_display].day_idx = ds->indexHari[0][j];
                    temp_shift_details[num_shifts_to_display].shift_idx = ds->indexHari[1][j];
                    num_shifts_to_display++;
                }
            }
            for (int x = 0; x < num_shifts_to_display - 1; x++) {
                for (int y = 0; y < num_shifts_to_display - x - 1; y++) {
                    if (temp_shift_details[y].day_idx > temp_shift_details[y+1].day_idx) {
                        ShiftDetail temp_detail = temp_shift_details[y];
                        temp_shift_details[y] = temp_shift_details[y+1];
                        temp_shift_details[y+1] = temp_detail;
                    }
                }
            }


            for (int j = 0; j < num_shifts_to_display; j++) {
                int day_idx = temp_shift_details[j].day_idx;
                int shift_idx = temp_shift_details[j].shift_idx;

                if (day_idx != -1 && shift_idx != -1) {
                    if (j > 0 && (day_idx / 7) > (current_week_start_day_idx / 7)) { 
                         g_string_append_printf(buffer, "  -- Shifts in Week %d: %d --\n", (current_week_start_day_idx / 7) + 1, shifts_this_week);
                         current_week_start_day_idx = (day_idx / 7) * 7;
                         shifts_this_week = 0;
                    }

                    g_string_append_printf(buffer, "  - %02d/%02d/%04d (%s) - %s\n",
                                           Jadwal[day_idx].dd, Jadwal[day_idx].mm, Jadwal[day_idx].yy,
                                           Jadwal[day_idx].namaHari, shifts_names[shift_idx]);
                    shifts_this_week++;
                }
            }
            if (num_shifts_to_display > 0 || ds->total == 0) {
                g_string_append_printf(buffer, "  -- Shifts in Week %d: %d --\n", (current_week_start_day_idx / 7) + 1, shifts_this_week);
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
