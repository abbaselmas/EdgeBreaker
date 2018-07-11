
//
// patcholes - code to patch hole in a triangle mesh
//
// Author: Dinesh Shikhare (dinesh@ncst.ernet.in)
// (last modified: June 26, 2001)
// 
// Usage: patcholes < input.off > output.off 
//

#include <iostream>
#include "HEPolMesh.h"
#include <fstream>
#include <sstream>
using namespace std;

void loadOFF(HEmesh& hem, string s);		// load OFF object
void saveOFF(HEmesh& hem, string s);		// save OFF object
void saveOVTable(HEmesh& hem, string s);	// save in OVTable format


int main(int argc, char** argv)
{
	if (argc == 4)
	{
		HEmesh hem;
		loadOFF(hem, argv[1]);

		vector<int> bvert;
		vector<int> loopextents;
		int numloops = hem.getBoundaries(bvert, loopextents);

		int dpts, dtris;
		vector<int> dummy;
		dtris = hem.triangulateLoops(bvert, loopextents, dummy);
		dpts = dummy.size();

		saveOFF(hem, argv[2]);
		saveOVTable(hem, argv[3]);
	}
	else
		cout << "Usage: patcholes input.off output.off output.ov\n";
	return 0;
}

void loadOFF(HEmesh& hem, string s)		// load OFF object
{
	ifstream infile(s);
	string str;
	infile >> str;

	int numpts, numtri, tmp;
	infile >> numpts >> numtri >> tmp;

	for (int i = 0; i < numpts; i++) {
		Vertex p;
		infile >> p.x >> p.y >> p.z;
		hem.addVertex(p);
	}

	for (int i = 0; i < numtri; i++) {
		int v[3];
		int jnk;
		infile >> jnk >> v[0] >> v[1] >> v[2];
		hem.addPolygon(3, v);
	}
	infile.close();
}

void saveOFF(HEmesh& hem, string s)
{
	ofstream infile(s);
	infile << "OFF" << endl;
	int numpts, numtri;
	hem.size(numpts, numtri);
	infile << numpts << " " << numtri << " 0" << endl;	

	for (int i = 0; i < numpts; i++) {
		Vertex v = hem.vertexlist[i].pt;
		infile << v.x << " " << v.y << " " << v.z << endl;
	}

	for (int i = 0; i < numtri; i++) {
		vector<int> vtxs;
		hem.getVertices(i, vtxs);
		infile << "3 " << vtxs[0] << " " << vtxs[1] << " " << vtxs[2] << endl;
	}
	infile.close();
}

void saveOVTable(HEmesh& hem, string s)
{
	ofstream infile(s);
	int numpts, numtri;
	hem.size(numpts, numtri);
	infile << numtri << endl;	
	for (int i = 0; i < numtri; i++) {
		vector<int> vtxs;
		hem.getVertices(i, vtxs);
		int t[3], o[3];
		hem.oppositeTrisVerts(i, t, o);
		infile << vtxs[0] << " " << (3*t[0] + o[0]) << endl;
		infile << vtxs[1] << " " << (3*t[1] + o[1]) << endl;
		infile << vtxs[2] << " " << (3*t[2] + o[2]) << endl;
	}

	infile << numpts << endl;
	for (int i = 0; i < numpts; i++) {
		Vertex v = hem.vertexlist[i].pt;
		infile << v.x << " " << v.y << " " << v.z << endl;
	}
	infile.close();
}

