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
normalizedImg normalize(ImageInput rgb);
vector <int> getNeighbors(normalizedImg norm, int l, int c);
normalizedImg filter(int nbF, int nbL, int nbC, normalizedImg norm);
int pixelVal(vector<int> neighbors);

void printRGB(ImageInput rgb);

int main()
{
    ImageInput IMG = fileRead();

//    printRGB(IMG);

    normalizedImg norm = normalize(IMG);


    //PRINT NORM
    for (int i = 0; i < IMG.nbL; i++){
        for (int j = 0; j < IMG.nbC; j++){
            cout << norm[i][j] << " ";
            if ((j + 1) % IMG.nbC == 0) cout << endl;
        }
    }

    cout << endl;
    cout << endl;
    cout << endl;

    normalizedImg filtered = filter(IMG.nbFilters, IMG.nbL, IMG.nbC, norm);

    for (int i = 0; i < IMG.nbL; i++){
        for (int j = 0; j < IMG.nbC; j++){
            cout << filtered[i][j] << " ";
            if ((j + 1) % IMG.nbC == 0) cout << endl;
        }
    }

    cout << endl;
    cout << endl;

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

    input.reducedColors.resize(input.nbR + 1);
    input.reducedColors[0].r = 0;
    input.reducedColors[0].g = 0;
    input.reducedColors[0].b = 0;

    //valeur des couleurs réduites
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

    //  Entrée des nbR - 1 seuils
    //  Calcule l'écart entre le i-ème seuil et le précédent, et vérifie que celui-ci
    //  soit supérieur à la constante epsilon, afin d'éviter deux seuls identiques

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

    cin >> input.nbFilters;
    if (input.nbFilters < 0) {
        error_nb_filter(input.nbFilters);
        exit(0);
    }

    cin >> input.header;
    cin >> input.nbC;
    cin >> input.nbL;
    cin >> input.max;

    input.inputImg.resize(input.nbL);
    for (int i = 0; i < input.nbC; i++)
        input.inputImg[i].resize(input.nbC);

    // Pixels de l'image d'entrée
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
    return input;
}

//OK
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

// OK
vector<int> getNeighbors(normalizedImg norm, int l, int c){
    vector<int> neighbors;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int y = c + j;
            int x = l + i;
            if (not(i == 0 && j == 0)) neighbors.push_back(norm[x][y]);
        }
    }
    return neighbors;
}

//OK
void printRGB(ImageInput rgb){
    for (int i = 0; i < rgb.nbL; i++){
        for (int j = 0; j < rgb.nbC; j++){
            cout << " (";
            cout << rgb.inputImg[i][j].r << ";";
            cout << rgb.inputImg[i][j].g << ";";
            cout << rgb.inputImg[i][j].b << ") ";
            if ((j + 1) % rgb.nbC == 0) cout << endl;
        }
    }
}

//OK
//return pixel value based on neigbors rule
int pixelVal(vector<int> neighbors){

    sort(neighbors.begin(), neighbors.begin() + 8);

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

//OK
normalizedImg filter(int nbF, int nbL, int nbC, normalizedImg norm){
    normalizedImg filtered;

    filtered.resize(nbL);
    for (int i = 0; i < nbC; i++)
        filtered[i].resize(nbC);

    filtered = norm;

    for (int n = 1; n <= nbF; n++){
        for (int i = 1; i <= nbL-2; i++) {
            for (int j = 1; j <= nbC-2; j++){
                vector<int> neighbors = getNeighbors(norm, i, j);
                int x = pixelVal(neighbors); //COMPLEXITY ?
                filtered[i][j] = x;
            }
        }
        norm = filtered;
    }

    // Bordure noire
    if (nbF > 0) {
        for (int i = 0; i < nbL; i++) {
            for (int j = 0; j < nbC; j++) {
                if (i == 0 or j == 0 or i == nbL - 1 or j == nbC - 1) {
                    filtered[i][j] = 0;
                }
            }
        }
    }

    return filtered;

}