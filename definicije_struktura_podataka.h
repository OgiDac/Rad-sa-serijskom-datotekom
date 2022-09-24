#define FBLOKIRANJA 3
#define OZNAKA_KRAJA_DATOTEKE -1

//Svaki slog predstavlja jedan dolazak.
typedef struct {
	int sifraProjekcije;
	char nazivFilma[31];
	char datum[11];
    int trajanje;
    int cena;
    char tipProjekcije[3];
	int deleted;    //1 - logicki obrisan slog, 0 - nije obrisan
} SLOG;

typedef struct Blok {
	SLOG slogovi[FBLOKIRANJA];
} BLOK;
