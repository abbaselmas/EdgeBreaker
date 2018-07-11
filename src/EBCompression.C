
// ------------------------------------------------------------------
// EBCompression - Modified version of Edgebreaker code supplied by
// 				Prof. Rossignac and his team (see acknowledgement
// 				at the bottom of this file). 
// 	
// 	Modified and Packaged by : Dinesh Shikhare (dinesh@ncst.ernet.in)
// ------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <EBUtils.h>
#include <EdgebreakerCompress.h>


/***************************** EB Compression ****************************/
/*
*	Arguments:
*		c - start compression from corner c
*		MeshType: 2 Mesh types are currently supported - MANIFOLD and TPATCH
*					MANIFOLD - is a manifold mesh, consistently oriented with no holes. 		
*					TPATCH - is a manifold mesh with boundary, consistently oriented.
*		FileFormat: BINARY or ASKII (See File Formats for detais)
*
*/

void EdgebreakerCompress::initCompression(int c, MeshType eMeshType)
{
	int i = 0;

	//init tables for marking visited vertices and triangles
	//was done in ProcessInputFile function

	//id of the last triangle compressed so far
	T = 0;

	c = PrevEdge(c);	

	//estimate 1st  vertex
	EncodeDelta(NextEdge(c));
	
	//if we do not have a hole mark 1st  vertex as visited
	//in which case estimate function can use it for estimation
	//if we do have a hole, we do not mark 1st  vertex as visited
	//and it is not used for estimation since it is a dummy vertex
	if (eMeshType==MANIFOLD) M[V[NextEdge(c)]] = 1;


	//estimate third vertex and mark it as visited
	EncodeDelta(c);
	M[V[c]] = 1;

	//estimate second vertex and mark it as visited
	EncodeDelta(PrevEdge(c));
	M[V[PrevEdge(c)]] = 1;

	//paint the triangle 
	U[E2T(c)] = 1; // mark the triangle as visited

	//traverse triangles incident on the first vertex
	//we do not want to store clers symbols for them
	int a = O[c];

	//we keep a count of number of triangles incident on the first corner
	int count = 1;
	//first traverse 'C' triangles 
	while (a != PrevEdge(O[PrevEdge(c)]))
	{
		//increment count for number of triangles incident on the first corner
		count++;

		//paint the triangle, increment # of triangles 
		U[E2T(a)] = 1;
		T++;

		//estimate next vertex and mark it as visited
		EncodeDelta(a);
		M[V[a]] = 1;
		
		//continue with the right neighbor 
		a = O[NextEdge(a)];
	}

	//traverse 'R' triangle incident on first vertex 
	U[E2T(a)] = 1;
	T++;
	count++;
	
	//write number of triangles incident on first vertex to clers file
	it1_count = (int) count;
	
	//start connectivity compression
	Compress(O[PrevEdge(a)]);
}



void EdgebreakerCompress::Compress(int c)
{
	//start traversal for triangle tree
	do 
	{
		//mark the triangle as visited
		U[E2T(c)] = 1; 
		T++;
		
		//check for handles
		CheckHandle(c);

		//test whether tip vertex was visited
		if (M[V[c]] == 0)	
		{	
			//append encoding of C to clers
			// fprintf(fclers, "%c\n", 'C');
			clers.push_back('C');
			
			//estimate next vertex and mark it as visited
			EncodeDelta(c);
			M[V[c]] = 1;

			//continue with the right neighbor
			c = RightTri(c, O);
		}										
		else 
		//test whether right triangle was visited
			if (U[E2T(RightTri(c, O))] > 0)
			{
			//test whether left triangle was visited
				if (U[E2T(LeftTri(c, O))] > 0)
				{
				//append code for E and pop
				// fprintf(fclers, "%c\n", 'E');
					clers.push_back('E');
					return; 
				}		
				else 
				{
				//append code for R, move to left triangle
				// fprintf(fclers, "%c\n", 'R');
					clers.push_back('R');
					c = LeftTri(c, O);
				}
			}
			else 
		//test whether left triangle was visited
				if (U[E2T(LeftTri(c, O))] > 0)
				{
			//append code for L, move to right triangle
			// fprintf(fclers, "%c\n", 'L');
					clers.push_back('L');
					c = RightTri(c, O);
				}	
				else 
				{
			//store corner number in decompression, to support handles
					U[E2T(c)] = T*3+2;

			//append code for S
			// fprintf(fclers, "%c\n", 'S');
					clers.push_back('S');

			//recursive call to visit right branch first
					Compress(RightTri(c, O));

			//move to left triangle
					c = LeftTri(c, O);

			//if the triangle to the left was visited, then  return
					if (U[E2T(c)]>0)
						return;
				} 
			} while(true);

		}


		void EdgebreakerCompress::CheckHandle(int c)
		{
	//check for handles from the right
			if (U[E2T(O[NextEdge(c)])] >1)
			{
		//write opposite corners for handle triangles into file
		// fprintf(fhandles, "%d %d\n", U[E2T(O[NextEdge(c)])], T*3+1);
				handles.push_back(U[E2T(O[NextEdge(c)])]);
				handles.push_back(T*3+1);
			}

	//check for handles from the left
			if (U[E2T(O[PrevEdge(c)])] >1)
			{
		//write opposite corners for handle triangles into file
		// fprintf(fhandles, "%d %d\n", U[E2T(O[PrevEdge(c)])], T*3+2);
				handles.push_back(U[E2T(O[PrevEdge(c)])]);
				handles.push_back(T*3+2);
			}
		}


