#include "node.h"
#include "globals.h"
#include "vartable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

void Node::interpret(){
	bool earlymatch = false;
	if (tag == "stat"){
		if (value == "for,2var" || value == "for,3var"){
			earlymatch = true;
			std::list<Node>::iterator si = children.begin();
			// Get iteration var
			if ((*si).tag != "name"){
				std::cout << "Invalid var in for loop" << std::endl;
				exit(-1);
			}
			std::string varname = (*si).value;

			// Get start value
			si++;
			if ((*si).tag == "var" && (*si).value == "name"){
				std::string varname = (*si).children.front().value;
				(*si) = vartable->getvar(varname);
			}
			if ((*si).tag != "int"){
				std::cout << "Invalid start range in for loop" << std::endl;
				exit(-1);
			}
			int startval = std::stoi((*si).value);

			// Get end value
			si++;
			if ((*si).tag == "var" && (*si).value == "name"){
				std::string varname = (*si).children.front().value;
				(*si) = vartable->getvar(varname);
			}
			std::cout << (*si).tag << "," << (*si).value << std::endl;
			if ((*si).tag != "int"){
				std::cout << "Invalid end range in for loop" << std::endl;
				exit(-1);
			}
			int endval = std::stoi((*si).value);

			// Get step size
			int stepval = 1;
			if (value == "for,3var"){
				si++;
				if ((*si).tag != "int"){
					std::cout << "Invalid step in for loop" << std::endl;
					exit(-1);
				}
				stepval = std::stoi((*si).value);
			}

			// Debug to see if it's correctly parsed
			if (debug_interpretation)
				std::cout << varname << " = " << startval << "," << endval << "," << stepval << std::endl;
			// Call children
			Node itervar("int",std::to_string(startval));
			vartable->setvar(varname,itervar);
			Node& varref = vartable->getvar(varname);
		
			bool done = false;	
			int varval;
			varval = std::stoi(varref.value);
			if (varval >= endval)
				done = true;

			Node copy;
			while (done == false){
				copy = *this;
				for(std::list<Node>::iterator i=copy.children.begin(); i!=copy.children.end(); i++)
        			(*i).interpret();
				varval = std::stoi(varref.value);
				if (varval >= endval)
					done = true;
				varval += stepval;
				varref.value = std::to_string(varval);
			}

			vartable->delvar(varname);
		}
	}
	if (earlymatch == false)
	{
		for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        	(*i).interpret();

		if (tag == "functioncall"){
			if (value == "2"){
				std::cout << "This type of function call is not supported" << std::endl;
				exit(-1);
			}
			if (children.size() >= 1){
				std::list<Node>::iterator si = children.begin();
				// Get func name
				Node& namecontainer = (*si);
				if (namecontainer.tag != "var" || namecontainer.value != "name"){
					std::cout << "Parser error, invalid function name" << std::endl;
					exit(-1);
				}
				auto nameiter = namecontainer.children.begin();
				std::string funcname = (*nameiter).value;
				nameiter++;
				while (nameiter != namecontainer.children.end()){
					funcname += "." + (*nameiter).value;
					nameiter++;
				}

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
				if (funcname == "print" || funcname == "io.write"){
					if (params.empty()){
						std::cout << "Print function needs an argument" << std::endl;
						exit(-1);
					}
					for (auto pariter = params.begin(); pariter != params.end(); pariter++){
						Node& par = *(*pariter);
						if (par.tag == "str"){
							std::cout << par.value;
						}
						else if (par.tag == "int"){
							std::cout << par.value;
						}
						else if (par.tag == "var" && par.value == "name"){
							std::string varname = par.children.front().value;
							Node& node = vartable->getvar(varname);
							std::cout << node.value;
						}
					}
					// Return
					tag = "NIL";
					value = "";
				}
				else if (funcname == "io.read"){
					std::getline(std::cin, value);
					int len = -1;
					tag = "str";
					if (params.size() != 0){
						Node& par1 = *(params.front());
						if (par1.tag == "str"){
							if (par1.value == "*number")
								tag = "int";
							else {
								std::cout << "This io.read function is not supported" << std::endl;
								exit(-1);
							}
						}
						else if (par1.tag == "int"){
							std::cout << "Variable length input is not supported by io.read" << std::endl;
							exit(-1);
						}
						else {
							std::cout << "Invalid io.read argument" << std::endl;
						}
					}

					// Return
				}
				else {
					std::cout << "Undefined function" << std::endl;
					exit(-1);
				}
			}
			children.clear();
		}
		else if (tag == "stat"){
			if (value == "assignment"){
				std::list<Node>& vars = children.front().children;
				std::list<Node>& vals = children.back().children;
				std::list<Node>::iterator variter = vars.begin();
				std::list<Node>::iterator valiter = vals.begin();
				while (variter != vars.end() && valiter != vals.end()){
					if (debug_interpretation)
						std::cout << (*variter).tag << "=" << (*valiter).tag << std::endl;
					if ((*variter).tag == "var" && (*variter).value == "name"){
						std::string varname = (*variter).children.front().value;
						vartable->setvar(varname, (*valiter));
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
				if (v1.tag == "var" && v1.value == "name")
					v1 = vartable->getvar(v1.children.front().value);
				if (v2.tag == "var" && v2.value == "name")
					v2 = vartable->getvar(v2.children.front().value);
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
}
