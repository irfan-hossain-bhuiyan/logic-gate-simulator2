#include <memory>
class Base{
	public:
	virtual ~Base(){}
};
template<typename T>
class Derived:public Base{
	public:
		T value;
		Derived(T val):value(val){}
};
int main(){
	Base* b=new Derived(10);
	Base* c=new Derived("It's ok");
	Derived<int>* a=dynamic_cast<Derived<int>*>(b);
}
