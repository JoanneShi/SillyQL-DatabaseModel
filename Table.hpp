// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "TableEntry.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

#ifndef Table_hpp
#define Table_hpp

#include <stdio.h>

class Table{
    std::uint32_t col=0;
    std::uint32_t row=0;
    
    std::vector<EntryType> types;
    std::vector<std::string> colNames;
    std::vector<std::vector<TableEntry>> allEntries;
    
    std::unordered_map<TableEntry,std::vector<std::uint32_t>> hash_index;
    std::map<TableEntry,std::vector<std::uint32_t>> bst_index; // sort(v.begin(), v.end(), greater<int>());
    
    char idx_type='n';
    std::uint32_t idx_idx;
    
    public:
        Table(){}
        void read_in();
        void print_all(std::vector<uint32_t> &,std::string&);
        uint32_t print_where(std::vector<uint32_t> &,uint32_t,const TableEntry&,char,std::string&);
    
        template<typename COMP_FUNCTOR>
        std::uint32_t print_helper(std::vector<uint32_t> &,std::string&,COMP_FUNCTOR);
    
        std::uint32_t bst_print_helper(std::vector<uint32_t> &,const TableEntry&,char,std::string&);
    
        std::uint32_t delete_helper(std::uint32_t,const TableEntry&,char,std::string&);
        
    
        void generate_hash_idx(const std::string&);
        void generate_bst_idx(const std::string&);
        
        void call_print();
        void call_delete();
        void call_insert();
        void join_helper();

        void getMode(int argc, char * argv[]);
};

class entryGreaterThan{
    const TableEntry& Val;
    const std::uint32_t colindex;
    
    public:
        entryGreaterThan(const TableEntry& value,const std::uint32_t colinput):Val(value),colindex(colinput){}
        bool operator()(const std::vector<TableEntry>& tname){
            return tname[colindex]>Val;
        }
};

class entryLessThan{
    const TableEntry& Val;
    const std::uint32_t colindex;
    
    public:
        entryLessThan(const TableEntry& value,const std::uint32_t colinput):Val(value),colindex(colinput){}
        bool operator()(const std::vector<TableEntry>& tname){
            return tname[colindex]<Val;
        }
};

class entryEqual{
    const TableEntry& Val;
    const std::uint32_t colindex;
    
    public:
        entryEqual(const TableEntry& value,const std::uint32_t colinput):Val(value),colindex(colinput){}
        bool operator()(const std::vector<TableEntry>& tname){
            return tname[colindex]==Val;
        }
};

std::unordered_map<std::string, Table> allTable;


#endif /* Table_hpp */
