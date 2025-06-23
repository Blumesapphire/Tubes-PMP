#include <gtk/gtk.h>
#include "dokter.h"
#include "penjadwalan.h"
#include "pengelolaan_data_dokter.h"
#include <windows.h>
#include <glib/gprintf.h>

ListNode *doctor_head = NULL;
HariKalender global_schedule[61]; 
dynamicArray global_violation_array;

int global_num_violations = 0;
GtkTextView *doctor_display_text_view;
GtkTextView *add_doctor_output_text_view;
GtkEntry *add_id_entry;
GtkEntry *add_nama_entry;
GtkEntry *add_max_shift_entry;

GtkCheckButton *day_check_buttons[7];
GtkCheckButton *shift_check_buttons[3];

GtkTextView *schedule_display_text_view;
GtkTextView *shift_violation_display_text_view;
GtkEntry *schedule_date_entry;

GtkWidget *calendarVio_window;
GtkWidget *calendarVio_widget;
GtkTextView *calendar_vio_view;

GtkWidget *calendar_window;
GtkWidget *calendar_widget;
GtkTextView *calendar_schedule_view;

GtkWidget *doctor_dropdown;
GtkTreeStore *doctor_store;

static void on_calendarVio_day_selected(GtkCalendar *calendar, gpointer user_data);

static void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
    guint year, month, day;
    gtk_calendar_get_date(calendar, &year, &month, &day);
    month += 1; // GTK months are 0-11

    // Find the matching schedule entry
    int found_index = -1;
    for (int i = 0; i < 61; i++) {
        if (global_schedule[i].dd == day && 
            global_schedule[i].mm == month && 
            global_schedule[i].yy == year) {
            found_index = i;
            break;
        }
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(calendar_schedule_view);
    if (found_index != -1) {
        gchar *schedule_text = formatSingleDayScheduleToString(&global_schedule[found_index]);
        gtk_text_buffer_set_text(buffer, schedule_text, -1);
        g_free(schedule_text);
    } else {
        gtk_text_buffer_set_text(buffer, "No schedule available for selected date", -1);
    }
}

GtkWidget* create_calendar_tab(void) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    // Calendar
    calendar_widget = gtk_calendar_new();
    g_signal_connect(calendar_widget, "day-selected", 
                    G_CALLBACK(on_calendar_day_selected), NULL);
    gtk_box_pack_start(GTK_BOX(box), calendar_widget, FALSE, FALSE, 0);

    // Schedule display view
    calendar_schedule_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(calendar_schedule_view, FALSE);
    gtk_text_view_set_wrap_mode(calendar_schedule_view, GTK_WRAP_WORD);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(calendar_schedule_view));
    gtk_box_pack_start(GTK_BOX(box), scrolled, TRUE, TRUE, 0);

    return box;
}

