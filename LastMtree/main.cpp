#include <iostream>
#include "routerObj.h"
#include "Entry.h"

int main()
{
	std::vector<double> f = { 1,2,3,4,5,6 };
	std::string path = "angels.mp3";
	_object* nuevo;

	routerObj root(f);
	nuevo = &root;
	Entry leaf(f,path);
	nuevo = &leaf;

	//delete root;
	//delete nuevo;

	return 0;
	
}