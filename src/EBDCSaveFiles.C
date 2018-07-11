#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <EBUtils.h>
#include <EdgebreakerDecompress.h>
#include <algorithm>
using namespace std;

// Write OFF file
void EdgebreakerDecompress::saveOFF(char* filename)
{
	ofstream ofs(filename);

	ofs << "OFF" << endl;
	int numpts, numtri;
	hem.size(numpts, numtri);
	ofs << numpts << " " << numtri << " 0" << endl;	

	for (int i = 0; i < numpts; i++) {
		Vertex v = hem.vertexlist[i].pt;
		ofs << v.x << " " << v.y << " " << v.z << endl;
	}

	for (int i = 0; i < numtri; i++) {
		vector<int> vtxs;
		hem.getVertices(i, vtxs);
		ofs << "3 " << vtxs[0] << " " << vtxs[1] << " " << vtxs[2] << endl;
	}

	ofs.close();
}

// Write OV TAble.
void EdgebreakerDecompress::saveOVTable(char* filename)
{
	ofstream ofs(filename);

	int numpts, numtri;
	hem.size(numpts, numtri);
	ofs << numtri << endl;	
	for (int i = 0; i < numtri; i++) {
		vector<int> vtxs;
		hem.getVertices(i, vtxs);
		int t[3], o[3];
		hem.oppositeTrisVerts(i, t, o);
		ofs << vtxs[0] << " " << (3*t[0] + o[0]) << endl;
		ofs << vtxs[1] << " " << (3*t[1] + o[1]) << endl;
		ofs << vtxs[2] << " " << (3*t[2] + o[2]) << endl;
	}

	ofs << numpts << endl;
	for (int i = 0; i < numpts; i++) {
		Vertex v = hem.vertexlist[i].pt;
		ofs << v.x << " " << v.y << " " << v.z << endl;
	}

	ofs.close();
}

void EdgebreakerDecompress::cleanupDummy()
{
	int numtri = C.size() + 1;
	vector<bool> trimask = vector<bool>(numtri, true);

	// mark triangles to be discarded and modify indices...
	int dummy_count = dummy.size();
	vector<int> dc = vector<int>(dummy_count);
	copy(dummy.begin(), dummy.end(), dc.begin());

	for (int i = 0; i < dummy_count; i++) {
		for (int j = 0; j < numtri; j++) {
			if (trimask[j] == false)
				continue;
			if ((V[3*j] == dc[i]) || (V[3*j+1] == dc[i]) || (V[3*j+2] == dc[i]))
				trimask[j] = false;
			else {
				for (int k = 0; k < 3; k++) {
					if (V[3*j+k] > dc[i])
						V[3*j+k] -= 1;
				}
			}
		}
		for (int j = i+1; j < dummy_count; j++) {
			dc[j] -= 1;
		}
	}

	// now populate HEmesh...
	int numpts = G_in.size();
	if (dummy_count > 0) {
		int from = 0;
		int to;
		for (int i = 0; i < dummy_count; i++) {
			to = dummy[i];
			for (int j = from; j < to; j++) {
				Vertex p = G[j];
				hem.addVertex(p);
			}
			from = to+1;
		}
		if (from < numpts) {
			to = numpts;
			for (int j = from; j < to; j++) {
				Vertex p = G[j];
				hem.addVertex(p);
			}
		}
	}
	else {
		for (int i = 0; i < numpts; i++) {
			Vertex p = G[i];
			hem.addVertex(p);
		}
	}

	for (int i = 0; i < numtri; i++) {
		if (trimask[i]) {
			int v[3];
			v[0] = V[3*i]; v[1] = V[3*i+1]; v[2] = V[3*i+2];
			hem.addPolygon(3, v);
		}
	}
}

// --------------- EBDCSaveFiles.C
