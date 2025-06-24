# Tubes-PMP
Tugas Besar Pemecahan Masalah dengan Pemrograman 
Aplikasi yang telah dibuat dengan tujuan menyelesaikan permasalahan penjadwalan di sebuah rumah sakit dengan menggunakan pemrograman.
Aplikasi ditulis dalam bahasa C, dengan UI oleh library GTK.
Aplikasi dibuat oleh :
Kelompok 2
13223032-Darren Johan
13223022-Joenathan Luther Sihotang
13223013-Kiyo Lee Tiono
13223002-Muhammad Faiz Naufal

# Petunjuk Run
Assistant scheduler dapat dinyalakan dengan menjalankan AssistantScheduler shortcut, atau dengan menjalankan AssistantScheduler.exe di libs

# Petunjuk Compile
run command di dalam msys2 mingw64 command prompt. Bila terjadi masalah, pastikan GTK 3.0 sudah terinstall:

gcc -o libs/AssistantScheduler src/main.c Function/dokter.c Function/penjadwalan.c -I Function $(pkg-config --cflags --libs gtk+-3.0) -lm

dan aplikasi akan dapat diakses melalui shortcut yang sudah ada di direktori ini.