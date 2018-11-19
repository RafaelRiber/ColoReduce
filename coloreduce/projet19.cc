/** \author Rafael Riber
*  \date	dec 2018
*/
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;

const int maxVal = 255;
const double epsilon = 0.001;

typedef struct {
    int r, g, b;
} Pixel;

typedef vector<vector<int>> normalizedImg;
typedef vector<vector<Pixel>>  rgbImg;
typedef vector<int>            neighborList;

//Décomposition du fichier d'entrée
struct ImageInput{
    int nbR;
    vector<Pixel> reducedColors;
    vector<double> thresholds;
    int nbFilters;
    string header;
    int nbC;
    int nbL;
    int max;
    rgbImg inputImg;
};

void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);

ImageInput fileRead();

void reducedInput(ImageInput& input);
void inputThresholds(ImageInput& input);
void inputPixels(ImageInput& input);

normalizedImg normalize(ImageInput rgb);

void getNeighbors(neighborList& n, normalizedImg norm, int l, int c);
int pixelVal(neighborList neighbors, int nbR);
void filter(normalizedImg& filtered, int nbF, int nbL, int nbC, int nbR);

rgbImg render(normalizedImg filtered, int nbL, int nbC, vector<Pixel> reducedColors);

void printRGB(rgbImg rgb, int nbL, int nbC);

