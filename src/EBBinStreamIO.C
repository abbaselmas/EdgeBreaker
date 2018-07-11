#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <EBBinStreamIO.h>
#include <algorithm>

int EBBinStreamIO::openFileWrite(char* filename)
{
	int fd = open(filename, O_CREAT | O_WRONLY,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	return fd;
}

int EBBinStreamIO::openFileRead(char* filename)
{
	int fd = open(filename, O_RDONLY, 0);
	return fd;
}

void EBBinStreamIO::close(int fd)
{
	if (fd != -1) ::close(fd);
}


// functions for writing to the stream...

void EBBinStreamIO::writeShortArray(int fd, short* buffer, int count)
{
	int retval = write(fd, buffer, count*sizeof(short));
	assert(retval == count*sizeof(short));
}

void EBBinStreamIO::writeIntArray(int fd, int* buffer, int count)
{
	int retval = write(fd, buffer, count*sizeof(int));
	assert(retval == count*sizeof(int));
}

void EBBinStreamIO::writeFloatArray(int fd, float* buffer, int count)
{
	int retval = write(fd, buffer, count*sizeof(float));
	assert(retval == count*sizeof(float));
}

void EBBinStreamIO::writeDoubleArray(int fd, double* buffer, int count)
{
	int retval = write(fd, buffer, count*sizeof(double));
	assert(retval == count*sizeof(double));
}

void EBBinStreamIO::writeCharArray(int fd, char* buffer, int count)
{
	int retval = write(fd, buffer, count*sizeof(char));
	assert(retval == count*sizeof(char));
}

void EBBinStreamIO::writeUCharArray(int fd, unsigned char* buffer, int count)
{
	int retval = write(fd, buffer, count*sizeof(char));
	assert(retval == count*sizeof(char));
}

void EBBinStreamIO::writeString(int fd, char* buffer)
{
	int len = strlen(buffer);
	int retval = write(fd, buffer, len*sizeof(char));
	assert(retval == len*sizeof(char));
}

void EBBinStreamIO::writeString(int fd, string& s)
{
	short len = (short) s.size();
	char* str = (char*) malloc((len+1)*sizeof(char));
	copy(s.begin(), s.end(), str);
	str[len] = '\0';
	int retval = write(fd, &len, sizeof(short));
	assert(retval == sizeof(short));
	retval = write(fd, str, len*sizeof(char));
	assert(retval == len*sizeof(char));
	free(str);
}

// functions for reading from the stream...

void EBBinStreamIO::readShortArray(int fd, short* buffer, int count)
{
	int retval = read(fd, buffer, count*sizeof(short));
	assert(retval == count*sizeof(short));
}

void EBBinStreamIO::readIntArray(int fd, int* buffer, int count)
{
	int retval = read(fd, buffer, count*sizeof(int));
	assert(retval == count*sizeof(int));
}

void EBBinStreamIO::readFloatArray(int fd, float* buffer, int count)
{
	int retval = read(fd, buffer, count*sizeof(float));
	assert(retval == count*sizeof(float));
}

void EBBinStreamIO::readDoubleArray(int fd, double* buffer, int count)
{
	int retval = read(fd, buffer, count*sizeof(double));
	assert(retval == count*sizeof(double));
}

void EBBinStreamIO::readCharArray(int fd, char* buffer, int count)
{
	int retval = read(fd, buffer, count*sizeof(char));
	assert(retval == count*sizeof(char));
}

void EBBinStreamIO::readUCharArray(int fd, unsigned char* buffer, int count)
{
	int retval = read(fd, buffer, count*sizeof(char));
	assert(retval == count*sizeof(char));
}

void EBBinStreamIO::readString(int fd, char** buffer)
{
	short len;
	int retval = read(fd, &len, sizeof(short));
	assert(retval == sizeof(short));
	*buffer = (char*) new char[len+1];
	retval = read(fd, *buffer, len*sizeof(char));
	assert(retval == len*sizeof(char));
	(*buffer)[len] = '\0';
}

void EBBinStreamIO::readString(int fd, string& s)
{
	short len;
	int retval = read(fd, &len, sizeof(short));
	assert(retval == sizeof(short));
	char* buffer = (char*) new char[len+1];
	retval = read(fd, buffer, len*sizeof(char));
	assert(retval == len*sizeof(char));
	buffer[len] = '\0';
	
	copy(&(buffer[0]), &(buffer[len]), s.begin());

	delete[] buffer;
}

// high-level vector I/O

void EBBinStreamIO::writeFloatVectorArray(int fd, vector<Vector>& vectors)
{
	int len = vectors.size();
	float* buffer = (float*) new float[3*len];
	for (int i = 0; i < len; i++) {
		buffer[3*i]     = vectors[i].x;
		buffer[3*i + 1] = vectors[i].y;
		buffer[3*i + 2] = vectors[i].z;
	}
	int retval = write(fd, buffer, 3*len*sizeof(float));
	assert(retval == 3*len*sizeof(float));

	delete[] buffer;
}

void EBBinStreamIO::readFloatVectorArray(int fd,
	vector<Vector>& vectors, int count)
{
	float* buffer = (float*) new float[3*count];
	int retval = read(fd, buffer, 3*count*sizeof(float));
	assert(retval == 3*count*sizeof(float));

	for (int i = 0; i < count; i++) {
		Vector v;
		v.x = buffer[3*i];
		v.y = buffer[3*i + 1];
		v.z = buffer[3*i + 2];

		vectors.push_back(v);
	}

	delete[] buffer;
}