/* 
 
 BMPtoASCII, 22/11/2009 
 Francesco Ramoni
 MIT License
 
*/

#include <stdio.h>
#include <string.h>

#define TOL 40

int main() {
	
	// parte dichiarativa
	
	FILE * fbmp;
	FILE * ftext;
	
	unsigned char buffer[3];
	char next[2];
	char path[200];
	int err_seek, err_read;
	int i, j;	
	int R, G, B;
	int img_size[2] = {};
	int resolution[1] = {};
	int width, Twidth, height;
	int pointer;
	int ctable [94][3]; // caratteri dal 33 al 126: 94 caratteri
	int cells = 0;
	
	int header[1] = {};

	// parte esecutiva
	
	
	// inizializzazione delle variabili
	
	next[1] = '\0';
	for (i = 0; i <= 93; i++) {
		for (j = 0; j <= 2; j++) {
			ctable[i][j] = -50;
		}
	}
	
	// aquisisce il path del file
	
	printf("\n\nASCIItoBMP, versione del 22/11/2009. Realizzato da Francesco Ramoni.\n\n");
	printf("Inserisci il path di un file bitmap (MAX 200 caratteri):\n");
	scanf("%s", path);
	
	// apre il file e, in caso di errore, chiede di nuovo il path
		
	fbmp = fopen(path,"rb");
	while (fbmp == NULL) {
		printf("\nErrore nell'apertura del file! Reinserisci il path del file (MAX 200 caratteri):\n");
		scanf("%s", path);
		fbmp = fopen(path,"rb"); 

	}
	
	// controlla la dimensiode di HEADER
	
	err_seek = fseek(fbmp, 14, 0);
    err_read = fread(header, 4, 1, fbmp);
	printf("\nDimensioni dell'header della bitmap: %d\n", header[0]);

	// controlla la risoluzione del bmp (dev'essere 24 bit)
	
	err_seek = fseek(fbmp, 28, 0);
	if (err_seek) {
		printf("\nSi è verificato un errore. Controlla che il file bitmap non sia danneggiato. Il programma verrà terminato.\n");
		return 0;
	}
	err_read = fread(resolution, 2, 1, fbmp); // bisognerebbe gestire gli eventuali errori di lettura... :)
	if (resolution[0] != 24) {
		printf("\nIl bitmap di input non è a 24 bit e quindi non è supportata.\nSe non puoi farne a meno, puoi sempre convertirla usando GIMP. Il programma verrà terminato.");
		return 0;
	}
	else {
		printf("\nBitmap OK. Procedo con la conversione...");
	} 
	
	// estrae le dimensioni del bmp in pixel
	
	err_seek = fseek(fbmp, 18, 0);
	if (err_seek) {
		printf("\nSi è verificato un errore. Controlla che il file bitmap non sia danneggiato. Il programma verrà terminato.\n");
		return 0;
	}
	err_read = fread(img_size, 4, 2, fbmp);
	width = img_size[0];
	height = img_size[1];
	
	printf("\nLe dimensioni in pixel dell'immagine: %d x %d\n", width, height);
	
	// crea il file di testo
	
	ftext = fopen("Immagine.txt", "w");
	
	// calcola la lunghezza "reale" di una riga di pixel
	
	switch (width % 4) {
		case 1:
			Twidth = width * 3 + 3;
			break;
		case 2:
			Twidth = width * 3 + 2;
			break;
		case 3:
			Twidth = width * 3 + 1;
			break;
		default:
			Twidth = width * 3;
			break;
	}
	
	// posiziona il cursore al primo pixel dell'ultima riga
	
	pointer = 54 + Twidth * (height - 1);
	
	// converte i pixel in caratteri: qui sta la magia del software...
	
	while (pointer > 53) {		
		err_seek = fseek(fbmp, pointer, 0);
		if (err_seek) {
			printf("\nSi è verificato un errore. Controlla che il file bitmap non sia danneggiato. Il programma verrà terminato.\n");
			return 0;
		}
		for (i = 0; i < width; i++) {
			err_read = fread(buffer, 1, 3, fbmp);
			B = buffer[0];
			G = buffer[1];
			R = buffer[2];
			for (j = 0; j <= 93; j++) {
				if (((B >= (ctable[j][0] - TOL)) && (B <= (ctable[j][0] + TOL))) && ((G >= (ctable[j][1] - TOL)) && (G <= (ctable[j][1] + TOL))) 
					&& ((R >= (ctable[j][2] - TOL)) && (R <= (ctable[j][2] + TOL)))) { // tolleranza era a 40
					next[0] = ((char) j) + 33;
					break; 
				}
				else {
					if (!(cells == 94)) {
						ctable[cells][0] = B;
						ctable[cells][1] = G;
						ctable[cells][2] = R;
						next[0] = ((char) cells) + 33;
						cells++;
						break;   
					}
				}
				
			}
			
			fprintf(ftext, next);
			fprintf(ftext, " "); // uno spazio per non "soffocare" l'immagine
			strcpy(next, " \0");				
		}
		fprintf(ftext, "\n");
		pointer -= Twidth;
	}
		
	fclose(fbmp);
	fclose(ftext);
	
	printf("\n\nConversione completata! Il file di testo generato si chiama 'Immagine.txt'. \nNota che se il bitmap era molto grande, sarà necessario diminuire\nla grandezza del font per poter visualizzare correttamente l'immagine.\n");
	
	return 0;
}


