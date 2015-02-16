#include "pp_token_emitter.h"

/*
 * The order in which strings are initialized must be exactly the same as in he
 * token_type enum.
 */
const std::vector<std::string> pp_token_emitter::_token_type_string_vector = {
	"whitespace_sequence",
	"new_line",
	"header_name",
	"identifier",
	"pp_number",
	"character_literal",
	"user_defined_character_literal",
	"string_literal",
	"user_defined_string_literal",
	"preprocessing_op_or_punc",
	"non_whitespace_char",
	"eof",
	"num_token"
};

int emit(const token_type type, const std::string &data)
{
	if (type == new_line)
		printf("new_line 0\n");
	else if (type == eof)
		printf("eof\n");
	else
		printf("%s %zu %s\n", _token_type_string_vector[type].c_str(),
				data.size(), data.c_str());
}
