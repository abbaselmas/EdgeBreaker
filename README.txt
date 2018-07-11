
==================================================
EDGEBREAKER
3D Compression Algorithm Packaged...

(http://www.ncst.ernet.in/~dinesh/research/compression/eb/)
Current Update: July 4, 2001.
First Released: June 26, 2001.
==================================================

Author: Dinesh Shikhare (dinesh@ncst.ernet.in)

Contents:
---------
1. Introduction
      1.1. What's new in this update?
2. Download the latest update
3. Implementation Notes
4. How to compile and run?
5. Report bugs, suggestions, questions, etc.
6. Copyleft!
7. Disclaimer!!!



1. Introduction:
----------------
Prof. Jarek Rossignac and his team have released a nice implementation
of Edgebreaker triangle mesh compression algorithm. It is available for
download on his homepage.

The code posted on Prof. Rossignac's homepage can handle compression of
meshes representing manifolds or manifolds with a single boundary. Also,
the original compression code reads in uncompressed mesh only from
OVTable format and the uncompression code writes the mesh back in OVTable
format. I found this very limiting for my work. Hence I extended their
code by developing pre-processing and post-processing filters to make
it more useful for my experiments.

I believe others too can utilize these extensions for their work, and
hence I'm releasing my version of Edgebreaker which wraps original code
released by Prof. Rossignac's team. In case you find this extension
useful, feel free to use it in your programs and modify it as per your
needs. Also, give me some feedback.

1.1. What's new in this update?
-------------------------------
In this update, I have introduced a binary file format for encoding
the CLERS labels, handles, indices of dummy vertices and coordinates
of vertices.


2. Download the latest update:
------------------------------
http://www.ncst.ernet.in/~dinesh/research/compression/eb/

My code has been developed on Linux platform using g++ compiler. I have
not yet tried to compile it on Windows. Will do so when I find time
(volunteers are welcome :-).

Future plans include extending this code to handle more general situations
like non-manifold geometry, handling mesh attributes like colour,
texture and a more refined API for the use of Edgebreaker and other
compression schemes. Also, on the anvil is a lossy compression format
that will quantize the prediction errors of coordinate values, colour
and other attributes.

The original code and documentation from Prof. Rossignac are posted at
http://www.gvu.gatech.edu/~jarek/edgebreaker/eb/


3. Implementation Notes:
------------------------

The extended code does the following tasks:

         Compression Module:
         ~~~~~~~~~~~~~~~~~~~

    * Read the mesh in OFF file format (efficient conversion from
      OFF format to OVTable data-structure implemented using half-edge
      data-structure). 
    * Identify holes in a given mesh. 
    * Plug each hole by inserting a dummy vertex at a mean position
      of the hole and dummy triangles incident on the dummy vertex and
      vertices on the hole. The resultant mesh is a manifold.
    * Convert the data to OVTable data-structure.
    * Invoke Prof. Rossignac's Edgebreaker code for compression of
      the mesh. The list of dummy vertices is then passed on to
      Edgebreaker. This code reorders the list of vertices in the mesh
      and the dummy vertices are tracked during the reordering.
    * The output of original code consists of 3 streams: CLERS labels,
      information about the HANDLES and the list of VERTICES. I output
      an additional stream containing indices to the dummy vertices in
      DUMMY stream.
                     OR
      Output the compressed file in a binary file format. (NEW!!)


         Decompression Module:
         ~~~~~~~~~~~~~~~~~~~~~

    * Read in the compressed data streams: CLERS, HANDLES, VERTICES
      and DUMMY.
                     OR
      Read in the binary compressed file format constaining CLERS, HANDLES,
      VERTICES and DUMMY data. (NEW!!)
    * Invoke Prof. Rossignac's Edgebreaker decompressor code to obtain
      the uncompressed version of the manifold object in OVTable
      data-structure.
    * Eliminate the dummy vertices and triangles incident onto the
      dummy vertices.
    * Output the uncompressed mesh in OFF or OVTable format.

Miscellaneous:

    * off2ov: a filter to translate OFF file representing a manifold
      surface to its OVTable representation. (This filter also patches
      holes, if any, in the mesh).
    * ov2off: a filter translate OVTable file into OFF file.
    * patcholes: a filter to accept a manifold with boundaries and
      return a manifold with holes patched with triangles. Triangulation
      is done by simply inserting a vertex at the mean position of each
      hole and constructing triangles around it.
      (Input: OFF file, Output: OFF or OVTable).


4. How to compile and run?
--------------------------
My code has been developed on Linux platform using g++ compiler. I have
not yet tried to compile it on Windows. Will do so when I find time
(volunteers are welcome :-).

(a) Compile:

    Go to src directory and say make. This should build edgebreaker
    library in the lib directory and some executables in src directory.
    The executable programs it builds are:
    (i) ebc - edgebreaker compression
    (ii) ebdc - edgebreaker decompression
    (iii) off2ov - filter to translate OFF files to OVTable format
    (iv) ov2off - filter to translate OVTable files to OFF format
    (v) patcholes - program to patch holes in a triangle mesh

(b) Run:

    The programs ebc and ebdc will output usage instructions on
    running them without arguments. The filters off2ov, ov2off and
    patcholes take input on stdin and send output on stdout.


5. Report bugs, suggestions, questions, etc.
--------------------------------------------
Please write to me if you discover some bugs, or think of some
extensions and would like to share them with me. I shall try to
incorporate them in the next release.

Write to dinesh@ncst.ernet.in to give your feedback.


6. Copyleft!
------------
If you use this code in any of your work, kindly acknowledge the efforts
of Prof. Rossignac's team as:
"The geometry compression part of this solution/product was derived from,
 or inspired by, the Edgebreaker compression technique and software developed
 by Prof. Rossignac and his colleagues in the College of Computing at the
 Georgia Institute of Technology."

And acknowledge my efforts for developing the pre-and post-processors
and packaging Prof. Rossignac's original code.


7. DISCLAIMER!!!
----------------
This code is being released straight out of my current experimental
development work. I don't claim the code to be complete in all respects
and also the scanty documentation is never going to be complete.
Use this code at your own risk!

