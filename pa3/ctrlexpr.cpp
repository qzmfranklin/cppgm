// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <string>

using namespace std;

// mock implementation of IsDefinedIdentifier for PA3
// return true iff first code point is odd
bool PA3Mock_IsDefinedIdentifier(const string& identifier)
{
	if (identifier.empty())
		return false;
	else
		return identifier[0] % 2;
}

int main()
{
	// TODO: Implement ctrlexpr as per PA3 assignment description
}

