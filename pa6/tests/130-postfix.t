int main()
{
	2+int('a');
	2+typename Cfoo::bar (123);
	2+int{2,3,4};
	2+typename ::Nfoo::bar("baz");
	dynamic_cast<C*>(q);
	static_cast<C*>(q);
	reinterpret_cast<C*>(q);
	const_cast<C*>(q);
	typeid(2+3);
	typeid(int);
}

