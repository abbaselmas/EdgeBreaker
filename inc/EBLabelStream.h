
// EBLabelStream - conversion of ascii coded CLERS stream to binary and vice versa

#ifndef _EB_LABEL_STREAM_H_
#define _EB_LABEL_STREAM_H_

#include <vector>
#include <map>
#include <algorithm>
using std::map;
using std::vector;

struct bio_buffer {
	int _bytptr;
	int _bitptr;
	int _symb_count;
	vector<unsigned char> _buffer;

	bio_buffer() {
			_bytptr = _bitptr = 0;
			_symb_count = 0;
		}
};

struct symbtab_entry {
	int _size;
	unsigned char _code;

	symbtab_entry() {
			_size = 0;
			_code = (unsigned char) 255;
		}

	symbtab_entry(int size, unsigned char code) {
			_size = size;
			_code = code;
		}
};


class EBLabelStream {

public:
	EBLabelStream();
	~EBLabelStream();

	void encodeStream(vector<char>& ascii, bio_buffer& binary);
	void decodeStream(bio_buffer& binary, vector<char>& ascii);

private:
	void initSymbTable(); 		// C=0, L=110, R=101, S=100, E=111.
	void addSymb(char symb, int size, unsigned char code);

	void setNextBit(bio_buffer& buffer, int val);
	int getNextBit(bio_buffer& buffer);

private:
	map<char, symbtab_entry> symb_table;

};

#endif // _EB_LABEL_STREAM_H_

