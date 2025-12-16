#include <stdio.h>
#include "header.h"
 
int main() {
    int pilihan;
    int sudahAdaBiner = 0;
    char namaBiner[] = "output.bin";
    char namaInput[100];
    printf("Masukkan nama file input (txt): ");
    scanf("%s", namaInput);
    FILE *file = fopen(namaInput, "r");
    if (!file){
        printf("Gagal membuka file %s\n", namaInput);
        return 0;
    }
    fclose(file);

    grupabjad();               // inisialisasi a–z
    bacaFile(namaInput);       // baca file teks
    sorting();                 // sorting sesuai aturan big challenge

    while (1) {
        printf("\n===== MENU BIG CHALLENGE =====\n");
        printf("1) Simpan luaran ke file biner\n");
        printf("2) Tampilkan n kata terbesar (0 < n <= 25)\n");
        printf("3) Selesai\n");
        printf("Pilihan anda: ");
        if (scanf("%d", &pilihan) != 1) {
            printf("Input tidak valid! Masukkan angka.\n");

            // bersihkan buffer sampai newline
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        continue;
}

        //pilihan=inputAngka();

        if (pilihan == 1) {
            simpanFileBin(namaBiner);
            sudahAdaBiner = 1;
        }
        else if (pilihan == 2) {
            int n;
            printf("Masukkan n: ");
        if (scanf("%d", &n) != 1) {
            printf("Input tidak valid! Masukkan angka.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        continue;
        }
            if (n <= 0 || n > 25) {
                printf("n tidak valid!\n");
                continue;
            }
            if (!sudahAdaBiner) {
                printf("File biner belum ada. Membuat file biner terlebih dahulu...\n");
                simpanFileBin(namaBiner);
                sudahAdaBiner = 1;
            }
            bacaFileBin(namaBiner);
            sorting();
            //tampilkanOutput();
            tampilkanNkata(n);
        }
        else if (pilihan == 3) {
            printf("Program selesai.\n");
            break;
        }
        else {
            printf("Pilihan tidak valid!\n");
        }
    }
    return 0;
}
