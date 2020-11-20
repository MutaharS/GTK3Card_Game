#include<gtk/gtk.h> // this include will most likely have a squiggly line under it, ignore that
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<time.h>

// CONSTANTS
#define ARRAY_SIZE 19   // array size 19 since i want to index arr[1] to arr[18]
#define BUFFER_SIZE 10  // character size for filenames (yes, the source names are quite small)
#define FLIPPED 2
void shuffle(char** array); // going to shuffle the source names to randomize the board

/*ENUM ARRAY TO CHECK IF CARD IS FLIPPED OVER
enum orientation{BACK,FRONT};
enum orientation card_pose[19];
*/
// Declare functions to avoid a million compiler warnings
void button1_setup();void button2_setup();void button3_setup();void button4_setup();
void button5_setup();void button6_setup();void button7_setup();void button8_setup();
void button9_setup();void button10_setup();void button11_setup();void button12_setup();
void button13_setup();void button14_setup();void button15_setup();void button16_setup();
void button17_setup();void button18_setup();

// GtkWidget setup
GtkBuilder      *builder; 
GtkWidget       *window;
GtkWidget       *fixed1;
// images
GtkWidget *image[19];  // 19 size: to start indexing from 1
// buttons
GtkWidget *button[19];
//cardsources (used to check if cards match, as well as getting images)
char** cardsource;  // start indexing at 1 for my brain to be able to relate things easier eg: (image[1],button[1],cardsource[1])
int cards_flipped = 0;
// Will store the current cards flipped
int f_index = 0;
char** flipped_cards;
int* flipped_card_nbrs;

int showing_cards = 0;

int main(int argc, char *argv[])
{
    srand(time(0));
    flipped_card_nbrs = malloc(FLIPPED*sizeof(int));
    cardsource = (char**) malloc(ARRAY_SIZE * sizeof(char*));
    flipped_cards = (char**) malloc(FLIPPED * sizeof(char*));
    for(int i = 1; i < ARRAY_SIZE; i++){
        cardsource[i] = (char*)malloc(BUFFER_SIZE*sizeof(char));
    }
    for(int i = 0; i < FLIPPED; i++) {
        flipped_cards[i] = malloc(BUFFER_SIZE*sizeof(char));
    }

    // Access the subdirectory cards that has the filenames and read them in
    // Filename is read to two of the array slots, since two cards will be the same card
    // This will also act as a way to check if two cards are the same (if their source is the same)
    FILE *f1 = popen("ls cards", "r");

	char* ftmp = malloc(20*sizeof(char));
    char* filename = malloc(20*sizeof(char));
	int i = 1;

	while (1) { 
		if (!fgets(ftmp, 1024, f1)) break;
		ftmp[strlen(ftmp)-1] = 0;	// chop new line character
        strcpy(filename, "cards/");	// subdirectory
		strcat(filename, ftmp);		// image file name
		//printf("%s\n", filename);
        strcpy(cardsource[i],filename);
        strcpy(cardsource[i+1],filename);
		i = i + 2;
		if (i > 18) break;
	}

	fclose(f1);
    shuffle(cardsource);    // shuffle the cardsources (so we dont play same game everytime)

    // Default gtk stuff
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
    gtk_builder_connect_signals(builder, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

    // Setup Buttons
    button1_setup();button2_setup();button3_setup();button4_setup();button5_setup();button6_setup();
    button7_setup();button8_setup();button9_setup();button10_setup();button11_setup();button12_setup();
    button13_setup();button14_setup();button15_setup();button16_setup();button17_setup();button18_setup();

    g_object_unref(builder);

    // Show the window
    gtk_widget_show(window);                
    gtk_main();

    return 0;
}

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

gboolean hide_tooltip (gpointer data)
{
    gtk_container_remove( GTK_CONTAINER(button[flipped_card_nbrs[0]]),image[flipped_card_nbrs[0]]);  
                    gtk_container_remove( GTK_CONTAINER(button[flipped_card_nbrs[1]]),image[flipped_card_nbrs[1]]);  
                    image[flipped_card_nbrs[0]] = gtk_image_new_from_file("card_back.png"); 
                    image[flipped_card_nbrs[1]] = gtk_image_new_from_file("card_back.png"); 
                    gtk_container_add(GTK_CONTAINER(button[flipped_card_nbrs[0]]),image[flipped_card_nbrs[0]]);       
                    gtk_container_add(GTK_CONTAINER(button[flipped_card_nbrs[1]]),image[flipped_card_nbrs[1]]);       
                    gtk_widget_show(image[flipped_card_nbrs[0]]);      
                    gtk_widget_show(image[flipped_card_nbrs[1]]);      
                    showing_cards = 0;
    return G_SOURCE_REMOVE; // same as FALSE. otherwise this function will be called again;
}

#define P_BUTTON_CLICKED(NBR)					\
        void on_button## NBR ##_clicked(GtkWidget *btn) {		\
            if(showing_cards == 1) return;  \
            /* Have not flipped two cards yet*/ \
            if(cards_flipped == 0 || cards_flipped == 1){       \
                if(cards_flipped == 1){                         \
                    if( NBR == flipped_card_nbrs[0]){               \
                        printf("YOU CAN'T MATCH THE SAME CARD!!!\n");   \
                        return;                                         \
                    }                                                   \
                }                                                       \
                flipped_card_nbrs[cards_flipped] = NBR;\
                char filename[BUFFER_SIZE];             \
                strcpy(filename,cardsource[NBR]);         \
                gtk_container_remove( GTK_CONTAINER(button[NBR]),image[NBR]);  \
                image[NBR] = gtk_image_new_from_file(filename);                \
                gtk_container_add(GTK_CONTAINER(button[NBR]),image[NBR]);       \
                gtk_widget_show(image[NBR]);      \
                strcpy(flipped_cards[f_index],filename);     \
                f_index++;          \
            }           \
            cards_flipped++;            \
            if(cards_flipped >= 2){ /* check for match */    \
                if(strcmp(flipped_cards[0],flipped_cards[1]) == 0){        \
                    printf("GOT A MATCH!\n");       \
                } else {                                \
                    /* show card for some time then take away*/     \
                    showing_cards = 1;  \
                    printf("SORRY BUCKO!\n");           \
                    g_timeout_add (2 * G_TIME_SPAN_MILLISECOND, hide_tooltip, NULL);    \
                }                       \
                /* return;                 \ */     \
                cards_flipped = 0;          \
                f_index = 0;                \
                return;                     \
            }                           \
		}

	P_BUTTON_CLICKED(1) P_BUTTON_CLICKED(2)  P_BUTTON_CLICKED(3)  P_BUTTON_CLICKED(4)  
	P_BUTTON_CLICKED(5) P_BUTTON_CLICKED(6) P_BUTTON_CLICKED(7) P_BUTTON_CLICKED(8)
	P_BUTTON_CLICKED(9)  P_BUTTON_CLICKED(10) P_BUTTON_CLICKED(11) P_BUTTON_CLICKED(12)
    P_BUTTON_CLICKED(13)  P_BUTTON_CLICKED(14) P_BUTTON_CLICKED(15) P_BUTTON_CLICKED(16)
    P_BUTTON_CLICKED(17)  P_BUTTON_CLICKED(18) 