int main()
{
    ImageInput image = fileRead();

    normalizedImg norm = normalize(image);

//    cout << endl;
//    for (int i = 0; i < image.nbL; i++){
//        for (int j = 0; j < image.nbC; j++){
//            cout << norm[i][j] << " ";
//            if ((j + 1) % image.nbC == 0) cout << endl;
//        }
//    }
//    cout << endl;

    filter(norm, image.nbFilters,image.nbL, image.nbC, image.nbR);

//    cout << endl;
//    for (int i = 0; i < image.nbL; i++){
//        for (int j = 0; j < image.nbC; j++){
//            cout << norm[i][j] << " ";
//            if ((j + 1) % image.nbC == 0) cout << endl;
//        }
//    }
//    cout << endl;

    rgbImg rendered = render(norm, image.nbL, image.nbC, image.reducedColors);

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

ImageInput fileRead(){
    ImageInput input;

    //Nombre reduit de couleurs - min 2, max 255
    cin >> input.nbR;
    if(input.nbR < 2 or input.nbR > maxVal){
        error_nbR(input.nbR);
        exit(0);
    }

    reducedInput(input);
    inputThresholds(input);

    cin >> input.nbFilters;
    if (input.nbFilters < 0) {
        error_nb_filter(input.nbFilters);
        exit(0);
    }

    string header;
    cin >> header;
    cin >> input.nbC;
    cin >> input.nbL;
    cin >> input.max;

    // Pixels de l'image d'entrée
    inputPixels(input);
    return input;
}

void reducedInput(ImageInput& input){
    input.reducedColors.resize(input.nbR + 1);
    input.reducedColors[0].r = 0;
    input.reducedColors[0].g = 0;
    input.reducedColors[0].b = 0;

    for (int i = 1; i <= input.nbR; i++){

        cin >> input.reducedColors[i].r;
        cin >> input.reducedColors[i].g;
        cin >> input.reducedColors[i].b;

        if(input.reducedColors[i].r < 0 or input.reducedColors[i].r > maxVal){
            error_color(i);
            exit(0);
        }
        if(input.reducedColors[i].g < 0 or input.reducedColors[i].g > maxVal){
            error_color(i);
            exit(0);
        }
        if(input.reducedColors[i].b < 0 or input.reducedColors[i].b > maxVal){
            error_color(i);
            exit(0);
        }
    }
}

void inputThresholds(ImageInput& input){
    input.thresholds.resize(input.nbR + 1);
    input.thresholds[0] = 0;

    for (int i = 1; i < input.nbR; i++){
        cin >> input.thresholds[i];
        double deltaThresholds = abs(input.thresholds[i] - input.thresholds[i-1]);
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

void inputPixels(ImageInput& input){
    input.inputImg.resize(input.nbL);
    for (int i = 0; i < input.nbC; i++)
        input.inputImg[i].resize(input.nbC);

    for (int i = 0; i < input.nbL; i++){
        for (int j = 0; j < input.nbC; j++){

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

normalizedImg normalize(ImageInput rgb){
    normalizedImg norm;

    int nbR = rgb.nbR;

    norm.resize(rgb.nbL);
    for (int i = 0; i < rgb.nbC; i++)
        norm[i].resize(rgb.nbC);


    for (int i = 0; i < rgb.nbL; i++){
        for (int j = 0; j < rgb.nbC; j++){
            int r = rgb.inputImg[i][j].r;
            int g = rgb.inputImg[i][j].g;
            int b = rgb.inputImg[i][j].b;

            double normInt = sqrt(r*r + g*g + b*b) / (sqrt(3) * maxVal);

            for (int k = 0; k <= nbR; k++){
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

void getNeighbors(neighborList& n, normalizedImg norm, int l, int c){
    int iter = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int y = c + j;
            int x = l + i;
            if (not(i == 0 && j == 0)){
                n[iter] = norm[x][y];
                iter++;
            }
        }
    }
}

//return pixel value based on neigbors rule
int pixelVal(neighborList neighbors, int nbR){
    sort(neighbors.begin(), neighbors.begin() + 8); // Linéaire car taille toujours 8

    int max(0), count(1);

    for (int i = 1; i < 8; i++) {
        if (neighbors[i] == neighbors[i - 1]) {
            count++;
            if (count >= 6) return neighbors[i];
        }
        else {
            if (max < count) max = count;
            count = 1;
        }
    }
    return 0;
}


void filter(normalizedImg& filtered, int nbF, int nbL, int nbC, int nbR) {

    normalizedImg copy = filtered;
    neighborList neighbors(8);


    for (int n = 1; n <= nbF; n++){
        for (int i = 1; i < nbL-1; i++) {
            for (int j = 1; j < nbC-1; j++){

                getNeighbors(neighbors, filtered, i, j);  //SUPER LONG ?!?!?!?!?!

                int x = pixelVal(neighbors, nbR);
                //int x = 0;

                //cout << i << " " << j << endl;

                copy[i][j] = x;
            }
        }
        filtered = copy;
    }

//    Bordure noire
    if (nbF > 0) {
        for (int i = 0; i < nbL; i++) {
            for (int j = 0; j < nbC; j++) {
                if (i == 0 or j == 0 or i == nbL - 1 or j == nbC - 1) {
                    copy[i][j] = 0;
                }
            }
        }
    }

    filtered = copy;
}

void printRGB(rgbImg rgb, int nbL, int nbC){
    cout << "P3" << endl;
    cout << nbC << " " << nbL << endl;
    cout << maxVal << endl;
    for (int i = 0; i < nbL; i++){
        for (int j = 0; j < nbC; j++){
            cout << rgb[i][j].r << " ";
            cout << rgb[i][j].g << " ";
            cout << rgb[i][j].b << " ";
        }
        cout << endl;
    }
}

rgbImg render(normalizedImg filtered, int nbL, int nbC, vector<Pixel> reducedColors){
    rgbImg rendered;

    rendered.resize(nbL);
    for (int i = 0; i < nbC; i++)
        rendered[i].resize(nbC);

    for (int i = 0; i < nbL; i++){
        for (int j = 0; j < nbC; j++){

            int pixelVal = filtered[i][j];

            rendered[i][j].r = reducedColors[pixelVal].r;
            rendered[i][j].g = reducedColors[pixelVal].g;
            rendered[i][j].b = reducedColors[pixelVal].b;
        }
    }
    return rendered;
}