gboolean doctor_has_violation_on_date(int doctor_id, int day, int month, int year) {
    for (int i = 0; i < global_violation_array.used; i++) {
        if (global_violation_array.array[i].dokter.id == doctor_id) {
            for (int j = 0; j < 90; j++) {
                int day_idx = global_violation_array.array[i].indexHari[0][j];
                if (day_idx == -1) break;
                
                if (global_schedule[day_idx].dd == day &&
                    global_schedule[day_idx].mm == month &&
                    global_schedule[day_idx].yy == year) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

static void update_calendar_marks(GtkComboBox *dropdown, gpointer user_data) {
    GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
    GtkTreeIter iter;
    
    // Clear all existing marks
    gtk_calendar_clear_marks(GTK_CALENDAR(calendarVio_widget));
    
    if (!gtk_combo_box_get_active_iter(dropdown, &iter)) {
        return; // No selection
    }
    
    gint doctor_id;
    gtk_tree_model_get(GTK_TREE_MODEL(doctor_store), &iter, 1, &doctor_id, -1);
    
    // Mark days with violations
    for (int i = 0; i < 61; i++) {
        if (doctor_has_violation_on_date(doctor_id, 
                                       global_schedule[i].dd, 
                                       global_schedule[i].mm, 
                                       global_schedule[i].yy)) {
            gtk_calendar_mark_day(GTK_CALENDAR(calendarVio_widget), 
                                 global_schedule[i].dd);
        }
    }
    
    // Update violation details
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    gtk_text_buffer_set_text(buffer, "Select a marked date to view violations", -1);
}

void populate_doctor_dropdown() {
    doctor_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);
    GtkTreeIter iter;
    
    for (size_t i = 0; i < global_violation_array.used; i++) {
        gtk_tree_store_append(doctor_store, &iter, NULL);
        gtk_tree_store_set(doctor_store, &iter,
                          0, global_violation_array.array[i].dokter.nama,
                          1, global_violation_array.array[i].dokter.id,
                          -1);
    }
    
    gtk_combo_box_set_model(GTK_COMBO_BOX(doctor_dropdown), 
                           GTK_TREE_MODEL(doctor_store));
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(doctor_dropdown), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(doctor_dropdown), renderer,
                                  "text", 0, NULL);
}

static void on_calendarVio_day_selected(GtkCalendar *calendar, gpointer user_data) {
    GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
    GtkTreeIter iter;
    
    if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(doctor_dropdown), &iter)) {
        return; // No doctor selected
    }
    
    gint doctor_id;
    gtk_tree_model_get(GTK_TREE_MODEL(doctor_store), &iter, 1, &doctor_id, -1);
    
    guint year, month, day;
    gtk_calendar_get_date(calendar, &year, &month, &day);
    month += 1; // months dari 0-11
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GString *violation_text = g_string_new(NULL);
    
    g_string_append_printf(violation_text, "Violations for %02d/%02d/%04d:\n\n", day, month, year);
    
    int found_violations = 0;
    for (int i = 0; i < global_violation_array.used; i++) {
        if (global_violation_array.array[i].dokter.id == doctor_id) {
            for (int j = 0; j < 90; j++) {
                int day_idx = global_violation_array.array[i].indexHari[0][j];
                if (day_idx == -1) break;
                
                if (global_schedule[day_idx].dd == day &&
                    global_schedule[day_idx].mm == month &&
                    global_schedule[day_idx].yy == year) {
                    int shift_idx = global_violation_array.array[i].indexHari[1][j];
                    g_string_append_printf(violation_text,
                        "- %s shift: %s\n",
                        global_schedule[day_idx].shift[shift_idx],
                        "Pelanggaran Shift");
                    found_violations++;
                }
            }
        }
    }
    
    if (!found_violations) {
        g_string_append(violation_text, "No violations found for this date");
    }
    
    gtk_text_buffer_set_text(buffer, violation_text->str, -1);
    g_string_free(violation_text, TRUE);
}

static void show_schedule_calendarVio(GtkButton *button, gpointer user_data) {
    calendarVio_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(calendarVio_window), "Schedule Calendar with Violations");
    gtk_window_set_default_size(GTK_WINDOW(calendarVio_window), 600, 500);
    gtk_window_set_position(GTK_WINDOW(calendarVio_window), GTK_WIN_POS_CENTER);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(calendarVio_window), main_box);

    // Create doctor selection dropdown
    GtkWidget *dropdown_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), dropdown_box, FALSE, FALSE, 0);
    
    GtkWidget *label = gtk_label_new("Select Doctor:");
    gtk_box_pack_start(GTK_BOX(dropdown_box), label, FALSE, FALSE, 0);
    
    doctor_dropdown = gtk_combo_box_new();
    gtk_box_pack_start(GTK_BOX(dropdown_box), doctor_dropdown, TRUE, TRUE, 0);
    
    // Create calendar widget
    calendarVio_widget = gtk_calendar_new();
    gtk_box_pack_start(GTK_BOX(main_box), calendarVio_widget, FALSE, FALSE, 0);
    
    // Create violation details view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled), text_view);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Connect signals
    g_signal_connect(doctor_dropdown, "changed", 
                    G_CALLBACK(update_calendar_marks), text_view);
    g_signal_connect(calendarVio_widget, "day-selected", 
                    G_CALLBACK(on_calendarVio_day_selected), text_view);
    
    // Populate dropdown
    populate_doctor_dropdown();
    
    gtk_widget_show_all(calendarVio_window);
}

void update_doctor_list_display() {
    gchar *doctors_text = formatDoktersToString(doctor_head);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(doctor_display_text_view);
    gtk_text_buffer_set_text(buffer, doctors_text, -1);
    g_free(doctors_text);
}

