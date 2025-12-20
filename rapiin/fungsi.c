#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "header.h"

huruf kelompok[26];

void bersihkanstring(char *kata) { //fungsi untuk membersihkan string dari karakter non-alfabet dan mengubah ke huruf kecil
    if (!kata) return; //jika pointer null, maka fungsi dihentikan
    char temp[2000]; 
    int j = 0;
    int spasi = 0;
    
    for (int i = 0; kata[i] != '\0'; i++) {
        unsigned char c = (unsigned char)kata[i];
        if (c >= 128) { //abaikan karakter non-ASCII
            continue;
        }
        c = tolower(c);    //ubah ke huruf kecil
        
        if (isalpha(c)) {  //simpan jika huruf alfabet
            temp[j++] = c;
            spasi = 0;
        } 
        else if (c=='\'') { //abaikan tanda petik
            continue;
        }
        else {
            if (!spasi){    //karakter lain diganti satu spasi
                temp[j++] = ' ';
                spasi = 1;
            }
        }
    }
    if (j>0 && temp[j-1]==' ') //hilangkan spasi di akhir string
        j--;

    temp[j] = '\0';
    strcpy(kata, temp);    //salin kembali ke kata asli
}
void grupabjad(){ //fungsi untuk menginisialisasi kelompok huruf
    for (int i = 0; i < 26; i++){    //inisialisasi 26 kelompok huruf dari 'a' sampai 'z'
        kelompok[i].abjad = 'a' + i; // Menentukan huruf awal untuk setiap kelompok
        kelompok[i].jumlah = 0;      // Mengosongkan jumlah kata pada tiap kelompok
    }
}

void tokenisasi (char *kalimat){ //fungsi untuk memecah kalimat menjadi kata-kata dan menyimpannya
    char *token = strtok(kalimat, " "); //memecah kalimat menjadi kata-kata berdasarkan spasi
    while (token != NULL){
        simpanKata(token);
        token = strtok(NULL, " ");
    }
}

void simpanKata(char *kata){ //fungsi untuk menyimpan kata ke dalam struktur data
    if (kata[0] == '\0') return; //abaikan kata kosong

    char hurufAwal = kata[0];
    if (hurufAwal < 'a' || hurufAwal > 'z') return; //abaikan jika bukan huruf
    
    int index = hurufAwal - 'a';
    huruf *g = &kelompok[index];

    for (int i=0;i<g->jumlah;i++){ //cek apakah kata sudah ada
        if(strcmp(kata, g->list[i].kataa) == 0){
            g->list[i].frek++;
            return;
        }
    }
            strcpy(g->list[g->jumlah].kataa, kata); //kalau belum ada, simpan kata baru
            g->list[g->jumlah].frek = 1;
            g->list[g->jumlah].panjang = strlen(kata);
            g->jumlah++;
}

static char *carikata(char *teks, const char *kata) { //fungsi untuk mencari kata dalam teks (case-insensitive)
    size_t n = strlen(kata);
    if (n == 0) return teks;
    for (char *p = teks; *p; p++) { //mengecek setiap posisi dalam teks
        size_t i;
        for (i = 0; i < n && p[i]; ++i) { //membandingkan karakter satu per satu
            if (tolower((unsigned char)p[i]) != tolower((unsigned char)kata[i])) break; 
        }
        if (i == n) return p;
    }
    return NULL;
}

void bersihin_isi(char *isi) { //fungsi untuk membersihkan isi teks dari elemen RTF/Style dan pola "sampah"
    if (!isi || isi[0] == '\0') return;

    //hapus semua teks di antara kurung kurawal '{...}'
    for (;;) {
        char *p = strchr(isi, '{');
        if (!p) break;
        char *q = strchr(p, '}');
        if (!q) break; // jika tidak ada '}', hentikan
        memmove(p, q + 1, strlen(q + 1) + 1);
    }

    //daftar pola yang akan dihapus
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

    //hapus semua pola yang ada dalam daftar
    for (int i = 0; pola[i] != NULL; ++i) {
        for (;;) {
            char *pos = carikata(isi, pola[i]);
            if (!pos) break;
            size_t len = strlen(pola[i]);
            // hapus pola dengan menggeser sisa string ke kiri
            memmove(pos, pos + len, strlen(pos + len) + 1);
            //tambahkan spasi jika perlu
            if (pos[0] != ' ') {
                memmove(pos + 1, pos, strlen(pos) + 1);
                pos[0] = ' ';
            }
        }
    }
}

