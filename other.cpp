#include "./basic_template.cpp"
#include <iostream>

int main(){
	Chars c=Chars("It works");
	auto print_c=[&c](){std::cout<<"C:"<<c.c_str()<<"\n";};
	c.insert('s', 2);
	print_c();
	c.erase(0);
	print_c();
	Chars c1=Chars(c);
	auto print_c1=[&c1](){std::cout<<"C1:"<<c1.c_str()<<"\n";};
	c.erase(0);
	print_c1();
	c.insert('b', 4);
	print_c1();
}	
