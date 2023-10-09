/**************************************************************************/
/*                            OLEKSY_PETIT                                */
/*                              02/06/23                                  */
/*                Compilation: "make" et ./main_interface                 */
/**************************************************************************/

#include "../headers/interface.h"
#include "../headers/fonction.h"
#include <time.h>

typedef struct myImage
{
    unsigned height;
    unsigned width;
    unsigned char *red;
    unsigned char *green;
    unsigned char *blue;
    unsigned char *alpha;
} myImage;

typedef struct result
{
	double result[10];
	
}result;

void new_network(NETWORK *);                                                   /* Initialisation du réseau */
void decoder_image(unsigned char *, char *, double *, unsigned *, unsigned *); /* Fonction permettant de décoder une image (utilisé dans test_image) */

result test_image(NETWORK *, char *); /* Fonction utilisé pour tester une image au format PNG */

myImage LireImage(char *);
void EcrireImage(myImage, char *);
myImage reduce_image(myImage);

/*Fonction qui gère l'interface graphique */
void SDL_interface(SDL_bool , SDL_bool , SDL_Window* , SDL_Renderer* , SDL_Window* , SDL_Renderer* , SDL_Surface** , struct Active_coordinates);

//void generate_images();
//void encoder_image(double *, int , int , int );								/* Fonction pour transformer un tableau d'une image en une image en format PNG */

int calculIA ();

void simpl_digit_module();
void interface_module();
void training_module();


int main (int argc, char **argv){
	

    printf("\nBienvenue dans ce programme capable de reconnaitre un chiffre dans une image de 28 par 28 pixels :\n");
    printf("\nVeuillez choisir entre les options suivantes :\n");
    printf("\n1)Tester une image que vous avez deposee préalablement dans le dossier 'images/imagesTest' \n");
    printf("\n2)Lancer l'interface graphique pour dessiner une image et la tester\n");
    printf("\n3)Entrainer le réseaux de neurones (attention cette operation prend plusieurs minutes) \n");

    int option=0;
    printf("\nVeuillez entrer l'option choisit :\n");
    scanf("%d", &option);
    


    while(option >3 || option <1){
        printf("\nVeuillez entrer une option entre 1 et 3 !\n");
        scanf("%d", &option);
    }

    printf("\n --------------------------------------------\n \n-------Vous avez choisis l'option : %d ------- \n --------------------------------------------\n  ", option);

    if (option ==1){
        simpl_digit_module();
    }

    if (option ==2){
        interface_module();
    }


    if (option ==3){
        training_module();
    }

    

    return EXIT_SUCCESS;
}

void simpl_digit_module(){
    // Déclaration de la structure NETWORK
    NETWORK network;
    // Initialisation de la structure NETWORK avec la fonction new_network()
    new_network(&network);

    // Déclaration des variables pour le chemin de l'image et le chemin complet du fichier
    char imagePath[100];
    char filePath[200];

    // Copie du préfixe du chemin du dossier des images dans le tableau filePath
    strcpy(filePath, "images/imagesTest/");

    // Demande à l'utilisateur d'entrer le nom de l'image
    printf("\nVeuillez entrer le nom de l'image\n");
    scanf("%s", imagePath);

    // Concaténation du nom de l'image avec le préfixe du chemin pour obtenir le chemin complet
    strcat(filePath, imagePath);

    // Affichage du chemin complet de l'image
    printf("%s \n", filePath);

    // Appel de la fonction test_image() pour obtenir le résultat de l'image
    result image_result = test_image(&network, filePath);

    // Initialisation de la variable max avec la première valeur du tableau result
    double max = image_result.result[0];

    // Déclaration de la variable index pour stocker l'indice correspondant au maximum
    int index;

    // Parcours du tableau result pour trouver le maximum et son indice
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        if (max < image_result.result[i]) {
            max = image_result.result[i];
            index = i;
        }
    }

    // Vérification si le maximum est inférieur à 25
    if (max < 25) {
        max = 999;
    }

    // Vérification si le maximum est inférieur à 999
    if (max < 999) {
        // Affichage du résultat : le chiffre reconnu
        printf("\n \n ----> Le chiffre est : %d \n", index);
    } else {
        // Affichage du message d'erreur : le chiffre n'est pas reconnu
        printf("Le chiffre n'est pas reconnu. Assurez-vous d'avoir bien placé l'image dans le dossier 'images/imageMenu' et que cette image représente clairement un chiffre.\n");
    }
}


