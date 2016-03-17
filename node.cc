#include "node.h"
#include "globals.h"

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


void Node::interpret(){
    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).interpret();
	if (tag == "stat"){
		if (value == "functioncall"){
			//std::cout << "functioncall"<< children.size() << std::endl;
			Node& fcall = children.front();
			if (fcall.children.size() >= 1){
				Node& funcnamecontainer = (*fcall.children.begin());
				if (funcnamecontainer.tag == "var" && funcnamecontainer.value == "name"){
					std::string& funcname = (*funcnamecontainer.children.begin()).value;
					if (funcname == "print"){
						std::list<Node>::iterator si = fcall.children.begin();
						si++;
						if (si->tag == "str")
							std::cout << si->value << std::endl;
					}
				}
			}
		}
	}
	else if (tag == "exp"){
		if (value == "binop"){
			std::list<Node>::iterator i = children.begin();
			Node& v1 = *i;
			i++;
			Node& op = *i;
			i++;
			Node& v2 = *i;
			std::cout << v1.value << op.value << v2.value << std::endl;
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
			std::cout << result << std::endl;

			value = std::to_string(result);
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
