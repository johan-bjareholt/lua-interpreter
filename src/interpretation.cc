#include "node.h"
#include "globals.h"
#include "vartable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

class BlockContext {
	public:
	Node* block;
	Node returnval;
	bool is_returned = false;

	BlockContext(Node& blocknode){
		block = &blocknode;
		returnval.tag = "NIL";
		returnval.value = "";
	}
	void set_returnval(Node& returnval){
		this->returnval = returnval;
		is_returned = true;
	}
};

/* Context variables */
std::list<BlockContext> blocklist;



void Node::interpret(){
	// Check if block context has a return val, and abort if it has
	BlockContext* context = nullptr;
	if (!blocklist.empty()){
		context = &blocklist.back();
		if (context->is_returned == true)
			return;
	}
	

	bool earlymatch = false;
	if (tag == "for"){
		if (value == "in"){
			std::cout << "for .. in is not yet supported" << std::endl;
			exit(-1);
		}
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
		if ((*si).tag != "int"){
			std::cout << "Invalid end range in for loop" << std::endl;
			exit(-1);
		}
		int endval = std::stoi((*si).value);

		// Get step size
		int stepval = 1;
		if (value == "3var"){
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
		vartable->addvar(varname,itervar);
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
	if (tag == "ifelse"){
		earlymatch = true;
		if (debug_interpretation)
			std::cout << "if-elseif-else" << std::endl;
		auto si = children.begin();
		// If
		Node& ifnode = (*si);
		// Else if
		si++;
		Node& ifcontainer = (*si);
		// Add if to front of elseif container
		ifcontainer.children.push_front(ifnode);
		// Go through ifs
		for (auto ifiter = ifcontainer.children.begin(); ifiter != ifcontainer.children.end(); ifiter++){
			Node& node = (*ifiter);
			Node& ifcondition = node.children.front();
			if (ifcondition.tag == "var" && ifcondition.value == "name"){
				std::string varname = ifcondition.children.front().value;
				ifcondition = vartable->getvar(varname);
			}
			ifcondition.interpret();
			//std::cout << ifcondition.tag <<":"<< ifcondition.value << std::endl;
			if (ifcondition.tag != "int"){
				std::cout << "Invalid if condition" << std::endl;
				exit(-1);
			}
			if (std::stoi(ifcondition.value) > 0){
				Node& ifblock = node.children.back();
				ifblock.interpret();
				goto EARLYMATCHEND;
			}
		}
		// Else
		si++;
		Node& elsenode = (*si);
		if (elsenode.value != "empty"){
			elsenode.interpret();
		}
	}
	if (tag == "funcdef"){
		earlymatch = true;
		std::string funcname = children.front().value;
		Node& funcdef = children.back();
		vartable->setvar(funcname, funcdef);
		if (debug_interpretation)
			std::cout << "Defined function " << funcname << std::endl;
	}
	EARLYMATCHEND:
	if (earlymatch == false)
	{

		for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        	(*i).interpret();


		if (tag == "return"){
			Node returnval;
			if (value == "explist"){
				returnval = children.front().children.front();
				if (returnval.tag == "var" && returnval.value == "name"){
					std::string varname = returnval.children.front().value;
					returnval = vartable->getvar(varname);
				}
			}
			else {
				children.clear();
			}
			context->set_returnval(returnval);
		}

		else if (tag == "funccall"){
			if (value == "2"){
				std::cout << "This type of function call is not supported" << std::endl;
				exit(-1);
			}
			if (children.size() < 1){
				std::cout << "parsing error" << std::endl;
				exit(-1);
			}
			std::list<Node>::iterator si = children.begin();
			// Get func name
			Node& namecontainer = (*si);
			if (namecontainer.tag != "var" || namecontainer.value != "name"){
				std::cout << "Parser error, cannot call something that is not a function" << std::endl;
				exit(-1);
			}
			auto nameiter = namecontainer.children.begin();
			std::string funcname = (*nameiter).value;
			if (debug_interpretation)
				std::cout << "Calling func " << funcname << std::endl;
			nameiter++;
			while (nameiter != namecontainer.children.end()){
				funcname += "." + (*nameiter).value;
				nameiter++;
			}

			// Get arguments
			si++;
			std::list<Node*> args;
			if (si->tag == "explist"){
				for (auto iter=si->children.begin(); iter != si->children.end(); iter++)
					args.push_back(&(*iter));
			}
			else if (si->tag == "str" || si->tag == "int")
				args.push_back(&(*si));
			else {
				std::cout << "Invalid parameters to function" << std::endl;
				exit(-1);
			}

			// Call function
			if (funcname == "print" || funcname == "io.write"){
				if (args.empty()){
					std::cout << "Print function needs an argument" << std::endl;
					exit(-1);
				}
				for (auto pariter = args.begin(); pariter != args.end(); pariter++){
					Node& par = *(*pariter);
					// Fetch var if it is one
					if (par.tag == "var" && par.value == "name"){
						std::string varname = par.children.front().value;
						par = vartable->getvar(varname);
					}
					// Print type
					if (par.tag == "str"){
						std::cout << par.value;
					}
					else if (par.tag == "int"){
						std::cout << par.value;
					}
					else {
						std::cout << funcname << " cannot print datatype " << par.tag << std::endl;
						exit(-1);
					}
				}
				if (funcname == "print")
					std::cout << std::endl;
				// Return
				tag = "NIL";
				value = "";
			}
			else if (funcname == "io.read"){
				std::getline(std::cin, value);
				int len = -1;
				tag = "str";
				if (args.size() != 0){
					Node& arg1 = *(args.front());
					if (arg1.tag == "str"){
						if (arg1.value == "*number")
							tag = "int";
						else {
							std::cout << "This io.read function is not supported" << std::endl;
							exit(-1);
						}
					}
					else if (arg1.tag == "int"){
						std::cout << "Variable length input is not supported by io.read" << std::endl;
						exit(-1);
					}
					else {
						std::cout << "Invalid io.read argument" << std::endl;
					}
				}
			}
			else {
				Node& funcdef = vartable->getvar(funcname);
				if (funcdef.tag != "funcbody"){
					std::cout << funcname << " is not a funcion" << std::endl;
					exit(-1);
				}
				bool noargs = funcdef.children.front().value == "empty";
				Node& parlist = funcdef.children.front().children.front();
				Node& funcbody = funcdef.children.back();
				
				auto argsiter = args.begin();
				auto pariter = parlist.children.begin();
				while (noargs == false && pariter != parlist.children.end() && argsiter != args.end()){
					// Get name
					std::string argname = (*pariter).value;
					// Get argument
					Node& arg = *(*argsiter);

					if (arg.tag == "var" && arg.value == "name"){
						std::string varname = arg.children.front().value;
						arg = vartable->getvar(varname);
					}
					arg.interpret();
					vartable->addvar(argname, arg);
					if (debug_interpretation)
						std::cout << argname << "=" << arg.tag << ":" << arg.value << std::endl;
					
					// Next iteration
					argsiter++;
					pariter++;
				}
				if (noargs == false && pariter != parlist.children.end()){
					std::cout << "Functioncall needs more arguments" << std::endl;
					exit(-1);
				}
				if (argsiter != args.end()){
					std::cout << "Functioncall has too many arguments" << std::endl;
					exit(-1);
				}

				// Get funcbody
				Node copy = funcbody;
				// Set context
				blocklist.push_back(BlockContext(copy));
				// Interpret
				copy.interpret();
				// Set returnval
				if (debug_interpretation)
					std::cout << "return set: " << blocklist.back().returnval.tag << ":" << blocklist.back().returnval.value << std::endl;
				*this = blocklist.back().returnval;
				blocklist.pop_back();

				// Remove arguments
				pariter = parlist.children.begin();
				argsiter= args.begin();
				while (noargs == false && pariter != parlist.children.end()){
					std::string argname = (*pariter).value;
					vartable->delvar(argname);
					pariter++;
					argsiter++;
				}
				if (debug_interpretation)
					std::cout << "return: " << tag << ":" << value << std::endl;
			}
			children.clear();
		}
		else if (tag == "varass"){
			std::list<Node>& vars = children.front().children;
			std::list<Node>& vals = children.back().children;
			std::list<Node>::iterator variter = vars.begin();
			std::list<Node>::iterator valiter = vals.begin();
			while (variter != vars.end() && valiter != vals.end()){
				Node& var = (*variter);
				Node& val = (*valiter);
				if (debug_interpretation)
					std::cout << var.tag << "=" << val.tag << std::endl;
				if (var.tag == "var" && var.value == "name"){
					std::string varname = var.children.front().value;
					vartable->setvar(varname, val);
				}
				else {
					std::cout << "Invalid assignment, value needs to be assigned to a variable" << std::endl;
					exit(-1);
				}
				variter++;
				valiter++;
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
				//std::cout << v1.tag << ":" << v1.value << "," << v1.tag << ":" << v2.value << std::endl;
				if (v1.tag != "int" || v2.tag != "int"){
					std::cout << "Cannot calculate something that is not a number" << std::endl;
					exit(-1);
				}
				//std::cout << v1.value << op.value << v2.value << std::endl;
				long val1 = std::stol(v1.value);
				long val2 = std::stol(v2.value);
				long result;
				if (op.value == "+")
					result = val1 + val2;
				else if (op.value == "-")
					result = val1 - val2;
				else if (op.value == "*")
					result = val1 * val2;
				else if (op.value == "/"){
					if (val2 == 0){
						std::cout << "Division by zero" << std::endl;
						exit(-1);
					}
					result = val1 / val2;
				}
				else if (op.value == "^"){
					std::cout << "operator ^ is not implemented" << std::endl;
					exit(-1);
				}
				else if (op.value == "%"){
					if (val2 == 0){
						std::cout << "Division by zero for modulo" << std::endl;
						exit(-1);
					}
					result = val1 % val2;
				}
				else if (op.value == "<")
					result = val1 < val2;
				else if (op.value == ">")
					result = val1 > val2;
				else if (op.value == ".."){
					std::cout << "operator .. is not implemented" << std::endl;
					exit(-1);
				}
				else if (op.value == "<=")
					result = val1 <= val2;
				else if (op.value == ">=")
					result = val1 >= val2;
				else if (op.value == "==")
					result = val1 == val2;
				else if (op.value == "~="){
					std::cout << "operator ~= is not implemented" << std::endl;
					exit(-1);
				}
				else if (op.value == "and")
					result = val1 && val2;
				else if (op.value == "or")
					result = val1 || val2;
				else {
					std::cout << "Fatal parsing error, invalid operator" << std::endl;
					exit(-1);
				}
				//std::cout << result << std::endl;
	
				this->value = std::to_string(result);
				this->tag = "int";
				this->children.clear();
			}
		}
        else if (tag == "exp"){
            if (children.size() == 1){
                Node& child = (*children.begin());
                if (child.tag == "int"){
                    this->tag = child.tag;
                    this->value = child.value;
                    this->children.clear();
                }
            }
        }
	}
}
