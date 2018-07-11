
// EBdatatypes - data types used by Alla Safonova for EB

#ifndef _EB_DATA_TYPES_H_
#define _EB_DATA_TYPES_H_

struct EBTriangle {
	int v[3];
};

struct Coord3D
{
    float x;
    float y;
    float z;

    Coord3D() {
    		x = y = z = 0.0;
    	}

    Coord3D(float xx, float yy, float zz) {
    		x = xx; y = yy; z = zz;
    	}
};

typedef Coord3D Vertex;
typedef Coord3D Vector;

#define MAX_SIZE 256

enum MeshType {MANIFOLD, TPATCH};
enum FileFormat {BINARY, ASKII};

#endif // _EB_DATA_TYPES_H_
