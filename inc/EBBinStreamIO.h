
// EBBinStreamIO -- binary format I/O stream

#ifndef _EB_BIN_STREAM_IO_H_
#define _EB_BIN_STREAM_IO_H_

#include <EBdatatypes.h>
#include <string>
#include <vector>
#include <assert.h>
using std::string;
using std::vector;

class EBBinStreamIO {

public:
	int openFileWrite(char* filename);
	int openFileRead(char* filename);

	void close(int fd);

	// Basic read/write functions...
	void writeShortArray (int fd, short* buffer,  int count);
	void writeIntArray   (int fd, int* buffer,    int count);
	void writeFloatArray (int fd, float* buffer,  int count);
	void writeDoubleArray(int fd, double* buffer, int count);
	void writeCharArray  (int fd, char* buffer,   int count);
	void writeUCharArray (int fd, unsigned char* buffer, int count);
	void writeString     (int fd, char* buffer);
	void writeString     (int fd, string& buffer);

	void readShortArray (int fd, short* buffer,  int count);
	void readIntArray   (int fd, int* buffer,    int count);
	void readFloatArray (int fd, float* buffer,  int count);
	void readDoubleArray(int fd, double* buffer, int count);
	void readCharArray  (int fd, char* buffer,   int count);
	void readUCharArray (int fd, unsigned char* buffer, int count);
	void readString     (int fd, char** buffer);
	void readString     (int fd, string& buffer);

	// Higher-level Vector I/O...
	void writeFloatVectorArray  (int fd, vector<Vector>& vectors);
	void readFloatVectorArray  (int fd, vector<Vector>& vectors, int count);
};

#endif // _EB_BIN_STREAM_IO_H_
