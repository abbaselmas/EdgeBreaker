
//
// off2ov - filter to translate off files to OVTable format
//
// Author: Dinesh Shikhare (dinesh@ncst.ernet.in)
// (last modified: June 26, 2001)
// 
// Usage: off2ov < input.off > output.ov 
//

#include <iostream>
#include <HEPolMesh.h>
#include <fstream>
#include <sstream>
using namespace std;

void loadOFF(HEmesh& hem, string s);		// load OFF object
void saveOVTable(HEmesh& hem, string s);	// save in OVTable format


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cerr << "Usage: off2ov input.off output.ov";
		return 0;
	}
	HEmesh hem;
	loadOFF(hem, argv[1]);

	vector<int> bvert;
	vector<int> loopextents;
	int numloops = hem.getBoundaries(bvert, loopextents);

	int dpts, dtris;
	vector<int> dummy;
	dtris = hem.triangulateLoops(bvert, loopextents, dummy);
	dpts = dummy.size();

	saveOVTable(hem, argv[2]);

	return 0;
}

void loadOFF(HEmesh& hem, string s)		// load OFF object
{
	std::ifstream infile(s);
	string str;
	infile >> str;
	
	int numpts, numtri, tmp;

	infile >> numpts >> numtri >> tmp; //vertex face count

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

void saveOVTable(HEmesh& hem, string s)
{
	std::ofstream ofile(s);
	int numpts, numtri;
	hem.size(numpts, numtri);
	ofile << numtri << endl;	
	for (int i = 0; i < numtri; i++) {
		vector<int> vtxs;
		hem.getVertices(i, vtxs);
		int t[3], o[3];
		hem.oppositeTrisVerts(i, t, o);
		ofile<< vtxs[0] << " " << (3*t[0] + o[0]) << endl;
		ofile << vtxs[1] << " " << (3*t[1] + o[1]) << endl;
		ofile << vtxs[2] << " " << (3*t[2] + o[2]) << endl;
	}

	ofile << numpts << endl;
	for (int i = 0; i < numpts; i++) {
		Vertex v = hem.vertexlist[i].pt;
		ofile << v.x << " " << v.y << " " << v.z << endl;
	}
	ofile.close();
}

