template<typename Y>
class C
{	
	auto x = [&](const C1& c1, const C2& c2) -> C3 { return c1+c2; };
};

