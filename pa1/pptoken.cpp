#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <unistd.h>
#include "pp_tokenizer.h"

int main(int argc, char const* argv[])
{
	pp_tokenizer g(stdin);
	while(g.is_eof())
		g.issue_token(stdout);
	return 0;
}
