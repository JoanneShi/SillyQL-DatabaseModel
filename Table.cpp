// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "xcode_redirect.hpp"
#include "Table.hpp"
#include <algorithm>
#include <getopt.h>
#include <set>
using namespace std;
bool quiet=false;
void Table::getMode(int argc, char * argv[]) {
    string mode;
    opterr = false;
    int choice;
    int option_index = 0;
    option long_options[] = {
        {"help",       no_argument,       nullptr, 'h'},
        {"quiet",      no_argument,       nullptr, 'q'},
        { nullptr,     0,         nullptr, '\0' }
    };
    
    while ((choice = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1) {
        switch (choice) {
        case 'h':
            cout<<"Welcome to SillyQL!!!!!\n";
            exit(0);
            break;
                
        case 'q': //quiet mode
            quiet=true;
            break;
        
        default:
            cerr << "Unknown option" << endl;
            exit(1);
        }
    }
}

void Table::read_in(){
    char cmd;
    string rubbish,tableName,coltype,colname,indextype;
    
    do{
        if(cin.fail()){
            cerr<<"Error: Reading from cin has failed\n";
            exit(1);
        }
        
        cout<<"% ";
        cin>>cmd;
        
        vector<uint32_t> colIdx;
        
        switch(cmd){
            case '#':
                getline(cin,rubbish);
                break;
            
            case 'C':
                cin>>rubbish>>tableName; //rubbish: reate
                
                if(allTable.count(tableName)){
                    getline(cin,rubbish);
                    cout<<"Error during CREATE: Cannot create already existing table "<<tableName<<"\n";
                    break;
                }
                
                allTable[tableName]; // create newtable in unordered_map -> initialize
                cin>>allTable[tableName].col;
                allTable[tableName].types.reserve(allTable[tableName].col);
                for(uint32_t i=0;i<allTable[tableName].col;++i){
                    cin>>coltype; //coltype
                    if(coltype=="string"){
                        allTable[tableName].types.push_back(EntryType::String);
                    } else if(coltype=="double"){
                        allTable[tableName].types.push_back(EntryType::Double);
                    } else if(coltype=="int"){
                        allTable[tableName].types.push_back(EntryType::Int);
                    } else if(coltype=="bool"){
                        allTable[tableName].types.push_back(EntryType::Bool);
                    }
                }
                
                cout<<"New table "<<tableName<<" with column(s) ";
                for(uint32_t j=0;j<allTable[tableName].col;++j){
                    cin>>colname;
                    cout<<colname<<" ";
                    allTable[tableName].colNames.push_back(colname);
                }
                cout<<"created\n";
                
                break;
                
            case 'I':
                call_insert();
                break;
                
            case 'P':
                call_print();
                break;
                
            case 'D':
                call_delete();
                break;
                
            case 'G':
                cin>>rubbish>>rubbish>>tableName>>indextype>>rubbish>>rubbish>>colname;
                if(!allTable.count(tableName)){ //not in map
                    getline(cin,rubbish);
                    cout<<"Error during GENERATE: "<<tableName<<" does not name a table in the database\n";
                    break;
                }
                
                if(true){
                    auto it=find(allTable[tableName].colNames.begin(),allTable[tableName].colNames.end(),colname);
                    if(it==allTable[tableName].colNames.cend()){
                        cout<<"Error during GENERATE: "<<colname<<" does not name a column in "<<tableName<<"\n";
                        break;
                    }
                    allTable[tableName].idx_idx=(uint32_t)distance(allTable[tableName].colNames.begin(), it);
                }
                allTable[tableName].bst_index.clear();
                allTable[tableName].hash_index.clear();
                
                if(indextype=="hash"){
                    allTable[tableName].idx_type='h';
                    generate_hash_idx(tableName);
                } else if(indextype=="bst"){
                    allTable[tableName].idx_type='b';
                    generate_bst_idx(tableName);
                }
               
                cout<<"Created "<<indextype<<" index for table "<<tableName<<" on column "<<colname<<"\n";
                break;
                
            case 'J':
                cin>>rubbish;
                join_helper();
                break;
                
            case 'R':
                cin>>rubbish>>tableName;
                if(!allTable.count(tableName)){ //not in map
                    getline(cin,rubbish);
                    cout<<"Error during REMOVE: "<<tableName<<" does not name a table in the database\n";
                    break;
                }
                allTable.erase(tableName);
                cout<<"Table "<<tableName<<" deleted\n";
                break;
            
            case 'Q':
                break;
            
            default:
                cout<<"Error: unrecognized command\n";
                getline(cin,rubbish);
        }
    } while(cmd!='Q');
}

void Table::join_helper(){
    string tableName1,tableName2,colName1,colName2,tableName;
    uint32_t printColNum,index,result=0;
    string printCol;
    string rubbish;
    char onetwo;
    vector<uint32_t> colIdx;
    vector<char> table1or2;
    
    cin>>tableName1>>rubbish>>tableName2>>rubbish>>colName1>>rubbish>>colName2>>rubbish>>rubbish>>printColNum;
    uint32_t temp_idx_idx1;
    uint32_t temp_idx_idx2;
    
    if(!allTable.count(tableName1)){ //not in map
        getline(cin,rubbish);
        cout<<"Error during JOIN: "<<tableName1<<" does not name a table in the database\n";
        return;
    } else if(!allTable.count(tableName2)){
        getline(cin,rubbish);
        cout<<"Error during JOIN: "<<tableName2<<" does not name a table in the database\n";
        return;
    } else{
        auto it=find(allTable[tableName1].colNames.begin(),allTable[tableName1].colNames.end(),colName1);
        if(it==allTable[tableName1].colNames.cend()){
            cout<<"Error during JOIN: "<<colName1<<" does not name a column in "<<tableName1<<"\n";
            getline(cin,rubbish);
            return;
        } else{
            temp_idx_idx1=(uint32_t)distance(allTable[tableName1].colNames.begin(), it);
        }
        
        it=find(allTable[tableName2].colNames.begin(),allTable[tableName2].colNames.end(),colName2);
        if(it==allTable[tableName2].colNames.cend()){
            cout<<"Error during JOIN: "<<colName2<<" does not name a column in "<<tableName2<<"\n";
            getline(cin,rubbish);
            return;
        } else{
            temp_idx_idx2=(uint32_t)distance(allTable[tableName2].colNames.begin(), it);
        }
    }
    
    for(uint32_t i=0;i<printColNum;++i){
        cin>>printCol>>onetwo;
        if(onetwo=='1'){
            tableName=tableName1;
        } else{
            tableName=tableName2;
        }
        auto it=find(allTable[tableName].colNames.begin(),allTable[tableName].colNames.end(),printCol);
        if(it==allTable[tableName].colNames.cend()){
            cout<<"Error during JOIN: "<<printCol<<" does not name a column in "<<tableName<<"\n";
            return;
        }
        index=(uint32_t)distance(allTable[tableName].colNames.begin(), it);
        colIdx.push_back(index);
        table1or2.push_back(onetwo);
    }
    
    if(!quiet){
        for(uint32_t i=0;i<colIdx.size();++i){
            if(table1or2[i]=='1'){
                cout<<allTable[tableName1].colNames[colIdx[i]]<<" ";
            } else{
                cout<<allTable[tableName2].colNames[colIdx[i]]<<" ";
            }
        }
        cout<<"\n";
    }
    
    unordered_map<TableEntry,vector<uint32_t>> temp;
    
    for(uint32_t i=0;i<allTable[tableName2].row;++i){
        temp[allTable[tableName2].allEntries[i][temp_idx_idx2]].push_back(i);
    }
    
    for(uint32_t i=0;i<allTable[tableName1].row;++i){
        if(temp.find(allTable[tableName1].allEntries[i][temp_idx_idx1])!=temp.end()){ //tableName2 CONTAINS that row(key) ->print
            result+=(uint32_t)temp[allTable[tableName1].allEntries[i][temp_idx_idx1]].size();
            for(auto output:temp[allTable[tableName1].allEntries[i][temp_idx_idx1]]){
                if(!quiet){
                    for(uint32_t j=0;j<colIdx.size();++j){
                        if(table1or2[j]=='1'){
                            cout<<allTable[tableName1].allEntries[i][colIdx[j]]<<" ";
                        } else{
                            cout<<allTable[tableName2].allEntries[output][colIdx[j]]<<" ";
                        }
                    }
                    cout<<"\n";
                }
            }
        }
    }
    cout<<"Printed "<<result<<" rows from joining "<<tableName1<<" to "<<tableName2<<"\n";
}


void Table::generate_hash_idx(const string& tableName){
    for(uint32_t i=0;i<allTable[tableName].row;++i){
        allTable[tableName].hash_index[allTable[tableName].allEntries[i][allTable[tableName].idx_idx]].push_back(i);
    }
}

void Table::generate_bst_idx(const string& tableName){
    //vector<uint32_t> & temp=allTable[tableName].bst_index[allTable[tableName].allEntries[0][allTable[tableName].idx_idx]];
    for(uint32_t i=0;i<allTable[tableName].row;++i){
        allTable[tableName].bst_index[allTable[tableName].allEntries[i][allTable[tableName].idx_idx]].push_back(i);
    }
}

uint32_t Table::bst_print_helper(vector<uint32_t> &allIdx,const TableEntry& val,char op,string& tName){
    uint32_t result=0;
    auto itbegin=allTable[tName].bst_index.begin();
    auto itend=allTable[tName].bst_index.end();
    if(op=='='){
        if(allTable[tName].bst_index.find(val)==allTable[tName].bst_index.end()){
            return 0;
        } else{
            vector<uint32_t> temp=allTable[tName].bst_index[val];
            for(uint32_t i:temp){
                if(!quiet){
                    for(uint32_t j:allIdx){
                        cout<<allTable[tName].allEntries[i][j]<<" ";
                    }
                    cout<<"\n";
                }
                result++;
            }
            return result;
        }
    } else if(op=='<'){
        itbegin=allTable[tName].bst_index.begin();
        itend=allTable[tName].bst_index.upper_bound(val);
        if(itbegin!=itend){
            itend--;
            if(itend->first!=val){
                itend++;
            }
        }
    } else if(op=='>'){
        itbegin=allTable[tName].bst_index.lower_bound(val);
        itend=allTable[tName].bst_index.end();
        if(itbegin!=itend){
          if(itbegin->first==val){
              itbegin++;
          }
        }
    }
    for(auto it=itbegin;it!=itend;it++){
        for(uint32_t i:it->second){
            if(!quiet){
                for(uint32_t j:allIdx){
                    cout<<allTable[tName].allEntries[i][j]<<" ";
                }
                cout<<"\n";
            }
            result++;
        }
    }
    return result;
}

template<typename COMP_FUNCTOR>
uint32_t Table::print_helper(vector<uint32_t>& allidx,string& tableName,COMP_FUNCTOR comp){
    uint32_t result=0;
    auto it=allTable[tableName].allEntries.begin();
    for(uint32_t i=0;i<allTable[tableName].row;++i){
        if(comp(*it)){
            if(!quiet){
                for(uint32_t i:allidx){
                    cout<<(*it)[i]<<" ";
                }
                cout<<"\n";
            }
            result++;
        }
        it++;
    }
    return result;
}

void Table::call_print(){
    string rubbish,tableName,colname,printCol;
    uint32_t printColNum,index;
    char opration;
    string isStr;
    double isDouble;
    int isInt;
    bool isBool;
    vector<uint32_t> colIdx;
    
    cin>>rubbish>>rubbish>>tableName>>printColNum;
    
    if(!allTable.count(tableName)){ //not in map
        getline(cin,rubbish);
        cout<<"Error during PRINT: "<<tableName<<" does not name a table in the database\n";
        return;
    }
    
    colIdx.reserve(printColNum);
    for(uint32_t i=0;i<printColNum;++i){
        cin>>printCol;
        auto it=find(allTable[tableName].colNames.begin(),allTable[tableName].colNames.end(),printCol);
        if(it==allTable[tableName].colNames.cend()){
            getline(cin,rubbish);
            cout<<"Error during PRINT: "<<printCol<<" does not name a column in "<<tableName<<"\n";
            return;
        }
        index=(uint32_t)distance(allTable[tableName].colNames.begin(), it);
        colIdx.push_back(index);
    }
    
    cin>>colname;
    if(colname=="ALL"){ //print all
        if(!quiet){
            for(uint32_t i:colIdx){
                cout<<allTable[tableName].colNames[i]<<" ";
            }
            cout<<"\n";
            
            print_all(colIdx,tableName);
            cout<<"Printed "<<allTable[tableName].row<<" matching rows from "<<tableName<<"\n";
        } else{
            cout<<"Printed "<<allTable[tableName].row<<" matching rows from "<<tableName<<"\n";
        }
        
    } else {
        uint32_t N=0;
        cin>>colname>>opration;
        
        auto it=find(allTable[tableName].colNames.begin(),allTable[tableName].colNames.end(),colname);
        
        if(it==allTable[tableName].colNames.cend()){
            getline(cin,rubbish);
            cout<<"Error during PRINT: "<<colname<<" does not name a column in "<<tableName<<"\n";
            return;
        }
        
        if(!quiet){
            for(uint32_t i:colIdx){
                cout<<allTable[tableName].colNames[i]<<" ";
            }
            cout<<"\n";
        }
        
        index=(uint32_t)distance(allTable[tableName].colNames.begin(), it);
        
        if(allTable[tableName].types[index]==EntryType::String){
            cin>>isStr;
            TableEntry value(isStr);
            if(allTable[tableName].idx_type=='b'&&allTable[tableName].idx_idx==index){
                N=bst_print_helper(colIdx,value,opration,tableName);
            } else{
                N=print_where(colIdx,index,value,opration,tableName);
            }
        } else if(allTable[tableName].types[index]==EntryType::Double){
            cin>>isDouble;
            TableEntry value(isDouble);
            if(allTable[tableName].idx_type=='b'&&allTable[tableName].idx_idx==index){
                N=bst_print_helper(colIdx,value,opration,tableName);
            } else{
                N=print_where(colIdx,index,value,opration,tableName);
            }
        } else if(allTable[tableName].types[index]==EntryType::Int){
            cin>>isInt;
            TableEntry value(isInt);
            if(allTable[tableName].idx_type=='b'&&allTable[tableName].idx_idx==index){
                N=bst_print_helper(colIdx,value,opration,tableName);
            } else{
                N=print_where(colIdx,index,value,opration,tableName);
            }
        } else if(allTable[tableName].types[index]==EntryType::Bool){
            cin>>isBool;
            TableEntry value(isBool);
            if(allTable[tableName].idx_type=='b'&&allTable[tableName].idx_idx==index){
                N=bst_print_helper(colIdx,value,opration,tableName);
            } else{
                N=print_where(colIdx,index,value,opration,tableName);
            }
        }
        cout<<"Printed "<<N<<" matching rows from "<<tableName<<"\n";
    }
}

uint32_t Table::print_where(vector<uint32_t> &allIdx,uint32_t idx, const TableEntry& val,char op,string& tName){
    uint32_t result=0;
    if(op=='='){
        result=print_helper(allIdx,tName,entryEqual(val,idx));
    } else if(op=='<'){
        result=print_helper(allIdx,tName,entryLessThan(val,idx));
    } else if(op=='>'){
        result=print_helper(allIdx,tName,entryGreaterThan(val,idx));
    }
    return result;
}

void Table::print_all(vector<uint32_t> &allIdx,string &tName){
    for(uint32_t i=0;i<allTable[tName].row;++i){
        for(uint32_t j=0;j<allIdx.size();++j){
            cout<<allTable[tName].allEntries[i][allIdx[j]]<<" ";
        }
        cout<<"\n";
    }
}

void Table::call_delete(){
    string rubbish,tableName,colname;
    char opration;
    uint32_t index;
    string isStr;
    double isDouble;
    int isInt;
    bool isBool;
    
    cin>>rubbish>>rubbish>>tableName>>rubbish>>colname>>opration;
    if(!allTable.count(tableName)){ //not in map
        getline(cin,rubbish);
        cout<<"Error during DELETE: "<<tableName<<" does not name a table in the database\n";
        return;
    }
    
    auto it=find(allTable[tableName].colNames.begin(),allTable[tableName].colNames.end(),colname);
    if(it==allTable[tableName].colNames.cend()){
        getline(cin,rubbish);
        cout<<"Error during DELETE: "<<colname<<" does not name a column in "<<tableName<<"\n";
        return;
    }
    index=(uint32_t)distance(allTable[tableName].colNames.begin(), it);
    uint32_t N=0;
    if(allTable[tableName].types[index]==EntryType::String){
        cin>>isStr;
        TableEntry value(isStr);
        N=delete_helper(index,value,opration,tableName);
    } else if(allTable[tableName].types[index]==EntryType::Double){
        cin>>isDouble;
        TableEntry value(isDouble);
        N=delete_helper(index,value,opration,tableName);
    } else if(allTable[tableName].types[index]==EntryType::Int){
        cin>>isInt;
        TableEntry value(isInt);
        N=delete_helper(index,value,opration,tableName);
    } else if(allTable[tableName].types[index]==EntryType::Bool){
        cin>>isBool;
        TableEntry value(isBool);
        N=delete_helper(index,value,opration,tableName);
    }
    cout<<"Deleted "<<N<<" rows from "<<tableName<<"\n";

    if(allTable[tableName].idx_type=='h'){
        allTable[tableName].hash_index.clear();
        generate_hash_idx(tableName);
    } else if(allTable[tableName].idx_type=='b'){
        allTable[tableName].bst_index.clear();
        generate_bst_idx(tableName);
    }
}

uint32_t Table::delete_helper(uint32_t idx,const TableEntry& val,char op,string& tName){
    uint32_t start=allTable[tName].row;
    uint32_t result;
    
    auto allentrybegin=allTable[tName].allEntries.begin();
    auto allentryend=allTable[tName].allEntries.begin()+allTable[tName].row;
    
    if(op=='='){
        allentryend=allTable[tName].allEntries.erase(remove_if(allentrybegin,allentryend,entryEqual(val,idx)),allTable[tName].allEntries.end());
    } else if(op=='<'){
        allentryend=allTable[tName].allEntries.erase(remove_if(allentrybegin,allentryend,entryLessThan(val,idx)),allTable[tName].allEntries.end());
    } else if(op=='>'){
        allentryend=allTable[tName].allEntries.erase(remove_if(allentrybegin,allentryend,entryGreaterThan(val,idx)),allTable[tName].allEntries.end());
    }
    
    result=(uint32_t)distance(allentrybegin,allentryend);
    allTable[tName].row=result;
    return start-result;
}

void Table::call_insert(){
    string rubbish,tableName;
    uint32_t insertRowNum;
    string isStr;
    double isDouble;
    int isInt;
    bool isBool;
    
    cin>>rubbish>>rubbish>>tableName>>insertRowNum>>rubbish;
    
    if(!allTable.count(tableName)){ //not in map
        getline(cin,rubbish);
        cout<<"Error during INSERT: "<<tableName<<" does not name a table in the database\n";
        return;
    }
    
    allTable[tableName].allEntries.resize(allTable[tableName].row+insertRowNum);
    
    for(uint32_t i=allTable[tableName].row;i<allTable[tableName].row+insertRowNum;++i){
        vector<TableEntry>& nowInsert=allTable[tableName].allEntries[i];
        nowInsert.reserve(allTable[tableName].col);
        
        for(uint32_t j=0;j<allTable[tableName].col;++j){
            switch(allTable[tableName].types[j]){
                case EntryType::String:
                    cin>>isStr;
                    nowInsert.emplace_back(isStr);
                    break;
                    
                case EntryType::Double:
                    cin>>isDouble;
                    nowInsert.emplace_back(isDouble);
                    break;
                    
                case EntryType::Int:
                    cin>>isInt;
                    nowInsert.emplace_back(isInt);
                    break;
                    
                case EntryType::Bool:
                    cin>>isBool;
                    nowInsert.emplace_back(isBool);
                    break;
                
                default:
                    break;
            }
        }
    }
    allTable[tableName].row+=insertRowNum;
    
    cout<<"Added "<<insertRowNum<<" rows to "<<tableName<<" from position "
    <<allTable[tableName].row-insertRowNum<<" to "<<allTable[tableName].row-1<<"\n";
    
    if(allTable[tableName].idx_type=='h'){
        for(uint32_t i=allTable[tableName].row-insertRowNum;i<allTable[tableName].row;++i){
            allTable[tableName].hash_index[allTable[tableName].allEntries[i][allTable[tableName].idx_idx]].push_back(i);
        }
    } else if(allTable[tableName].idx_type=='b'){
        for(uint32_t i=allTable[tableName].row-insertRowNum;i<allTable[tableName].row;++i){
            allTable[tableName].bst_index[allTable[tableName].allEntries[i][allTable[tableName].idx_idx]].push_back(i);
        }
    }
}

int main(int argc, char *argv[]){
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    xcode_redirect(argc,argv);
    
    cin>>boolalpha;
    cout<<boolalpha;
    
    Table attempt;
    attempt.getMode(argc, argv);
    attempt.read_in();
    cout<<"Thanks for being silly!\n";
    
    return 0;
}
