#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

class pp_tokenizer {
public:
	pp_tokenizer(FILE *fin);

	/*
	 * Issue the token to the given file
	 *
	 * @return
	 *  0 = success
	 *  1 = no token available
	 * -1 = passed eof 
	 *
	 * @param fout
	 * file descriptor to receive the output
	 */
	int issue_token(FILE *fout);

	/*
	 * @return
	 * true = the last issued token is EOF
	 * TODO: The function name is very misleading. Change it.
	 */
	bool is_eof() const;

private:
	int _type;
	std::string _data;

	class Stream {
	public:
		Stream(FILE *fp);
		int fetch();
		void push(const int ch);
	private:
		FILE *fp;
		int _buf[BUFSIZ];
	} _stream;

	enum {
		start = 0,

		whitespace_sequence,
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

		end,
		NUM_STATES
	};
	uint64_t _state = 0;

	bool _is_ready_to_issue() const;
	uint32_t _get_bit_mask_for(const int state) const;

	static std::vector<uint64_t> _state_bit_mask_list;
	const static std::vector<std::string> _token_type_to_string_map;
	const static std::unordered_set<char> _basic_source_character_set;

	const static std::unordered_map<std::string, std::string> _alternative_token_map;
	const static std::unordered_map<char, char> _trigraph_sequence_map;

	const static std::unordered_set<char> _h_char_set;
	const static std::unordered_set<char> _q_char_set;
	const static std::unordered_set<char> _nondigit_set;
	const static std::unordered_set<char> _digit_set;
	const static std::unordered_set<std::string> _keyword_set;
};