/***************************** Estimate functions ************************/
/*
* This function does not do any prediction, it just writes vertices into array
*/
		void EdgebreakerCompress::EncodeNoPrediction(int c)
		{
	//Store vertex coordinates into file
	/*
	fprintf(fvertices, "%f %f %f\n", G[V[c]].x,
									 G[V[c]].y,	
									 G[V[c]].z);
	*/
			vertices.push_back(G[V[c]]);
		}


		void EdgebreakerCompress::EncodeWithPrediction(int c)
		{
			Vector vPred, delta;
			Vertex zeroV = Vertex(0.0, 0.0, 0.0);

			if (M[V[O[c]]] > 0 && M[V[PrevEdge(c)]] > 0) 
			{
				vPred = VPlus(G_est[V[NextEdge(c)]], G_est[V[PrevEdge(c)]]);
				vPred = VMinus(vPred, G_est[V[O[c]]]);
				delta = VMinus(G[V[c]], vPred);
		//return vPred;
			}
			else if (M[V[O[c]]] > 0) 
			{
				vPred = VMult(G_est[V[NextEdge(c)]], 2);
				vPred = VMinus(vPred, G_est[V[O[c]]]);
				delta = VMinus(G[V[c]], vPred);
		//return vPred;
			}
			else if (M[V[NextEdge(c)]] > 0 && M[V[PrevEdge(c)]] > 0) 
			{
				vPred = VPlus(G_est[V[NextEdge(c)]], G_est[V[PrevEdge(c)]]);
				vPred = VMult(vPred, 0.5f);
				delta = VMinus(G[V[c]], vPred);
		//return vPred;
			}
			else if (M[V[NextEdge(c)]] > 0) 
			{
				vPred = G_est[V[NextEdge(c)]];
				delta = VMinus(G[V[c]], vPred);
		//return vPred;
			}
			else if (M[V[PrevEdge(c)]] > 0) 
			{
				vPred = G_est[V[PrevEdge(c)]];
				delta = VMinus(G[V[c]], vPred);
		//return vPred;
			}
			else
			{
				vPred = zeroV;
				delta = VMinus(G[V[c]], vPred);
			}

			G_est[V[c]] = VPlus(delta, vPred);

	/*
	fprintf(fvertices, "%f %f %f\n", delta.x,
									 delta.y,	
									 delta.z);	
	*/
			vertices.push_back(delta);
		}


		void EdgebreakerCompress::EncodeDelta(int c)
		{
			if ((dummyVertices.size() > 0) && (V[c] >= dummyVertices[0]))
				newDummy.push_back(vertices.size());

			EncodeNoPrediction(c);
	//EncodeWithPrediction(c);

		}


/**************************************************************************
*
* Any use of this code or of the ideas it embodies should be acknowledged
* in writing in all publications describing the resulting product or
* research contribution as follows: "This solution is based on the
* Edgebreaker compression technique and software developed by Prof.
* Rossignac and his colleagues at Georgia Institute of Technology."    
*
*
* Software developed by: Alla Safonova at Georgia Tech
* Last modified: 05/11/2001
*
**************************************************************************/

