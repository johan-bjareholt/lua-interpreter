#include <iostream>

#include "globals.h"
#include "vartable.h"

/*
   Table Entry
*/

TableEntry::TableEntry(std::string name, Node& value, TableEntry* next) {
	this->name	= name;
	this->value	= value;
	this->next	= next;
}

/*
	VarTable 
*/

VarTable::VarTable(){
	for (int i=0; i<HTSIZE; i++)
		hashtable[i] = nullptr;
}

int VarTable::genhash(std::string name){
	int hash = 0;
	for (int i=0; i<5 && i<name.length(); i++)
		hash += name[i];
	hash = hash % HTSIZE;
	return hash;
}

void VarTable::addvar(std::string name, Node& value){
	int hash = genhash(name);
	TableEntry* tableentry = new TableEntry(name, value);
	if (hashtable[hash] != nullptr)
		tableentry->next = hashtable[hash];
	hashtable[hash] = tableentry;
	if (debug_interpretation)
		std::cout << "Added variable " << name << std::endl;
}

void VarTable::setvar(std::string name, Node& value){
	TableEntry* tableentry = gettableentry(name);
	// Replace variable if it already exists
	if (tableentry != nullptr){
		tableentry->value = value;
		if (debug_interpretation)
			std::cout << "Reassigned variable " << name << std::endl;
	}
	// Create new var
	else
		addvar(name, value);
}

bool VarTable::delvar(std::string name){
	int hash = genhash(name);
	TableEntry* te = hashtable[hash];
	TableEntry* prev = nullptr;
	bool found = false;
	while (found == false && te != nullptr){
		Node* vnode = &te->value;
		if (te->name == name){
			found = true;
			if (prev != nullptr)
				prev->next = te->next;
			else
				hashtable[hash] = te->next;
			delete te;
			if (debug_interpretation)
				std::cout << "Deleted variable " << name << std::endl;
		}
		else {
			prev = te;
			te = te->next;
		}
	}
	return found;
}

TableEntry* VarTable::gettableentry(std::string name){
	int hash = genhash(name);
	TableEntry* te = hashtable[hash];
	Node* vnode = nullptr;
	bool found = false;
	while (found == false && te != nullptr){
		vnode = &te->value;
		if (te->name == name){
			found = true;
		}
		else {
			te = te->next;
		}
	}
	return te;
}

Node& VarTable::getvar(std::string name){
	TableEntry* te = gettableentry(name);
	if (te == nullptr){
		std::cout << "Variable " << name << " is not defined" << std::endl;
		exit(-1);
	}
	return te->value;
}
