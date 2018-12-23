#include "Model.h"

#include <string>
#include <sstream>

using namespace tinyxml2;

Model::Model(Application *parent) : app(parent)
{
}
Model::~Model()
{
}

std::string Model::make_stackline(unsigned val, unsigned addr)
{
	std::stringstream ss;
	ss << "[" << num_to_hexstr(addr) << "] " << num_to_hexstr(val);
	return ss.str();
}
std::string Model::make_strline(const c_line & l, unsigned addr)
{
	std::string cmd;
	std::pair<std::string, int> arg1, arg2;

	std::tie(cmd, arg1, arg2) = l;

	std::stringstream line_ss;

	line_ss << "0x" << std::setw(8) << std::setfill('0') << std::hex << addr;
	line_ss << " " << cmd;

	if (cmd == "mov")
	{
		line_ss << " " << arg1.first;
		line_ss << ", " << arg2.first;
	}
	else
		line_ss << " " << arg1.first;

	return line_ss.str();
}
std::string Model::num_to_hexstr(unsigned num)
{
	std::stringstream s_addr;
	s_addr << "0x" << std::setw(8) << std::setfill('0') << std::hex << num;
	return s_addr.str();
}

void Model::insert_stackline(unsigned val, unsigned addr)
{
	std::string src_text = app->stack_tb->getText();
	std::string str_addr = "[" + num_to_hexstr(addr) + "]";

	auto last_line = src_text.find(str_addr);
	if (last_line != std::string::npos)
	{
		src_text.replace
		(
			last_line,
			make_stackline(stack.at(addr), addr).length(),
			make_stackline(val, addr)
		);
	}
	else
		app->stack_tb->appendText(make_stackline(val, addr) + "\n");
}

void Model::select_line(unsigned addr, OgreBites::TextBox *tb)
{
	std::string src_text = tb->getText();

	std::string hex_addr = num_to_hexstr(addr);
	std::string search_str = (tb == app->code_tb) ? hex_addr : "[" + hex_addr + "]";

	/*удаление старых стрелок, если таке есть*/
	auto oldarrange_idx = src_text.find("\t<--");
	if (oldarrange_idx != std::string::npos)
	{
		src_text.replace(oldarrange_idx, std::string("\t<--").length(), "");
		tb->setText(src_text);
	}

	/*ищем строку с данным адресом*/
	auto addr_idx = src_text.find(search_str);

	if (addr_idx == std::string::npos)
	{
		/*если не нашли, eip указывает на старое место*/
		//cur_line -= 4;
		return;
	}

	/*указываем на новую строку кода, изменяем eip*/
	std::string src_strline = tb == app->code_tb ? make_strline(cmd_list[addr], addr) : make_stackline(stack[addr], addr);
	std::string rep_strline = src_strline + "\t<--";

	src_text.replace(addr_idx, src_strline.length(), rep_strline);

	tb->clearText();
	tb->setText(src_text);
}
void Model::exec_line(unsigned addr)
{
	if (cmd_list.find(addr) == cmd_list.end())
	{
		app->getTray()->showOkDialog("Code execution", "EXCEPTION:\n" + std::string("EIP is invalid"));
		return;
	}

	c_line code_line = cmd_list[addr];

	std::string cmd;
	std::pair<std::string, int> arg1, arg2;

	std::tie(cmd, arg1, arg2) = code_line;

	try
	{
		if (cmd == "push")		push(arg1);
		else if (cmd == "pop")	pop(arg1);				
		else if (cmd == "mov")	mov(arg1, arg2);
		else if (cmd == "jmp" || cmd == "call" || cmd == "ret")
		{
			if (cmd == "jmp") jmp(arg1);
			else if (cmd == "call")
			{
				push({ std::to_string(eip + 4), value });
				jmp(arg1);
			}
			else if (cmd == "ret") ret();

			select_line(eip, app->code_tb);
			return;
		}
	}
	catch (const std::exception &ex)
	{
		app->getTray()->showOkDialog("Code execution", "WARNING:\n" + std::string(ex.what()));
	}

	set_register("eip", eip + 4);
	select_line(eip, app->code_tb);
}

