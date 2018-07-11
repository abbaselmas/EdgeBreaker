
// HEPolMesh - Half-edge data structure for polygonal mesh

#include <HEPolMesh.h>
#include <algorithm>

HEmesh::~HEmesh()
{
	vertexlist.erase(vertexlist.begin(), vertexlist.end());
	edgelist.erase(edgelist.begin(), edgelist.end());
	polylist.erase(polylist.begin(), polylist.end());
	clearTable();
}

void HEmesh::addVertex(Vertex v)
{
	vertexlist.push_back(HEvertex(v));
	vector<int> x;
	_tmptable.push_back(x);
}

void HEmesh::addVertex(float x, float y, float z)
{
	vertexlist.push_back(HEvertex(x, y, z));
	vector<int> xx;
	_tmptable.push_back(xx);
}

void HEmesh::addPolygon(int n, int* indices)
{
	HEpolygon p;
	int prev = -1;

	for (int i = 0; i < n; i++) {
		HEedge e;
		e.v = indices[(i+1)%n];

		_tmptable[indices[i]].push_back(edgelist.size());
		if (vertexlist[indices[i]].edge == -1) {
			vertexlist[indices[i]].edge = edgelist.size();
		}
		int inv = -1;
		int k = indices[(i+1)%n];
		for (int j = 0; j < _tmptable[k].size(); j++) {
			int ce = _tmptable[k][j];
			if (edgelist[ce].v == indices[i]) {
				inv = ce;
				break;
			}
		}
		if (inv != -1) {
			edgelist[inv].invHE = edgelist.size();
			e.invHE = inv;
		}

		if (p.firstHE == -1) {
			p.firstHE = edgelist.size();
		}
		else {
			edgelist[prev].nextHE = edgelist.size();
		}

		e.polygon = polylist.size();
		prev = edgelist.size();
		edgelist.push_back(e);
	}

	edgelist[prev].nextHE = p.firstHE;
	polylist.push_back(p);
}

void HEmesh::size(int& numpts, int& numpol)
{
	numpts = vertexlist.size();
	numpol = polylist.size();
}

int HEmesh::vertexNeighbours(int vtx_no, vector<int>& neighvtx)
{
	neighvtx.erase(neighvtx.begin(), neighvtx.end());

	int incidentHE = vertexlist[vtx_no].edge;
	neighvtx.push_back(edgelist[incidentHE].v);
	int currentHE = edgelist[edgelist[incidentHE].invHE].nextHE;

	while ((currentHE >= 0) && (currentHE != incidentHE)) {
		neighvtx.push_back(edgelist[currentHE].v);
		currentHE = edgelist[edgelist[currentHE].invHE].nextHE;
	}

	if (currentHE == -1)
		return -1;
	else
		return neighvtx.size();
}

void HEmesh::adjacentPolygons(int pol_no, vector<int>& neighpol)
{
	neighpol.erase(neighpol.begin(), neighpol.end());

	int firstedge = polylist[pol_no].firstHE;
	neighpol.push_back(edgelist[edgelist[firstedge].invHE].polygon);
	int currentedge = edgelist[firstedge].nextHE;

	while ((currentedge >= 0) && (currentedge != firstedge)) {
		neighpol.push_back(edgelist[edgelist[currentedge].invHE].polygon);
		currentedge = edgelist[currentedge].nextHE;
	}
}

void HEmesh::getVertices(int pol_no, vector<int>& verts)
{
	verts.erase(verts.begin(), verts.end());

	int firstedge = polylist[pol_no].firstHE;
	verts.push_back(edgelist[firstedge].v);
	int currentedge = edgelist[firstedge].nextHE;

	while ((currentedge >= 0) && (currentedge != firstedge)) {
		verts.push_back(edgelist[currentedge].v);
		currentedge = edgelist[currentedge].nextHE;
	}

	rotate(verts.begin(), verts.begin() + (verts.size() - 1), verts.end());
}

// makes sense only for tri-mesh
int HEmesh::oppositeVertices(int tri_no, int& o1, int& o2, int& o3)
{
	int edge = polylist[tri_no].firstHE;
	o1 = edgelist[edgelist[edgelist[edgelist[edge].nextHE].invHE].nextHE].v;

	edge = edgelist[edge].nextHE;
	o2 = edgelist[edgelist[edgelist[edgelist[edge].nextHE].invHE].nextHE].v;

	edge = edgelist[edge].nextHE;
	o3 = edgelist[edgelist[edgelist[edgelist[edge].nextHE].invHE].nextHE].v;

	return edge;
}

