#include <assert.h>
#include <EdgebreakerCompress.h>
#include <EBUtils.h>
#include <stdio.h>
#include <EBLabelStream.h>
#include <EBBinStreamIO.h>
#include <algorithm>

void EdgebreakerCompress::saveAscii()
{
	FILE* fclers = NULL;	 		// clers file
	FILE* fvertices = NULL;			// vertices
	FILE* fhandles = NULL;			// handles pairs
	FILE* fdummy = NULL;			// dummy vertices (removed during decoding)

	fclers = fopen("clers.txt", "w+t");
	if (fclers == NULL)
		PrintErrorAndQuit((char*)"Can not open clers file\n");

	fvertices = fopen("vertices.txt", "w+t");
	if (fvertices == NULL)
		PrintErrorAndQuit((char*)"Can not open vertices file\n");

	fhandles = fopen("handles.txt", "w+t");
	if (fhandles == NULL)
		PrintErrorAndQuit((char*)"Can not open handles file\n");

	fdummy = fopen("dummy.txt", "w+t");
	if (fhandles == NULL)
		PrintErrorAndQuit((char*)"Can not open handles file\n");

	//write mesh type to clers file
	if (eMeshType == MANIFOLD)
	{
		if (eFileFormat == ASKII)
			fprintf(fclers, "%s\n", "MANIFOLD");
	}
	else if (eMeshType == TPATCH)
	{
		if (eFileFormat == ASKII)
			fprintf(fclers, "%s\n", "TPATCH");
	}
	else
		PrintErrorAndQuit((char*)"Not supported mesh type\n");

	if (eFileFormat == ASKII)
		fprintf(fclers, "%d\n", it1_count);
	int clers_count = clers.size();
	for (int i = 0; i < clers_count; i++) {
		fprintf(fclers, "%c\n", clers[i]);
	}

	int handles_count = handles.size() / 2;
	for (int i = 0; i < handles_count; i++) {
		fprintf(fhandles, "%d %d\n", handles[2*i], handles[2*i+1]);
	}

	int vtx_count = vertices.size();
	for (int i = 0; i < vtx_count; i++) {
		fprintf(fvertices, "%f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
	}

	int dummy_count = newDummy.size();
	for (int i = 0; i < dummy_count; i++) {
		fprintf(fdummy, "%d\n", newDummy[i]);
	}

	fclose(fclers);
	fclose(fvertices);
	fclose(fhandles);
	fclose(fdummy);
}


void EdgebreakerCompress::saveBinary(char* filename)
{
	assert(filename);
	EBBinStreamIO bsio;
	int fd = bsio.openFileWrite(filename);

	// MANIFOLD - 00, TPATCH - 01, INTERIOR_TPATCH - 10, HOLES - 11
	unsigned char type = (unsigned char) 0;
	bsio.writeUCharArray(fd, &type, 1);

	bsio.writeIntArray(fd, &it1_count, 1);	// # tris incident on first vtx

	int numclers = clers.size();			// CLERS labels bin stream...
	bio_buffer buffer;
	EBLabelStream ebls;
	ebls.encodeStream(clers, buffer);
	int buff_size = buffer._buffer.size();
	bsio.writeIntArray(fd, &numclers, 1);
	bsio.writeIntArray(fd, &buff_size, 1);
	unsigned char *ucbuff = (unsigned char*) new unsigned char[buff_size];
	copy(buffer._buffer.begin(), buffer._buffer.end(), ucbuff);
	bsio.writeUCharArray(fd, ucbuff, buff_size);
	delete[] ucbuff;

	int numhandles = handles.size() / 2;		// handles
	bsio.writeIntArray(fd, &numhandles, 1);
	if (numhandles > 0) {
		int* hands = (int*) new int[2*numhandles];
		copy(handles.begin(), handles.end(), hands);
		bsio.writeIntArray(fd, hands, 2*numhandles);
		delete[] hands;
	}

	int numvert = vertices.size();				// vertex coords
	bsio.writeIntArray(fd, &numvert, 1);
	float* coords = (float*) new float[3*numvert];
	for (int i = 0; i < numvert; i++) {
		coords[3*i]     = vertices[i].x;
		coords[3*i + 1] = vertices[i].y;
		coords[3*i + 2] = vertices[i].z;
	}
	bsio.writeFloatArray(fd, coords, 3*numvert);
	delete[] coords;

	int numdummy = newDummy.size();				// dummy vertices
	bsio.writeIntArray(fd, &numdummy, 1);
	if (numdummy > 0) {
		int* dum = (int*) new int[numdummy];
		copy(newDummy.begin(), newDummy.end(), dum);
		bsio.writeIntArray(fd, dum, numdummy);
		delete[] dum;
	}

	bsio.close(fd);
}