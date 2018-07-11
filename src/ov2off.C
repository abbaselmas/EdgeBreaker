#include <iostream>
#include <fstream>
using namespace std;

void readOV(int& numpts, int& numtri,
	float **x, float **y, float **z, int **v1, int **v2, int **v3, string s);
void writeOFF(int numpts, int numtri,
	float *x, float *y, float *z, int *v1, int *v2, int *v3, string s);

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ov2off input.ov output.off";
		return 0;
	}
	int numpts, numtri;
	int *v1, *v2, *v3;	// array of triangle indices
	float *x, *y, *z;	// array of vertex coordinates

	readOV(numpts, numtri, &x, &y, &z, &v1, &v2, &v3, argv[1]);
	writeOFF(numpts, numtri, x, y, z, v1, v2, v3, argv[2]);

	return 0;
}

void readOV(int& numpts, int& numtri,
	float **x, float **y, float **z, int **v1, int **v2, int **v3, string s)
{
	ifstream infile(s);
	infile >> numtri;
	*v1 = new int[numtri];
	*v2 = new int[numtri];
	*v3 = new int[numtri];
	int i1, i2, i3, tmp;
	for (int i = 0; i < numtri; i++) {
		infile >> i1 >> tmp;
		infile >> i2 >> tmp;
		infile >> i3 >> tmp;
		(*v1)[i] = i1;
		(*v2)[i] = i2;
		(*v3)[i] = i3;
	}

	infile >> numpts;
	*x = new float[numpts];
	*y = new float[numpts];
	*z = new float[numpts];
	for (int i = 0; i < numpts; i++) {
		infile >> (*x)[i] >> (*y)[i] >> (*z)[i];
	}
	infile.close();
}

void writeOFF(int numpts, int numtri,
	float *x, float *y, float *z, int *v1, int *v2, int *v3, string s)
{
	ofstream infile(s);
	infile << "OFF" << endl;
	infile << numpts << " " << numtri << " 0" << endl;
	for (int i = 0; i < numpts; i++) {
		infile << x[i] << " " << y[i] << " " << z[i] << endl;
	}
	for (int i = 0; i < numtri; i++) {
		infile << "3 " << v1[i] << " " << v2[i] << " " << v3[i] << endl;
	}
	infile.close();
}

