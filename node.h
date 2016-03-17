#pragma once

#include <sstream>
#include <list>


class Node {
    public:
    std::string tag, value;
	int line;
    std::list<Node> children;
    Node(std::string t, std::string v);
    Node();
	void interpret();
    void dumps_str(std::stringstream& ss, int depth=0);
	void dumps_dot(std::stringstream& ss, int depth=0);
};
