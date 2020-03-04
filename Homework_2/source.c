#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SAMPLE_NAME_LENGTH 50
#define MAX_STR_LENGTH 50
#define MAX_NUMOF_FILE 50
#define MAX_CONTENT_LENGTH 1000
#define TABLE_SIZE 11
#define R_NUMBER 31

char* concatDirectory(char*, char*);
void createHashTable(char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH], char*, char*);
void insert2Table(char*, unsigned long int, char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]); 
void displayHashTable(char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]);
void addDocument(char*, char*, char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]);
unsigned long int power(int, int);

int main(int argc, char *argv[]) {
	
	char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]; // hash tablosu
	char run = 1; // mainin çalýþýr halde kalmasýný saðlayan deðiþken
	int i; // döngü deðiþkeni
	char docName[MAX_SAMPLE_NAME_LENGTH]; // okunan dökümanýn ismi
	char docDirectory[MAX_SAMPLE_NAME_LENGTH]; // okunan dökümanýn lokasyonu
	
	for(i = 0; i < TABLE_SIZE; i++){
		hashTable[i][0] = '\0';
	}
	createHashTable(hashTable, "index.txt", "directory/");	

	while(run != '0'){
		printf("Display Hash-Table        : 1\n");
		printf("Add new doc to Hash-Table : 2\n");
		printf("Exit                      : 0\n");
		scanf(" %c", &run);
		switch(run){
			case '0': {
				printf("Program ended.");
				break;
			}
			case '1': {
				displayHashTable(hashTable);
				break;
			}
			case '2': {
				printf("Enter document's name : ");
				scanf("%s",docName);
				printf("Enter document's directory : ");
				scanf("%s", docDirectory);
				addDocument(docName, docDirectory, hashTable);
				break;
			}
			default: {
				printf("error : Entered undefined input.\n");
			}
		} 			     
	}
	
	return 0;
}

void createHashTable(char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH], char* fileName, char* directory) {
	
	FILE* samples = NULL; // index dosyasý
	FILE* doc = NULL; // dökümanlar
	char sampleNames[MAX_NUMOF_FILE][MAX_SAMPLE_NAME_LENGTH]; // sampleden okunan dosyalarýn isimlerinin tutulacaðý matris
	int numOfFiles = 0; // samples içerisindeki dosya sayýsý
	int i, j; // döngü deðiþkenleri
	unsigned long int key, sum; // key ve key oluþturmada kullanýlan yardýmcý sum deðeri
	char str[MAX_STR_LENGTH]; // key oluþturmada kullanýlan string dizisi
	char sampleFileName[MAX_SAMPLE_NAME_LENGTH]; // sampleden okunan string ile oluþturulan dosya uzantýsýnýn tutulduðu string
		
	samples = fopen(fileName, "r");
	if(!samples){
		printf("File not found.\n");
		return;
	}
	
	while(!feof(samples)){
		fscanf(samples, "%s", &sampleNames[numOfFiles]);
		//printf("%s\n", sampleName);
		numOfFiles++;	
	}
	fclose(samples);
	
	j = 0;
	while(j < numOfFiles){
		
		strcpy(sampleFileName, concatDirectory(directory, sampleNames[j]));
		doc = fopen(sampleFileName, "r");
		if(doc == NULL){
			printf("File not found (%s)\n", sampleFileName);
			j++;
			continue;
		}
		key = 0;
		while(!feof(doc)){
			fscanf(doc, "%s", &str);
			sum = 0;
			i = 0;
			while(str[i] != '\0'){
				sum += str[i] * power(R_NUMBER, strlen(str) - i - 1);
				i++;
			}
			key += sum;
		}
		fclose(doc);
		// key deðerini hesapladýk, tabloya ekleme iþlemi :
		insert2Table(sampleNames[j], key, hashTable);
		j++;
	}
}

void insert2Table(char* itemName, unsigned long int key, char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]){
	
	unsigned int MM = TABLE_SIZE - 1; // 2. key deðerinin hesaplanmasý için kullanýlan deðer
	unsigned long int key1 = key % TABLE_SIZE; // h1key 1 deðeri
	unsigned long int key2 = 1 + ( key % MM ); // h2key 2 deðeri
	int i = 0, j = 0; // döngü deðiþkenleri
	
	key = ( key1 + i * key2 ) % TABLE_SIZE;
	
	while( ( i < TABLE_SIZE ) && ( hashTable[ key ][0] != '\0' ) ){
		i++;
		key = ( key1 + i * key2 ) % TABLE_SIZE;
	}
	if( i < TABLE_SIZE ){
		strcpy(hashTable[key], itemName);
		return;
	}	
}

