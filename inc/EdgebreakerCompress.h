
// EdgebreakerCompress -- Wrapper for EBCompress and pre-processing

#ifndef _EDGE_BREAKER_COMPRESS_H_
#define _EDGE_BREAKER_COMPRESS_H_

#include <EBdatatypes.h>
#include <vector>
#include <HEPolMesh.h>

class EdgebreakerCompress {

public:
	EdgebreakerCompress();
	~EdgebreakerCompress();

	void loadOFF(char* filename);			// load OFF object
	void loadOVTable(char* filename);		// load OVTable object

	void compress();

	void saveAscii();
	void saveBinary(char* filename);

private: // pre-processing methods and data members
	void updateOVTable();
	void cleanupOFF();

	HEmesh hem;
	vector<int> dummyVertices;

private: // edgebreaker methods
	void initCompression(int c, MeshType eMeshType);
	void Compress(int c); 
	void CheckHandle(int c);

	void EncodeNoPrediction(int c);
	void EncodeWithPrediction(int c);
	void EncodeDelta(int c);
	
private: // data members
	char sOVTable[MAX_SIZE];
	MeshType eMeshType;
	FileFormat eFileFormat;
	int nStartCorner;

	// Variables for storing Input OVTable and geometry
	int* O;				// Input Opposite table
	int* V;				// Input Vertex indices table
	Vertex*  G;			// Input Geometry table
	Vertex*  G_est;		// Input Geometry table

	// Compression variables
	int T;				// triangles count
	int N;				// vertices count
	int *M;				// Vetex marking array
	int *U;				// Triangles marking array

private: // output stream data...
	int it1_count;				// # tris incident on first vtx
	vector<char> clers;			// CLERS labels
	vector<int> handles;		// info on handles.
	vector<Vertex> vertices;	// vertex coordinates
	vector<int> newDummy;		// indices of artificially inserted vertices
};

#endif // _EDGE_BREAKER_COMPRESS_H_
