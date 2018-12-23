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

std::string Model::make_stackline(int val, int addr)
{
	std::stringstream ss;
	ss << "[" << num_to_hexstr(addr) << "] " << num_to_hexstr(val);

	return ss.str();
}
std::string Model::make_strline(const c_line & l, int addr)
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
std::string Model::num_to_hexstr(int num)
{
	std::stringstream s_addr;
	s_addr << "0x" << std::setw(8) << std::setfill('0') << std::hex << num;
	return s_addr.str();
}

void Model::select_line(int addr, OgreBites::TextBox *tb)
{
	std::string src_text = tb->getText();

	std::string hex_addr = num_to_hexstr(addr);
	std::string search_str = (tb == app->code_tb) ? hex_addr : "[" + hex_addr + "]";

	/*ищем строку с данным адресом*/
	auto addr_idx = src_text.find(search_str);

	if (addr_idx == std::string::npos)
	{
		/*если не нашли, eip указывает на старое место*/
		//cur_line -= 4;
		return;
	}

	/*удаление старых стрелок, если таке есть*/
	auto oldarrange_idx = src_text.find("\t<--");
	if (oldarrange_idx != std::string::npos)
	{
		src_text.replace(oldarrange_idx, std::string("\t<--").length(), "");
		addr_idx -= std::string("\t<--").length();
	}

	/*указываем на новую строку кода, изменяем eip*/
	std::string src_strline = tb == app->code_tb ? make_strline(cmd_list[addr], addr) : make_stackline(stack[addr], addr);
	std::string rep_strline = src_strline + "\t<--";

	src_text.replace(addr_idx, src_strline.length(), rep_strline);

	tb->clearText();
	tb->setText(src_text);
}

void Model::exec_line(int addr)
{
	if (cmd_list.find(addr) == cmd_list.end()) return;

	c_line code_line = cmd_list[addr];

	std::string cmd;
	std::pair<std::string, int> arg1, arg2;

	std::tie(cmd, arg1, arg2) = code_line;

	try
	{
		if (cmd == "push")
		{
			int val = unpack_arg(arg1);
			
			stack[esp + 4] = val;
			app->stack_tb->appendText(make_stackline(val, esp + 4) + "\n");

			set_reg("esp", esp + 4);
			select_line(esp, app->stack_tb);
		}
		else if (cmd == "pop")
		{
			if (arg1.second == value)
				throw std::exception("Invalid instruction arguments");
			else if (esp >= 0)
			{
				set_reg(arg1.first, stack[esp]);
				set_reg("esp", esp - 4);

				if (esp < 0)
					app->stack_tb->clearText();
				else
					select_line(esp, app->stack_tb);		
			}
			else
				app->getTray()->showOkDialog("Code execution", "EXCEPTION:\n" + std::string("ESP is invalid"));
		}


		if (cmd_list.find(eip + 4) == cmd_list.end())
		{
			throw std::exception("No more instructions for execution");
		}
		else
		{
			set_reg("eip", eip + 4);
			select_line(eip, app->code_tb);
		}
	}
	catch (const std::exception &ex)
	{
		app->getTray()->showOkDialog("Code execution", "WARNING:\n" + std::string(ex.what()));
	}
}

int & Model::get_reg(std::string reg_s)
{
	if (reg_s == "esp")
		return esp;
	else if (reg_s == "eip")
		return eip;
	else if (reg_s == "eax")
		return eax;

	return ebp;
}
void Model::set_reg(std::string reg_s, int val)
{
	if (reg_s == "esp")
	{
		esp = val; app->esp->setItems({ num_to_hexstr(val) });
	}
	else if (reg_s == "eip")
	{
		eip = val; app->eip->setItems({ num_to_hexstr(val) });
	}
	else if (reg_s == "eax")
	{
		eax = val; app->eax->setItems({ num_to_hexstr(val) });
	}
	else if (reg_s == "ebp")
	{
		ebp = val; app->ebp->setItems({ num_to_hexstr(val) });
	}
}

int & Model::unpack_arg(const std::pair<std::string, int>& arg)
{
	auto convert_to_int = [](std::string val) -> int
	{
		int retval;

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
			retval = boost::lexical_cast<int>(val);

		return retval;
	};

	int val = (arg.second == reg) ? get_reg(arg.first) : convert_to_int(arg.first);

	return val;
}

std::pair<std::string, int>* Model::args_parse(int arg_amount, XMLElement *xml_element, bool & err_code)
{
	boost::regex reg_base("^(eax|ebp|eip|esp)$");
	boost::regex val_base("^(\\d{1,}|0x(?:[a-f]|\\d){1,})$");

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
		eip = 0; esp = -4; ebp = 0; eax = 0;

		cmd_list.clear();
		stack.clear();

		app->code_tb->clearText();
		app->stack_tb->clearText();

		boost::regex cmd_base("^(mov|pop|push|jmp|call)$");

		unsigned line = 0;
		unsigned not_parsed = 0;
		std::cout << "\nXML parse proccess: \n";
		for (XMLNode *node = xml_code->FirstChildElement(); node; node = node->NextSibling())
		{
			XMLElement *xml_element = node->ToElement();

			/*return null if not exist*/
			const char *cmd = xml_element->Attribute("cmd");

			if (!cmd || !boost::regex_match(std::string(cmd), cmd_base))
			{
				not_parsed++;
				std::cout << "# line: " << line + 1 << ".\tXML parse status: attr \"cmd\" illegal or not found.\n";
				continue;
			}

			/*parse args*/
			bool err_code = false;
			std::pair<std::string, int> *args = args_parse(((std::string(cmd) == "mov") ? 2 : 1), xml_element, err_code);

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

		/*stack init*/
		set_reg("esp", esp);
		//app->stack_tb->appendText(make_stackline(0, 0) + "\n");
		//select_line(0, app->stack_tb);
	}
	else app->getTray()->showOkDialog("Code parsing", "Status: invalid code file format");

	std::cout << std::endl;
}