void interface_module(){
    /* INITIALISATION DES FENETRES ET RENDUS */
    init_video();
    SDL_Window *home_window = NULL;
    SDL_Renderer *home_render = NULL;
    create_Window_and_Render(&home_window, HOME_WINDOW_WIDTH, HOME_WINDOW_HEIGHT, &home_render, 0 , 0);
    
    SDL_Window *paint_window = NULL;
    SDL_Renderer *paint_render = NULL;
    SDL_Surface* BackEndSurfaces[17];
    //init_BackEndSurfaces( BackEndSurfaces);
    
    /*BOOLEN VRAI/FAUX = MARCHE/ARRET*/
    SDL_bool program_launched = SDL_TRUE;
    
    /*BOOLEN VRAI/FAUX = MODE DESSIN OUI/NON*/
    SDL_bool active_paint = SDL_FALSE;
    
    /*INITIALISATION COORDONNEES*/
    struct Active_coordinates coords;
    init_Coordinates(&coords);
    
    /*LANCEMENT DE L'INTERFACE*/
    SDL_interface(program_launched, active_paint, home_window, home_render, paint_window, paint_render, BackEndSurfaces, coords);
    
    /*LIBERATION DES FENETRES ET RENDUS*/
    Free_All(paint_render, paint_window, home_render, home_window);
    SDL_Quit();
}


void training_module(){
    printf("Vous etes dans le module d'entrainement \n");
    printf("Veuillez entrer le nombre d'entrainement pour le réseaux\n");
    int NUMBER_TRAINING;
    scanf("%d", &NUMBER_TRAINING);

    printf("Le programme prend environ une dizaine de minutes pour s'auto-entraîner sur la base de donnée, certains modèles sont déjà entraînés dans le dossier \"database\" !\n");
    /* Ouverture du fichier de sauvegarde du réseau et de la base de donnée */
    FILE *file = fopen("database/network_300_10.csv", "w");
    FILE *file_img = fopen("database/images_data.csv", "rb");

    /* Allouer des tableaux pour stocker les images et les étiquettes */
    mnist_image_t train_images[PAQUET_IMG];
    NETWORK network;

    /* S'assurer que les fichiers sont ouvert correctement */
    if (file == NULL || file_img == NULL)
    {
        printf("Erreur d'ouverture des fichiers\n");
        exit(1);
    }

    /* Initialisation des poids du réseau de façon aléatoire */
    init_network(&network, &train_images[0]);

    /* Entrainer le réseau en travaillant NUMBER_TRAINING fois sur la base de donnée */
    int j = 0;
    while (j < NUMBER_TRAINING)
    {
        rewind(file_img);
        training(file_img, &network, train_images, j, NUMBER_TRAINING);
        j++;
    }

    save_network(file, network);
    printf("\nEntraînement du réseau terminé !\n");
    /* FIN DE L'ENTRAINEMENT */

    /* Test du l'IA avec un nombre du fichier d'entraînement */
    printf("Petit test sur 2 images sur lesquels on s'est entraîné: \n");

    network.layer[0].input_buffer = train_images[0].data;
    output_calcul(&network);

    printf("Numéro de l'image: %d\n", train_images[0].label);

    for (int j = 0; j < OUTPUT_SIZE; j++)
    {
        printf("%d: %lf ", j, network.layer[1].output_buffer[j] * 100);
    }
    printf("\n");

    /* Test du l'IA avec un nombre du fichier d'entraînement */
    network.layer[0].input_buffer = train_images[11].data;
    output_calcul(&network);

    printf("Numéro de l'image: %d\n", train_images[11].label);

    for (int j = 0; j < OUTPUT_SIZE; j++)
    {
        printf("%d: %lf ", j, network.layer[1].output_buffer[j] * 100);
    }
    printf("\n");

    /* Fermeture des fichiers ouverts et libération de la mémoire */
    fclose(file);
    fclose(file_img);

    free_network(&network);
    free(network.layer[0].output_buffer);
    free(network.layer[1].output_buffer);
}







