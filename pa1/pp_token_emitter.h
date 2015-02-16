#include <string>
#include <vector>

class pp_token_emitter {
public:
	/*
	 * Usually enum types should be capital letters. But EOF is a reserved C
	 * identifier/macro for the actual EOF character. Also, lower cases
	 * makes it easier to compare to the C++ standard specification.
	 */
	enum token_type {
		whitespace_sequence = 0,
		new_line,
		header_name,
		identifier,
		pp_number,
		character_literal,
		user_defined_character_literal,
		string_literal,
		user_defined_string_literal,
		preprocessing_op_or_punc,
		non_whitespace_char,
		eof,
		num_token
	};

	/*
	 * @return
	 * 0 = token successful emitted
	 * other = failure of some kind
	 */
	int emit(const token_type type, const std::string &data);

private:
	const static std::vector<std::string> _token_type_string_vector;
};
