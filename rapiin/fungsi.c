#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "header.h"

huruf kelompok[26];

void bersihkanstring(char *kata) {
    if (!kata) return;
    char temp[200000];
    int j = 0;
    int spasi = 0;
    
    for (int i = 0; kata[i] != '\0'; i++) {
        unsigned char c = (unsigned char)kata[i];
        // DETEKSI apostrophe rusak UTF-8: â€™ (E2 80 99)
        if (c >= 128) {
            continue;
        }
        c = tolower(c);
        
        if (isalpha(c)) {
            temp[j++] = c;
            spasi = 0;
        } 
        else if (c=='\'') {
            continue;
        }
        else {
            if (!spasi){
                temp[j++] = ' ';
                spasi = 1;
            }
        }
    }
    if (j>0 && temp[j-1]==' ')
        j--;

    temp[j] = '\0';
    strcpy(kata, temp);
}
void grupabjad(){
    for (int i = 0; i < 26; i++){
        kelompok[i].abjad = 'a' + i;
        kelompok[i].jumlah = 0;
    }
}

void tokenisasi (char *kalimat){
    char *token = strtok(kalimat, " ");
    while (token != NULL){
        simpanKata(token);
        token = strtok(NULL, " ");
    }
}

void simpanKata(char *kata){
    if (kata[0] == '\0') return; //abaikan kata kosong

    char hurufAwal = kata[0];
    if (hurufAwal < 'a' || hurufAwal > 'z') return; //abaikan jika bukan huruf
    
    int index = hurufAwal - 'a';
    huruf *g = &kelompok[index];

    for (int i=0;i<g->jumlah;i++){
        if(strcmp(kata, g->list[i].kataa) == 0){
            g->list[i].frek++;
            return;
        }
    }
            strcpy(g->list[g->jumlah].kataa, kata);
            g->list[g->jumlah].frek = 1;
            g->list[g->jumlah].panjang = strlen(kata);
            g->jumlah++;
}

static char *carikata(char *teks, const char *kata) {
    size_t n = strlen(kata);
    if (n == 0) return teks;
    for (char *p = teks; *p; p++) {
        size_t i;
        for (i = 0; i < n && p[i]; ++i) {
            if (tolower((unsigned char)p[i]) != tolower((unsigned char)kata[i])) break;
        }
        if (i == n) return p;
    }
    return NULL;
}

void bersihin_isi(char *isi) {
    if (!isi || isi[0] == '\0') return;

    // 1) Hapus blok {...} berulang kali (RTF/Style blocks)
    for (;;) {
        char *p = strchr(isi, '{');
        if (!p) break;
        char *q = strchr(p, '}');
        if (!q) break; // jika tidak ada '}', hentikan
        memmove(p, q + 1, strlen(q + 1) + 1);
    }

    // 2) Daftar pola "sampah" yang ingin dihapus (ganti dgn satu spasi)
    const char *pola[] = {
        "Font Definitions",
        "Style Definitions",
        "MsoNormal",
        "MsoChpDefault",
        "MsoPapDefault",
        "Font",
        "font",
        "Cambria",
        "Calibri",
        "page Section",
        "PageSection",
        "paginationwidow",
        "panose",
        NULL
    };

    // Untuk setiap pola: cari dan hapus (ganti dengan spasi) — lakukan berulang sampai tidak ada
    for (int i = 0; pola[i] != NULL; ++i) {
        for (;;) {
            char *pos = carikata(isi, pola[i]);
            if (!pos) break;
            size_t len = strlen(pola[i]);

            // HAPUS pola saja (jangan hapus sisa teks). Ganti lokasi pola dengan satu spasi.
            // memmove sisa kiri menimpa pola.
            memmove(pos, pos + len, strlen(pos + len) + 1);

            // letakkan satu spasi di posisi pos jika bukan spasi
            if (pos[0] != ' ') {
                memmove(pos + 1, pos, strlen(pos) + 1);
                pos[0] = ' ';
            }
        }
    }
}

void bacaFile(char *namaFile){
    FILE *file = fopen(namaFile, "r");
    if (!file){
        printf("Gagal membuka file %s\n", namaFile);
        return;
    }

    char line[5000];
    char *isi;

    while (fgets(line, sizeof(line), file)){

        isi = ambilIsiTag(line, "title");
        if (isi != NULL){
            bersihin_isi(isi);
            bersihkanstring(isi);
            tokenisasi(isi);
        }

        isi = ambilIsiTag(line, "body");
        if (isi != NULL){
            bersihin_isi(isi);
            bersihkanstring(isi);
            tokenisasi(isi);
        }
    }

    fclose(file);
}

