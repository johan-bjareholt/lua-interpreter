#pragma once

#include <sstream>
#include <list>


class Node {
    public:
    std::string tag, value;
    std::list<Node> children;
    Node(std::string t, std::string v);
    Node();
    void dump(std::stringstream& ss, int depth=0);
};
