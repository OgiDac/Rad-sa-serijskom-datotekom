#include "operacije_nad_datotekom.h"

FILE *otvoriDatoteku(char *filename) {
	FILE *fajl = fopen(filename, "rb+");
	if (fajl == NULL) {
		printf("Doslo je do greske! Moguce da datoteka \"%s\" ne postoji.\n", filename);
	} else {
		printf("Datoteka \"%s\" otvorena.\n", filename);
	}
	return fajl;
}

void kreirajDatoteku(char *filename) {
	FILE *fajl = fopen(filename, "wb");
	if (fajl == NULL) {
		printf("Doslo je do greske prilikom kreiranja datoteke \"%s\"!\n", filename);
	} else {
		//upisi pocetni blok
		BLOK blok;
		blok.slogovi[0].sifraProjekcije = OZNAKA_KRAJA_DATOTEKE;
		fwrite(&blok, sizeof(BLOK), 1, fajl);
		printf("Datoteka \"%s\" uspesno kreirana.\n", filename);
		fclose(fajl);
	}
}

SLOG *pronadjiSlog(FILE *fajl, int sifraProjekcije) {
	if (fajl == NULL) {
		return NULL;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;

	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {
				//nema vise slogova
				return NULL;
			}

			if (blok.slogovi[i].sifraProjekcije == sifraProjekcije && !blok.slogovi[i].deleted) {
                //Ako se sifraProjekcije poklapa i slog NIJE logicki obrisan
                //(logicki obrisane slogove tretiramo kao da i ne
                //postoje u datoteci).

				SLOG *slog = (SLOG *)malloc(sizeof(SLOG));
				memcpy(slog, &blok.slogovi[i], sizeof(SLOG));
				return slog;
			}
		}
	}

	return NULL;
}

SLOG *pronadjiNajskupljiSlog(FILE *fajl) {
	if (fajl == NULL) {
		return NULL;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
    SLOG *najskuplji = (SLOG *)malloc(sizeof(SLOG));
    najskuplji->cena = 0;
	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {
				//nema vise slogova
				return najskuplji;
			}

			if (!blok.slogovi[i].deleted) {
                //Ako se sifraProjekcije poklapa i slog NIJE logicki obrisan
                //(logicki obrisane slogove tretiramo kao da i ne
                //postoje u datoteci).

				if(((double)blok.slogovi[i].cena/(double)blok.slogovi[i].trajanje) > ((double)najskuplji->cena/(double)najskuplji->trajanje))
                    memcpy(najskuplji, &blok.slogovi[i], sizeof(SLOG));
			}
		}
	}

	return najskuplji;
}

void dodajSlog(FILE *fajl, SLOG *slog) {
	if (fajl == NULL) {
		printf("Datoteka nije otvorena!\n");
		return;
	}

	SLOG *slogStari = pronadjiSlog(fajl, slog->sifraProjekcije);
	if (slogStari != NULL) {
        //U datoteci vec postoji slog sa tim evid. brojem,
        //pa ne mozemo upisati novi slog.
        printf("Vec postoji slog sa tim evid brojem!\n");
        return;
    }

	BLOK blok;
	fseek(fajl, -sizeof(BLOK), SEEK_END); //u poslednji blok upisujemo novi slog
	fread(&blok, sizeof(BLOK), 1, fajl);

	int i;
	for (i = 0; i < FBLOKIRANJA; i++) {
		if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {
            //Ovo je mesto gde se nalazi slog sa oznakom
            //kraja datoteke; tu treba upisati novi slog.
			memcpy(&blok.slogovi[i], slog, sizeof(SLOG));
			break;
		}
	}

	i++; //(na to mesto u bloku cemo upisati krajDatoteke)

	if (i < FBLOKIRANJA) {
        //Jos uvek ima mesta u ovom bloku, mozemo tu smestiti slog
        //sa oznakom kraja datoteke.
		blok.slogovi[i].sifraProjekcije = OZNAKA_KRAJA_DATOTEKE;

		//Sada blok mozemo vratiti u datoteku.
		fseek(fajl, -sizeof(BLOK), SEEK_CUR);
		fwrite(&blok, sizeof(BLOK), 1, fajl);
	} else {
		//Nema vise mesta u tom bloku, tako da moramo
        //praviti novi blok u koji cemo upisati slog
        //sa oznakom kraja datoteke.

		//Prvo ipak moramo vratiti u datoteku blok
        //koji smo upravo popunili:
		fseek(fajl, -sizeof(BLOK), SEEK_CUR);
		fwrite(&blok, sizeof(BLOK), 1, fajl);

		//Okej, sad pravimo novi blok:
		BLOK noviBlok;
		noviBlok.slogovi[0].sifraProjekcije = OZNAKA_KRAJA_DATOTEKE;
		//(vec smo na kraju datoteke, nema potrebe za fseekom)
		fwrite(&noviBlok, sizeof(BLOK), 1, fajl);
	}

	if (ferror(fajl)) {
		printf("Greska pri upisu u fajl!\n");
	} else {
		printf("Upis novog sloga zavrsen.\n");
	}
}

void ispisiSveSlogove(FILE *fajl) {
	if (fajl == NULL) {
		printf("Datoteka nije otvorena!\n");
		return;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
	int rbBloka = 0;
	printf("BL SL Sifra projekcije   Naziv      Datum       Trajanje    Cena    TipProjekcije\n");
	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {
				printf("B%d S%d *\n", rbBloka, i);
                break; //citaj sledeci blok
			}


			if (!blok.slogovi[i].deleted) {
                printf("B%d S%d ", rbBloka, i);
                ispisiSlog(&blok.slogovi[i]);
                printf("\n");
            }
		}

		rbBloka++;
	}
}

