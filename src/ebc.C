#include <assert.h>
#include <stdlib.h>
#include <EdgebreakerCompress.h>
#include <string.h>
#include <iostream>
using namespace std;

void usage()
{
	fprintf(stderr, "Usage: ebc input.{ov | off} {A | B}");
	exit(1);
}

int main(int argc, char** argv)
{
	EdgebreakerCompress ebc;

	if (argc == 3) {
		string st = argv[1];
		if(st.substr(st.find_last_of(".") + 1) == "off")
			ebc.loadOFF(argv[1]);
		else if (st.substr(st.find_last_of(".") + 1) == "ov")
			ebc.loadOVTable(argv[1]);
		else {
			usage();
			return 0;
		}
		ebc.compress();

		if (!strcmp(argv[2], "B"))
			ebc.saveBinary((char *)"output.beb");
		else if (!strcmp(argv[2], "A"))
			ebc.saveAscii();
		else
			usage();
		return 0;
	}
	else
		usage();

	return 0;
}