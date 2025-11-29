#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD 50000
#define MAX_LEN 100

// ------------------------------------------------------------
// Struktur data kata & grup abjad
// ------------------------------------------------------------
typedef struct {
    char kata[MAX_LEN];
    int freq;
    int len;
} Word;

typedef struct {
    char abjad;
    int jumlah;
    Word list[MAX_WORD];
} Group;

Group kelompok[26];

// ------------------------------------------------------------
// Fungsi: Membersihkan kata → huruf saja + lowercase
// ------------------------------------------------------------
void bersihkanKata(char *s) {
    char temp[MAX_LEN];
    int idx = 0;

    for (int i = 0; s[i]; i++) {
        if (isalpha(s[i])) {
            temp[idx++] = tolower(s[i]);
        }
    }
    temp[idx] = '\0';
    strcpy(s, temp);
}

// ------------------------------------------------------------
// Tambahkan kata ke kelompok abjad
// ------------------------------------------------------------
void tambahKata(char *kata) {
    if (strlen(kata) == 0) return;

    char awal = kata[0];
    if (awal < 'a' || awal > 'z') return;

    int idx = awal - 'a';
    Group *g = &kelompok[idx];

    // jika kata sudah ada → tambah frekuensi
    for (int i = 0; i < g->jumlah; i++) {
        if (strcmp(g->list[i].kata, kata) == 0) {
            g->list[i].freq++;
            return;
        }
    }

    // kata baru
    strcpy(g->list[g->jumlah].kata, kata);
    g->list[g->jumlah].freq = 1;
    g->list[g->jumlah].len = strlen(kata);
    g->jumlah++;
}

// ------------------------------------------------------------
// Proses file input TXT
// ------------------------------------------------------------
void prosesFile(char *nama) {
    FILE *fp = fopen(nama, "r");
    if (!fp) {
        printf("File tidak ditemukan.\n");
        exit(1);
    }

    char line[1000];
    int inUrl = 0;

    while (fgets(line, sizeof(line), fp)) {

        if (strstr(line, "<url>")) inUrl = 1;
        if (strstr(line, "</url>")) { inUrl = 0; continue; }
        if (inUrl) continue;

        char *token = strtok(line, " \t\n");
        while (token) {
            bersihkanKata(token);
            tambahKata(token);
            token = strtok(NULL, " \t\n");
        }
    }

    fclose(fp);
}

// ------------------------------------------------------------
// Sorting sesuai kriteria:
// frekuensi → panjang kata → alfabet (descending)
// ------------------------------------------------------------
void urutkan() {
    for (int i = 0; i < 26; i++) {

        Group *g = &kelompok[i];

        for (int a = 0; a < g->jumlah - 1; a++) {
            for (int b = a + 1; b < g->jumlah; b++) {

                int swap = 0;

                if (g->list[a].freq < g->list[b].freq) swap = 1;
                else if (g->list[a].freq == g->list[b].freq &&
                         g->list[a].len < g->list[b].len) swap = 1;
                else if (g->list[a].freq == g->list[b].freq &&
                         g->list[a].len == g->list[b].len &&
                         strcmp(g->list[a].kata, g->list[b].kata) < 0) swap = 1;

                if (swap) {
                    Word temp = g->list[a];
                    g->list[a] = g->list[b];
                    g->list[b] = temp;
                }
            }
        }
    }
}

// ------------------------------------------------------------
// Simpan ke file binary sesuai format
// ------------------------------------------------------------
void simpanBinary(char *nama) {
    FILE *fp = fopen(nama, "wb");

    for (int i = 0; i < 26; i++) {

        Group *g = &kelompok[i];

        fwrite(&g->abjad, sizeof(char), 1, fp);
        fwrite(&g->jumlah, sizeof(int), 1, fp);

        for (int k = 0; k < g->jumlah; k++) {
            fwrite(&g->list[k].len, sizeof(int), 1, fp);
            fwrite(g->list[k].kata, sizeof(char), g->list[k].len, fp);
            fwrite(&g->list[k].freq, sizeof(int), 1, fp);
        }
    }

    fclose(fp);
    printf("Data berhasil disimpan ke %s\n", nama);
}

// ------------------------------------------------------------
// Tampilkan n kata TANPA KOMA + panjang string per kata
// ------------------------------------------------------------
void tampilkanN(char *nama, int n) {
    FILE *fp = fopen(nama, "rb");
    if (!fp) {
        printf("File binary belum ada.\n");
        return;
    }

    printf("\n=== HASIL (maks %d kata per abjad, tanpa koma) ===\n", n);

    for (int i = 0; i < 26; i++) {

        char abj;
        int jumlah;

        fread(&abj, sizeof(char), 1, fp);
        fread(&jumlah, sizeof(int), 1, fp);

        int totalPanjang = 0;

        printf("\n%c {\n", abj);

        for (int k = 0; k < jumlah; k++) {

            int len, freq;
            char kata[MAX_LEN];

            fread(&len, sizeof(int), 1, fp);
            fread(kata, sizeof(char), len, fp);
            kata[len] = '\0';
            fread(&freq, sizeof(int), 1, fp);

            totalPanjang += len;

            if (k < n) {
                printf("    %s (%d) [%d]\n", kata, freq, len);  // tanpa koma
            }
        }

        printf("}  [jumlah kata: %d]  [total panjang: %d]\n",
               jumlah, totalPanjang);
    }

    fclose(fp);
}

// ------------------------------------------------------------
// MAIN PROGRAM
// ------------------------------------------------------------
int main() {

    // Inisialisasi kelompok abjad
    for (int i = 0; i < 26; i++) {
        kelompok[i].abjad = 'a' + i;
        kelompok[i].jumlah = 0;
    }

    char inputTxt[100];
    printf("Masukkan nama file input (txt): ");
    scanf("%s", inputTxt);

    prosesFile(inputTxt);
    urutkan();

    int pilihan;
    char namaBin[] = "output.bin";

    do {
        printf("\nMenu:\n");
        printf("1. Simpan ke file binary\n");
        printf("2. Tampilkan n kata dari file binary\n");
        printf("3. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);

        if (pilihan == 1) {
            simpanBinary(namaBin);
        }
        else if (pilihan == 2) {

            int n;
            printf("Masukkan n (0 < n <= 25): ");
            scanf("%d", &n);

            // simpan agar binary selalu ada
            simpanBinary(namaBin);
            tampilkanN(namaBin, n);
        }

    } while (pilihan != 3);

    return 0;
}
