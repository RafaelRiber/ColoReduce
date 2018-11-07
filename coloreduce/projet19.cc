/** \author Rafael Riber
*  \date	dec 2018
*/
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <string>
using namespace std;

const int MAX = 255;
const double EPSILON = 0.001;

typedef struct {
    int r, g, b;
} Pixel;

typedef vector<vector<double>> normalizedImg;
typedef vector<vector<Pixel>>  rgbImg;

//Décomposition du fichier d'entrée
struct ImageInput{
    int nBr;
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

void error_filetype(string header);

ImageInput fileRead();
normalizedImg normalize(ImageInput rgb);

int main()
{
	ImageInput IMG = fileRead();

//    for (int i = 0; i < IMG.inputImg.size(); i++){
//        for (int j = 0; j < IMG.inputImg[i].size(); j++){
//            cout << IMG.inputImg[i][j].r << endl;
//            cout << IMG.inputImg[i][j].g << endl;
//            cout << IMG.inputImg[i][j].b << endl;
//        }
//    }

    normalizedImg norm = normalize(IMG);

    cout << "DONE" << endl;
	return 0;
}

void error_nbR(int nbR)
{
	cout << "Invalid number of colors: " << nbR << endl;
    exit(0);
}

void error_color(int id)
{
	cout << "Invalid color value " << id << endl;
    exit(0);
}

void error_threshold(double invalid_val)
{
	cout << "Invalid threshold value: " << invalid_val << endl;
    exit(0);
}

void error_nb_filter(int nb_filter)
{
	cout << "Invalid number of filter: " << nb_filter << endl;
    exit(0);
}

void error_filetype(string header)
{
	cout << "Invalid filetype header: " << header << endl;
    exit(0);
}

ImageInput fileRead(){
	ImageInput input;

	//Nombre reduit de couleurs - min 2, max 255
	cin >> input.nBr;
	if(input.nBr < 2 or input.nBr > MAX) error_nbR(input.nBr);

	input.reducedColors.resize(input.nBr + 1);
	input.reducedColors[0].r = 0;
	input.reducedColors[0].g = 0;
	input.reducedColors[0].b = 0;

	//valeur des couleurs réduites
	for (int i = 1; i <= input.nBr; i++){
		int r(0), g(0), b(0);

		cin >> r;
		cin >> g;
		cin >> b;

		if(r < 0 or r > MAX){
			error_color(r);
		}
		if(g < 0 or g > MAX){
			error_color(g);
		}
		if(b < 0 or b > MAX){
			error_color(b);
		}
		input.reducedColors[i].r = r;
		input.reducedColors[i].g = g;
		input.reducedColors[i].b = b;
	}

	//  Entrée des nBr - 1 seuils
	//  Calcule l'écart entre le i-ème seuil et le précédent, et vérifie que celui-ci
	//  soit supérieur à la constante EPSILON, afin d'éviter deux seuls identiques

    input.thresholds.resize(input.nBr + 1);
	input.thresholds[0] = 0;

	for (int i = 1; i < input.nBr; i++){
		cin >> input.thresholds[i];
		double deltaThresholds = abs(input.thresholds[i] - input.thresholds[i-1]);
		if(deltaThresholds < EPSILON) error_threshold(input.thresholds[i]);
	}

	input.thresholds[input.nBr] = 1.0;

	cin >> input.nbFilters;
	if (input.nbFilters <= 0) error_nb_filter(input.nbFilters);

	cin >> input.header;
	if (input.header != "P3") error_filetype(input.header);

	cin >> input.nbC;
	cin >> input.nbL;
	cin >> input.max;

    input.inputImg.resize(input.nbC);       //CORRECT
    for (int i = 0; i < input.nbL; i++)
        input.inputImg[i].resize(input.nbL);


    int x = input.inputImg.size();
    int y = input.inputImg[0].size();

    // Pixels de l'image d'entrée
	for (int i = 0; i < x; i++){
	    for (int j = 0; j < y; j++){
			int r(0), g(0), b(0);
			cin >> r;
			cin >> g;
			cin >> b;
			if(r < 0 or r > MAX){
				error_color(r);
			}
			if(g < 0 or g > MAX){
				error_color(g);
			}
			if(b < 0 or b > MAX){
				error_color(b);
			}
			input.inputImg[i][j].r = r;
			input.inputImg[i][j].g = g;
			input.inputImg[i][j].b = b;
	    }
	}
	return input;
}

normalizedImg normalize(ImageInput rgb){
    normalizedImg norm;

    norm.resize(rgb.nbC);
    for (int i = 0; i < rgb.nbL; i++)
        norm[i].resize(rgb.nbL);

    int x = norm.size();
    int y = norm[0].size();

    for (int i = 0; i < x; i++){
        for (int j = 0; j < y; j++){
            int r = rgb.inputImg[i][j].r;
            int g = rgb.inputImg[i][j].g;
            int b = rgb.inputImg[i][j].b;

            double normInt = sqrt(r*r + g*g + b*b) / (sqrt(3) * MAX);

            norm[i][j] = normInt;
        }
    }
    return norm;
}