static void on_add_doctor_button_clicked(GtkButton *button, gpointer user_data) {
    int id;
    const gchar *nama;
    int max_shift;
    gchar *output_message;
    GtkTextBuffer *buffer_output = gtk_text_view_get_buffer(add_doctor_output_text_view);
    const gchar *id_str = gtk_entry_get_text(add_id_entry);
    const gchar *max_shift_str = gtk_entry_get_text(add_max_shift_entry);
    nama = gtk_entry_get_text(add_nama_entry);
    if (sscanf(id_str, "%d", &id) != 1 || id <= 0) {
        output_message = g_strdup("Error: Invalid ID. Please enter a positive integer.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        return;
    }
    if (findDokterById(doctor_head, id) != NULL) {
        output_message = g_strdup_printf("Error: Doctor with ID %d already exists. Please use a unique ID.", id);
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        return;
    }

    if (sscanf(max_shift_str, "%d", &max_shift) != 1 || max_shift <= 0) {
        output_message = g_strdup("Error: Invalid Max Shift. Please enter a positive integer.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        return;
    }
    if (strlen(nama) == 0) {
        output_message = g_strdup("Error: Name cannot be empty.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        return;
    }
    GString *day_prefs = g_string_new("");
    const gchar *days[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
    for (int i = 0; i < 7; i++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(day_check_buttons[i]))) {
            if (day_prefs->len > 0) {
                g_string_append(day_prefs, ",");
            }
            g_string_append(day_prefs, days[i]);
        }
    }
    if (day_prefs->len == 0) {
        output_message = g_strdup("Error: Please select at least one preferred day.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        g_string_free(day_prefs, TRUE);
        return;
    }
    GString *shift_prefs = g_string_new("");
    const gchar *shifts[] = {"Pagi", "Siang", "Malam"};
    for (int i = 0; i < 3; i++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(shift_check_buttons[i]))) {
            if (shift_prefs->len > 0) {
                g_string_append(shift_prefs, ",");
            }
            g_string_append(shift_prefs, shifts[i]);
        }
    }
    if (shift_prefs->len == 0) {
        output_message = g_strdup("Error: Please select at least one preferred shift.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        g_string_free(day_prefs, TRUE);
        g_string_free(shift_prefs, TRUE);
        return;
    }
    int result = addDokter_gui(&doctor_head, id, nama, max_shift, day_prefs->str, shift_prefs->str);

    if (result == 0) {
        output_message = g_strdup_printf("Success: Doctor '%s' (ID: %d) added.", nama, id);
        gtk_entry_set_text(add_id_entry, "");
        gtk_entry_set_text(add_nama_entry, "");
        gtk_entry_set_text(add_max_shift_entry, "");
        for (int i = 0; i < 7; i++) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(day_check_buttons[i]), FALSE);
        }
        for (int i = 0; i < 3; i++) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(shift_check_buttons[i]), FALSE);
        }
        update_doctor_list_display(); 
    } else {
        output_message = g_strdup("Error: Failed to add doctor.");
    }

    gtk_text_buffer_set_text(buffer_output, output_message, -1);
    g_free(output_message);
    g_string_free(day_prefs, TRUE);
    g_string_free(shift_prefs, TRUE);
}

