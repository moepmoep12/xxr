#include <iostream>
#include "state.h"

int main()
{
	State<> a("#11##0#");
	State<> b("#11#00#");

	std::cout << (a.contains(b)) << std::endl;

	getchar();

	return 0;
}