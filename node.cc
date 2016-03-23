#include "node.h"
#include "globals.h"
#include "vartable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

Node::Node(std::string t, std::string v) : tag(t), value(v){
	line = linenr;
}


Node::Node() {
	line = linenr;
	tag="uninitialised";
	value="uninitialised";
} // Bison needs a default constructor.

void Node::operator==(Node& node){
	line = node.line;
	tag = node.tag;
	value = node.tag;
}


void Node::interpret(){
    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).interpret();
	if (tag == "stat"){
		if (value == "functioncall"){
			Node& fcall = children.front();
			if (fcall.children.size() >= 1){
				std::list<Node>::iterator si = fcall.children.begin();
				// Get func name
				Node& funcnamecontainer = (*si);
				std::string funcname;
				if (funcnamecontainer.tag == "var" && funcnamecontainer.value == "name")
					funcname = (*funcnamecontainer.children.begin()).value;

				// Get arguments
				si++;
				std::list<Node*> params;
				if (si->tag == "explist"){
					for (auto iter=si->children.begin(); iter != si->children.end(); iter++)
						params.push_back(&(*iter));
				}
				else if (si->tag == "str" || si->tag == "int")
					params.push_back(&(*si));
				else {
					std::cout << "Invalid parameters to function" << std::endl;
					exit(-1);
				}

				// Call function
				if (funcname == "print"){
					if (params.empty()){
						std::cout << "Print function needs an argument" << std::endl;
						exit(-1);
					}
					Node& par1 = *params.front();
					if (par1.tag == "str"){
						std::cout << par1.value << std::endl;
					}
					else if (par1.tag == "int"){
						std::cout << par1.value << std::endl;
					}
					else if (par1.tag == "var" && par1.value == "name"){
						std::string varname = par1.children.front().value;
						Node& node = vartable->getvar(varname);
						std::cout << node.value << std::endl;
					}
				}
				else {
					std::cout << "Undefined function" << std::endl;
					exit(-1);
				}
			}
		}
		else if (value == "assignment"){
			std::list<Node>& vars = children.front().children;
			std::list<Node>& vals = children.back().children;
			std::list<Node>::iterator variter = vars.begin();
			std::list<Node>::iterator valiter = vals.begin();
			while (variter != vars.end() && valiter != vals.end()){
				std::cout << (*variter).tag << "=" << (*valiter).tag << std::endl;
				if ((*variter).tag == "var" && (*variter).value == "name"){
					std::string varname = (*variter).children.front().value;
					vartable->addvar(varname, (*valiter));
				}
				else {
					std::cout << "Invalid assignment, value needs to be assigned to a variable" << std::endl;
					exit(-1);
				}

				variter++;
				valiter++;
			}
		}
	}
	else if (tag == "op"){
		if (value == "binop"){
			std::list<Node>::iterator i = children.begin();
			Node& v1 = *i;
			i++;
			Node& op = *i;
			i++;
			Node& v2 = *i;
			if (v1.tag == "var" && v1.value == "name"){
				v1 = vartable->getvar(v1.children.front().value);
				std::cout << v1.tag << std::endl;
			}
			if (v2.tag == "var" && v2.value == "name"){
				v2 = vartable->getvar(v2.children.front().value);
				std::cout << v2.tag << std::endl;
			}
			if (v1.tag != "int" || v2.tag != "int"){
				std::cout << "Cannot calculate on something that is not a number" << std::endl;
				exit(-1);
			}
			//std::cout << v1.value << op.value << v2.value << std::endl;
			int result;
			if (op.value == "+")
				result = std::stoi(v1.value) + std::stoi(v2.value);
			else if (op.value == "-")
				result = std::stoi(v1.value) - std::stoi(v2.value);
			else if (op.value == "*")
				result = std::stoi(v1.value) * std::stoi(v2.value);
			else if (op.value == "/")
				result = std::stoi(v1.value) / std::stoi(v2.value);
			else if (op.value == "^"){
				std::cout << "operator ^ is not implemented" << std::endl;
				exit(-1);
			}
			else if (op.value == "%")
				result = std::stoi(v1.value) % std::stoi(v2.value);
			else if (op.value == "<")
				result = std::stoi(v1.value) < std::stoi(v2.value);
			else if (op.value == ">")
				result = std::stoi(v1.value) > std::stoi(v2.value);
			else if (op.value == ".."){
				std::cout << "operator .. is not implemented" << std::endl;
				exit(-1);
			}
			else if (op.value == "<=")
				result = std::stoi(v1.value) <= std::stoi(v2.value);
			else if (op.value == ">=")
				result = std::stoi(v1.value) >= std::stoi(v2.value);
			else if (op.value == "==")
				result = std::stoi(v1.value) == std::stoi(v2.value);
			else if (op.value == "~="){
				std::cout << "operator ~= is not implemented" << std::endl;
				exit(-1);
			}
			else if (op.value == "and")
				result = std::stoi(v1.value) && std::stoi(v2.value);
			else if (op.value == "or")
				result = std::stoi(v1.value) || std::stoi(v2.value);
			else {
				std::cout << "Fatal parsing error, invalid operator" << std::endl;
				exit(-1);
			}
			//std::cout << result << std::endl;

			value = std::to_string(result);
			tag = "int";
			children.clear();
		}
	}
}


void Node::dumps_str(std::stringstream& ss, int depth) {
    for(int i=0; i<depth; i++)
        ss << " ";
    ss << tag << ":" << value << std::endl;
    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).dumps_str(ss, depth+1);
}

void Node::dumps_dot(std::stringstream& ss, int depth) {
	if (depth == 0){
		ss << "digraph parsetree {" << std::endl;
		ss << "    size=\"6,6\";" << std::endl;
		ss << "    node [color=lightblue2, style=filled];" << std::endl;
	}

    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++){
    	for(int si=0; si<depth+4; si++)
        	ss << " ";
    	ss << '"' << tag << ':' << value << "@L" << line;
 		ss << "\" -> \"";
		ss << (*i).tag << ':' << (*i).value << "@L" << (*i).line << "\" ;" << std::endl;
	}

    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).dumps_dot(ss, depth+1);
	
	if (depth == 0){
		ss << "}" << std::endl;
	}
}
