#pragma once
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "Application.h"
#include <tinyxml2.h>

#include <deque>
#include <tuple>

#define reg			1
#define value		2
					
					   /*cmd                    arg1, type1                  arg2, type2*/
typedef std::tuple<std::string, std::pair<std::string, int>, std::pair<std::string, int>> c_line;
typedef std::pair<std::string, int> argument;

class Model
{
public:
	Model(Application *parent = 0);
	~Model();

	void parse_code(const char *xml_name);

	void select_line(unsigned addr, OgreBites::TextBox *tb);

	void exec_line(unsigned addr);
	void exec_line() { exec_line(eip); }

	void set_register(const char*, unsigned);

	void push(const argument &arg);
	void pop(const argument &arg);
	void mov(const argument &arg1, const argument &arg2);
	void jmp(const argument &arg);
	void ret();

private:
	bool esp_invalid = false;
	bool stack_overflow = false;
	unsigned esp = 0, ebp = 0, eip = 0, eax = 0;

	std::map<int, c_line>	cmd_list;
	std::map<int, int>		stack;
	std::map<std::string, unsigned> labels;

	Application *app;

	std::pair<std::string, int>* args_parse(int arg_amount, tinyxml2::XMLElement *xml_element, bool & err_code);

	void		insert_stackline(unsigned val, unsigned addr);
	std::string make_stackline(unsigned val, unsigned addr);
	std::string make_strline(const c_line &l, unsigned addr);
	std::string num_to_hexstr(unsigned num);

	unsigned &get_reg(std::string reg_s);
	unsigned &unpack_arg(const std::pair<std::string, int> &arg);
};

