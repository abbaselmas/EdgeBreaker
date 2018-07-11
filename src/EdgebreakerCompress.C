
// EdgeBreakerCompress -- Wrapper for EBCompress and pre-processing

#include <assert.h>
#include <EdgebreakerCompress.h>

EdgebreakerCompress::EdgebreakerCompress()
{
	// initialize the data member of EB
	eMeshType = MANIFOLD;
	eFileFormat = ASKII;
	nStartCorner = 0;
	O = NULL;
	V = NULL;
	G = NULL;
	G_est = NULL;
	T = 0;
	N = 0;
	M = NULL;
	U = NULL;
}

EdgebreakerCompress::~EdgebreakerCompress()
{
	// disallocate memory
	if (O != NULL)
		delete[] O;
	if (V != NULL)
		delete[] V;
	if (G != NULL)
		delete[] G;
	if (U != NULL)
		delete[] U;
	if (M != NULL)
		delete[] M;
}

void EdgebreakerCompress::compress()
{
	// Compress Mesh
	initCompression(nStartCorner, eMeshType);
}

// ----------------- EdgeBreakerCompress.C
