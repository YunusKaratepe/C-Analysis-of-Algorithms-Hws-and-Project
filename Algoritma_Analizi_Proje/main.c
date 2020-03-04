#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACTOR_NAME 60
#define MAX_MOVIE_NAME 100
#define R_NUMBER 31
#define MAX_ACTOR_NUM 300000
#define MAX_MOVIE_NUM 15000
#define QUEUE_SIZE 500000

typedef struct actor{
	char name[MAX_ACTOR_NAME]; // aktörün adý
	struct movie** movies; // aktörün oynadýðý filmerin adreslerini tutan dizi
	unsigned char num_movies; // bir aktör en fazla 255 filmde oynar kabul edildi --
	unsigned int visited:1; // bu aktörün daha önceden gezilmiþ olduðunu gösteren visited deðiþkeni
	struct movie* parent_movie; // graf üzerinde gezilen yolu yazdýrmaya yardýmcý olacak parent_movie deðiþkeni
}ACTOR;

typedef struct movie{ // ayný struct yapýsýnýn film için olaný
	char name[MAX_MOVIE_NAME];
	struct actor** actors;
	unsigned char num_actors; // bir filmde en fazla 255 aktör oynar kabul edildi --
	unsigned int visited:1;
	struct actor* parent_actor;
}MOVIE;

typedef struct que_actor{
	ACTOR* array[QUEUE_SIZE]; // kuyruk dizisi
	int start; // kuyruðun ilk elemanýný tutan indis.
	int end; // kuyruða eklenecek elemanýn eklenecek indisini tutan deðiþken.
}Q_ACTOR;
typedef struct que_movie{ // ayný kuyruðun film için olaný
	MOVIE* array[QUEUE_SIZE]; 
	int start;
	int end;
}Q_MOVIE;	


unsigned long int insertActor(char*, ACTOR*); // aktör ekleyip ekledikten sonra eklenen indisin ( key ) deðerini döndüren fonksiyon
ACTOR* searchActor(char*, ACTOR*); // aktörü arayýp, bulunursa struct actor olarak aktörün adresini, bulunamaz ise NULL döndüren fonksiyon
unsigned long int power(int base, int exponent); // üst alma fonksiyonu
void bfs(ACTOR*, MOVIE*, char*, char*); // breadth first search yaparak kaç adýmda bulunduðunu ve bulunurken izlenen yolu ekrana yazdýran fonksiyon
void enqueActor(ACTOR*, Q_ACTOR*); // aktör kuyruðuna aktör ekleyen fonksiyon
void enqueMovie(MOVIE*, Q_MOVIE*); // movie kuyruðuna movie ekleyen fonksiyon
ACTOR* dequeActor(Q_ACTOR*); // aktör kuyruðundan aktörün adresini döndüren fonksiyon
MOVIE* dequeMovie(Q_MOVIE*); // movie kuyruðundan movienin adresini döndüren fonksiyon
void displayPath(ACTOR*, int); // bulunan yolu ( tersten sýralý ) baþtan sona yazdýrmak için kullandýðým, yolu ekrana yazan fonksiyon



