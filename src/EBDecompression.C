#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <EBdatatypes.h>
#include <EBUtils.h>
#include <EdgebreakerDecompress.h>

using namespace std;

/***************************** EB Decompression **************************/
/*
* Decompress the mesh
*/
void EdgebreakerDecompress::initDecompression()
{
	int i = 0;

	//Initialization of O and V tables is done in InitDecompression function
	//aftet we read clers from the file and findout how many tryangles are in 
	//the mesh.
	
	//id of the last triangle decompressed so far
	T = 0;								
	//id of the last vertex encountered
	N = 2;							
	//id of the last handle encountered
	A = 0;
	
	//Init Memory and some variables
	InitDecompressionModule();

	//start connectivity decompression
	DecompressConnectivity(2);						

	//Initialization of tables for marking visited vertices and triangles
	//Done in InitDecompression function

	//estimate 1st  vertex
	G[0] = DecodeDelta(0);	
	//if we do not have a hole mark 1st  vertex as visited
	if (eMeshType == MANIFOLD) M[0] = 1;

	//estimate third vertex and mark it as visited
	G[1] = DecodeDelta(2);	
	//G[1] = DecodeDelta(1);	
	M[1] = 1;

	//estimate second vertex and mark it as visited
	G[2] = DecodeDelta(1);	
	//G[2] = DecodeDelta(2);	
	M[2] = 1;

	//id of the last vertex encountered
	N = 2;
	//paint the triangle and go to opposite corner
	U[0] = 1; 
	
	//start vertices decompression
	DecompressVertices(O[2]);
}

void EdgebreakerDecompress::InitDecompressionModule()
{
	int nNumOfTriangles = C.size() + 1;
	int nNumOfVertices = G_in.size();

	//Allocate Memory for Tables
	V = new int[3*nNumOfTriangles];		//table of vertex Ids for each corner
	O = new int[3*nNumOfTriangles];		//table of opposite corner Ids for each corner or free edge orientation

	//Init memory
	for (int i = 0; i<3*nNumOfTriangles; i++) {O[i] = -3; V[i] = 0;}

	//Initialize the first triangle 
		V[0] = 0; V[1] = 2; V[2] = 1;
//	V[0] = 0; V[1] = 1; V[2] = 2;
	O[0] = -1; O[1] = -1;

	//Allocate memory for M and U tables
	U = new int[nNumOfTriangles];
	M = new int[nNumOfVertices];

	//init tables for marking visited vertices and triangles
	for (int i = 0; i < nNumOfTriangles; i++) U[i] = 0;
		for (int i = 0; i < nNumOfVertices; i++) M[i] = 0;

	//Allocate memory for geometry array
			G = new Vertex[nNumOfVertices];
	}


	void EdgebreakerDecompress::DecompressConnectivity(int c) 
	{
	//Loop builds triangle tree and zips it up
		do {
		//new triangle
			T++;							

		//attach new triangle, link opposite corners
			O[c] = 3*T;	O[3*T] = c;			

		//enter vertex Ids for shared vertices
			V[3*T+1] = V[PrevEdge(c)];		
			V[3*T+2] = V[NextEdge(c)];

		//move corner to new triangle
			c = NextEdge(O[c]);	

		//select operation based on next symbol
			switch (C[T-1])	{
				case 'C':						
			//C: left edge is free, store ref to new vertex			
				O[NextEdge(c)] = -1;
				V[3*T] = ++N;
				break;
				case 'L':						
			//L: orient free edge
				O[NextEdge(c)] = -2;
			//check for handles, if non, try to zip
				if (!CheckHandle(NextEdge(c)))
					Zip(NextEdge(c));
				break;
				case 'R':						
			//R: orient free edge, check for handles, go left 
				O[c] = -2;
				CheckHandle(c);
				c = NextEdge(c);
				break;
				case'S':						
			//O[NextEdge(c)] = -10;
			//S: recursion going right, then go left
				DecompressConnectivity(c);
				c = NextEdge(c);
			//if the triangle to the left was visited, then return	
				if (O[c] >= 0)
					return;
				break;
				case 'E':						
			//E: left and right edges are  free
				O[c] = -2;
				O[NextEdge(c)] = -2;
			//check for handles on the right
				CheckHandle(c);
			//check for handles on the left, if non, try to zip
				if (!CheckHandle(NextEdge(c)))
					Zip(NextEdge(c));			
			//pop 	
				return;
				break;
			}
		} while (true);
	}


	bool EdgebreakerDecompress::CheckHandle(int c)
	{
	//check if this is a handle
	//if (A < H.size() && c == H[A+1])
		if (A >= H.size() || c != H[A+1])
			return false;
		else {
		//link opposite corners
			O[c] = H[A];
			O[H[A]] = c;

		//find corner of next free edge if any 
			int a = PrevEdge(c);
			while ( (O[a] >=0) && (a != H[A]) )
				a = PrevEdge(O[a]);

		//zip if found cw edge
			if (O[a] == -2)
				Zip(a);

		//find corner of next free edge if any
			a = PrevEdge(O[c]);
			while ( (O[a] >=0) && (a != c) )
				a = PrevEdge(O[a]);

		//zip if found cw edge
			if (O[a] == -2)
				Zip(a);

		//next handle
			A += 2;
			return true;
		}
	}

	void EdgebreakerDecompress::Zip(int c)
	{
	//tries to zip free edges opposite c
		int b = NextEdge(c);	

	//search clockwise for free edge
		while (O[b] >= 0 && O[b] != c) {
			b = NextEdge(O[b]);
		}

	//pop if no zip possible
		if (O[b] != -1) {
			return;
		}

  	//link opposite corners
		O[c] = b; O[b] = c;

	//assign co-incident corners
		int a = NextEdge(c);
		V[NextEdge(a)]=V[NextEdge(b)];
		while (O[a]>=0 && a!=b) {
			a = NextEdge(O[a]);
			V[NextEdge(a)]=V[NextEdge(b)];
		}

	//find corner of next free edge on right
		c = PrevEdge(c);
		while (O[c] >=0 && c!= b) {	
			c = PrevEdge(O[c]);
		}

	//try to zip again
		if (O[c] == -2)
			Zip(c);
	}

	void EdgebreakerDecompress::DecompressVertices(int c)
	{
	//start traversal for triangle tree
		do {
		//mark the triangle as visited
			U[E2T(c)] = 1; 

		//test whether tip vertex was visited
			if (M[V[c]] == 0) {	
			//update new vertex
				N++;
				G[N] = DecodeDelta(c);	
			//mark tip vertex as visited
				M[V[c]] = 1;
			//continue with the right neighbor
				c = RightTri(c, O);
			}										
			else 
		//test whether right triangle was visited
				if (U[E2T(RightTri(c, O))] == 1) {
			//test whether left triangle was visited
					if (U[E2T(LeftTri(c, O))] == 1)
					{
				//E, pop
						return; 
					}									
					else 
					{
				//R,move to left triangle
						c = LeftTri(c, O);
					}
				}
				else 
		//test whether left triangle was visited
					if (U[E2T(LeftTri(c, O))] == 1) {
			//L, move to right triangle
						c = RightTri(c, O);
					}	
					else {
			//S, recursive call to visit right branch first
						DecompressVertices(RightTri(c, O));
			//move to left triangle
						c = LeftTri(c, O);
			//if the triangle to the left was visited, then return
						if (U[E2T(c)]>0)
							return;
					} 
				} while (true);
			}


