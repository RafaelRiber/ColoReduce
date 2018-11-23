/** \author Rafael Riber
*  \date	dec 2018
*/
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <string>

using namespace std;

const int maxVal = 255;
const double epsilon = 0.001;

typedef struct {
    int r, g, b;
} Pixel;

typedef vector<vector<int>> normalizedImg;
typedef vector<vector<Pixel>>  rgbImg;

typedef vector<Pixel> redCol;
typedef vector<int> counter;

//Décomposition du fichier d'entrée
struct ImageInput{
    int nbR;
    int nbFilters;
    int nbC;
    int nbL;
    int max;
    redCol rColors;
    vector<double> thresholds;
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

void filter(normalizedImg& norm, int nbF, int nbL, int nbC, int nbR);
int getPixelValue(int x, int y, int nbR, normalizedImg& copy);
rgbImg render(normalizedImg filtered, int nbL, int nbC, redCol rColors);
void printRGB(rgbImg rgb, int nbL, int nbC);

int main()
{
    ImageInput image = fileRead();
    normalizedImg norm = normalize(image);

    filter(norm, image.nbFilters, image.nbL, image.nbC, image.nbR);

    rgbImg rendered = render(norm, image.nbL, image.nbC, image.rColors);
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
    input.rColors.resize(input.nbR + 1);
    input.rColors[0].r = 0;
    input.rColors[0].g = 0;
    input.rColors[0].b = 0;

    for (int i = 1; i <= input.nbR; i++){

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

    rgbImg inputImg = vector<vector<Pixel>> (input.nbL,vector<Pixel>(input.nbC));
    input.inputImg = inputImg;

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

    normalizedImg norm = vector<vector<int>> (rgb.nbL,vector<int>(rgb.nbC));

    int nbR(rgb.nbR);

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

void filter(normalizedImg& norm, int nbF, int nbL, int nbC, int nbR) {

    normalizedImg copy = norm;
    int val(0);

    for (int n = 1; n <= nbF; n++){
        for (int x = 1; x < nbL-1; x++) {
            for (int y = 1; y < nbC-1; y++){
                val = getPixelValue(x, y, nbR, copy);
                norm[x][y] = val;
            }
        }
        copy = norm;
    }

//  Bordure noire
    if (nbF > 0) {
        for (int i = 0; i < nbL; i++) {
            for (int j = 0; j < nbC; j++) {
                if (i == 0 or j == 0 or i == nbL - 1 or j == nbC - 1) {
                    norm[i][j] = 0;
                }
            }
        }
    }
}

int getPixelValue(int x, int y, int nbR, normalizedImg& copy){

    counter count(nbR + 1);
    int current(0);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 or j != 0) {

                current = copy[x + i][y + j];

                for (int c = 0; c <= nbR; c++) {
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

rgbImg render(normalizedImg filtered, int nbL, int nbC, redCol rColors){
    rgbImg rendered;

    rgbImg inputImg = vector<vector<Pixel>> (nbL,vector<Pixel>(nbC));
    rendered = inputImg;

    for (int i = 0; i < nbL; i++){
        for (int j = 0; j < nbC; j++){

            int pixelVal = filtered[i][j];

            rendered[i][j].r = rColors[pixelVal].r;
            rendered[i][j].g = rColors[pixelVal].g;
            rendered[i][j].b = rColors[pixelVal].b;
        }
    }
    return rendered;
}