#include <assert.h>
#include <stdio.h>
#include <EdgebreakerDecompress.h>
#include <EBUtils.h>
#include <EBBinStreamIO.h>
#include <EBLabelStream.h>
#include <algorithm>
#include <string.h>

static void myScanf(FILE* fFile, char* format, void* var);

void EdgebreakerDecompress::loadBinary(char* filename)
{
	assert(filename);
	EBBinStreamIO bsio;
	int fd = bsio.openFileRead(filename);

	// object type...
	unsigned char meshtype;
	bsio.readUCharArray(fd, &meshtype, 1);
	if (meshtype == (unsigned char) 0)
		eMeshType = MANIFOLD;
	else if (meshtype == (unsigned char) 1)
		eMeshType = TPATCH;

	// # tris incident on first vtx
	bsio.readIntArray(fd, &I, 1);

	// CLERS labels array
	int num_clers, buff_size;
	bsio.readIntArray(fd, &num_clers, 1);
	bsio.readIntArray(fd, &buff_size, 1);
	unsigned char* buff = (unsigned char*) new unsigned char[buff_size];
	bsio.readUCharArray(fd, buff, buff_size);
	bio_buffer binary;
	for (int i = 0; i < buff_size; i++) {
		binary._buffer.push_back(buff[i]);
	}
	binary._symb_count = num_clers;
	delete[] buff;
	EBLabelStream ebls;

	//Write (I-2) Cs and 1R to the beginning of the array
	for (int i = 0; i < (I-2); i++)
		C.push_back('C');
	C.push_back('R');

	ebls.decodeStream(binary, C);

	// Handles
	bsio.readIntArray(fd, &A, 1);
	if (A > 0) {
		int* hds = (int*) new int[2*A];
		bsio.readIntArray(fd, hds, 2*A);
		for (int i = 0; i < 2*A; i++) {
			H.push_back(hds[i]);
		}
		delete[] hds;
	}

	// Vertex coordinates
	int numvert;
	bsio.readIntArray(fd, &numvert, 1);
	float* vbuff = (float*) new float[3*numvert];
	bsio.readFloatArray(fd, vbuff, 3*numvert);
	for (int i = 0; i < numvert; i++) {
		Vertex v;
		v.x = vbuff[3*i];
		v.y = vbuff[3*i + 1];
		v.z = vbuff[3*i + 2];
		G_in.push_back(v);
	}
	delete[] vbuff;

	// Dummy vertex ids
	int numdummy;
	bsio.readIntArray(fd, &numdummy, 1);
	if (numdummy > 0) {
		int* dbuff = (int*) new int[numdummy];
		bsio.readIntArray(fd, dbuff, numdummy);
		for (int i = 0; i < numdummy; i++) {
			dummy.push_back(dbuff[i]);
		}
		delete[] dbuff;
	}

	bsio.close(fd);
}