/***************************** Estimate functions ************************/
/*
* This function assume vertices was not encoded using any prediction 
* It just reads the next vertex 
*/
			Vertex EdgebreakerDecompress::DecodeNoPrediction(int c)
			{
				return G_in[V[c]];
			}

			Vertex EdgebreakerDecompress::DecodeWithPrediction(int c)
			{
				static int EBVcount = 0;

				Vertex vPred;
				Vector delta = G_in[EBVcount];
				EBVcount++;

				if (M[V[O[c]]] > 0 && M[V[PrevEdge(c)]] > 0) {
					vPred = VPlus(G[V[NextEdge(c)]], G[V[PrevEdge(c)]]);
					vPred = VMinus(vPred, G[V[O[c]]]);
					vPred = VPlus(delta, vPred);
					return vPred;
				}

				if (M[V[O[c]]] > 0) {
					vPred = VMult(G[V[NextEdge(c)]], 2);
					vPred = VMinus(vPred, G[V[O[c]]]);
					vPred = VPlus(delta, vPred);
					return vPred;
				}

				if (M[V[NextEdge(c)]] > 0 && M[V[PrevEdge(c)]] > 0) {
					vPred = VPlus(G[V[NextEdge(c)]], G[V[PrevEdge(c)]]);
					vPred = VMult(vPred, 0.5f);
					vPred = VPlus(delta, vPred);
					return vPred;
				}

				if (M[V[NextEdge(c)]] > 0) {
					vPred = G[V[NextEdge(c)]];
					vPred = VPlus(delta, vPred);
					return vPred;
				}
				else if (M[V[PrevEdge(c)]] > 0) {
					vPred = G[V[PrevEdge(c)]];
					vPred = VPlus(delta, vPred);
					return vPred;
				}
				else {
					vPred = delta;
					return vPred;
				}
			}

			Vertex EdgebreakerDecompress::DecodeDelta(int c)
			{
				return DecodeNoPrediction(c);
	//return DecodeWithPrediction(c);	
			}