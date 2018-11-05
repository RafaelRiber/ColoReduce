/** \author Rafael Riber
 *  \date	dec 2018
 */
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
using namespace std;

const int MAX = 255;
const double EPSILON = 0.001;

typedef vector<vector<array<int, 3>>> RGBImg;
typedef vector<vector<double>> normalisedImg;

//Décomposition du fichier d'entrée
struct InputImage{
	int nBr;
	vector<array<int, 3>> reducedColors = {{0,0,0}};
	vector<double> thresholds = {0.0, 0.0};
	int nBfilters;
	string header;
	int nbC;
	int nbL;
	int MAX;
	RGBImg inputImg = { { {0,0,0} } };
};

void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);

InputImage fileRead();
normalisedImg thresholding(InputImage RGB);

int main()
{
  InputImage IMG = fileRead();
	return 0;
}

void error_nbR(int nbR) //OK
{
	cout << "Invalid number of colors: " << nbR << endl;
}

void error_color(int id) //OK
{
	cout << "Invalid color value " << id << endl;
}

void error_threshold(double invalid_val) //OK
{
	cout << "Invalid threshold value: " << invalid_val << endl;
}

void error_nb_filter(int nb_filter) //OK
{
	cout << "Invalid number of filter: " << nb_filter << endl;
}

void error_filetype(string header) //OK
{
	cout << "Invalid filetype header: " << header << endl;
}

InputImage fileRead(){
  InputImage IN;

	//Nombre reduit de couleurs - min 2, max 255
	cin >> IN.nBr;
	if(IN.nBr < 2 or IN.nBr > MAX) error_nbR(IN.nBr);

	//valeur des couleurs réduites
	for (int i = 1; i <= IN.nBr; i++){
		for (int j = 0; j <= 2; j++){
			cin >> IN.reducedColors[i][j];
			if(IN.reducedColors[i][j] < 0 or IN.reducedColors[i][j] > MAX){
				 error_color(IN.reducedColors[i][j]);
			 }
		}
	}
	/*
	//DEBUG: PRINT TABLE
	cout << "REDUCED COLOR TABLE:" << endl;
	for (int i = 0; i <= IN.nBr; i++){
		for (int j = 0; j <= 2; j++){
			cout << IN.reducedColors[i][j] << endl;
		}
		cout << endl;
	}
	*/

	//Entrée des nBr - 1 seuils
	for (int i = 0; i < IN.nBr - 1; i++){
		cin >> IN.thresholds[i];
		if (i >= 1){
			double deltaThresholds = abs(IN.thresholds[i] - IN.thresholds[i-1]);
			if(deltaThresholds < EPSILON) error_threshold(IN.thresholds[i]);
		}
	}

	cin >> IN.nBfilters;
	if (IN.nBfilters <= 0) error_nb_filter(IN.nBfilters);

	cin >> IN.header;
	if (IN.header != "P3") error_filetype(IN.header);
	cin >> IN.nbC;
	cin >> IN.nbL;
	cin >> IN.MAX;

	//Pixels de l'image
	for (int i = 0; i < IN.nbL; i++){
		for(int j = 0; j < IN.nbC; j++){
			for (int k = 0; k <= 2; k++){
				cin >> IN.inputImg[i][j][k];
			}
		}
	}
	return IN;
}

//DRAFT
normalisedImg thresholding(InputImage RGB){
	normalisedImg normImg;
	int sumRGBSquared = 0;

	//READ RGB[i][j], and norm = sqrt(R^2 + G^2 + B^2) / (sqrt(3) * MAX))
	for (int i = 0; i < RGB.nbL; i++){
		for(int j = 0; j < RGB.nbC; j++){
			for (int k = 0; k <= 2; k++){
				sumRGBSquared += RGB.inputImg[i][j][k] * RGB.inputImg[i][j][k];
			}
			normImg[i][j] = sqrt(sumRGBSquared) / (sqrt(3) * MAX);
			cout << normImg[i][j] << endl;
		}
	}

	return normImg;
}

//RGBImg outpoutImage(RGB, nbL, nbC){}