int main(){
	
	char c; // dosyadan karakter-karakter okuma iþleminde kullanýlan buffer
	char tmp_movie[ MAX_MOVIE_NAME ]; // dosyadan anlýk olarak okunan film
	char tmp_actor[ MAX_ACTOR_NAME ]; // dosyadan anlýk olarak okunan aktör
	int i; // döngülerde kullanýlan indis deðiþkeni
	int movie_index = 0; // hangi satýrdaki filmin okunduðunu gösteren deðiþken ( örn: movie_index = 50 --> 50. 49. satýrdaki filmin okuma iþlemi )
	unsigned long int key; // aktörün hashmapteki indisini tutan deðiþken
	
	// movies ve actors dizilerinin tanýmlanmasý ve ayarlanmasý --
	MOVIE* movies = (MOVIE*) malloc (sizeof(MOVIE) * MAX_MOVIE_NUM); // movie dizisii
	ACTOR* actors = (ACTOR*) malloc (sizeof(ACTOR) * MAX_ACTOR_NUM); // actor dizisi
	
	
	// dosyadan okuma iþlemi --
	FILE* fp = fopen("data.txt", "r");
	c = getc(fp);
	while(c != EOF){
		i = 0;
		while(c != '/'){
			tmp_movie[i] = c;
			i++;
			c = getc(fp);
		}
		tmp_movie[i] = '\0';	
		strcpy(movies[ movie_index ].name, tmp_movie);
		movies[ movie_index ].num_actors = 0;
		movies[ movie_index ].visited = 0;
		
		movies[ movie_index ].actors = (ACTOR**) malloc(sizeof(ACTOR*));
		while(c != '\n' && c != EOF){
			i = 0;
			c = getc(fp);
			while(c != '/' && c != '\n' && c != EOF){
				tmp_actor[i] = c;
				i++;
				c = getc(fp);
			}
			tmp_actor[i] = '\0';
			
			key = insertActor(tmp_actor, actors);
			
			movies[ movie_index ].actors = (ACTOR**) realloc(movies[ movie_index ].actors, sizeof(ACTOR*) * ( 1 + movies[ movie_index ].num_actors));
			if(actors[ key ].num_movies == 0){
				actors[ key ].movies = (MOVIE**) malloc(sizeof(MOVIE*));
			}
			else{
				actors[key].movies = (MOVIE**) realloc( actors[key].movies, ( 1 + actors[key].num_movies ) * sizeof(MOVIE*) );	
			}
			
			movies[ movie_index ].actors[ movies[ movie_index ].num_actors ] = &actors[key];
			actors[ key ].movies[ actors[ key ].num_movies ] = &movies[ movie_index ];
			
			actors[key].num_movies++;
			movies[ movie_index ].num_actors++;
		}
		
		//printf("Film Adi : %s\n", movies[movie_index].name);
		movie_index++;
		c = getc(fp);
	}
	fclose(fp);
	
	char source_actor[MAX_ACTOR_NAME]; // kullanýcýdan alýnacak baþlangýç aktörü
	char dest_actor[MAX_MOVIE_NAME]; // kullanýcýdan alýnacak hedef aktörü
	
	
	printf("Enter first actor: ");
	gets(source_actor);
	printf("Enter destination actor: ");
	gets(dest_actor);
	
	bfs(actors, movies, source_actor, dest_actor);
	
	free(movies);
	free(actors);
	
	return 0;
}

unsigned long int insertActor(char* actor_name, ACTOR* actor_list){
	
	unsigned int i = 0; // linear probing için kullanýlan deðiþken
	unsigned long int key = 0; // strinden oluþturulan key sayýsý
	
	while(actor_name[i] != '\0'){
		key += actor_name[i] * power(R_NUMBER, i);
		i++;
	}
	key = key % MAX_ACTOR_NUM;
	
	i = 0;
	while( i < MAX_ACTOR_NUM && actor_list[ (i + key) % MAX_ACTOR_NUM ].name[0] != '\0' && 
			strcmp( actor_list[ (i + key) % MAX_ACTOR_NUM ].name, actor_name ) != 0 )		
		i++;
	
	// bulunma durumu --
	if( strcmp( actor_list[ (key + i) % MAX_ACTOR_NUM ].name, actor_name ) == 0 )
		return ( (key + i) % MAX_ACTOR_NUM); // bulunduðu indis deðeri olan key'i döndürüyoruz --
	
	if( i < MAX_ACTOR_NUM ){
		strcpy( actor_list[ (key + i) % MAX_ACTOR_NUM ].name, actor_name );
		actor_list[ (key + i) % MAX_ACTOR_NUM ].visited = 0;
		actor_list[ (key + i) % MAX_ACTOR_NUM ].num_movies = 0;
		return ( (key + i) % MAX_ACTOR_NUM); // eklediðimiz indis deðeri olan key'i döndürüyoruz --
	}
	else{
		printf("Error: Map-Size insufficient.\n");
	}
}

