//----------------------------------------------------------------------------------------------------------------------
// Rev Terminal
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On January 15th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <string>
#include <iostream>

// Rev headers
#include <revScript/variant/variant.h>
#include <revScript/scriptVM.h>

using std::string;
using std::cin;
using rev::script::Variant;

int main (int //_argc
	, const char** //_argv
	)
{
	rev::script::ScriptVM vm;
	Variant dst;
	
	while(1)
	{
		string code;
		cin >> code;
		vm.execute(code.c_str());
		vm.getVar("a", dst);

		if(dst.type() == Variant::Type::integer)
			std::cout << dst.asInteger() << std::endl;
		if(dst.type() == Variant::Type::real)
			std::cout << dst.asReal() << std::endl;
		cin >> code;
	}
	return 0;
}