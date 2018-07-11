
CXX = g++
CXXFLAGS = -g -I../inc

all : eblib ebc ebdc off2ov ov2off patcholes

SRC = EBUtils.C EBCLoadFiles.C EBCompression.C EdgebreakerCompress.C \
	EBCSaveFiles.C EdgebreakerDecompress.C EBDecompression.C \
	EBDCLoadFiles.C EBDCSaveFiles.C HEPolMesh.C EBBinStreamIO.C \
	EBLabelStream.C

OBJ = $(SRC:.C=.o)

ebc : $(OBJ) ebc.C
	@rm -f ebc
	$(CXX) $(CXXFLAGS) $(OBJ) ebc.C -o ebc

ebdc : $(OBJ) ebdc.C
	@rm -f ebdc
	$(CXX) $(CXXFLAGS) $(OBJ) ebdc.C -o ebdc

off2ov : $(OBJ) off2ov.C
	@rm -f off2ov
	$(CXX) $(CXXFLAGS) $(OBJ) off2ov.C -o off2ov

ov2off : $(OBJ) ov2off.C
	@rm -f ov2off
	$(CXX) $(CXXFLAGS) $(OBJ) ov2off.C -o ov2off

patcholes : $(OBJ) patcholes.C
	@rm -f patcholes
	$(CXX) $(CXXFLAGS) $(OBJ) patcholes.C -o patcholes

eblib: $(OBJ)
	ar rvs libeb.a $(OBJ)
	mv libeb.a ../lib

clean:
	rm -f $(OBJ) ebc ebdc off2ov ov2off patcholes ../lib/libeb.a *~ ../inc/*~
