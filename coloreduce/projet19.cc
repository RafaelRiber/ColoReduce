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
typedef vector<vector<double>> normalizedImg;

//Décomposition du fichier d'entrée
struct ImageInput{
	int nBr;
	vector<int> reducedColors = {0,0,0};
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
normalizedImg normalize(ImageInput rgb);

int main()
{
	ImageInput IMG = fileRead();
	normalize(IMG);

	cout << "DONE" << endl;
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

void error_filetype(string header)
{
	cout << "Invalid filetype header: " << header << endl;
}

ImageInput fileRead(){
	ImageInput IN;

	//Nombre reduit de couleurs - min 2, max 255
	cin >> IN.nBr;
	if(IN.nBr < 2 or IN.nBr > MAX) error_nbR(IN.nBr);

	//valeur des couleurs réduites
	int tmp(0);
	for (int i = 1; i <= 3*IN.nBr; i++){
			cin >> tmp;
			if(tmp < 0 or tmp > MAX){
				error_color(tmp);
			}
			IN.reducedColors.push_back(tmp);
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
	// for (int i = 0; i < IN.inputImg.size(); i++){
	// 	cout << i << " - " << IN.inputImg[i] << endl;
	// 	if ((i+1) % 3 == 0) cout << endl;
	// }
	return IN;
}

normalizedImg normalize(ImageInput rgb){
	vector<double> normalizedVector;

	for (int k = 0; k < rgb.inputImg.size(); k = k + 3){
		int r = rgb.inputImg[k];
		int g = rgb.inputImg[k+1];
		int b = rgb.inputImg[k+2];

		//cout << r << ";" << g << ";" << b << endl;

		r *= r;
		g *= g;
		b *= b;
		double normPixel = sqrt(r + g + b)/(sqrt(3)*(MAX));
		normalizedVector.push_back(normPixel);
	}
	 cout << "size: " << normalizedVector.size() << endl;
	 cout << "nbL: " << rgb.nbL << endl;
	 cout << "nbC: " << rgb.nbC << endl;

	//DEBUG - PRINT list of normalized vals
	// for (int i = 0; i < normalizedVector.size(); i++){
	// 	cout << i << " - " << normalizedVector[i] << endl;
	// }

	//convert to 2D vector
	normalizedImg norm = {{0},{0}};
	int j(0);
	int k(0);

	for (int i = 0; i < (rgb.nbL * rgb.nbC); i++){

		cout << "i: " << i << " (" << j << ";" << k << ") - " << normalizedVector[i]<<endl;


		//BUG A i=4


		norm[j][k] = normalizedVector[i];

		cout << "IIIII  " << i;
		k++;
		if ((i+1) % rgb.nbC == 0){
			j++;
			k = 0;
		}
	}

	//DEBUG - Print norm matrix
	// for (int i = 0; i < rgb.nbL; i++){
	// 	for (int j = 0; j < rgb.nbC; j++){
	// 		cout << norm[i][j] << endl;
	// 	}
	// }

	return norm;
}
