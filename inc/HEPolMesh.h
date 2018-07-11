
// HEPolMesh - Half-edge data structure for polygonal mesh

#ifndef _HALF_EDGE_POL_MESH_H_
#define _HALF_EDGE_POL_MESH_H_

#include <vector>
#include <EBdatatypes.h>
using std::vector;
class HEedge;
class HEpolygon;


struct HEvertex {
	HEvertex(float x, float y, float z) {
			pt.x = x; pt.y = y; pt.z = z;
			edge = -1;
		}

	HEvertex(Vertex p) {
			pt = p;
			edge = -1;
		}

	Vertex pt;
	int edge;			// reference to some HE starting from this vertex o-->
};


class HEedge {
public:
	HEedge() { v = nextHE = invHE = polygon = -1; }

public:
	int v;						// vertex to which this HE is incident -->o
	int nextHE;					// next HE of current polygon
	int invHE;					// oppositely oriented edge of adjacent polygon
	int polygon;				// reference to the polygon
};


class HEpolygon {
public:
	HEpolygon() { firstHE = -1; }
public:
	int firstHE;	// first HE of the polygon
};


class HEmesh {
public:
	HEmesh() {}
	~HEmesh();

	void addVertex(Vertex v);
	void addVertex(float x, float y, float z);
	void addPolygon(int n, int* indices);

	// Queries...
	void size(int& numpts, int& numpol);
	int vertexNeighbours(int vtx_no, vector<int>& neighvtx);
	void adjacentPolygons(int pol_no, vector<int>& neighpol);
	void getVertices(int pol_no, vector<int>& verts);
	int oppositeVertices(int tri_no, int& o1, int& o2, int& o3);	// makes sense only for tri-mesh
	void oppositeTrisVerts(int tri_no, int* t, int* v);
	int getBoundaries(vector<int>& bVertices, vector<int>& loopExtents);

	int triangulateLoops(vector<int>& bVertices, vector<int>& loopExtents,
		vector<int>& dummyVtxIndices);

	// house keeping
	void clearTable();

	// for debug...
	void dumpall();

public: // data
	vector<HEvertex>  vertexlist;
	vector<HEedge>    edgelist;
	vector<HEpolygon> polylist;

private:
	vector< vector<int> > _tmptable;

	void _growBoundary(int edg, vector<bool>& visited,
		vector<int>& bVertices, vector<int>& loopExtents);
};

#endif // _HALF_EDGE_POL_MESH_H_
