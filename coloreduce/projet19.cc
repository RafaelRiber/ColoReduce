/** \author Rafael Riber
 *  \date	dec 2018
 */
#include <iostream>
#include <vector>
#include <array>
using namespace std;

const int MAX = 255;

typedef vector<vector<array<int, 3>>> RGBImg;

//Décomposition du fichier d'entrée
struct InputImage{
	int nBr;
	vector<array<int, 3>> reducedColors = {{0,0,0}};
	vector<float> thresholds = {0.0, 0.0};
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

int main()
{

  InputImage IMG = fileRead(); //TODO-> ERROR HANDLING
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

//DELTA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void error_threshold(double invalid_val)
{
	cout << "Invalid threshold value: " << invalid_val << endl;
}

void error_nb_filter(int nb_filter)
{
	cout << "Invalid number of filter: " << nb_filter << endl;
}

InputImage fileRead(){
  InputImage IN;

	//Nombre reduit de couleurs
	cin >> IN.nBr;

	//valeur des couleurs réduites
	for (int i = 0; i <= IN.nBr - 1; i++){
		for (int j = 0; j <= 2; j++){
			cin >> IN.reducedColors[i][j];
		}
	}

	//Entrée des nBr - 1 seuils
	for (int i = 0; i < IN.nBr - 1; i++){
		cin >> IN.thresholds[i];
	}

	cin >> IN.nBfilters;
	cin >> IN.header;
	cin >> IN.nbC;
	cin >> IN.nbL;
	cin >> IN.MAX;

	//Pixels de l'image
	for (int i = 0; i < IN.nbL; i++){
		for(int j = 0; j < IN.nbC; j++){
			for (int k = 0; k < 2; k++){
				cin >> IN.inputImg[i][j][k];
			}
		}
	}
  return IN;
}
