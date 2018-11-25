/*
 * ICC Pratique
 * Mini-Projet  - ColoReduce
 *
 * Date:   Décembre 2018
 * Auteur: Rafael Riber - EL BA1
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

typedef struct {
    int r, g, b;
} Pixel;

typedef vector<vector<int>>   normImg;
typedef vector<vector<Pixel>> rgbImg;
typedef vector<Pixel>         redCol;
typedef vector<double>        thresholdList;
typedef vector<int>           counter;

const int maxVal(255);
const double epsilon(0.001);

//Décomposition du fichier d'entrée
struct InputImg{
    int nbR; // Nombre de couleurs réduites
    int nbF; // Nombre de filtrages
    int nbC; // Nombre de pixels horizontaux
    int nbL; // Nombre de pixels verticaux
    redCol rColors;           // Liste des couleurs réduites
    thresholdList thresholds; // Liste des seuils
    rgbImg inputImg;          // Pixels de l'image d'entrée
};

// Fonctions d'erreur
void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);

// Fonctions de lecture
InputImg fileRead();
void inputReduced(InputImg &input);
void inputThresholds(InputImg& input);
void inputFilters(InputImg& input);
void inputDimensions(InputImg& input);
void inputPixels(InputImg& input);

// Fonctions de transformation
normImg normalize(InputImg rgb);
void filter(normImg& norm, int nbF, int nbL, int nbC, int nbR);
int getPixelValue(int x, int y, int nbR, normImg& copy);

// Fonction de rendu
rgbImg render(normImg filtered, int nbL, int nbC, redCol rColors);
void printRGB(rgbImg rgb, int nbL, int nbC);

int main()
{
    InputImg image(fileRead());

    normImg norm(normalize(image));

    filter(norm, image.nbF, image.nbL, image.nbC, image.nbR);

    rgbImg rendered(render(norm, image.nbL, image.nbC, image.rColors));

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

void inputReduced(InputImg &input){
    // Nombre de couleurs
    cin >> input.nbR;
    if(input.nbR < 2 or input.nbR > maxVal){
        error_nbR(input.nbR);
        exit(0);
    }

    // Valeurs des couleurs
    input.rColors = vector<Pixel>(input.nbR + 1, {0,0,0});
    input.rColors[0] = {0,0,0}; //La première couleur est toujours 0

    for (int i(1); i <= input.nbR; i++){
        cin >> input.rColors[i].r;
        cin >> input.rColors[i].g;
        cin >> input.rColors[i].b;

        if(input.rColors[i].r < 0 or input.rColors[i].r > maxVal){
            error_color(i);
            exit(0);
        }
        if(input.rColors[i].g < 0 or input.rColors[i].g > maxVal){
            error_color(i);
            exit(0);
        }
        if(input.rColors[i].b < 0 or input.rColors[i].b > maxVal){
            error_color(i);
            exit(0);
        }
    }
}

void inputThresholds(InputImg& input){
    input.thresholds = vector<double> (input.nbR, 0.0);
    input.thresholds[0] = 0;

    for (int i(1); i < input.nbR; i++){
        cin >> input.thresholds[i];
        double deltaThresholds(abs(input.thresholds[i] - input.thresholds[i-1]));
        if(deltaThresholds < epsilon){
            error_threshold(input.thresholds[i]);
            exit(0);
        }
        if(input.thresholds[i] < input.thresholds[i-1]){
            error_threshold(input.thresholds[i]);
            exit(0);
        }
    }

    input.thresholds[input.nbR] = 1.0;
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
    rgbImg inputImg = vector<vector<Pixel>> (input.nbL,vector<Pixel>(input.nbC));
    input.inputImg = inputImg;

    for (int i(0); i < input.nbL; i++){
        for (int j(0); j < input.nbC; j++){

            cin >> input.inputImg[i][j].r;
            cin >> input.inputImg[i][j].g;
            cin >> input.inputImg[i][j].b;

            if(input.inputImg[i][j].r < 0 or input.inputImg[i][j].r > maxVal){
                error_color(input.inputImg[i][j].r);
                exit(0);
            }
            if(input.inputImg[i][j].g < 0 or input.inputImg[i][j].g > maxVal){
                error_color(input.inputImg[i][j].g);
                exit(0);
            }
            if(input.inputImg[i][j].b < 0 or input.inputImg[i][j].b > maxVal){
                error_color(input.inputImg[i][j].b);
                exit(0);
            }
        }
    }
}

normImg normalize(InputImg rgb){

    normImg norm = vector<vector<int>> (rgb.nbL,vector<int>(rgb.nbC));

    int nbR(rgb.nbR);

    for (int i(0); i < rgb.nbL; i++){
        for (int j(0); j < rgb.nbC; j++){
            int r(rgb.inputImg[i][j].r);
            int g(rgb.inputImg[i][j].g);
            int b(rgb.inputImg[i][j].b);

            // Calcul de l'intensité normalisée
            double normInt(sqrt(r*r + g*g + b*b) / (sqrt(3) * maxVal));

            for (int k(0); k <= nbR; k++){
                if (k == nbR && normInt >= rgb.thresholds[k-1]){
                    norm[i][j] = nbR;
                }
                if (normInt >= rgb.thresholds[k-1] && normInt < rgb.thresholds[k]){
                    norm[i][j] = k;
                }
            }
        }
    }
    return norm;
}

void filter(normImg& norm, int nbF, int nbL, int nbC, int nbR) {

    normImg copy = norm;
    int val(0);

    for (int n(1); n <= nbF; n++){
        for (int x(1); x < nbL-1; x++) {
            for (int y(1); y < nbC-1; y++){
                val = getPixelValue(x, y, nbR, copy);
                norm[x][y] = val;
            }
        }
        copy = norm;
    }

//  Bordure noire
    if (nbF > 0) {
        for (int i(0); i < nbL; i++) {
            for (int j(0); j < nbC; j++) {
                if (i == 0 or j == 0 or i == nbL - 1 or j == nbC - 1) {
                    norm[i][j] = 0;
                }
            }
        }
    }
}

int getPixelValue(int x, int y, int nbR, normImg& copy){

    counter count(nbR + 1);
    int current(0);
    for (int i(-1); i <= 1; i++) {
        for (int j(-1); j <= 1; j++) {
            if (i != 0 or j != 0) {

                current = copy[x + i][y + j];

                for (int c(0); c <= nbR; c++) {
                    if (c == current) {
                        count[c] = count[c] + 1;

                        if (count[c] >= 6) {
                            return c;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

rgbImg render(normImg filtered, int nbL, int nbC, redCol rColors){
    rgbImg rendered;

    rgbImg inputImg = vector<vector<Pixel>> (nbL,vector<Pixel>(nbC));
    rendered = inputImg;

    for (int i(0); i < nbL; i++){
        for (int j(0); j < nbC; j++){

            int pixelVal(filtered[i][j]);

            rendered[i][j].r = rColors[pixelVal].r;
            rendered[i][j].g = rColors[pixelVal].g;
            rendered[i][j].b = rColors[pixelVal].b;
        }
    }
    return rendered;
}

void printRGB(rgbImg rgb, int nbL, int nbC){
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