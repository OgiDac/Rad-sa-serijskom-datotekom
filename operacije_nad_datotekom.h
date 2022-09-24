#ifndef OPERACIJE_NAD_DATOTEKOM_H
#define OPERACIJE_NAD_DATOTEKOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include "definicije_struktura_podataka.h"

FILE *otvoriDatoteku(char *filename);
//void kreirajDatoteku(char *filename);
SLOG *pronadjiSlog(FILE *fajl, int sifraProjekcije);
void dodajSlog(FILE *fajl, SLOG *slog);
void ispisiSveSlogove(FILE *fajl);
//void ispisiSlog(SLOG *slog);
//void azurirajSlog(FILE *fajl, int sifraProjekcije, char *oznakaCelije, int duzinaKazne);
//void obrisiSlogLogicki(FILE *fajl, int sifraProjekcije);
void obrisiSlogFizicki(FILE *fajl, int sifraProjekcije);

#endif
