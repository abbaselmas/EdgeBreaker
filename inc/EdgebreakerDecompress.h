
// EdgebreakerDecompress -- Wrapper for EBDecompress and post-processing

#ifndef _EDGE_BREAKER_DECOMPRESS_H_
#define _EDGE_BREAKER_DECOMPRESS_H_

#include <vector>
#include <cstdio>
#include <EBdatatypes.h>
#include <HEPolMesh.h>

class EdgebreakerDecompress {

public:
	EdgebreakerDecompress();
	~EdgebreakerDecompress();

	void loadAscii(char* directory);
	void loadBinary(char* filename);

	void uncompress();

	void saveOFF(char* filename);
	void saveOVTable(char* filename);

private: // edgebreaker decompressor methods

	void InitDecompressionModule();
	void initDecompression();
	void DecompressConnectivity(int c);
	bool CheckHandle(int c);
	void Zip(int c);
	void DecompressVertices(int c);
	Vertex DecodeDelta(int c);
	Vertex DecodeNoPrediction(int c);
	Vertex DecodeWithPrediction(int c);

	void ReadMeshType(FILE* fclers);
	void ReadNumTriOnFirstVertex(FILE* fclers);
	void ReadClersAndUppendClersForFirstVertexTri(FILE* fclers);

	void cleanupDummy();

private: // edgebreaker data members...
	MeshType eMeshType;
	FileFormat eFileFormat;
	int nNumOfTriangles;
	int nNumOfVertices;
	int* O;					//Output Opposite table
	int* V;					//Output Vertex indices table
	Vertex* G;				//Output Geometry table

	int T;					// triangles count
	int N;					// vertices count
	int I;					// number of triangles on first vertex
	int A;					// handles count
	int *M;					// Vetex marking array
	int *U;					// Triangles marking array

	vector<int> H;			// Handle corners array
	vector<char> C;			// Clers array
	vector<Vertex> G_in;	// Geometry array
	vector<int> dummy;		// dummy vertices

	HEmesh hem;
};

#endif // _EDGE_BREAKER_DECOMPRESS_H_

