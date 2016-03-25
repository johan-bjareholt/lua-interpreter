#pragma once

#include "node.h"

#include <string>


class TableEntry {
	public:
		std::string name;
		Node value;
		TableEntry* next;
		TableEntry(std::string name, Node& node, TableEntry* next=nullptr);
};


class VarTable {
	private:
		static const int HTSIZE = 50;
		
		TableEntry* hashtable[HTSIZE];
		int genhash(std::string name);

		TableEntry* gettableentry(std::string name);
	public:
		VarTable();
		void addvar(std::string name, Node& value);
		void setvar(std::string name, Node& value);
		bool delvar(std::string name);
		Node& getvar(std::string name);
};
