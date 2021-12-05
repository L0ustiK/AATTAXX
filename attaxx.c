#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <MLV/MLV_all.h>

#define TAILLE_PLATEAU 5 // MAX 9
#define TAILLE_MAX_NOM 20

/**
 *@struct Joueur
 *@brief Structure qui gere les joueurs
 *
 */
typedef struct
{
    char nom[TAILLE_MAX_NOM]; //!<nom du joueur
    char symbol; //!<symbole pour chaque joueur
    int score;  //!<score 
} Joueur;

/**
 *@struct Plateau
 *@brief Structure qui gere le plateau 
 */
typedef struct
{
    char plateau[TAILLE_PLATEAU + 2][TAILLE_PLATEAU + 2]; // case valide de 1 à TAILLE_PLATEAU
    Joueur* joueur[2];
} Plateau;


/**
 * @brief fonction qui verifie que le joueur rentre une saisie valide
 */
void clearBuffer() // vide le buffer au cas ou l'utilisateur a saisie un nom avec espace ou des coordonne incorect
{

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/**
 * @brief fonction qui initialise le plateau
 * @param plateau
 */
void initPlateau(Plateau* plateau) // Initialise le plateau de jeu
{
    for (int i = 0; i < TAILLE_PLATEAU+2; i++)
    {
        for (int j = 0; j < TAILLE_PLATEAU+2; j++)
        {
            if (i == 1 && j == 1) {
                plateau->plateau[i][j] = 'x';
            }else if (i == TAILLE_PLATEAU && j == TAILLE_PLATEAU) {
                plateau->plateau[i][j] = 'x';
            }else if (i == 1 && j == TAILLE_PLATEAU) {
                plateau->plateau[i][j] = 'o';
            }else if (i == TAILLE_PLATEAU && j == 1) {
                plateau->plateau[i][j] = 'o';
            }else {
                plateau->plateau[i][j] = '.';
            }
        }
    }
}

/**
 * @brief fonction qui affiche le plateau
 * @param plateau
 */
void affichePlateau(Plateau plateau)
{
    for (int i = 0; i < TAILLE_PLATEAU+2; i++)
    {
        for (int j = 0; j < TAILLE_PLATEAU+2; j++)
        {
            if (i == 0 || i == TAILLE_PLATEAU+1 || j == 0 || j == TAILLE_PLATEAU+1) {
                printf("* ");
            }else {
                printf("%c ", plateau.plateau[i][j]);
            }
        }
        printf(" \n");
    }
}
/**
 * @brief fonction qui dessine le tableau et les pions des joueurs
 * @param plateau, tourDe
 */
void affichePlateauGraphique(Plateau plateau, int tourDe)
{
    MLV_clear_window(MLV_COLOR_WHITE);
    char charTourDe[40] = "Au tour de: ";
    strcat(charTourDe, plateau.joueur[tourDe]->nom);
    MLV_draw_text(20, 25, charTourDe, MLV_COLOR_BLACK); // Nom du joueur
    // DESSINE LE TABLEAU
    for (int x = 0; x < TAILLE_PLATEAU; x++)
    {
        for (int y = 0; y < TAILLE_PLATEAU; y++)
        {
            MLV_draw_rectangle((x*100)+10, (y*100)+60, 100, 100, MLV_COLOR_BLACK);
        }
    }
    // DESSINE LES PIONS
     for (int x = 0; x <= TAILLE_PLATEAU; x++)
    {
        for (int y = 0; y <= TAILLE_PLATEAU; y++)
        {
            if (plateau.plateau[x][y] == 'o')
            {
                MLV_draw_circle(((x-1)*100)+60, ((y-1)*100)+110, 36, MLV_COLOR_BLACK);
            }else if (plateau.plateau[x][y] == 'x')
            {
                MLV_draw_filled_circle(((x-1)*100)+60, ((y-1)*100)+110, 36, MLV_COLOR_BLACK);
            }
        }
    }

    MLV_actualise_window();
}
/**
 * @brief fonction qui transforme les clique d'un joueur pour qu'il corresponde à une case
 * @param *x, *y
 */
void cliqueVersCase(int* x, int* y)
{
    *x = ((*x-10)/100)+1;
    *y = ((*y-10)/100)+1;
}
/**
 * @brief fonction qui permet de compter les points
 * @param plateau
 */
void comptePoint(Plateau* plateau) // Compte les points des joueur
{
    plateau->joueur[0]->score = 0; // On s'assure qu'on compte bien a partir de 0
    plateau->joueur[1]->score = 0;
    for (int i = 0; i < TAILLE_PLATEAU+2; i++)
    {
        for (int j = 0; j < TAILLE_PLATEAU+2; j++)
        {
            switch (plateau->plateau[i][j])
            {
            case 'o':
                plateau->joueur[0]->score++;
                break;
            case 'x':
                plateau->joueur[1]->score++;
                break;
            default:
                break;
            }
        }
    }
}
/**
 * @brief fonction qui verifie si un coup est valide ou non
 * @param plateau, symbolAdverse, x, y
 * @return 0 si le coup n'est pas valide, 1 si tout est bon
 */
int verifieCoup(Plateau plateau, char symbolAdverse, int x, int y)
{
    // dans le tableau ?
    if (x < 1 || x > TAILLE_PLATEAU || y < 1 || y > TAILLE_PLATEAU) {
        return 0;
    }
    // on pose sur un truc ?
    if (plateau.plateau[x][y] != '.') {
        return 0;
    }
    // on regarde si on pose a côté du bon symbol
    for (int i = x-1; i <= x+1; i++)
    {
        for (int j = y-1; j <= y+1; j++)
        {
            if (i == x && j == y) continue;
            if (plateau.plateau[i][j] == symbolAdverse) return 1;
        }
    }
    return 0;
}
/**
 * @brief fonction qui permet de jouer un coup
 * @param *plateau, symbol, x, y
 * @return 1 si la vérification n'est pas bonne, 0 si tout vas bien
 */
int joueCoup(Plateau *plateau, char symbol, int x, int y)
{
    char symbolAdverse = symbol == 'o' ? 'x' : 'o'; // On determine le symbol adverse
    if (verifieCoup(*plateau, symbolAdverse, x, y))
    {
        plateau->plateau[x][y] = symbol; // On pose notre piece
        // On convertie les pieces adverse adjacente
        for (int i = x-1; i <= x+1; i++)
        {
            for (int j = y-1; j <= y+1; j++)
            {
                if (i == x && j == y) continue;
                if (plateau->plateau[i][j] == symbolAdverse) {
                    plateau->plateau[i][j] = symbol;
                }
            }
        }
    }else return 1; // Si la verification n'est pas bonne
    return 0;
}
/**
 * @brief fonction qui gere la victoire ou non d'un joueur
 * @param plateau
 * @return 1 si un joueur à gagner, 0 si le jeu continue
 */
int victoire(Plateau plateau)
{
    comptePoint(&plateau); // On s'assure que le points sont bien compté
    if (plateau.joueur[0]->score == 0 || plateau.joueur[1]->score == 0) {
        return 1;
    }
    for (int x = 1; x <= TAILLE_PLATEAU; x++)
    {
        for (int y = 1; y <= TAILLE_PLATEAU; y++)
        {
            if (plateau.plateau[x][y] == '.')
            {
                return 0;
            }
        }
    }
    return 1;
}
/**
 * @brief fonction qui gere l'algorythme de l'IA
 * @param plateau, x, y, bextX, bestY, bestscore
 */
void IAalgo(Plateau plateau, int* x, int* y, int bestX, int bestY, int bestScore)
// Variable dans les arguments pour potetielement la rendre reccursive et augmenter le niveaux de l'IA
{
    for (int i = 1; i <= TAILLE_PLATEAU; i++)
    {
        for (int j = 1; j <= TAILLE_PLATEAU; j++)
        {
            Plateau plateauCopie = plateau;
            if (!joueCoup(&plateauCopie, 'x', i, j))
            {
                comptePoint(&plateauCopie);
                if(plateauCopie.joueur[0]->score == 0) // Si le coup gagne on le joue
                {
                    *x = i; *y = j;
                    return;
                }
                if(plateauCopie.joueur[1]->score > bestScore) // Sinon on regarde qu'elle coup fait marquer le plus de point
                {
                    bestScore = plateauCopie.joueur[1]->score;
                    bestX = i; bestY = j;
                }
            }
        }
    }
    *x = bestX; *y = bestY;
}
/**
 * @brief fonction qui la difficulté de l'IA
 * @param difficulte, plateau
 */
void IA(int difficulte, Plateau* plateau)
{ // On remarque que le symbol de l'IA seras toujours 'x'
    int x = 0; int y = 0;
    if (difficulte == 0)
    {
        while (joueCoup(plateau, 'x', x, y))
        {
            x = (rand() + 1) % TAILLE_PLATEAU+1; // aleatoire entre 1 et TAILLE_PLATEAU
            y = (rand() + 1) % TAILLE_PLATEAU+1;
        }
    }else 
    {
        IAalgo(*plateau, &x, &y, 0, 0, -1);
        joueCoup(plateau, 'x', x, y);
    }
}

/**
 *@brief Le programme principale
 *@param argc, argv[]
*/
int main(int argc, char* argv[])
{
    srand(time(NULL)); // init de l'aleatoire
    // Variable d'option de jeu
    int modeGraphique = 0; // 0 -> ASCII | 1 -> graphique
    int modeDeuxJoueur = 0; // 0 -> IA | 1 -> deux joueurs

    // Lecture des arguments (le dernier l'emporte)
    for (int i = 1; i < argc; i++)
    {
        switch (argv[i][1])
        {
        case 'a':
            modeGraphique = 0; 
            break;
        case 'g':
            modeGraphique = 1;
            break;
        case 'h':
            modeDeuxJoueur = 1;
            break;
        case 'o':
            modeDeuxJoueur = 0;
            break;
        default:
            printf("%c : commande inconnue\n", argv[i][1]);
            break;
        }
    }

    // Initialisation de la partie
    Joueur j1, j2;
    printf("Quel est le nom du premier joueur (symbol o) : ");
    scanf(" %s", j1.nom);
    clearBuffer();
    j1.symbol = 'o';
    if (modeDeuxJoueur)
    {
        printf("Quel est le nom du second joueur (symbol x) : ");
        scanf(" %s", j2.nom);
        clearBuffer();
    }else {
        strcpy(j2.nom, "IA"); // strcpy -> assigner une valeur au tableau de char
    }
    j2.symbol = 'x';
    printf("\n");

    Plateau plateau;
    plateau.joueur[0] = &j1;
    plateau.joueur[1] = &j2;
    initPlateau(&plateau);
    comptePoint(&plateau);

    // Boucle d'option (pour plus tard)
    // Boucle de jeu
    int x = 0; // variable pour la saisie des points
    int y = 0;
    char xC, yC; // variable pour le controle de saisie des points
    int tourDe = 0; // O -> j1, 1 -> j2

    if (modeGraphique)
    {
        MLV_create_window("ATTAXX by seb et leo", "", (100 * TAILLE_PLATEAU)+20, (100 * TAILLE_PLATEAU)+70);
        MLV_wait_mouse_or_seconds(NULL, NULL, 5);
        while (1)
        {
            affichePlateauGraphique(plateau, tourDe);
            if (tourDe == 1 && !modeDeuxJoueur) { // si c'est le tour de l'IA
                MLV_wait_seconds(1); // Simule une reflexion de l'IA
                IA(1, &plateau);
            }else { // Si c'est pas a l'IA
                while (joueCoup(&plateau, plateau.joueur[tourDe]->symbol, x, y)) // Tant que le point poser n'est pas valide
                {
                    MLV_wait_mouse(&x, &y);
                    cliqueVersCase(&x, &y);
                    printf("x: %d, y: %d", x, y);
                }
            }
            comptePoint(&plateau);
            if (victoire(plateau)) 
            {
                MLV_wait_seconds(2);
                break;
            }
            tourDe = tourDe ? 0 : 1; // On change le tour du joueur
        }
    }
    else // ASCII mode
    {
        affichePlateau(plateau);
        printf("\n");
        while (1)
        {
            if (tourDe == 1 && !modeDeuxJoueur) { // si c'est le tour de l'IA
                IA(1, &plateau);
            }else { // Si c'est pas a l'IA
                while (joueCoup(&plateau, plateau.joueur[tourDe]->symbol, x, y)) // Tant que le point poser n'est pas valide
                {
                    printf("%s (%c): veuillez saisir les coordonnées où jouer (entre 1 et %d) : ",
                        plateau.joueur[tourDe]->nom, plateau.joueur[tourDe]->symbol, TAILLE_PLATEAU);
                    scanf(" %c %c", &xC, &yC);
                    clearBuffer();
                    if (isdigit(xC) && isdigit(yC)) { // On securise la saisie
                        x = ((int)xC) - 48; // le code ASCII des chiffre commence a 48
                        y = ((int)yC) - 48;
                    }
                }
            }
            affichePlateau(plateau);
            comptePoint(&plateau);
            printf("Score actuel : %s(%c) %d - %s(%c) %d\n\n",
                   plateau.joueur[0]->nom, plateau.joueur[0]->symbol, plateau.joueur[0]->score,
                   plateau.joueur[1]->nom, plateau.joueur[1]->symbol, plateau.joueur[1]->score);
            if (victoire(plateau)) break;

            tourDe = tourDe ? 0 : 1; // On change le tour du joueur
        }
        // Affiche le vainceur
        if (plateau.joueur[0]->score == plateau.joueur[1]->score) {
            printf("Egalité");
        }else if (plateau.joueur[0]->score > plateau.joueur[1]->score) {
            printf("Bravo %s, vous avez gagné",plateau.joueur[0]->nom);
        }else {
            printf("Bravo %s, vous avez gagné",plateau.joueur[1]->nom);
        }
        printf(" %d a %d.\n", plateau.joueur[0]->score, plateau.joueur[1]->score);
    }
    
    return 0;
}



/*! \mainpage Page principale du DM ATTAXX
 *
 * \tableofcontents
 * 
 * \section section_intro Introduction
 *
 * Bienvenue sur la doc de notre jeu ATTAXX, vous trouverez tout ce qu'il y a à savoir sur ce jeu.
 *
 *
 * \section section_tdm Table des matières
 * - \ref page_README
 * - \ref page_jouer
 *
 */

/*! 
 *
 * \page page_README 
 * @author Sébastien SAUTIER et Léo ATTIG
 * @note Voici notre version du jeu vidéos ATTAXX qui contient une version graphique et une version ASCII
 * @version 1.0
 * @date 05/12/2021
 * \page page_jouer
 * Pour jouer à ce jeu, les règles sont simples : \n
 * Un coup consiste à placer un pion de sa couleur sur une des huit cases voisines d’un pion adverse
 * déjà posé. La case doit être vide pour que l’on puisse poser un pion dessus. De plus, après avoir posé
 * un pion, tous les pions adverses sur les cases voisines de la case du pion posé changent de couleur en devenant de la couleur du pion posé. \n
 * Le jeu se termine lorsque le plateau de jeu est rempli ou lorsqu’un joueur ne possède plus de
 * pions. Le gagnant est alors celui qui a le plus de pions sur le plateau. \n
 * -a pour indiquer que l’affichage sera en ASCII (option excluant l’option suivante) \n
 * -g pour indiquer que l’affichage sera graphique (option exclu l’option précédente) \n
 * -h pour indiquer que deux joueurs humains joueront (option excluant l’option suivante) \n
 * -o pour indiquer qu’un joueur humain jouera contre l’ordinateur (option exclu l’option précédente) \n
 * 
 */