#pragma once

#include <sstream>
#include <list>


class Node {
    public:
    std::string tag, value;
	int line;
    int id;
    std::list<Node> children;
    Node(std::string t, std::string v);
    Node();
	void operator==(Node& node);
	void interpret();
    void dumps_str(std::ostream& ss, int depth=0);
	void dumps_dot(std::ostream& ss, int depth=0);
};