/*Fonction qui gère l'interface graphique */
void SDL_interface(SDL_bool program_launched, SDL_bool active_paint, SDL_Window* home_window, SDL_Renderer* home_render, SDL_Window* paint_window, SDL_Renderer* paint_render, SDL_Surface** BackEndSurfaces, struct Active_coordinates coords){
	
	 int digit_recognised;
    struct ActiveRectImages_ ActiveRectImages;
    double programSection = 0;
    int* frm = malloc(sizeof(int));
    *frm = 1;
    int* time = malloc(sizeof(int));
    *time = 0;
    int* pastTime = malloc(sizeof(int));
    *pastTime = 0;

    while (program_launched) {
        *time = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            SDL_GetMouseState(&(coords.x), &(coords.y));
            handle_event(coords, ActiveRectImages, &event, &active_paint, &program_launched, &programSection, &home_window, &home_render, &paint_window, &paint_render, BackEndSurfaces);
        }
        SDL_RenderClear(home_render);
        display_gif_background(time, pastTime, frm, &home_render);

        // Partie 1: Section 0
        if (programSection == 0) {
            // Affichage de l'image "intelligence_artificielle.png"
            char* imagePath = "images/imageMenu/intelligence_artificielle.png";
            int x = HOME_WINDOW_WIDTH / 2 - getImageWidth(imagePath) / 2;
            int y = HOME_WINDOW_HEIGHT * 0.25 - getImageHeight(imagePath) / 2;
            display_image(imagePath, home_render, x, y);

            // Affichage de l'image "entree.png" avec un effet de clignotement
            if ((*time) % 1000 < 750) {
                imagePath = "images/imageMenu/entree.png";
                x = HOME_WINDOW_WIDTH / 2 - getImageWidth(imagePath) / 2;
                y = HOME_WINDOW_HEIGHT * 0.90 - getImageHeight(imagePath) / 2;
                display_image(imagePath, home_render, x, y);
            }

            // Affichage de l'image "credits.png" avec interaction lorsque survolée
            imagePath = "images/imageMenu/credits.png";
            char* imagePathHoover = "images/imageMenu/credits_survol.png";
            x = 0;
            y = 0;
            ActiveRectImages.credit_image.x = x;
            ActiveRectImages.credit_image.y = y;
            ActiveRectImages.credit_image.w = getImageWidth(imagePath);
            ActiveRectImages.credit_image.h = getImageHeight(imagePath);
            full_display_image(imagePath, imagePathHoover, home_render, x, y, coords);
        }
		if (programSection==0.25){
			programSection = 0.5;
		}
		// Partie 2: Section 0.25
	if (programSection == 0.25) {
		programSection = 0.5;
	}

	// Partie 3: Section 0.5
	if (programSection == 0.5) {
		// Affichage de l'image "credits_details.png"
		char* imagePath = "images/imageMenu/credits_details.png";
		int x = HOME_WINDOW_WIDTH / 2 - getImageWidth(imagePath) / 2;
		int y = HOME_WINDOW_HEIGHT * 0.25 - getImageHeight(imagePath) / 2;
		display_image(imagePath, home_render, x, y);

		// Affichage de l'image "retour.png" avec interaction lorsque survolée
		imagePath = "images/imageMenu/retour.png";
		char* imagePathHoover = "images/imageMenu/retour_survol.png";
		x = 0;
		y = 0;
		ActiveRectImages.retour_image.x = x;
		ActiveRectImages.retour_image.y = y;
		ActiveRectImages.retour_image.w = getImageWidth(imagePath);
		ActiveRectImages.retour_image.h = getImageHeight(imagePath);
		full_display_image(imagePath, imagePathHoover, home_render, x, y, coords);
	}

		// Partie 4: Section 1
		if (programSection == 1) {
			// Affichage de l'image "dessiner_image.png" avec interaction lorsque survolée
			char* imagePath = "images/imageMenu/dessiner_image.png";
			char* imagePathHoover = "images/imageMenu/dessiner_image_survol.png";
			int x = HOME_WINDOW_WIDTH / 2 - getImageWidth(imagePath) / 2;
			int y = HOME_WINDOW_HEIGHT * 0.5 - getImageHeight(imagePath) / 2;
			ActiveRectImages.paint_image.x = x;
			ActiveRectImages.paint_image.y = y;
			ActiveRectImages.paint_image.w = getImageWidth(imagePath);
			ActiveRectImages.paint_image.h = getImageHeight(imagePath);
			full_display_image(imagePath, imagePathHoover, home_render, x, y, coords);

			// Affichage de l'image "retour.png" avec interaction lorsque survolée
			imagePath = "images/imageMenu/retour.png";
			imagePathHoover = "images/imageMenu/retour_survol.png";
			x = 0;
			y = 0;
			ActiveRectImages.retour_image.x = x;
			ActiveRectImages.retour_image.y = y;
			ActiveRectImages.retour_image.w = getImageWidth(imagePath);
			ActiveRectImages.retour_image.h = getImageHeight(imagePath);
			full_display_image(imagePath, imagePathHoover, home_render, x, y, coords);
		}

		if ((programSection>=1.2) && (programSection<20) ){
			char *imagePath = "images/imageMenu/dessiner_image.png";
			int x = HOME_WINDOW_WIDTH/2 - getImageWidth(imagePath)/2;
			int y = HOME_WINDOW_HEIGHT*0.10 - getImageHeight(imagePath)/2;
			display_image(imagePath, home_render, x , y );
			
			imagePath = "images/imageMenu/dessiner_image_details.png";
			x = HOME_WINDOW_WIDTH/2 - getImageWidth(imagePath)/2;
			y = HOME_WINDOW_HEIGHT*0.30 - getImageHeight(imagePath)/2;
			display_image(imagePath, home_render, x , y );
			
			imagePath = "images/imageMenu/dessiner_image_details2.png";
			x = HOME_WINDOW_WIDTH/2 - getImageWidth(imagePath)/2;
			y = HOME_WINDOW_HEIGHT*0.40 - getImageHeight(imagePath)/2;
			display_image(imagePath, home_render, x , y );
			
			imagePath = "images/imageMenu/retour.png";
			char *imagePathHoover = "images/imageMenu/retour_survol.png";
			x = 0 ;
			y = 0 ;
			ActiveRectImages.retour_image.x = x ; 
			ActiveRectImages.retour_image.y = y ;
			ActiveRectImages.retour_image.w =getImageWidth(imagePath); 
			ActiveRectImages.retour_image.h = getImageHeight(imagePath);
			full_display_image(imagePath, imagePathHoover, home_render, x , y , coords);
			
			if (programSection == 2.2){
				digit_recognised = calculIA();
				programSection=2.3;
			}
			if (programSection == 2.3){
				
				imagePath = "images/imageMenu/resultat.png";
				x = HOME_WINDOW_WIDTH/3 - getImageWidth(imagePath)/2;
				y = HOME_WINDOW_HEIGHT*0.90 - getImageHeight(imagePath)/2;
				display_image(imagePath, home_render, x , y );
				
				int xrel = getImageWidth(imagePath)/2 ;
				
				if (digit_recognised<10){
					char path[40];
					sprintf(path, "images/imageMenu/%d.png", digit_recognised);
					x =HOME_WINDOW_WIDTH/3 + xrel + getImageWidth(path)/2 ;
					y = HOME_WINDOW_HEIGHT*0.90 - getImageHeight(imagePath)/2;
					display_image(path, home_render, x , y );
				}
				else {
					imagePath = "images/imageMenu/nan.png";
					x =HOME_WINDOW_WIDTH/3 + xrel ;
					y = HOME_WINDOW_HEIGHT*0.90 - getImageHeight(imagePath)/2;
					display_image(imagePath, home_render, x , y );
				}
			}
		}
		SDL_RenderPresent(home_render);
		paint_mode ( &programSection ,&paint_window, &paint_render);	
		SDL_RenderPresent(paint_render);
		handle_active_paint(active_paint, 8, paint_render, &coords, BackEndSurfaces);
	}
	free(frm);
	free(time);
	free(pastTime);	
}