unsigned & Model::get_reg(std::string reg_s)
{
	if (reg_s == "esp")
		return esp;
	else if (reg_s == "eip")
		return eip;
	else if (reg_s == "eax")
		return eax;

	return ebp;
}
void Model::set_register(const char *reg_s, unsigned val)
{
	if (std::string(reg_s) == std::string("esp"))
	{
		esp = val; app->esp->setItems({ num_to_hexstr(val) });
		select_line(val, app->stack_tb);
	}
	else if (std::string(reg_s) == std::string("eip"))
	{
		eip = val; app->eip->setItems({ num_to_hexstr(val) });
	}
	else if (std::string(reg_s) == std::string("eax"))
	{
		eax = val; app->eax->setItems({ num_to_hexstr(val) });
	}
	else if (std::string(reg_s) == std::string("ebp"))
	{
		ebp = val; app->ebp->setItems({ num_to_hexstr(val) });
	}
}

void Model::push(const argument &arg1)
{
	if (stack_overflow)
		throw std::exception("Stack overflow");
	else
	{
		if (esp - 4 == 0x0)
			stack_overflow = true;

		esp_invalid = false;
		unsigned val = unpack_arg(arg1);

		stack[esp - 4] = val;
		insert_stackline(stack[esp - 4], esp - 4);

		set_register("esp", esp - 4);
		select_line(esp, app->stack_tb);
	}
}
void Model::pop(const argument & arg1)
{
	if (arg1.second == value)
		throw std::exception("Invalid instruction arguments");
	else if (!stack.empty() && esp_invalid == false && stack.find(esp) != stack.end())
	{
		if (esp + 4 < esp)
			esp_invalid = true;

		stack_overflow = false;
		set_register(arg1.first.c_str(), stack[esp]);

		set_register("esp", esp + 4);
		select_line(esp, app->stack_tb);
	}
	else
		throw std::exception("Stack empty or ESP is invalid");
}
void Model::mov(const argument & arg1, const argument & arg2)
{
	if (arg1.second != reg)
		throw std::exception("Invalid instruction arguments");

	set_register(arg1.first.c_str(), unpack_arg(arg2));
}
void Model::jmp(const argument & arg1)
{
	if (arg1.second == value && labels.find(arg1.first) != labels.end())
	{
		set_register("eip", labels[arg1.first]);
	}
	else
		set_register("eip", unpack_arg(arg1));
}
void Model::ret()
{
	if (cmd_list.find(eip) == cmd_list.end())
		throw std::exception("EIP is invalid");

	pop({ "eip", reg });
}

unsigned & Model::unpack_arg(const std::pair<std::string, int>& arg)
{
	auto convert_to_int = [](std::string val) -> unsigned
	{
		unsigned retval;

		if (boost::regex_match(val, boost::regex("^0x(?:[a-f]|\\d){1,}$")))
		{
			val.replace(0, 2, "");

			std::stringstream ss;
			ss << std::hex << val;

			if (ss.good() && !ss.eof()) ss >> retval;
			else
			{
				throw std::exception(("Bad cast hex value: " + val).c_str());
			}
		}
		else
			retval = boost::lexical_cast<unsigned>(val);

		return retval;
	};

	unsigned val = (arg.second == reg) ? get_reg(arg.first) : convert_to_int(arg.first);

	return val;
}

