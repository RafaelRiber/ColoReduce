/* ICC Pratique - CS-119
 * Mini-Projet  - ColoReduce
 *
 * Auteur: Rafael Riber
 * Numéro SCIPER: 296142
 * Date:  Décembre 2018
 *
 * Ce programme prend en entrée un fichier contenant des instructions de
 * normalisation (nombre et valeurs RGB de couleurs réduites, seuils de
 * normalisation) et d'un nombre F de filtrages à effectuer, ainsi qu'une image au
 * format PPM, et renvoie cette image, réduite aux couleurs données en entrée, puis
 * filtrée F fois.
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
using namespace std;

const int maxVal(255);             // Intensité maximale d'une composante de couleur
const int minNbR(2);               // Nombre minimum de couleurs réduites
const int maxNbR(255);             // Nombre maximum de couleurs réduites

const int filterColor(0);          // Indice de couleur utilisé dans le filtrage
                                   // dans le cas ou on ne trouverait pas 6 voisins
                                   // de la même valeur

const int maxSameVals(6);          // Condition d'arrêt pour le filtrage
const double epsilon(0.001);       // Utile dans la vérification des seuils
const double firstThreshold(0.0);  // Premier seuil implicite
const double lastThreshold(1.0);   // Dernier seuil implicite

struct Color;

typedef vector<vector<int>>   NormImg;
typedef vector<vector<Color>> RGBImg;
typedef vector<Color>         ReducedColors;
typedef vector<double>        ThresholdList;
typedef vector<int>           NeighborCounter;

struct Color{
    int r, g, b;
};

//Décomposition du fichier d'entrée
struct InputImg{
    int nbR;                  // Nombre de couleurs réduites
    int nbF;                  // Nombre de filtrages
    unsigned int nbC;         // Nombre de pixels horizontaux
    unsigned int nbL;         // Nombre de pixels verticaux
    ReducedColors rColors;    // Liste des couleurs réduites
    ThresholdList thresholds; // Liste des seuils
    RGBImg inputImg;          // Pixels de l'image d'entrée
};

// Première couleur réduite
const Color black = {0,0,0};  // Première couleur réduite constante

// Fonctions d'erreur
void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);


// Fonctions de lecture
InputImg fileRead();
Color colorRead();
void inputReduced(InputImg& input);
void inputThresholds(InputImg& input);
void inputFilters(InputImg& input);
void inputDimensions(InputImg& input);
void inputPixels(InputImg& input);

// Fonctions de transformation
NormImg normalize(InputImg rgb);
void filter(NormImg& source, int nbL, int nbC, int nbF, int nbR);
int getPixelValue(int x, int y, int nbR, NormImg& source);
void blackEdge(NormImg& norm, int nbL, int nbC, int nbF);

// Fonction de rendu
RGBImg render(NormImg filtered, int nbL, int nbC, ReducedColors rColors);
void   printRGB(RGBImg rgb, int nbL, int nbC);

int main()
{
    // On lit le fichier d'entrée, et on le stocke dans la structure "image"
    InputImg image = fileRead();

    // On seuille l'image d'entrée, et on stocke le résultat dans un tableau "norm"
    NormImg norm = normalize(image);

    // On filtre l'image normalisée
    filter(norm, image.nbL, image.nbC, image.nbF, image.nbR);

    // On crée une image RGB résultat à partir des couleurs réduites et de l'image
    // filtrée
    RGBImg rendered = render(norm, image.nbL, image.nbC, image.rColors);

    // On imprime l'image RGB résultat correctement formatée au format PPM
    printRGB(rendered, image.nbL, image.nbC);

    return 0;
}

void error_nbR(int nbR)
{
    cout << "Invalid number of colors: " << nbR << endl;
}

void error_color(int id)
{
    cout << "Invalid color value " << id << endl;
}

void error_threshold(double invalid_val)
{
    cout << "Invalid threshold value: " << invalid_val << endl;
}

void error_nb_filter(int nb_filter)
{
    cout << "Invalid number of filter: " << nb_filter << endl;
}

InputImg fileRead(){
    InputImg input;

    // Entrée des couleurs réduites
    inputReduced(input);

    // Entrée des seuils
    inputThresholds(input);

    // Entrée des filtres
    inputFilters(input);

    // L'en-tête ne nous intéresse pas
    string header;
    cin >> header;

    // Entrée des dimensions
    inputDimensions(input);

    // La valeur maximale ne nous intéresse pas
    int m;
    cin >> m;

    // Pixels de l'image d'entrée
    inputPixels(input);

    return input;
}

void inputReduced(InputImg& input){
    int n(0);

    cin >> n;
    if(n < minNbR or n > maxNbR){
        error_nbR(n);
        exit(0);
    }
    input.nbR = n;

    // La première couleur est toujours le noir
    input.rColors.push_back(black);

    for (int i(1); i < n+1; i++){
        Color p(colorRead());

        if(p.r < 0 or p.r > maxVal){
            error_color(i);
            exit(0);
        }
        if(p.g < 0 or p.g > maxVal){
            error_color(i);
            exit(0);
        }
        if(p.b < 0 or p.b > maxVal){
            error_color(i);
            exit(0);
        }

        input.rColors.push_back(p);
    }
}

void inputThresholds(InputImg& input){

    input.thresholds.push_back(firstThreshold);  // Le premier seuil est fixe

    for (int i(1); i < input.nbR; i++){
        double t(0);
        cin >> t;

        // Vérification d'écart entre le seuil actuel et le précédent
        double deltaThresholds(abs(t - input.thresholds[i-1]));

        if(deltaThresholds < epsilon){
            error_threshold(t);
            exit(0);
        }
        if(t < input.thresholds[i-1]){
            error_threshold(t);
            exit(0);
        }

        input.thresholds.push_back(t);
    }
    input.thresholds.push_back(lastThreshold);   // Le dernier seuil est fixe
}

void inputFilters(InputImg& input){
    cin >> input.nbF;
    if (input.nbF < 0) {
        error_nb_filter(input.nbF);
        exit(0);
    }
}

void inputDimensions(InputImg& input){
    cin >> input.nbC;
    cin >> input.nbL;
}

void inputPixels(InputImg& input){
    unsigned int l(input.nbL);
    unsigned int c(input.nbC);

    input.inputImg.resize(l);

    for (auto &i : input.inputImg) {
        for (size_t j(0); j < c; j++) {

            Color p(colorRead());

            if (p.r < 0 or p.r > maxVal) {
                error_color(p.r);
                exit(0);
            }
            if (p.g < 0 or p.g > maxVal) {
                error_color(p.g);
                exit(0);
            }
            if (p.b < 0 or p.b > maxVal) {
                error_color(p.b);
                exit(0);
            }

            i.push_back(p);
        }
    }
}

// Lecture d'une couleur
Color colorRead(){
    Color p = black;
    cin >> p.r;
    cin >> p.g;
    cin >> p.b;
    return p;
}

NormImg normalize(InputImg rgb){
    int l = rgb.nbL;
    int c = rgb.nbC;
    NormImg normOut(l,vector<int>(c));

    int nbR(rgb.nbR);

    for (unsigned int i(0); i < rgb.nbL; i++){
        for (unsigned int j(0); j < rgb.nbC; j++){

            int r(rgb.inputImg[i][j].r);
            int g(rgb.inputImg[i][j].g);
            int b(rgb.inputImg[i][j].b);

            // Calcul de l'intensité normalisée
            double normInt(sqrt(r*r + g*g + b*b) / (sqrt(3) * maxVal));

            for (int k(0); k <= nbR; k++){
                if (k == nbR && normInt >= rgb.thresholds[k-1]){
                    normOut[i][j] = nbR;
                }
                if (normInt >= rgb.thresholds[k-1] && normInt < rgb.thresholds[k]){
                    normOut[i][j] = k;
                }
            }
        }
    }
    return normOut;
}

void filter(NormImg& source, int nbL, int nbC, int nbF, int nbR) {

    NormImg destination = source;
    int val(0);

    for (int n(1); n <= nbF; n++){
        for (int x(1); x < nbL-1; x++) {
            for (int y(1); y < nbC-1; y++){
                val = getPixelValue(x, y, nbR, source);
                destination[x][y] = val;
            }
        }
        source = destination;
    }

//  Bordure noire
    blackEdge(source, nbL, nbC, nbF);
}

int getPixelValue(int x, int y, int nbR, NormImg& source){

    NeighborCounter count(nbR + 1);
    int current(0);

    // On itère parmi les voisins du pixel de coordonnées (x,y)
    for (int i(-1); i <= 1; i++) {
        for (int j(-1); j <= 1; j++) {

            if (i != 0 or j != 0) {
                // On stocke la valeur d'un voisin
                current = source[x + i][y + j];

                // On compte le nombre de voisins identiques dans une liste "count"
                for (int c(0); c <= nbR; c++) {
                    if (c == current) {

                        count[c] = count[c] + 1;

                        // Condition d'arrêt si on a 6 voisins de même valeur
                        if (count[c] >= maxSameVals) {
                            return c;
                        }
                    }
                }
            }
        }
    }
    return filterColor;
}

void blackEdge(NormImg& norm, int nbL, int nbC, int nbF) {
    if (nbF > 0) {
        for (int i(0); i < nbL; i++) {
            for (int j(0); j < nbC; j++) {

                // Si le pixel est en bordure, on modifie
                if (i == 0 or j == 0 or i == nbL - 1 or j == nbC - 1) {
                    norm[i][j] = filterColor;
                }
            }
        }
    }
}

RGBImg render(NormImg filtered, int nbL, int nbC, ReducedColors rColors){
    RGBImg rendered;

    RGBImg inputImg(nbL,vector<Color>(nbC));
    rendered = inputImg;

    for (int i(0); i < nbL; i++){
        for (int j(0); j < nbC; j++){

            int normPixelVal(filtered[i][j]);

            rendered[i][j] = rColors[normPixelVal];
        }
    }
    return rendered;
}

void printRGB(RGBImg rgb, int nbL, int nbC){
    cout << "P3" << endl;
    cout << nbC << " " << nbL << endl;
    cout << maxVal << endl;
    for (int i(0); i < nbL; i++){
        for (int j(0); j < nbC; j++){
            cout << rgb[i][j].r << " ";
            cout << rgb[i][j].g << " ";
            cout << rgb[i][j].b << " ";
        }
        cout << endl;
    }
    cout << endl; // Pour que le résultat soit identique à celui de la demo
}