void new_network(NETWORK *network)
{
    FILE *file = fopen("database/network_300_10_15.csv", "r");
    if (file == NULL)
    {
        printf("Erreur d'ouverture de network_300_10_15.csv\n");
        exit(1);
    }
    network->layer[0].weight = (double *)malloc(sizeof(double) * INPUT_SIZE * HIDDEN_SIZE);
    network->layer[0].bias = (double *)malloc(sizeof(double) * HIDDEN_SIZE);

    network->layer[1].weight = (double *)malloc(sizeof(double) * HIDDEN_SIZE * OUTPUT_SIZE);
    network->layer[1].bias = (double *)malloc(sizeof(double) * OUTPUT_SIZE);

    network->layer[0].output_buffer = (double *)malloc(sizeof(double) * HIDDEN_SIZE);
    network->layer[1].input_buffer = network->layer[0].output_buffer;
    network->layer[1].output_buffer = (double *)malloc(sizeof(double) * OUTPUT_SIZE);
    load_network(file, network);
    fclose(file);
}

void decoder_image(unsigned char *image, char *nom_fichier, double *data, unsigned *width, unsigned *height)
{
    unsigned error;
    /* Décoder l'image */
    error = lodepng_decode32_file(&image, width, height, nom_fichier);
    /* s'il y a une erreur, l'afficher */
    if (error)
    {
        //printf("error%u: %s\n", error, lodepng_error_text(error));
        Error("L'image spécifiee n'est pas reconnu, assurez vous d'avoir bien placé l'image dans 'images/imagesTest . Vous devez aussi ajouter '.png' lors de l'entree du nom de l'image. \n");
    }

    /* Convert RGBA in Black & White  */
    for (int i = 0; i < MNIST_IMAGE_SIZE; i++)
    {
        data[i] = (double)image[i * 4] / 255;
    }
}


