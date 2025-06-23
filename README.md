# Tubes-PMP
Tugas Besar Pemecahan Masalah dengan Pemrograman 
Kelompok 2

# Petunjuk Compile
run command di dalam msys2 mingw64 command prompt. Pastikan Gtk-3.0 sudah terinstall :

pertama,
cd "rute ke folder"

kedua,
gcc -o tubes src/main.c Function/dokter.c Function/pengelolaan_data_dokter.c Function/penjadwalan.c -I Function $(pkg-config --cflags --libs gtk+-3.0) -lm

ketiga.
./tubes
