#include "transmog.h"
#include <iostream>

int main(int argc, char** argv){
	std::cout << __FUNCSIG__ << std::endl;
	int i = tmg_foo(3,2);
	return 0;
}