result test_image(NETWORK *network, char *image_name)
{
	result result_o;
    unsigned char *image = NULL;
    double *data = (double *)malloc(sizeof(double) * MNIST_IMAGE_SIZE);
    unsigned width = 28, height = 28;


    /* Décoder l'image passé en argument */
    decoder_image(image, image_name, data, &width, &height);

    /* Calcul de la sortie, puis affichage des résultats */
    network->layer[0].input_buffer = data;
    output_calcul(network);

	
	//printf("Résultat de l'image testée:\n");
    for (int j = 0; j < OUTPUT_SIZE; j++)
    {
		result_o.result[j] = network->layer[1].output_buffer[j] * 100 ;
        //printf("%d: %lf ", j, network->layer[1].output_buffer[j] * 100);
    }
    //printf("\n");

    free(image);
    free(data);
	return result_o;
}

myImage LireImage(char *nom_fichier)
{

    myImage im;
    unsigned error;
    unsigned char *image;

    /*Decode the image*/
    error = lodepng_decode32_file(&image, &im.width, &im.height, nom_fichier);
    /*if there's an error, display it*/
    if (error)
    {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }

    im.red = malloc(sizeof(unsigned char) * im.height * im.width);
    im.green = malloc(sizeof(unsigned char) * im.height * im.width);
    im.blue = malloc(sizeof(unsigned char) * im.height * im.width);
    im.alpha = malloc(sizeof(unsigned char) * im.height * im.width);

    for (int i = 0; i < im.height * im.width; i++)
    {
        im.red[i] = image[i * 4 + 0];
        im.green[i] = image[i * 4 + 1];
        im.blue[i] = image[i * 4 + 2];
        im.alpha[i] = image[i * 4 + 3];
    }

    free(image);
    return im;
}

void EcrireImage(myImage im, char *nom_fichier)
{

    unsigned error;
    unsigned char *image = malloc(sizeof(unsigned char) * im.width * im.height * 4);
    int i = 0;
    int x, y;
    i = 0;
    for (y = 0; y < im.height; y++)
        for (x = 0; x < im.width; x++)
        {
            if (im.red[i] < 50)
            {
                image[4 * im.width * y + 4 * x + 0] = 0;
                image[4 * im.width * y + 4 * x + 1] = 0;
                image[4 * im.width * y + 4 * x + 2] = 0;
            }
            else if (im.red[i] < 200)
            {
                image[4 * im.width * y + 4 * x + 0] = im.red[i];
                image[4 * im.width * y + 4 * x + 1] = im.green[i];
                image[4 * im.width * y + 4 * x + 2] = im.blue[i];
            }
            else
            {
                image[4 * im.width * y + 4 * x + 0] = 255;
                image[4 * im.width * y + 4 * x + 1] = 255;
                image[4 * im.width * y + 4 * x + 2] = 255;
            }
            image[4 * im.width * y + 4 * x + 3] = im.alpha[i];
            i++;
        }

    /*Encode the image*/
    error = lodepng_encode32_file(nom_fichier, image, im.width, im.height);

    /*if there's an error, display it*/
    if (error)
    {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }

    free(image);
    free(im.red);
    free(im.green);
    free(im.blue);
    free(im.alpha);
}

