#include "basic_template.h"
#include <iostream>
#include <string>
int main(){
	Chars z0="ABS";
	std::cout<<"z0:"<<z0<<"\n";
	Chars z1;
	std::cout<<"z1:"<<z1<<"\n";
	std::string a="Okey doky";
	Chars z2=a;
	std::cout<<"z2:"<<z2<<"\n";
	Chars z3=std::move(z0);
	std::cout<<"z3:"<<z3<<"\n";
	std::cout<<"z0:"<<z0<<"\n";

}
