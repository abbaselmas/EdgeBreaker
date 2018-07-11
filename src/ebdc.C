#include <assert.h>
#include <stdlib.h>
#include <EdgebreakerDecompress.h>
#include <string.h>
#include <iostream>
using namespace std;

void usaged()
{
	fprintf(stderr, "Usage: ebdc {input.beb | ascii files dir} output.<off | ov>");
	exit(1);
}

int main(int argc, char** argv)
{
	EdgebreakerDecompress ebdc;

	if (argc == 3) {
		string st = argv[1];
		if (st.substr(st.find_last_of(".") + 1) == "beb")
			ebdc.loadBinary(argv[1]);
		else
			ebdc.loadAscii(argv[1]);

		ebdc.uncompress();
		st = argv[2];
		if (st.substr(st.find_last_of(".") + 1) == "off") {
			ebdc.saveOFF(argv[2]);
		}
		else if (st.substr(st.find_last_of(".") + 1) == "ov") {
			ebdc.saveOVTable(argv[2]);
		}
	}
	else
		usaged();

	return 0;
}