std::pair<std::string, int>* Model::args_parse(int arg_amount, XMLElement *xml_element, bool & err_code)
{
	boost::regex reg_base("^(eax|ebp|eip|esp)$");
	boost::regex val_base("^(\\d{1,}|0x(?:[a-f]|\\d){1,}|\\w{1,})$");

	/*parse args*/
	std::string key_word = "arg";
	std::pair<std::string, int> *args = new std::pair<std::string, int>[2];

	bool arg_parse_err = false;

	for (int i = 0; i < arg_amount; i++)
	{
		std::string arg_str = key_word + std::to_string(i + 1);
		const char *arg_val = xml_element->Attribute(arg_str.c_str());

		bool is_reg = false, is_val = false;

		if (arg_val)
		{
			is_reg = boost::regex_match(std::string(arg_val), reg_base);
			is_val = boost::regex_match(std::string(arg_val), val_base);
		}

		if (!arg_val || !is_reg && !is_val)
		{
			err_code = true;
			return new std::pair<std::string, int>[2]();
		}

		args[i] = std::make_pair(std::string(arg_val), (is_reg ? reg : value));
	}

	return args;
}
void Model::parse_code(const char * xml_name)
{
	XMLDocument code_doc;

	if (code_doc.LoadFile("C:\\Users\\Борис\\Documents\\Visual Studio 2017\\Projects\\oop_lab5\\oop_lab5\\code.xml"))
	{
		app->getTray()->showOkDialog("Code parsing", "Status: xml file not found");
		return;
	}

	XMLElement *xml_code = code_doc.FirstChildElement("CODE");

	if (xml_code)
	{
		stack_overflow = false;
		esp_invalid = false;
		eip = 0; esp = 0; ebp = 0; eax = 0;

		cmd_list.clear();
		stack.clear();

		app->code_tb->clearText();
		app->stack_tb->clearText();

		boost::regex cmd_base("^(mov|pop|push|jmp|call|ret)$");

		unsigned line = 0;
		unsigned not_parsed = 0;
		std::cout << "\nXML parse proccess: \n";
		for (XMLNode *node = xml_code->FirstChildElement(); node; node = node->NextSibling())
		{
			XMLElement *xml_element = node->ToElement();

			/*return null if not exist*/
			const char *cmd = xml_element->Attribute("cmd");
			
			if (!cmd)
			{
				/*it's label*/
				const char *label_str = xml_element->Attribute("label");
				if (label_str)
				{
					labels[std::string(label_str)] = line * 4;
					app->code_tb->appendText(std::string(label_str) + ":\n");
					continue;
				}
			}

			if (!cmd || !boost::regex_match(std::string(cmd), cmd_base))
			{
				not_parsed++;
				std::cout << "# line: " << line + 1 << ".\tXML parse status: attr \"cmd\" illegal or not found.\n";
				continue;
			}

			/*parse args*/
			bool err_code = false;
			int arg_am = 0;

			if (std::string(cmd) != "ret") 
				arg_am = (std::string(cmd) == "mov") ? 2 : 1;

			std::pair<std::string, int> *args = args_parse(arg_am, xml_element, err_code);

			if (err_code == true)
			{
				not_parsed++;
				std::cout << "# line: " << line + 1 << ".\tXML parse status: invalid arguments.\n";
				continue;
			}
			else
			{
				c_line l(std::string(cmd), args[0], args[1]);
				cmd_list[line * 4] = l;

				app->code_tb->appendText(make_strline(l, line * 4) + "\n");
				std::cout << "# line: " << line + 1 << ".\tXML parse status: success.\n";

				line++;
			}

			delete[] args;
		}

		std::string message = "Status: SUCCESS.\nParsed: " + std::to_string(line - not_parsed) +
			".\nNot Parsed: " + std::to_string(not_parsed);
		app->getTray()->showOkDialog("Code parsing", message);

		select_line(0, app->code_tb);

		/*set_registers*/
		set_register("esp", esp);
		set_register("eax", eax);
		set_register("eip", eip);
		set_register("ebp", ebp);
	}
	else app->getTray()->showOkDialog("Code parsing", "Status: invalid code file format");

	std::cout << std::endl;
}