#define BUTTON_SETUP(NBR)					\
        void button## NBR ##_setup() {		\
            char num[10];                     \
            sprintf(num, "%d", NBR); /* store number in str  */         \
            char* button_setup =  malloc(BUFFER_SIZE*sizeof(char));                  \
            char* image_setup =  malloc(BUFFER_SIZE*sizeof(char));                  \
            strcpy(button_setup,"button"); \
            strcpy(image_setup,"image"); \
            strcat(button_setup,num);   \
            strcat(image_setup,num);    \
        	button[NBR] = GTK_WIDGET(gtk_builder_get_object(builder, button_setup)); \
            image[NBR] = GTK_WIDGET(gtk_builder_get_object(builder, image_setup)); \
            free(button_setup); \
            free(image_setup);  \
		}

        BUTTON_SETUP(1) BUTTON_SETUP(2) BUTTON_SETUP(3) BUTTON_SETUP(4) BUTTON_SETUP(5) BUTTON_SETUP(6)
        BUTTON_SETUP(7) BUTTON_SETUP(8) BUTTON_SETUP(9) BUTTON_SETUP(10) BUTTON_SETUP(11) BUTTON_SETUP(12)
        BUTTON_SETUP(13) BUTTON_SETUP(14) BUTTON_SETUP(15) BUTTON_SETUP(16) BUTTON_SETUP(17) BUTTON_SETUP(18)



void shuffle(char** array) {
  int currentIndex = 18; 
  char* temporaryValue = malloc(BUFFER_SIZE*sizeof(char));
  int randomIndex;

  // While there remain elements to shuffle...
  while (currentIndex > 1) {
    // Pick a remaining element...
    randomIndex = (rand() % currentIndex) + 1;
    currentIndex -= 1;

    // And swap it with the current element.
    strcpy(temporaryValue, array[currentIndex]);
    strcpy(array[currentIndex], array[randomIndex]);
    strcpy(array[randomIndex],temporaryValue);
  }
  free(temporaryValue);
}