void bacaFile(char *namaFile){ //fungsi untuk membaca file dan mengekstrak isi dari tag <title> dan <body>
    FILE *file = fopen(namaFile, "r");
    if (!file){
        printf("Gagal membuka file %s\n", namaFile);
        return;
    }

    char line[5000];
    char *isi;

    while (fgets(line, sizeof(line), file)){
        isi = ambilIsiTag(line, "title"); //ambil isi tag <title>
        if (isi != NULL){
            bersihin_isi(isi);
            bersihkanstring(isi);
            tokenisasi(isi);
        }
        isi = ambilIsiTag(line, "body"); //ambil isi tag <body>
        if (isi != NULL){
            bersihin_isi(isi);
            bersihkanstring(isi);
            tokenisasi(isi);
        }
    }
    fclose(file);
}

char* ambilIsiTag(char* line, const char* tag) { //fungsi untuk mengambil isi dari tag tertentu dalam sebuah baris teks
    static char hasil[200000]; 
    char mulai[50], akhir[50];

    sprintf(mulai, "<%s>", tag);
    sprintf(akhir, "</%s>", tag);

    char* p = strstr(line, mulai); //cari tag pembuka
    if (!p) return NULL;

    p += strlen(mulai); //pindah ke setelah tag pembuka
    char* q = strstr(p, akhir); //cari tag penutup
    if (!q) return NULL;

    int len = q - p; //hitung panjang isi antara tag
    strncpy(hasil, p, len);
    hasil[len] = '\0';
    return hasil;
}

int banding(word a, word b){ //fungsi untuk membandingkan dua kata berdasarkan frekuensi, panjang, dan urutan alfabetis
    if (a.frek != b.frek){
        return b.frek - a.frek; // urutkan berdasarkan frekuensi (menurun)
    }
    if (a.panjang != b.panjang){
        return b.panjang - a.panjang; // urutkan berdasarkan panjang (menurun)
    }
    return strcmp(b.kataa, a.kataa); // urutkan secara alfabetis (menaik)
}

void sorting(){ //fungsi untuk mengurutkan kata-kata dalam setiap kelompok huruf
    for (int i = 0; i < 26; i++){ 
        huruf *g = &kelompok[i]; //ambil kelompok huruf
        for (int j = 0; j < g->jumlah - 1; j++){ //lakukan bubble sort
            for (int k = 0; k < g->jumlah - j - 1; k++){
                if (banding(g->list[k], g->list[k + 1]) > 0){ //bandingkan dua kata untuk menentukan urutan
                    word temp = g->list[k]; 
                    g->list[k] = g->list[k + 1];
                    g->list[k + 1] = temp;
                }
            }
        }
    }
}

void simpanFileBin(const char *namaFile){ //fungsi untuk menyimpan data ke file biner
    FILE *fp = fopen(namaFile, "wb");
    if (!fp){
        printf("Gagal membuka file %s untuk penulisan biner\n", namaFile);
        return;
    }
    for (int i = 0; i < 26; i++){
        huruf *g = &kelompok[i]; //ambil kelompok huruf
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

void bacaFileBin(const char *namaFile){ //fungsi untuk membaca data dari file biner
    FILE *fp = fopen(namaFile, "rb");
    if (!fp){
        printf("Gagal membuka file %s untuk pembacaan biner\n", namaFile);
        return;
    }
    //inisialisasi kelompok huruf
    for (int i=0;i<26; i++){
        kelompok[i].abjad = 'a'+i;
        kelompok[i].jumlah=0;
    }
    //baca data dari file biner
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

void tampilkanNkata(int n) { //fungsi untuk menampilkan n kata teratas dari setiap kelompok huruf
    int indent = 8;
    printf("abjad\t{kata (frekuensi)}\n");
    for (int i = 0; i < 26; i++) {
        huruf *g = &kelompok[i];

        //tampilkan huruf awal
        printf("%c\t{", g->abjad);

        if (g->jumlah == 0) {
            printf("}\n\n");
            continue;
        }
        //tentukan batas jumlah kata yang akan ditampilkan
        int batas = (n < g->jumlah) ? n : g->jumlah;
        int ngitungChar = indent;
        int maksbaris = 100;
        //tampilkan kata-kata beserta frekuensinya
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