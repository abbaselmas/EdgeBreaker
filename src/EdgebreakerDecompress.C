
// EdgebreakerDecompress -- Wrapper for EBDecompress and post-processing

#include <EdgebreakerDecompress.h>

EdgebreakerDecompress::EdgebreakerDecompress()
{
	eMeshType = MANIFOLD;
	eFileFormat = ASKII;
	O = NULL;				//Output Opposite table
	V = NULL;				//Output Vertex indices table
	G = NULL;				//Output Geometry table

	T = 0;				//triangles count
	N = 0;				//vertices count
	I = 0;				//number of triangles on first vertex
	A = 0;				//handles count
	M = NULL;			//Vetex marking array
	U = NULL;			//Triangles marking array
}

EdgebreakerDecompress::~EdgebreakerDecompress()
{
	// deallocate memory
	if (O != NULL)
		delete [] O;
	if (V != NULL)
		delete [] V;
	if (G != NULL)
		delete [] G;
	if (U != NULL)
		delete [] U;
	if (M != NULL)
		delete [] M;
}

void EdgebreakerDecompress::uncompress()
{
	// Decompress Mesh
	initDecompression();

	cleanupDummy();
}

// ----------------- EdgebreakerDecompress.C
