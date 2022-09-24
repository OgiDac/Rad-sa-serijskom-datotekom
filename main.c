#include "stdio.h"
#include "stdlib.h"

#include "operacije_nad_datotekom.h"

// gcc *.c && ./a.out

int main() {

	int running = 1;
	int userInput;

	int dan;
    int mjesec;
    int godina;
    char sgodina[5];
    char sdan[3];
    char smjesec[3];
    char datum[11];

	FILE *fajl = NULL;

	while (running) {
		printf("Odaberite opciju:\n");
		printf("1 - Otvaranje datoteke\n");
		printf("2 - Formiranje datoteke\n");
		printf("3 - Pretraga datoteke\n");
		printf("4 - Unos sloga\n");
		printf("5 - Ispis svi slogova\n");
		//printf("6 - Azuriranje sloga\n");
		//printf("7 - Brisanje sloga (logicko)\n");
		printf("6 - Brisanje sloga (fiziko)\n");
		printf("7 - Najskuplja projekcija\n");
		printf("8 - Filmovi za sve tipove\n");
		printf("0 - Izlaz\n");
		if (fajl == NULL) {
			printf("!!! PAZNJA: datoteka jos uvek nije otvorena !!!\n");
		}
		scanf("%d", &userInput);
		getc(stdin);

		switch(userInput) {
			case 1:
				{
					char filename[20];
					printf("Unesite ime datoteke za otvaranje: ");
					scanf("%s", &filename[0]);
					fajl = otvoriDatoteku(filename);
					printf("\n");
					break;
				}
			case 2:
				{
					char filename[20];
					printf("Unesite ime datoteke za kreiranje: ");
					scanf("%s", filename);
					kreirajDatoteku(filename);
					printf("\n");
					break;
				}
			case 3:
				{
					int sifraProjekcije;
					printf("Unesite sifru projekcije trazenog prijema: ");
					scanf("%d", &sifraProjekcije);
					SLOG *slog = pronadjiSlog(fajl, sifraProjekcije);
					if (slog == NULL) {
                        printf("Nema tog sloga u datoteci.\n");
					} else {
                        printf("Evid.Br   Sif.Zat      Dat.Vrem.Dol  Celija  Kazna\n");
                        ispisiSlog(slog);
                        printf("\n");
					}
					printf("\n");
					break;
				}
			case 4:
				{
					SLOG slog;



					printf("Sifra projekcije: ");
					scanf("%d", &slog.sifraProjekcije);
					printf("Naziv filma: ");
					scanf("%s", slog.nazivFilma);



					printf("Datum dolaska (dd-mm-YYYY): ");
					scanf("%d-%d-%d", &dan, &mjesec, &godina);
                    itoa(godina, sgodina, 10);
                    itoa(mjesec, smjesec, 10);
                    itoa(dan, sdan, 10);

                    strcat(datum, sdan);
                    strcat(datum, "-");

                    strcat(datum, smjesec);
                    strcat(datum, "-");

                    strcat(datum, sgodina);
					strcpy(slog.datum,datum);
					strcpy(datum, "");
					if(godina < 2021) {
                        strcat(slog.nazivFilma, "*");
					}
					else if(mjesec < 6 && godina == 2021) {
                        strcat(slog.nazivFilma, "*");
					}



					printf("Trajanje: ");
					scanf("%d", &slog.trajanje);
					printf("Cena: ");
					scanf("%d", &slog.cena);
					printf("Tip projekcije: ");
					scanf("%s", slog.tipProjekcije);
					slog.deleted = 0;
					dodajSlog(fajl, &slog);
					printf("\n");
					break;
				}
			case 5:
				{
					ispisiSveSlogove(fajl);
					printf("\n");
					break;
				}

			case 6:
				{
					int evidBroj;
					printf("Unesite evid. broj sloga za FIZICKO brisanje: ");
					scanf("%d", &evidBroj);
					obrisiSlogFizicki(fajl, evidBroj);
					printf("\n");
					break;
				}
            case 7:
                {
                    SLOG* slog;
                    slog = pronadjiNajskupljiSlog(fajl);
                    ispisiSlog(slog);
                    break;
                }
            case 8:
                {
                    char tipoviProjekcija[100][4];
                    //strcpy(tipoviProjekcija[0], "adsad");
                    int size = pronadjiTipoveProjekcija(fajl, tipoviProjekcija);
                    for(int i = 0; i< size; i++) {
                        printf("Za tip projekcije %s: \n", tipoviProjekcija[i]);
                        ispisiZaTip(fajl, tipoviProjekcija[i]);
                    }
                    break;
                }
			case 0:
				{
					running = 0;
					if (fajl != NULL) {
						fclose(fajl);
					}
				}
		}
	}

	return 0;

}




