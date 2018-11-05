/** \author Rafael Riber
 *  \date	dec 2018
 *  \ColoReduce - File Read subroutine
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

int main()
{
	InputImage IMG;

	//Nombre reduit de couleurs
	cin >> IMG.nBr;

	//valeur des couleurs réduites
	for (int i = 0; i <= IMG.nBr - 1; i++){
		for (int j = 0; j <= 2; j++){
			cin >> IMG.reducedColors[i][j];
		}
	}

	//Entrée des nBr - 1 seuils
	for (int i = 0; i < IMG.nBr - 1; i++){
		cin >> IMG.thresholds[i];
	}

	cin >> IMG.nBfilters;
	cin >> IMG.header;
	cin >> IMG.nbC;
	cin >> IMG.nbL;
	cin >> IMG.MAX;

	//Pixels de l'image
	for (int i = 0; i < IMG.nbL; i++){
		for(int j = 0; j < IMG.nbC; j++){
			for (int k = 0; k < 2; k++){
				cin >> IMG.inputImg[i][j][k];
			}
		}
	}
	return 0;
}
