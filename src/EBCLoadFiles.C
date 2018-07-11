#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <EdgebreakerCompress.h>
#include <EBUtils.h>
using namespace std;

void EdgebreakerCompress::loadOFF(char* filename)		// load OFF object
{
	assert(filename);
	ifstream ifs(filename);

	char ch;
	do {
		ifs.get(ch);
	} while (ch != '\n');

	int numpts, numtri;
	int tmp;
	ifs >> numpts >> numtri >> tmp;

	for (int i = 0; i < numpts; i++) {
		Vertex p;
		ifs >> p.x >> p.y >> p.z;
		hem.addVertex(p);
	}

	for (int i = 0; i < numtri; i++) {
		int v[3];
		int jnk;
		ifs >> jnk >> v[0] >> v[1] >> v[2];
		hem.addPolygon(3, v);
	}

	ifs.close();

	vector<int> bvert;
	vector<int> loopextents;
	int numloops = hem.getBoundaries(bvert, loopextents);
	cout << "Mesh has " << numloops << " loop(s)." << endl;
	for (int i = 0; i < numloops; i++) {
		cout << "loop " << i << ":";
		for (int j = loopextents[2*i]; j < loopextents[2*i+1]; j++) {
			cout << " " << bvert[j];
		}
		cout << endl;
	}

	if (numloops > 0) {
		int dpts, dtris;
		dtris = hem.triangulateLoops(bvert, loopextents, dummyVertices);
		dpts = dummyVertices.size();
		cout << "Inserted " << dpts << " dummy points and "
		<< dtris << " dummy triangles." << endl;
	}

	updateOVTable();
}

void EdgebreakerCompress::loadOVTable(char* filename)	// load OVTable object
{
	int i = 0;
	FILE* pInFile = NULL;

	//Number of Triangles 	
	int nNumOfTriangles;
	//Number of Vertices 			
	int nNumOfVertices;							

	//Open the file.
	pInFile = fopen(filename, "rt");
	if (pInFile == NULL)
		PrintErrorAndQuit((char*)"Can not open input file\n");

	//Read number of triangles from the file
	if (fscanf(pInFile, "%d", &nNumOfTriangles) == -1)
		PrintErrorAndQuit((char*)"Error reading file\n");


	//Allocate memory for V Table
	V = new int [nNumOfTriangles*3]; 

	//Allocate memory for O Table
	O = new int [nNumOfTriangles*3]; 

	//Read VO TAble from the file
	for (i = 0; i<nNumOfTriangles; i++)
	{
		if (fscanf(pInFile, "%d %d", &(V[i*3]), &(O[i*3]))==-1)
			PrintErrorAndQuit((char*)"Error reading file\n");
		if (fscanf(pInFile, "%d %d", &(V[i*3+1]), &(O[i*3+1])) == -1)
			PrintErrorAndQuit((char*)"Error reading file\n");
		if (fscanf(pInFile, "%d %d", &(V[i*3+2]), &(O[i*3+2])) == -1)
			PrintErrorAndQuit((char*)"Error reading file\n");
	}


	//Read number of vertices from the file
	if (fscanf(pInFile, "%d", &nNumOfVertices) == -1)
		PrintErrorAndQuit((char*)"Error reading file\n");
	
	//Allocate memory for vertex array
	G = new Vertex [nNumOfVertices]; 
	G_est = new Vertex [nNumOfVertices]; 

	//Read all vertices from the file
	for (i = 0; i<nNumOfVertices; i++)
	{
		if (fscanf(pInFile, "%f %f %f", &(G[i].x), &(G[i].y), &(G[i].z)) == -1)
			PrintErrorAndQuit((char*)"Error reading file\n");
	}

	//Allocate memory for M and U tables
	M = new int [nNumOfVertices];	//Table for marking visited vetrices
	U = new int [nNumOfTriangles];	//Table for marking visited triangles

	//init tables for marking visited vertices and triangles
	for (i = 0; i<nNumOfVertices; i++) M[i] = 0;
		for (i = 0; i<nNumOfTriangles; i++) U[i] = 0;

	//Close the file.
			fclose(pInFile);
	}

// support functions...

	void EdgebreakerCompress::updateOVTable()
	{
	// OVT connectivity info
		int numpts, numtri;
		hem.size(numpts, numtri);

	//Allocate memory for V and O Tables
		V = new int [numtri*3];
		O = new int [numtri*3];

		for (int i = 0; i < numtri; i++) {
			vector<int> vtxs;
			hem.getVertices(i, vtxs);
			int t[3], o[3];
			hem.oppositeTrisVerts(i, t, o);
			V[3*i]   = vtxs[0]; O[3*i]   = 3*t[0] + o[0];
			V[3*i+1] = vtxs[1]; O[3*i+1] = 3*t[1] + o[1];
			V[3*i+2] = vtxs[2]; O[3*i+2] = 3*t[2] + o[2];
		}

	//Allocate memory for vertex array
		G = new Vertex [numpts]; 
		G_est = new Vertex [numpts]; 

		for (int i = 0; i < numpts; i++) {
			Vertex p = hem.vertexlist[i].pt;
			G[i].x = p.x;
			G[i].y = p.y;
			G[i].z = p.z;
		}

	//Allocate memory for M and U tables
	M = new int [numpts];	//Table for marking visited vetrices
	U = new int [numtri];	//Table for marking visited triangles

	//init tables for marking visited vertices and triangles
	for (int i = 0; i<numpts; i++) M[i] = 0;
		for (int i = 0; i<numtri; i++) U[i] = 0;
	}

void EdgebreakerCompress::cleanupOFF()
{
	hem.vertexlist.erase(hem.vertexlist.begin(), hem.vertexlist.end());
	hem.edgelist.erase(hem.edgelist.begin(), hem.edgelist.end());
	hem.polylist.erase(hem.polylist.begin(), hem.polylist.end());
	hem.clearTable();
}