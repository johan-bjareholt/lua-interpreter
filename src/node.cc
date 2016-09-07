#include "node.h"
#include "globals.h"
#include "vartable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

static int next_id = -1;

Node::Node(std::string t, std::string v) : tag(t), value(v){
	line = linenr;
    id = next_id++;
}

Node::Node() {
	line = linenr;
    id = next_id++;
	tag="uninitialised";
	value="uninitialised";
} // Bison needs a default constructor.

void Node::operator==(Node& node){
	line = node.line;
	tag = node.tag;
	value = node.tag;
	for (auto iter=node.children.begin(); iter!=node.children.end(); iter++){
		children.push_back(Node());
		children.back() = *iter;
	}
}

void Node::dumps_str(std::ostream& ss, int depth) {
    for(int i=0; i<depth; i++)
        ss << " ";
    ss << tag << ":" << value << std::endl;
    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).dumps_str(ss, depth+1);
}

void Node::dumps_dot(std::ostream& ss, int depth) {
	if (depth == 0){
		ss << "digraph parsetree {" << std::endl;
		ss << "    size=\"6,6\";" << std::endl;
		ss << "    node [color=lightblue2, style=filled];" << std::endl;
	}

    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++){
    	for(int si=0; si<depth+4; si++)
        	ss << " ";
    	ss << '"' << tag << ':' << value << "@L" << line << "#" << id;
 		ss << "\" -> \"";
		ss << (*i).tag << ':' << (*i).value << "@L" << (*i).line << "#" << (*i).id;
        ss << "\" ;" << std::endl;
	}

    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).dumps_dot(ss, depth+1);
	
	if (depth == 0){
		ss << "}" << std::endl;
	}
}
