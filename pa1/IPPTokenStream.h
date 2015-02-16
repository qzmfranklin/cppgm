#pragma once

struct IPPTokenStream
{
	virtual void emit_whitespace_sequence() = 0;
	virtual void emit_new_line() = 0;
	virtual void emit_header_name(const string& data) = 0;
	virtual void emit_identifier(const string& data) = 0;
	virtual void emit_pp_number(const string& data) = 0;
	virtual void emit_character_literal(const string& data) = 0;
	virtual void emit_user_defined_character_literal(const string& data) = 0;
	virtual void emit_string_literal(const string& data) = 0;
	virtual void emit_user_defined_string_literal(const string& data) = 0;
	virtual void emit_preprocessing_op_or_punc(const string& data) = 0;
	virtual void emit_non_whitespace_char(const string& data) = 0;
	virtual void emit_eof() = 0;

	virtual ~IPPTokenStream() {}
};