// makes sense only for tri-mesh
void HEmesh::oppositeTrisVerts(int tri_no, int* t, int* v)
{
	int edge = polylist[tri_no].firstHE;
	int invedge = edgelist[edge].invHE;
	t[2] = edgelist[invedge].polygon;
	vector<int> vtx;
	getVertices(edgelist[invedge].polygon, vtx);
	if (vtx[0] == edgelist[invedge].v)
		v[2] = 1;
	else if (vtx[1] == edgelist[invedge].v)
		v[2] = 2;
	else
		v[2] = 0;

	edge = edgelist[edge].nextHE;
	invedge = edgelist[edge].invHE;
	t[0] = edgelist[invedge].polygon;
	getVertices(edgelist[invedge].polygon, vtx);
	if (vtx[0] == edgelist[invedge].v)
		v[0] = 1;
	else if (vtx[1] == edgelist[invedge].v)
		v[0] = 2;
	else
		v[0] = 0;

	edge = edgelist[edge].nextHE;
	invedge = edgelist[edge].invHE;
	t[1] = edgelist[invedge].polygon;
	getVertices(edgelist[invedge].polygon, vtx);
	if (vtx[0] == edgelist[invedge].v)
		v[1] = 1;
	else if (vtx[1] == edgelist[invedge].v)
		v[1] = 2;
	else
		v[1] = 0;
}

int HEmesh::getBoundaries(vector<int>& bVertices, vector<int>& loopExtents)
{
	int edgecount = edgelist.size();
	vector<bool> visited(edgecount, false);
	for (int i = 0; i < edgecount; i++) {
		if ((visited[i] == false) && (edgelist[i].invHE == -1))
			_growBoundary(i, visited, bVertices, loopExtents);
		visited[i] = true;
	}

	return (loopExtents.size() / 2);
}

void HEmesh::_growBoundary(int startedge, vector<bool>& visited,
	vector<int>& bVertices, vector<int>& loopExtents)
{
	loopExtents.push_back(bVertices.size());	// start of loop

	int currentHE = startedge;
	do {
		int e = currentHE;
		bVertices.push_back(edgelist[currentHE].v);
		while (true) {
			e = edgelist[e].nextHE;
			visited[e] = true;
			if (edgelist[e].invHE == -1) {
				currentHE = e;
				break;
			}
			e = edgelist[e].invHE;
			visited[e] = true;
		}
	} while (currentHE != startedge);

	loopExtents.push_back(bVertices.size());	// end of loop
}

int HEmesh::triangulateLoops(vector<int>& bVertices, vector<int>& loopExtents,
	vector<int>& dummyVtxIndices)
{
	int dummyTriCount = 0;
	int numloops = loopExtents.size() / 2;
	for (int i = 0; i < numloops; i++) {
		int from = loopExtents[2*i];
		int to   = loopExtents[2*i + 1];

		// compute mean position of the loop ...
		Vertex mean;
		mean.x = mean.y = mean.z = 0.0;
		for (int j = from; j < to; j++) {
			Vertex vtx = vertexlist[bVertices[j]].pt;
			mean.x += vtx.x;
			mean.y += vtx.y;
			mean.z += vtx.z;
		}
		mean.x /= (to - from);
		mean.y /= (to - from);
		mean.z /= (to - from);

		// insert dummy vertex...
		int dummyIndex = vertexlist.size();
		dummyVtxIndices.push_back(dummyIndex);
		addVertex(mean);

		// insert dummy triangles...
		for (int j = 0; j < (to-from); j++) {
			int v[3];
			v[0] = dummyIndex;
			v[1] = bVertices[to - (j+1)];
			int i2 = to - (j+2);
			if (i2 < from) i2 = to - 1;
			v[2] = bVertices[i2];
			addPolygon(3, v);
			dummyTriCount++;
		}
	}

	return dummyTriCount;
}


// house keeping...
void HEmesh::clearTable()
{
	int numpts = vertexlist.size();
	for (int i = 0; i < numpts; i++) {
		_tmptable[i].erase(_tmptable[i].begin(), _tmptable[i].end());
	}
	_tmptable.erase(_tmptable.begin(), _tmptable.end());
}