myImage reduce_image(myImage im1)
{

    int sum = 0;
    int i, j;

    myImage im2;

    im2.red = malloc(sizeof(unsigned char) * im1.height * im1.width);
    im2.green = malloc(sizeof(unsigned char) * im1.height * im1.width);
    im2.blue = malloc(sizeof(unsigned char) * im1.height * im1.width);
    im2.alpha = malloc(sizeof(unsigned char) * im1.height * im1.width);

    for (i = 0; i < (im1.width) * (im1.height); i++)
    {
        if (i >= 0 && i <= (im1.width - 1)) // Ligne du haut
        {
            im2.red[i] = im1.red[i];
            im2.green[i] = im1.green[i];
            im2.blue[i] = im1.blue[i];
        }
        else if (i >= im1.width * (im1.height - 1)) // Ligne du bas
        {
            im2.red[i] = im1.red[i];
            im2.green[i] = im1.green[i];
            im2.blue[i] = im1.blue[i];
        }
        else if (i % im1.width == 0 || i % (im1.width) == (im1.width - 1)) // Ligne gauche et droite
        {
            im2.red[i] = im1.red[i];
            im2.green[i] = im1.green[i];
            im2.blue[i] = im1.blue[i];
        }
        else /* Dans ce cas, on fait une moyenne sur les 8 pixels autour du pixel */
        {
            for (j = -1; j < 2; j++) /* Somme sur les 3 pixels au dessus */
            {
                sum += (int)im1.red[i + j];
            }

            for (j = -1; j < 2; j++) /* Somme sur les 3 pixels au milieu (dont le pixel lui même) */
            {
                sum += (int)im1.red[i - (im1.width) + j];
            }

            for (j = -1; j < 2; j++) /* Somme sur les 3 pixels en dessous */
            {
                sum += (int)im1.red[i + (im1.width) + j];
            }

            sum = sum / 9;

            im2.red[i] = sum;
            im2.green[i] = sum;
            im2.blue[i] = sum;
            sum = 0;
        }

        im2.alpha[i] = im1.alpha[i];
    }

    myImage im3;

    im3.red = malloc(sizeof(unsigned char) * im1.height / 2 * im1.width / 2);
    im3.green = malloc(sizeof(unsigned char) * im1.height / 2 * im1.width / 2);
    im3.blue = malloc(sizeof(unsigned char) * im1.height / 2 * im1.width / 2);
    im3.alpha = malloc(sizeof(unsigned char) * im1.height / 2 * im1.width / 2);

    for (i = 0; i < (im1.width); i = i + 2)
    {
        for (j = 0; j < (im1.height); j = j + 2)
        {
            im3.red[i / 2 * (im1.height / 2) + j / 2] = im2.red[i * im1.height + j];
            im3.green[i / 2 * (im1.height / 2) + j / 2] = im2.green[i * im1.height + j];
            im3.blue[i / 2 * (im1.height / 2) + j / 2] = im2.blue[i * im1.height + j];
            im3.alpha[i / 2 * (im1.height / 2) + j / 2] = im2.alpha[i * im1.height + j];
        }
    }
    im3.height = im1.height / 2;
    im3.width = im1.width / 2;

    free(im2.red);
    free(im2.green);
    free(im2.blue);
    free(im2.alpha);

    return im3;
}


int calculIA() {
    // Déclaration et initialisation des variables
    result result_reco[11]; // Tableau de résultats de reconnaissance
    result moyenne;
    for (int i = 0; i < 10; i++) {
        moyenne.result[i] = 0;
    }
    NETWORK network;
    new_network(&network); // Initialisation du réseau

    char imagePath[60];
    // Boucle pour traiter les images de test
    for (int i = 0; i < 11; i++) {
        // Construction du chemin de l'image de test
        sprintf(imagePath, "images/number_paint_size%d.png", i + 5);
        myImage aux1, aux2;
        aux1 = LireImage(imagePath); // Lecture de l'image
        aux2 = LireImage(imagePath);
        while ((aux2.width != 28)) {
            aux2 = reduce_image(aux1); // Réduction de l'image
            aux1 = aux2;
        }

        // Construction du chemin de l'image de sortie
        sprintf(imagePath, "images/out%d.png", i + 5);
        EcrireImage(aux2, imagePath); // Écriture de l'image réduite
        result_reco[i] = test_image(&network, imagePath); // Reconnaissance de l'image

        // Boucle pour afficher les résultats
        for (int j = 0; j < 10; j++) {
            //printf("%f \n", result_reco[i].result[j]);
        }
    }

    // Calcul de la moyenne des résultats
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            moyenne.result[j] += result_reco[i].result[j];
        }
    }

    int max = 0;
    int index = 999;
    // Recherche du maximum dans la moyenne des résultats
    for (int i = 0; i < 10; i++) {
        if (moyenne.result[i] > max) {
            max = moyenne.result[i];
            index = i;
        }
    }

    // Vérification si le maximum divisé par 10 est inférieur à 25
    if ((max / 10) < 25) {
        index = 999;
    }

    return index; // Retourne l'indice du chiffre reconnu
}
