#include "node.h"
#include "globals.h"

#include <iostream>
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
		ss << "	size=\"6,6\";" << std::endl;
		ss << "node [color=lightblue2, style=filled];" << std::endl;
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