void ispisiSlog(SLOG *slog) {
	printf("%d                  %s    %s      %d      %d      %s\n",
        slog->sifraProjekcije,
		slog->nazivFilma,
		slog->datum,
		slog->trajanje,
		slog->cena,
		slog->tipProjekcije);
}


void obrisiSlogFizicki(FILE *fajl, int sifraProjekcije) {

    SLOG *slog = pronadjiSlog(fajl, sifraProjekcije);
    if (slog == NULL) {
        printf("Slog koji zelite obrisati ne postoji!\n");
        return;
    }

    //Trazimo slog sa odgovarajucom vrednoscu kljuca, i potom sve
    //slogove ispred njega povlacimo jedno mesto unazad.

    BLOK blok, naredniBlok;
    int evidBrojZaBrisanje;
    evidBrojZaBrisanje = sifraProjekcije;

    fseek(fajl, 0, SEEK_SET);
    while (fread(&blok, 1, sizeof(BLOK), fajl)) {
        for (int i = 0; i < FBLOKIRANJA; i++) {

            if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {

                if (i == 0) {
                    //Ako je oznaka kraja bila prvi slog u poslednjem bloku,
                    //posle brisanja onog sloga, ovaj poslednji blok nam
                    //vise ne treba;
                    printf("(skracujem fajl...)\n");
                    fseek(fajl, -sizeof(BLOK), SEEK_END);
					long bytesToKeep = ftell(fajl);
					ftruncate(fileno(fajl), bytesToKeep);
					fflush(fajl); //(da bi se odmah prihvatio truncate)
                }

                printf("Slog je fizicki obrisan.\n");
                return;
            }

            if (blok.slogovi[i].sifraProjekcije == evidBrojZaBrisanje) {

               /* if (blok.slogovi[i].sifraProjekcije == sifraProjekcije && blok.slogovi[i].deleted) {
                    //To jeste evidBroj koji je koristnik trazio, ali je logicki obrisan,
                    //a ne zelimo da korisnik moze obrisati logicki obirsane slogove.
                    //Trazimo dalje, da nadjemo slog koji ima odgovarajuci evidBroj i
                    //NIJE logicki obrisan.
                    continue;
                }*/

                //Obrisemo taj slog iz bloka tako sto sve slogove ispred njega
                //povucemo jedno mesto unazad.
                for (int j = i+1; j < FBLOKIRANJA; j++) {
                    memcpy(&(blok.slogovi[j-1]), &(blok.slogovi[j]), sizeof(SLOG));
                }

                //Jos bi hteli na poslednju poziciju u tom bloku upisati prvi
                //slog iz narednog bloka, pa cemo zato ucitati naredni blok...
                int podatakaProcitano = fread(&naredniBlok, sizeof(BLOK), 1, fajl);

                //...i pod uslovom da uopste ima jos blokova posle trenutnog...
                if (podatakaProcitano) {

                    //(ako smo nesto procitali, poziciju u fajlu treba vratiti nazad)
                    fseek(fajl, -sizeof(BLOK), SEEK_CUR);

                    //...prepisati njegov prvi slog na mesto poslednjeg sloga u trenutnom bloku.
                    memcpy(&(blok.slogovi[FBLOKIRANJA-1]), &(naredniBlok.slogovi[0]), sizeof(SLOG));

                    //U narednoj iteraciji while loopa, brisemo prvi slog iz narednog bloka.
                    evidBrojZaBrisanje = naredniBlok.slogovi[0].sifraProjekcije;
                }

                //Vratimo trenutni blok u fajl.
                fseek(fajl, -sizeof(BLOK), SEEK_CUR);
                fwrite(&blok, sizeof(BLOK), 1, fajl);
                fseek(fajl, 0, SEEK_CUR);

                if (!podatakaProcitano) {
                    //Ako nema vise blokova posle trentnog, mozemo prekinuti algoritam.
                    printf("Slog je fizicki obrisan.\n");
                    return;
                }

                //To je to, citaj sledeci blok
                break;
            }

        }
    }
}


int pronadjiTipoveProjekcija(FILE* fajl, char tipoviProjekcija[100][4]) {
    if (fajl == NULL) {
		return NULL;
	}
	printf("%s", tipoviProjekcija[0]);
    int size = 0;
    int j = 0;

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {
				//nema vise slogova
				return size;
			}

			if (!blok.slogovi[i].deleted) {
                //Ako se sifraProjekcije poklapa i slog NIJE logicki obrisan
                //(logicki obrisane slogove tretiramo kao da i ne
                //postoje u datoteci).
                for(j = 0; j< size; j++) {
                    if(strcmp(tipoviProjekcija[j], blok.slogovi[i].tipProjekcije) == 0)
                        break;
                }
                if(j == size) {
                    strcpy(tipoviProjekcija[size], blok.slogovi[i].tipProjekcije);
                    size++;
                }
			}
		}
	}

	return size;

}

void ispisiZaTip(FILE* fajl, char* tip) {
if (fajl == NULL) {
		return NULL;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraProjekcije == OZNAKA_KRAJA_DATOTEKE) {
				//nema vise slogova
				return;
			}

			if (!blok.slogovi[i].deleted && (strcmp(blok.slogovi[i].tipProjekcije, tip)==0)) {
                ispisiSlog(&blok.slogovi[i]);

			}
		}
	}

	return ;


}