void addDocument(char* docName, char* docDirectory, char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]){
	
	FILE* doc = NULL; // eklenecek döküman
	FILE* samples = NULL; // index dosyasý
	int i = 0, j = 0; // döngü deðiþkenleri
	unsigned long int sum, key; // key deðerinin hesaplamnasýnda kullanýlan deðiþkenler
	char str[MAX_STR_LENGTH]; // dökümandan okumada kullanýlacak string
	char doc1Content[MAX_CONTENT_LENGTH] = ""; // eklenecek dökümanýn içeriði
	char doc2FileName[MAX_SAMPLE_NAME_LENGTH];// veri tabanýnda içeriði kontrol edilecek dökümanýn ismi
	char doc1FileName[MAX_SAMPLE_NAME_LENGTH];// eklenecek dökümanýn ismi
	char c;
	
	strcpy(doc1FileName, concatDirectory(docDirectory, docName));
	
	doc = fopen(doc1FileName, "r");
	if(doc == NULL){
		printf("error : Document not found (%s)\n", doc1FileName);
		return;
	}
	key = 0;
	while(!feof(doc)){
		fscanf(doc, "%s", &str);
		i = 0;
		sum = 0;
		while(str[i] != '\0'){
			sum += str[i] * power(R_NUMBER, strlen(str) - i - 1);
			i++;
		}
		key += sum;	
	}
	fseek(doc, 0, SEEK_SET);
	i = 0;
	
	while(!feof(doc)){	
		c = fgetc(doc);
		if(c != EOF)
			doc1Content[i] = c;
		i++;	
	}	
	fclose(doc);
	doc1Content[i] = '\0';
	
	unsigned int MM = TABLE_SIZE - 1;// 2. key deðerinin hesaplanmasý için kullanýlan deðer
	unsigned long int key1 = key % TABLE_SIZE;// h1key deðeri
	unsigned long int key2 = 1 + ( key % MM );// h2key deðeri
	i = 0;
	key = ( key1 + i * key2 ) % TABLE_SIZE;
	printf("Start Key %lu\n", key);
	while( ( i < TABLE_SIZE ) && ( hashTable[ key ][0] != '\0') ){
		
		strcpy(doc2FileName, concatDirectory("directory/", hashTable[key]));
		j = 0;
		doc = fopen(doc2FileName, "r");
		while(!feof(doc) && doc1Content[j] != '\0' && fgetc(doc) == doc1Content[j]){
			j++;	
		}
		if(!feof(doc))
			fgetc(doc);
		if(doc1Content[j] == '\0' && feof(doc)){
			printf("error : Document's content is same with (%s)\n", hashTable[key]);
			fclose(doc);
			return;
		}else{
			i++;
			key = ( key1 + i * key2 ) % TABLE_SIZE;
			printf("Collusion for document %s New Key : %lu\n", docName, key);
		}	
	}
	fclose(doc);
	if(hashTable[ key ][0] == '\0'){
		strcpy(hashTable[key], docName);
		strcpy(doc1FileName, concatDirectory("directory/", docName));
		doc = fopen(doc1FileName, "w");
		fputs(doc1Content, doc);
		fclose(doc);
		doc = fopen("index.txt", "a");
		fputs(docName, doc);
		fputc('\n', doc);
		fclose(doc);
		printf("Document successfully added to Hash-Map.\n");
		return;
	}
	
	
	printf("error : Hash-Table is full.\n");
	
}

char* concatDirectory(char* directory, char* sampleName){
	
	char* newString = malloc( strlen(directory) + strlen(sampleName) + 2 );
	strcpy(newString, "");
	strcat(newString, directory);
	strcat(newString, sampleName);
	strcat(newString, ".txt");
	return newString;
}

void displayHashTable(char hashTable[TABLE_SIZE][MAX_SAMPLE_NAME_LENGTH]){
	int i;
	printf("HASH TABLE\n");
	for(i = 0; i < TABLE_SIZE; i++){
		printf("Table %d => %s\n", i, hashTable[i]);
	}	
}

unsigned long int power(int base, int exponent){ // bizim üssümüz 0 dan küçük olamayacaðý için yeterli üs alma fonksiyonu.
	unsigned int result = 1;
	if(exponent > 0){
		int i;
		for(i = 0; i < exponent; i++)
			result *= base;
	}
	return result;
}
