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

typedef vector<int> RGBImg;
typedef vector<vector<double>> normalisedImg;

//Décomposition du fichier d'entrée
struct ImageInput{
	int nBr;
	vector<array<int, 3>> reducedColors = {{0,0,0}};
	vector<double> thresholds = {0.0, 0.0};
	int nbFilters;
	string header;
	int nbC;
	int nbL;
	int max;
	RGBImg inputImg = {0,0,0,0};
};

void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);

ImageInput fileRead();

int main()
{
	ImageInput IMG = fileRead();

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

ImageInput fileRead(){
	ImageInput IN;

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

	//Entrée des nBr - 1 seuils
	for (int i = 0; i < IN.nBr - 1; i++){
		cin >> IN.thresholds[i];
		if (i >= 1){
			double deltaThresholds = abs(IN.thresholds[i] - IN.thresholds[i-1]);
			if(deltaThresholds < EPSILON) error_threshold(IN.thresholds[i]);
		}
	}

	cin >> IN.nbFilters;
	if (IN.nbFilters <= 0) error_nb_filter(IN.nbFilters);

	// cout << "filters: " << IN.nbFilters << endl;

	cin >> IN.header;
	if (IN.header != "P3") error_filetype(IN.header);

	// cout << "good header !" << endl;

	cin >> IN.nbC;
	cin >> IN.nbL;
	cin >> IN.max;

	int numPixels = IN.nbL * IN.nbC;
	int numValues = 3*numPixels;

	// cout << "col: " << IN.nbC << endl;
	// cout << "lin: " << IN.nbL << endl;
	// cout << "max: " << IN.max << endl;
	// cout << numPixels << " pixels" << endl;
	// cout << numValues << " values" << endl;

	IN.inputImg.resize(numValues);

	for (int i = 0; i < numValues; i++){
		cin >> IN.inputImg[i];
	}

	// cout << "Image successfully read:" << endl << endl;
	for (int i = 0; i < IN.inputImg.size(); i++){
		cout << i << " - " << IN.inputImg[i] << endl;
		if ((i+1) % 3 == 0) cout << endl;
	}
	return IN;
}
