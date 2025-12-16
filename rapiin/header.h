
typedef struct{
    char kataa[50];
    int frek;
    int panjang;
    }word;

typedef struct{
    char abjad;
    int jumlah;
    word list[100000];
    }huruf;
    
extern huruf kelompok[26];
void bersihkanstring(char *kata);
void grupabjad();
void tokenisasi (char *kalimat);
void simpanKata(char *kata);
void bacaFile(char *namaFile);
void sorting();
void tampilkanOutput();
char* ambilIsiTag(char* line, const char* tag);
void simpanFileBin(const char *namaFile);
void bacaFileBin(const char *namaFile);
void tampilkanNkata(int n);
int banding(word a, word b);
void bersihin_isi(char *isi);
