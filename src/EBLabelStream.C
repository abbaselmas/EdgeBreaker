
// EBLabelStream - conversion of ascii coded CLERS stream to binary and vice versa

#include <EBLabelStream.h>
#include <iostream>
#include <math.h>

EBLabelStream::EBLabelStream()
{
	initSymbTable();
}

EBLabelStream::~EBLabelStream()
{
	// nothing here yet.
}

void EBLabelStream::encodeStream(vector<char>& ascii, bio_buffer& binary)
{
	int stream_size = ascii.size();
	int bit_count = 0;
	for (int i = 0; i < stream_size; i++) {
		symbtab_entry e = symb_table[ascii[i]];
		bit_count += e._size;
	}

	int buffer_size = (int) ceil(1.0 * bit_count / 8);
	binary._buffer = vector<unsigned char>(buffer_size, (unsigned char)0);
	binary._symb_count = stream_size;

	for (int i = 0; i < stream_size; i++) {
		symbtab_entry e = symb_table[ascii[i]];
		int size = e._size;
		unsigned char c = e._code;
		unsigned char one = 1;
		for (int j = 0; j < size; j++) {
			unsigned char mask = one << (size-(j+1));
			if (c & mask)
				setNextBit(binary, 1);
			else
				setNextBit(binary, 0);
		}
	}
}

// This code needs generalization.
// For now it can only handle CLERS codes of Edgebreaker
// 		C=0, L=110, R=101, S=100, E=111.
void EBLabelStream::decodeStream(bio_buffer& binary, vector<char>& ascii)
{
	binary._bytptr = 0;
	binary._bitptr = 0;
	int symb_count = binary._symb_count;

	for (int i = 0; i < symb_count; i++) {
		int b1 = getNextBit(binary);
		if (b1 == 0)
			ascii.push_back('C');
		else {
			int b2 = getNextBit(binary);
			int b3 = getNextBit(binary);
			if ((b2 == 0) && (b3 == 0))
				ascii.push_back('S');
			else if ((b2 == 0) && (b3 == 1))
				ascii.push_back('R');
			else if ((b2 == 1) && (b3 == 0))
				ascii.push_back('L');
			else if ((b2 == 1) && (b3 == 1))
				ascii.push_back('E');
		}
	}
}


// C=0, L=110, R=101, S=100, E=111.
void EBLabelStream::initSymbTable()
{
	addSymb('C', 1, (unsigned char) 0);
	addSymb('L', 3, (unsigned char) 6);
	addSymb('R', 3, (unsigned char) 5);
	addSymb('S', 3, (unsigned char) 4);
	addSymb('E', 3, (unsigned char) 7);
}

void EBLabelStream::addSymb(char symb, int size, unsigned char code)
{
	symb_table[symb] = symbtab_entry(size, code);
}

void EBLabelStream::setNextBit(bio_buffer& buffer, int val)
{
	int byte = buffer._bytptr;
	int bit  = buffer._bitptr;
	unsigned char c = buffer._buffer[byte];
	if (val) {
		c |= (1 << (7-bit));
		buffer._buffer[byte] = c;
	}
	
	buffer._bitptr = (bit + 1) % 8;
	if (buffer._bitptr == 0)
		buffer._bytptr += 1;
}

int EBLabelStream::getNextBit(bio_buffer& buffer)
{
	int byte = buffer._bytptr;
	int bit  = buffer._bitptr;
	unsigned char c = buffer._buffer[byte];
	int val = (c & (1 << (7-bit))) ? 1 : 0;

	buffer._bitptr = (bit + 1) % 8;
	if (buffer._bitptr == 0)
		buffer._bytptr += 1;

	return val;
}

// -------------------- EBLabelStream.C