ACTOR* searchActor(char* actor_name, ACTOR* actor_list){
	
	unsigned int i = 0; // linear probing için kullanýlan deðiþken
	unsigned long int key = 0; // strinden oluþturulan key sayýsý
	
	while(actor_name[i] != '\0'){
		key += actor_name[i] * power(R_NUMBER, i);
		i++;
	}
	key = key % MAX_ACTOR_NUM;
	
	i = 0;
	while( i < MAX_ACTOR_NUM && actor_list[ (i + key) % MAX_ACTOR_NUM ].name[0] != '\0' && 
			strcmp( actor_list[ (i + key) % MAX_ACTOR_NUM ].name, actor_name ) != 0 )		
		i++;
	
	// bulunma durumu --
	if( strcmp( actor_list[ (key + i) % MAX_ACTOR_NUM ].name, actor_name ) == 0 )
		return &actor_list[ ( (key + i) % MAX_ACTOR_NUM) ]; // aktörün adresini döndürüyoruz.
	
	printf("Error: Actor not found.\n");
	return NULL;
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

void enqueActor(ACTOR* actor, Q_ACTOR* que){
	que->array[ que->end ] = actor;
	que->end++;
	actor->visited = 1;
}

void enqueMovie(MOVIE* movie, Q_MOVIE* que){
	que->array[ que->end ] = movie;
	que->end++;
	movie->visited = 1;
}

ACTOR* dequeActor(Q_ACTOR *que){
	if(que->start == que->end){
		printf("Error: Actor queue is empty.");
		return NULL; // que boþtur.
	}
	que->start++;
	return que->array[ que->start - 1 ];
}

MOVIE* dequeMovie(Q_MOVIE *que){
	if(que->start == que->end){
		printf("Error: Movie queue is empty.");
		return NULL; // que boþtur.
	}
	que->start++;
	return que->array[ que->start - 1 ];
}


void bfs(ACTOR* actors, MOVIE* movies, char* source, char* dest){
	
	Q_ACTOR* q_actor = (Q_ACTOR*) malloc(sizeof(Q_ACTOR)); // aktör kuyruðu
	Q_MOVIE* q_movie = (Q_MOVIE*) malloc(sizeof(Q_MOVIE)); // film kuyruðu
	q_actor->start = 0; // aktör kuyruðunun start indisi 0 lanýyor
	q_actor->end = 0; // aktör kuyruðunun end indisi 0 lanýyor
	q_movie->start = 0; // movie kuyruðunun baþlangýç deðeri 0 lanýyor
	q_movie->end = 0; // movie kuyruðunun end deðeri 0 lanýyor
	int distance = 0; // 2 aktörün birbirine olan uzaklýðý
	int found = 0; // bulundu deðiþkeni
	int i; // döngü deðiþkeni
	

	
	ACTOR* tmp_actor = searchActor(source, actors); // Armstrond Darrell
	ACTOR* dest_actor = searchActor(dest, actors); // Beatty Warren
	if(tmp_actor == NULL || dest_actor == NULL){
		printf("Error: Destination or Source actors missing.");
		return;
	}
	MOVIE* tmp_movie = (MOVIE*) malloc(sizeof(MOVIE));
	enqueActor(tmp_actor, q_actor);
	while( distance <= 6 && (!found) ){

		while( q_actor->start < q_actor->end && (!found) ){
			tmp_actor = dequeActor(q_actor);
			//printf("Aktor: %s Oynadigi Film Sayisi: %d\n", tmp_actor->name, tmp_actor->num_movies);
			if( dest_actor == tmp_actor ){
				found = 1;
			}
			else{
				for(i = 0; i < tmp_actor->num_movies; i++){
					if( tmp_actor->movies[i]->visited == 0 ){
						tmp_actor->movies[i]->parent_actor = tmp_actor;
						enqueMovie(tmp_actor->movies[i], q_movie);
					}
				}
			}
		}
		if(found == 0){
			distance++;
			while( q_movie->start < q_movie->end ){
				tmp_movie = dequeMovie(q_movie);
				for(i = 0; i < tmp_movie->num_actors; i++){
					if( tmp_movie->actors[i]->visited == 0 ){
						tmp_movie->actors[i]->parent_movie = tmp_movie;
						enqueActor(tmp_movie->actors[i], q_actor);
					}
				}
			}
		}
	}
	
	printf("Source Actor     : %s\n", source);
	printf("Destination Actor: %s\n", dest);
	
	
	if(distance > 6){	
		printf("There is no connection.\n");
	}else{
		printf("There is connection with distance: %d\n", distance);
	}
	
	printf("Path:\n");
	displayPath(dest_actor, distance);
	
	
	free(q_actor);
	free(q_movie);
}

void displayPath(ACTOR* dest_actor, int distance){
	
	if(distance > 1)
		displayPath(dest_actor->parent_movie->parent_actor, distance - 1);
	
	if(distance > 0)
		printf("%s - %s : \"%s\"  \n", dest_actor->name, dest_actor->parent_movie->parent_actor->name, dest_actor->parent_movie->name);
}