static void on_delete_doctor_button_clicked(GtkButton *button, gpointer user_data) {
    int id;
    gchar *output_message;
    GtkTextBuffer *buffer_output = gtk_text_view_get_buffer(add_doctor_output_text_view);

    const gchar *id_str = gtk_entry_get_text(add_id_entry);

    if (sscanf(id_str, "%d", &id) != 1 || id <= 0) {
        output_message = g_strdup("Error: Invalid ID. Please enter a positive integer for deletion.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        return;
    }

    int result = hapusDokter_gui(&doctor_head, id);

    if (result == 0) {
        output_message = g_strdup_printf("Success: Doctor with ID %d deleted.", id);
        update_doctor_list_display(); 
        gtk_entry_set_text(add_id_entry, "");
    } else if (result == -1) {
        output_message = g_strdup_printf("Error: Doctor with ID %d not found.", id);
    } else {
        output_message = g_strdup("Error: Failed to delete doctor.");
    }
    gtk_text_buffer_set_text(buffer_output, output_message, -1);
    g_free(output_message);
}


static void on_generate_schedule_button_clicked(GtkButton *button, gpointer user_data) {
    if (global_violation_array.array != NULL) {
        freeArray(&global_violation_array);
    }

    initArray(&global_violation_array, 1);

    global_num_violations = 0;


    buatJadwal(global_schedule, &global_num_violations, doctor_head, &global_violation_array);

    gchar *schedule_text = formatScheduleToString(global_schedule, 61, doctor_head);
    GtkTextBuffer *schedule_buffer = gtk_text_view_get_buffer(schedule_display_text_view);
    gtk_text_buffer_set_text(schedule_buffer, schedule_text, -1);
    g_free(schedule_text);

    gchar *violation_text = formatViolationsToString(&global_violation_array, global_schedule);
    GtkTextBuffer *violation_buffer = gtk_text_view_get_buffer(shift_violation_display_text_view);
    gtk_text_buffer_set_text(violation_buffer, violation_text, -1);
    g_free(violation_text);

    g_print("Schedule generated. Violations: %d\n", global_num_violations);
}

static void on_show_schedule_by_date_button_clicked(GtkButton *button, gpointer user_data) {
    const gchar *date_str = gtk_entry_get_text(schedule_date_entry);
    int dd, mm, yy;
    gchar *output_message;
    GtkTextBuffer *schedule_buffer = gtk_text_view_get_buffer(schedule_display_text_view);

    if (sscanf(date_str, "%d/%d/%d", &dd, &mm, &yy) != 3) {
        output_message = g_strdup("Error: Invalid date format. Use DD/MM/YYYY.");
        gtk_text_buffer_set_text(schedule_buffer, output_message, -1);
        g_free(output_message);
        return;
    }

    time_t t = time(NULL);
    struct tm *local_time = localtime(&t);
    int current_dd = local_time->tm_mday;
    int current_mm = local_time->tm_mon + 1; 
    int current_yy = local_time->tm_year + 1900; 
    struct tm input_time = {0};
    input_time.tm_mday = dd;
    input_time.tm_mon = mm - 1;
    input_time.tm_year = yy - 1900;
    input_time.tm_isdst = -1; 

    time_t input_raw_time = mktime(&input_time);
    if (input_raw_time == (time_t)-1) {
        output_message = g_strdup("Error: Invalid date provided. Please check the date.");
        gtk_text_buffer_set_text(schedule_buffer, output_message, -1);
        g_free(output_message);
        return;
    }
    double seconds_diff = difftime(input_raw_time, t);
    int days_diff = (int)round(seconds_diff / (60 * 60 * 24));
    if (days_diff < 0 || days_diff >= 30) {
        output_message = g_strdup_printf("Schedule not available for %02d/%02d/%04d. The schedule covers the next 30 days only.", dd, mm, yy);
        gtk_text_buffer_set_text(schedule_buffer, output_message, -1);
        g_free(output_message);
        return;
    }
    gchar *single_day_schedule_text = formatSingleDayScheduleToString(&global_schedule[days_diff]);
    gtk_text_buffer_set_text(schedule_buffer, single_day_schedule_text, -1);
    g_free(single_day_schedule_text);
}

static void on_save_schedule_button_clicked(GtkButton *button, gpointer user_data) {
    gchar *output_message;
    GtkTextBuffer *buffer_output = gtk_text_view_get_buffer(shift_violation_display_text_view);

    if (global_schedule[31].dd == 0) {
        output_message = g_strdup("Error: Please generate a schedule first before saving.");
        gtk_text_buffer_set_text(buffer_output, output_message, -1);
        g_free(output_message);
        return;
    }

    simpanJadwalKeCSV(global_schedule, 61, "Data/jadwal.csv");
    output_message = g_strdup("Schedule saved to Data/jadwal.csv successfully.");
    gtk_text_buffer_set_text(buffer_output, output_message, -1);
    g_free(output_message);
}

// fungsi mengganti dokter yang sedang bertugas
static void on_change_doctor_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Change Doctor on Duty",
        NULL,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        (const gchar *)"OK", GTK_RESPONSE_OK,
        (const gchar *)"Cancel", GTK_RESPONSE_CANCEL,
        NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Date entry
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Date (DD/MM/YYYY):"), 0, 0, 1, 1);
    GtkWidget *date_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), date_entry, 1, 0, 2, 1);

    // Shift dropdown
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Shift:"), 0, 1, 1, 1);
    GtkWidget *shift_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(shift_combo), "Pagi");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(shift_combo), "Siang");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(shift_combo), "Malam");
    gtk_combo_box_set_active(GTK_COMBO_BOX(shift_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), shift_combo, 1, 1, 2, 1);

    // Current doctor dropdown (will be filled after date/shift selection)
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Current Doctor:"), 0, 2, 1, 1);
    GtkWidget *current_doctor_combo = gtk_combo_box_text_new();
    ListNode *cur = doctor_head;
    while (cur) {
        gchar buf[128];
        g_snprintf(buf, sizeof(buf), "%s (ID: %d)", cur->data.nama, cur->data.id);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(current_doctor_combo), NULL, buf);
        cur = cur->next;
    }
    gtk_grid_attach(GTK_GRID(grid), current_doctor_combo, 1, 2, 2, 1);

    // Replacement doctor dropdown (all registered doctors)
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Replacement Doctor:"), 0, 3, 1, 1);
    GtkWidget *replacement_doctor_combo = gtk_combo_box_text_new();
    // Fill with all doctors
    cur = doctor_head;
    while (cur) {
        gchar buf[128];
        g_snprintf(buf, sizeof(buf), "%s (ID: %d)", cur->data.nama, cur->data.id);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(replacement_doctor_combo), NULL, buf);
        cur = cur->next;
    }
    gtk_grid_attach(GTK_GRID(grid), replacement_doctor_combo, 1, 3, 2, 1);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        // Get date, shift, current doctor, replacement doctor
        const gchar *date_str = gtk_entry_get_text(GTK_ENTRY(date_entry));
        int dd, mm, yy;
        if (sscanf(date_str, "%d/%d/%d", &dd, &mm, &yy) != 3) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Invalid date format.");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
            gtk_widget_destroy(dialog);
            return;
        }
        int shift_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(shift_combo));
        int found_idx = -1;
        for (int i = 0; i < 61; i++) {
            if (global_schedule[i].dd == dd && global_schedule[i].mm == mm && global_schedule[i].yy == yy) {
                found_idx = i;
                break;
            }
        }
        if (found_idx == -1) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Date not found in schedule.");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
            gtk_widget_destroy(dialog);
            return;
        }
        // Get current doctor index
        int cur_doc_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(current_doctor_combo));
        if (cur_doc_idx == -1) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Select Dokter yang ingin diganti.");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
            gtk_widget_destroy(dialog);
            return;
        }
        //get replacement doctor index
        int rep_doc_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(replacement_doctor_combo));
        if (rep_doc_idx == -1) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Select Dokter yang ingin menggantikan.");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
            gtk_widget_destroy(dialog);
            return;
        }
        // Get doctor IDs
        int old_id = -1, new_id = -1;
        // Find old doctor id
        int count = 0;
        ListNode *cur2 = doctor_head;
        while (cur2) {
            if (count == cur_doc_idx) {
                old_id = cur2->data.id;
                break;
            }
            cur2 = cur2->next;
            count++;
        }
        // Find new doctor id
        count = 0;
        cur2 = doctor_head;
        while (cur2) {
            if (count == rep_doc_idx) {
                new_id = cur2->data.id;
                break;
            }
            cur2 = cur2->next;
            count++;
        }
        if (old_id == -1 || new_id == -1) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Doctor selection error.");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
            gtk_widget_destroy(dialog);
            return;
        }
        ListNode *rep;
        // Replace doctor in schedule
        for (int j = 0; j < global_schedule[found_idx].kebutuhanDokter[shift_idx]; j++) {
            if (global_schedule[found_idx].ArrayDokter[j][shift_idx].id == old_id) {
                rep = findDokterById(doctor_head, new_id);
                global_schedule[found_idx].ArrayDokter[j][shift_idx] = rep->data;
                break;
            }
        }
        int lokasiDokter = -1;
        //remove from violation array
        for (int i = 0; i < global_violation_array.used; i++) {
            if (global_violation_array.array[i].dokter.id == old_id) {
                lokasiDokter = i;
                for (int j = 0; j < 90; j++) {
                    if (global_violation_array.array[i].indexHari[0][j] == found_idx) {
                        //remove from violation shift array
                        for (int k = j; k < 89-j; k++) {
                            global_violation_array.array[i].indexHari[0][k] = global_violation_array.array[i].indexHari[0][k + 1];
                            global_violation_array.array[i].indexHari[1][k] = global_violation_array.array[i].indexHari[1][k + 1];
                        }
                        break;      
                    }
                }   
                break;
            }   
        }
        int flagRemove= 1;
        //remove from violation shift array if empty
        for (int i=0;i<90;i++){
            if (global_violation_array.array[lokasiDokter].indexHari[0][i] != -1) {
                flagRemove= 0; // still has violations
                break;  
            }
        }
        if (flagRemove) {
            //remove from array
            for (int i = lokasiDokter; i < global_violation_array.used - 1; i++) {
                global_violation_array.array[i] = global_violation_array.array[i + 1];
            }
            global_violation_array.used--;
        }

        GtkWidget *info = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Doctor replaced successfully.");
        gtk_dialog_run(GTK_DIALOG(info));
        gtk_widget_destroy(info);
    }
    gtk_widget_destroy(dialog);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *notebook;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Doctor Schedule Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);
    GtkWidget *doctor_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(doctor_vbox), 10);
    GtkWidget *doctor_tab_label = gtk_label_new("Doctor Management");
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(doctor_vbox), grid, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("ID:"), 0, 0, 1, 1);
    add_id_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(add_id_entry), 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Nama:"), 0, 1, 1, 1);
    add_nama_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(add_nama_entry), 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Max Shift per Week:"), 0, 2, 1, 1);
    add_max_shift_entry = GTK_ENTRY(gtk_entry_new());
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(add_max_shift_entry), 1, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Preferred Days:"), 0, 3, 1, 1);
    GtkWidget *day_checkbox_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    const gchar *days[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
    for (int i = 0; i < 7; i++) {
        day_check_buttons[i] = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(days[i]));
        gtk_box_pack_start(GTK_BOX(day_checkbox_box), GTK_WIDGET(day_check_buttons[i]), FALSE, FALSE, 0);
    }
    gtk_grid_attach(GTK_GRID(grid), day_checkbox_box, 1, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Preferred Shifts:"), 0, 4, 1, 1);
    GtkWidget *shift_checkbox_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    const gchar *shifts[] = {"Pagi", "Siang", "Malam"};
    for (int i = 0; i < 3; i++) {
        shift_check_buttons[i] = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(shifts[i]));
        gtk_box_pack_start(GTK_BOX(shift_checkbox_box), GTK_WIDGET(shift_check_buttons[i]), FALSE, FALSE, 0);
    }
    gtk_grid_attach(GTK_GRID(grid), shift_checkbox_box, 1, 4, 2, 1);
    GtkWidget *add_doctor_button = gtk_button_new_with_label("Add Doctor");
    g_signal_connect(add_doctor_button, "clicked", G_CALLBACK(on_add_doctor_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), add_doctor_button, 1, 5, 1, 1);
    GtkWidget *delete_doctor_button = gtk_button_new_with_label("Delete Doctor (by ID)");
    g_signal_connect(delete_doctor_button, "clicked", G_CALLBACK(on_delete_doctor_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), delete_doctor_button, 2, 5, 1, 1);
    add_doctor_output_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(add_doctor_output_text_view, FALSE);
    gtk_text_view_set_wrap_mode(add_doctor_output_text_view, GTK_WRAP_WORD);
    GtkWidget *scrolled_window_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window_output), GTK_WIDGET(add_doctor_output_text_view));
    gtk_widget_set_size_request(scrolled_window_output, -1, 50); // Small height for messages
    gtk_box_pack_start(GTK_BOX(doctor_vbox), scrolled_window_output, FALSE, FALSE, 5);
    GtkWidget *doctor_list_label = gtk_label_new("Registered Doctors:");
    gtk_box_pack_start(GTK_BOX(doctor_vbox), doctor_list_label, FALSE, FALSE, 5);
    doctor_display_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(doctor_display_text_view, FALSE);
    gtk_text_view_set_wrap_mode(doctor_display_text_view, GTK_WRAP_WORD);
    GtkWidget *scrolled_window_doctors = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window_doctors), GTK_WIDGET(doctor_display_text_view));
    gtk_box_pack_start(GTK_BOX(doctor_vbox), scrolled_window_doctors, TRUE, TRUE, 5);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), doctor_vbox, doctor_tab_label);
    GtkWidget *schedule_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(schedule_vbox), 10);
    GtkWidget *schedule_tab_label = gtk_label_new("Schedule Management");
    GtkWidget *generate_schedule_button = gtk_button_new_with_label("Generate 30-Day Schedule");
    g_signal_connect(generate_schedule_button, "clicked", G_CALLBACK(on_generate_schedule_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(schedule_vbox), generate_schedule_button, FALSE, FALSE, 0);
    GtkWidget *save_schedule_button = gtk_button_new_with_label("Save Schedule to jadwal.csv");
    g_signal_connect(save_schedule_button, "clicked", G_CALLBACK(on_save_schedule_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(schedule_vbox), save_schedule_button, FALSE, FALSE, 0);
    GtkWidget *calendar_button = gtk_button_new_with_label("View Violation in Calendar");
    g_signal_connect(calendar_button, "clicked", G_CALLBACK(show_schedule_calendarVio), NULL);
    gtk_box_pack_start(GTK_BOX(schedule_vbox), calendar_button, FALSE, FALSE, 0);
    GtkWidget *date_input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(schedule_vbox), date_input_box, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(date_input_box), gtk_label_new("Show Schedule for Date (DD/MM/YYYY):"), FALSE, FALSE, 0);
    schedule_date_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(schedule_date_entry, "e.g., 25/12/2024");
    gtk_box_pack_start(GTK_BOX(date_input_box), GTK_WIDGET(schedule_date_entry), TRUE, TRUE, 0);
    GtkWidget *show_date_schedule_button = gtk_button_new_with_label("Show Date Schedule");
    g_signal_connect(show_date_schedule_button, "clicked", G_CALLBACK(on_show_schedule_by_date_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(date_input_box), show_date_schedule_button, FALSE, FALSE, 0);
    GtkWidget *change_doctor_button = gtk_button_new_with_label("Change Doctor on Duty");
    g_signal_connect(change_doctor_button, "clicked", G_CALLBACK(on_change_doctor_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(schedule_vbox), change_doctor_button, FALSE, FALSE, 0);
    GtkWidget *schedule_label = gtk_label_new("Generated Schedule:");
    gtk_box_pack_start(GTK_BOX(schedule_vbox), schedule_label, FALSE, FALSE, 5);
    schedule_display_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(schedule_display_text_view, FALSE);
    gtk_text_view_set_wrap_mode(schedule_display_text_view, GTK_WRAP_WORD);
    GtkWidget *scrolled_window_schedule = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window_schedule), GTK_WIDGET(schedule_display_text_view));
    gtk_box_pack_start(GTK_BOX(schedule_vbox), scrolled_window_schedule, TRUE, TRUE, 5);
    GtkWidget *shift_violation_label = gtk_label_new("Shift Summary and Violations:");
    gtk_box_pack_start(GTK_BOX(schedule_vbox), shift_violation_label, FALSE, FALSE, 5);
    shift_violation_display_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(shift_violation_display_text_view, FALSE);
    gtk_text_view_set_wrap_mode(shift_violation_display_text_view, GTK_WRAP_WORD);
    GtkWidget *scrolled_window_violations = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window_violations), GTK_WIDGET(shift_violation_display_text_view));
    gtk_box_pack_start(GTK_BOX(schedule_vbox), scrolled_window_violations, TRUE, TRUE, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), schedule_vbox, schedule_tab_label);

    //add a calendar tab here
    GtkWidget *calendar_tab = create_calendar_tab();
    GtkWidget *calendar_tab_label = gtk_label_new("Calendar View");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), calendar_tab, calendar_tab_label);
    //

    doctor_head = createDokterList(); 
    update_doctor_list_display();
    initArray(&global_violation_array, 1);
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    // --- Set DLL search path (FIXED: Use absolute path) ---
    char exe_path[MAX_PATH];
    char libs_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);  // Get .exe path
    char *last_slash = strrchr(exe_path, '\\');
    if (last_slash) *last_slash = '\0';            // Remove .exe filename
    snprintf(libs_path, sizeof(libs_path), "%s\\libs", exe_path);
    SetDllDirectoryA(libs_path);                   // Now points to /path/to/app/libs

    // --- Initialize GTK (with error checking) ---
    if (!gtk_init_check(&argc, &argv)) {
        g_printerr("Failed to initialize GTK. Check DLLs in %s\n", libs_path);
        return 1;
    }
    
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.doctor.schedule", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (global_violation_array.array != NULL) {
        freeArray(&global_violation_array);
    }

    ListNode *current = doctor_head;
    while (current != NULL) {
        ListNode *next = current->next;
        free(current);
        current = next;
    }
    if (doctor_store != NULL) {
        g_object_unref(doctor_store);
    }
    return status;
}