void EdgebreakerDecompress::loadAscii(char* directory)
{
	// Open ASCII encoded EB files...
	FILE* fclers = NULL;
	FILE* fvertices = NULL;
	FILE* fhandles = NULL;
	FILE* fdummy = NULL;

	char sTempStr[MAX_SIZE];

	strcpy(sTempStr, directory);
	strcat(sTempStr, "//");
	strcat(sTempStr, "clers.txt");

	fclers = fopen(sTempStr, "rt");
	if (fclers == NULL)
		PrintErrorAndQuit((char*)"Can not open clers file\n");

	strcpy(sTempStr, directory);
	strcat(sTempStr, "//");
	strcat(sTempStr, "vertices.txt");

	fvertices = fopen(sTempStr, "rt");
	if (fvertices == NULL)
		PrintErrorAndQuit((char*)"Can not open vertices file\n");

	strcpy(sTempStr, directory);
	strcat(sTempStr, "//");
	strcat(sTempStr, "handles.txt");

	fhandles = fopen(sTempStr, "rt");
	if (fhandles == NULL)
		PrintErrorAndQuit((char*)"Can not open handles file\n");

	strcpy(sTempStr, directory);
	strcat(sTempStr, "//");
	strcat(sTempStr, "dummy.txt");

	fdummy = fopen(sTempStr, "rt");
	if (fhandles == NULL)
		PrintErrorAndQuit((char*)"Can not open handles file\n");

	//Read input vertices
	if (eFileFormat == ASKII) {
		Vertex vTemp;
		myScanf(fvertices, (char*)"%f", (void*)&vTemp.x);
		myScanf(fvertices, (char*)"%f", (void*)&vTemp.y);
		myScanf(fvertices, (char*)"%f", (void*)&vTemp.z);
		do {
			G_in.push_back(vTemp);
			myScanf(fvertices, (char*)"%f", (void*)&vTemp.x);
			myScanf(fvertices, (char*)"%f", (void*)&vTemp.y);
			myScanf(fvertices, (char*)"%f", (void*)&vTemp.z);
		} while (!feof(fvertices));
	}

	//Read handle corners
	if (eFileFormat == ASKII) {
		int nTemp;
		myScanf(fhandles, (char*)"%d", (void*)&nTemp);
		do {
			H.push_back(nTemp);
			myScanf(fhandles, (char*)"%d", (void*)&nTemp);
		} while (!feof(fhandles));
	}

	// Read MeshType, degree of 1st vertex and CLERS stream
	ReadMeshType(fclers);
	ReadNumTriOnFirstVertex(fclers);
	ReadClersAndUppendClersForFirstVertexTri(fclers);

	// Read indices of the dummy vertices...
	if (eFileFormat == ASKII) {
		int d;
		myScanf(fdummy, (char*)"%d", (void*)(&d));
		while (!feof(fdummy)) {
			dummy.push_back(d);
			myScanf(fdummy, (char*)"%d", (void*)(&d));
		}
	}

	// Close files
	if (fclers != NULL)
		fclose(fclers);
	if (fvertices != NULL)
		fclose(fvertices);
	if (fhandles != NULL)
		fclose(fhandles);
	if (fdummy != NULL)
		fclose(fdummy);
}

/*
* Read Mesh Type
*/
void EdgebreakerDecompress::ReadMeshType(FILE* fclers)
{
	char sMeshType[MAX_SIZE];
	if (eFileFormat == ASKII) {
		//read mesh type
		myScanf(fclers, (char*)"%s", (void*)sMeshType);

		//remember mesh type
		if (strcmp("MANIFOLD", sMeshType)==0)
		{
			eMeshType = MANIFOLD;
			printf("MeshType - MANIFOLD\n");
		}
		else if (strcmp("TPATCH", sMeshType)==0)
		{
			eMeshType = TPATCH;
			printf("MeshType - TPATCH\n");
		}
		else
			PrintErrorAndQuit((char*)"Not supported mesh type\n");
	}
}

/*
* Read numbet of triangles incident on first vertex
*/
void EdgebreakerDecompress::ReadNumTriOnFirstVertex(FILE* fclers)
{
	if (eFileFormat == ASKII) {
		//read numbet of triangles 
		myScanf(fclers, (char*)"%d", (void*)&I);
	}
	
}

/*
* Read clers into C array,
* Append (I-2) Cs and 1R to the beginning of the array
*/
void EdgebreakerDecompress::ReadClersAndUppendClersForFirstVertexTri(FILE* fclers)
{
	if (eFileFormat == ASKII)
	{
		//Write (I-2) Cs and 1R to the beginning of the array

		//First write I-2 C's
		for (int i = 0; i < (I-2); i++) {
			C.push_back('C');
		}

		//now write one R
		C.push_back('R');

		//now read clers from input file
		char cTemp; 
		//read new line character
		myScanf(fclers, (char*)"%c", (void*)&cTemp);
		//read clers symbol
		myScanf(fclers, (char*)"%c", (void*)&cTemp);

		do {
			C.push_back(cTemp);

			//read new line character
			myScanf(fclers, (char*)"%c", (void*)&cTemp);

			//read clers symbol
			myScanf(fclers, (char*)"%c", (void*)&cTemp);

		} while (!feof(fclers));
	}
}


void myScanf(FILE* fFile, char* format, void* var)
{
	fscanf(fFile,  format, var);
//	if (fscanf(fFile,
//			   format, 
//			  var) == -1)
//		PrintErrorAndQuit("Error reading file\n");

}