char* ambilIsiTag(char* line, const char* tag) {
    static char hasil[200000];
    char mulai[50], akhir[50];

    sprintf(mulai, "<%s>", tag);
    sprintf(akhir, "</%s>", tag);

    char* p = strstr(line, mulai);
    if (!p) return NULL;

    p += strlen(mulai);
    char* q = strstr(p, akhir);
    if (!q) return NULL;

    int len = q - p;
    strncpy(hasil, p, len);
    hasil[len] = '\0';
    return hasil;
}

int banding(word a, word b){
    if (a.frek != b.frek){
        return b.frek - a.frek; // urutkan berdasarkan frekuensi (menurun)
    }
    if (a.panjang != b.panjang){
        return b.panjang - a.panjang; // urutkan berdasarkan panjang (menurun)
    }
    return strcmp(b.kataa, a.kataa); // urutkan secara alfabetis (menaik)
}

void sorting(){
    for (int i = 0; i < 26; i++){
        huruf *g = &kelompok[i];
        for (int j = 0; j < g->jumlah - 1; j++){
            for (int k = 0; k < g->jumlah - j - 1; k++){
                if (banding(g->list[k], g->list[k + 1]) > 0){
                    word temp = g->list[k];
                    g->list[k] = g->list[k + 1];
                    g->list[k + 1] = temp;
                }
            }
        }
    }
}

void simpanFileBin(const char *namaFile){
    FILE *fp = fopen(namaFile, "wb");
    if (!fp){
        printf("Gagal membuka file %s untuk penulisan biner\n", namaFile);
        return;
    }
    for (int i = 0; i < 26; i++){
        huruf *g = &kelompok[i];
        fwrite(&g->abjad, sizeof(char), 1, fp);
        fwrite(&g->jumlah, sizeof(int), 1, fp);
        for (int j = 0; j < g->jumlah; j++){
            word *w = &g->list[j];
            fwrite(&w->panjang, sizeof(int), 1, fp);
            fwrite(&w->kataa, sizeof(char), w->panjang, fp);
            fwrite(&w->frek, sizeof(int), 1, fp);
        }
    }
    fclose(fp);
    printf("file biner '%s' berhasil disimpan.\n", namaFile);
}

void bacaFileBin(const char *namaFile){
    FILE *fp = fopen(namaFile, "rb");
    if (!fp){
        printf("Gagal membuka file %s untuk pembacaan biner\n", namaFile);
        return;
    }
    for (int i=0;i<26; i++){
        kelompok[i].abjad = 'a'+i;
        kelompok[i].jumlah=0;
    }
    for (int i = 0; i < 26; i++){
        huruf *g = &kelompok[i];
        fread(&g->abjad, sizeof(char), 1, fp);
        fread(&g->jumlah, sizeof(int), 1, fp);
        for (int j = 0; j < g->jumlah; j++){
            word *w = &g->list[j];
            fread(&w->panjang, sizeof(int), 1, fp);
            fread(&w->kataa, sizeof(char), w->panjang, fp);
            w->kataa[w->panjang] = '\0'; // tambahkan null terminator
            fread(&w->frek, sizeof(int), 1, fp);
        }
    }
    fclose(fp);
    printf("file biner '%s' berhasil dibaca.\n", namaFile);
}

void tampilkanNkata(int n) {
    int indent = 8;
    printf("abjad\t{kata (frekuensi)}\n");
    for (int i = 0; i < 26; i++) {
        huruf *g = &kelompok[i];

        // Print abjad
        printf("%c\t{", g->abjad);

        if (g->jumlah == 0) {
            printf("}\n\n");
            continue;
        }

        int batas = (n < g->jumlah) ? n : g->jumlah;
        int ngitungChar = indent;
        int maksbaris = 100;

        for (int j = 0; j < batas; j++) {
            char item[200];
            sprintf(item, "%s (%d)", g->list[j].kataa, g->list[j].frek);
            int len = strlen(item);

            if(ngitungChar + len > maksbaris) {
                printf("\n");
                for(int s = 0; s < indent; s++) printf(" ");
                ngitungChar = indent;
            }
            printf("%s", item);
            ngitungChar += len; // +2 untuk koma dan ngitungChar
        
            if (j < batas - 1){
                printf(", ");
                ngitungChar += 2;
            }
        }

        printf("}\n\n");
    }
}