#include "node.h"

#include <iostream>
#include <sstream>
#include <list>


Node::Node(std::string t, std::string v) : tag(t), value(v){

}


Node::Node() {
	tag="uninitialised";
	value="uninitialised";
} // Bison needs a default constructor.


void Node::dump(std::stringstream& ss, int depth) {
    for(int i=0; i<depth; i++)
        ss << " ";
    ss << tag << ":" << value << std::endl;
    for(std::list<Node>::iterator i=children.begin(); i!=children.end(); i++)
        (*i).dump(ss, depth+1);
}
