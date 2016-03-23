#pragma once

#include "node.h"

#include <string>

class VarTable {
	private:
		static const int HTSIZE = 50;
		
		class TableEntry {
			public:
			std::string name;
			Node value;
			TableEntry* next;
			
			TableEntry(std::string name, Node& value, TableEntry* next=nullptr){
				this->name	= name;
				this->value	= value;
				this->next	= next;
			}
		};

		TableEntry* hashtable[HTSIZE];
		int genhash(std::string name);

	public:
		VarTable();
		void addvar(std::string name, Node& value);
		bool delvar(std::string name);
		Node& getvar(std::string name);
};
