#include<iostream>
#include<string>
#include<fstream>

using namespace std;

class SymbolInfo
{
    string name;
    string type;

public:

    SymbolInfo *next;
    SymbolInfo *prev;

    SymbolInfo(string name = "", string type = "")
    {
        this->name = name;
        this->type = type;
        next = 0;
        prev = 0;
    }
    void SetName(string str)
    {
        this->name = str;
    }
    string GetName()
    {
        return this->name;
    }

    void SetType(string type)
    {
        this->type = type;
    }

    string GetType()
    {
        return this->type;
    }

};

class ScopeTable
{
    int id,pos,hashVal;
    SymbolInfo **table;
    int tableSize;

public:

    ScopeTable *parentScope;
    ScopeTable(int id = 0)
    {
        this->id = id;
        table = 0;
        tableSize = 0;
        parentScope = 0;
    }
    ~ScopeTable()
    {
        for(int i =0; i< tableSize ; i++)
        {
            SymbolInfo *temp=table[i]->next;
            while(table[i] !=0)
            {
                delete table[i];
                table[i]=temp;
                if(temp !=0 )
                    temp = temp->next;
            }
        }
        if(table) delete []table;
    }
    int initialize(int Size)
    {
        tableSize = Size;
        table = new SymbolInfo*[tableSize];
        for(int i =0; i< Size ; i++)
        {
            table[i] = new SymbolInfo();
        }
        return tableSize;
    }
    unsigned int APHash(const std::string& str)
    {
        unsigned int hash = 0xAAAAAAAA;

        for(std::size_t i = 0; i < str.length(); i++)
        {
          hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ str[i] * (hash >> 3)) :
                                   (~((hash << 11) + (str[i] ^ (hash >> 5))));
        }

        return hash;
    }

    bool Insert(string name, string type)
    {
        if(Search(name) != 0) return false;
        //int hashVal;
        pos = 0;
        hashVal=APHash(name)%tableSize;

        SymbolInfo * item = new SymbolInfo(name, type);
        SymbolInfo * head = table[hashVal];
        item->next = 0;
        while(head->next != 0)
        {
             head = head->next;
             pos++;
        }
        head->next = item;
        item->prev = head;

        return true;
    }

    bool Insert(SymbolInfo* ob)
    {
        if(Search(ob->GetName()) != 0) return false;
        //int hashVal;
        pos = 0;
        hashVal=APHash(ob->GetName())%tableSize;

        SymbolInfo * item = ob;
        SymbolInfo * head = table[hashVal];
        item->next = 0;
        while(head->next != 0)
        {
             head = head->next;
             pos++;
        }
        head->next = item;
        item->prev = head;

        return true;
    }
    
    SymbolInfo * Search(string name)
    {
        pos = 0;
        hashVal = APHash(name)%tableSize;
        SymbolInfo* head = table[hashVal]->next;
        while(head != 0 && head->GetName() != name)
        {
            head = head->next;
            pos++;
        }
        return head;
    }

    bool Delete(string name)
    {
        SymbolInfo* head = Search(name);
        //cout <<"here"<<endl;
        if(head == 0) return false;
        SymbolInfo* temp;
        temp = head->prev;
        temp->next = head->next;
        if(head->next != 0) head->next->prev = temp;

        delete head;
        return true;
    }
    int getPos()
    {
        return pos;
    }
    int getHash()
    {
        return hashVal;
    }
    int getID()
    {
        return id;
    }
    void printTable(ofstream &fout)
    {
        SymbolInfo * head;
        for(int i=0; i<tableSize; i++)
        {
            fout <<" "<< i<<" -->  ";
            cout <<" "<< i<<" -->  ";

            head = table[i]->next;
            while(head != 0)
            {
                fout << "<"<<head->GetName() <<":"<<head->GetType()<< ">    ";
                cout << "<"<<head->GetName() <<":"<<head->GetType()<< ">    ";
                head = head->next;
            }
            fout << endl;
            cout << endl;
        }
    }
};

class SymbolTable
{
    int id;
    int tableSize;
    ScopeTable* currentScope;
public:
    SymbolTable(int Size)
    {
        id = 1;
        tableSize = Size;
        currentScope = new ScopeTable(id);
        currentScope->initialize(Size);
    }

    void EnterScope(ofstream &fout)
    {
        id++;
        ScopeTable *temp = new ScopeTable(id);
        temp->initialize(tableSize);
        temp->parentScope = currentScope;
        currentScope = temp;
        fout <<" New ScopeTable with id "<<id<<" created"<<endl<<endl;
        cout <<" New ScopeTable with id "<<id<<" created"<<endl<<endl;
    }

    void ExitScope(ofstream &fout)
    {
        if(id == 0) {
            fout<<"No scopetable found!"<<endl<<endl;
            cout <<"No scopetable found!"<<endl<<endl;
            return;
        }
        ScopeTable *temp = currentScope;
        if(currentScope->parentScope != 0) currentScope = currentScope->parentScope;
        delete temp;
        fout<<" ScopeTable with id "<<id <<" removed"<<endl<<endl;
        cout<<" ScopeTable with id "<<id <<" removed"<<endl<<endl;
        id--;
        if(id==0) currentScope = 0;
        return;
    }

    bool Insert(string name, string type, ofstream &fout)
    {
        if(id == 0) {
            fout<<"No scopetable found!"<<endl<<endl;
            cout <<"No scopetable found!"<<endl<<endl;
            return false;
        }
        if(currentScope->Insert(name, type) == true)
        {
            fout <<" Inserted in ScopeTable# "<< currentScope->getID() << " at position "<<currentScope->getHash()<<", " <<currentScope->getPos()<<endl<<endl;
            cout <<" Inserted in ScopeTable# "<< currentScope->getID() << " at position "<<currentScope->getHash()<<", " <<currentScope->getPos()<<endl<<endl;
            return true;
        }
        else
        {
            fout <<" <"<<name<<","<<type<<"> already exists in current ScopeTable"<<endl<<endl;
            cout <<" <"<<name<<","<<type<<"> already exists in current ScopeTable"<<endl<<endl;
            return false;
        }
    }
	
    bool Insert(SymbolInfo* ob)
    {
        if(id == 0) {
            return false;
        }
        
       return currentScope->Insert(ob);
        
    }
    
    bool Remove(string name,ofstream &fout)
    {
        if(id == 0) {
            fout<<"No scopetable found!"<<endl<<endl;
            cout <<"No scopetable found!"<<endl<<endl;
            return false;
        }
        if(currentScope->Delete(name) == true)
        {
            fout <<" Found in ScopeTable# "<< currentScope->getID() << " at position "<<currentScope->getHash()<<", " <<currentScope->getPos()<<endl<<endl;
            cout <<" Found in ScopeTable# "<< currentScope->getID() << " at position "<<currentScope->getHash()<<", " <<currentScope->getPos()<<endl<<endl;
            fout <<" Deleted entry at "<<currentScope->getHash()<<", "<<currentScope->getPos() <<"from current ScopeTable"<<endl<<endl;
            cout <<" Deleted entry at "<<currentScope->getHash()<<", "<<currentScope->getPos() <<"from current ScopeTable"<<endl<<endl;
            return true;
        }
        else
        {
            fout <<" Not Found"<<endl<<endl;
            cout <<" Not Found"<<endl<<endl;
            fout <<" "<<name<<" not found"<<endl<<endl;
            cout <<" "<<name<<" not found"<<endl<<endl;
            return false;
        }
    }

    SymbolInfo* LookUp(string name,ofstream &fout)
    {
        if(id == 0) {
            fout<<"No scopetable found!"<<endl<<endl;
            cout <<"No scopetable found!"<<endl<<endl;
            return 0;
        }
        SymbolInfo* temp ;
        ScopeTable* scope = currentScope;

        while(scope != 0)
        {
            temp = scope->Search(name);
            if(temp != 0)
            {
                fout <<" Found in ScopeTable# "<< scope->getID() << " at position "<<scope->getHash()<<", " <<scope->getPos()<<endl<<endl;
                cout <<" Found in ScopeTable# "<< scope->getID() << " at position "<<scope->getHash()<<", " <<scope->getPos()<<endl<<endl;
                return temp;
            }
            scope = scope->parentScope;
        }

        fout <<" Not Found "<<endl<<endl;
        cout <<" Not Found "<<endl<<endl;
        return 0;
    }

    void PrintCurrentScope(ofstream &fout)
    {
        currentScope->printTable(fout);
    }

    void PrintAllScope(ofstream &fout)
    {
        if(id == 0) {
            fout<<"No scopetable found!"<<endl<<endl;
            cout <<"No scopetable found!"<<endl<<endl;
            return;
        }
        int i =id;
        ScopeTable * temp = currentScope;
        while(temp != 0)
        {
            fout <<" ScopeTable # "<<i<<endl;
            cout <<" ScopeTable # "<<i<<endl;
            temp->printTable(fout);
            temp = temp->parentScope;
            i--;
            fout <<endl;
            cout <<endl;
        }
    }
};

int main()
{
    int Size;

    string name, type,ch;
    ifstream fin("input.txt");
    ofstream fout("output1.txt");

    if(!fin) {cout <<"can't open file"; return 0;}
    fin>> Size;
    fout <<Size<<endl;
    SymbolTable table(Size);
    while(fin)
    {
        fin >> ch;

        if(ch == "I")
        {
            fout <<"I ";
            cout <<"I ";
            fin >> name >> type;
            fout <<name<<" "<<type<<endl<<endl;
            cout <<name<<" "<<type<<endl<<endl;
            SymbolInfo* temp = new SymbolInfo(name,type);
            if (table.Insert(temp)) cout << "Inserted"<< endl<<endl;
            else cout <<"not inserted "<<endl<<endl;
        }

        else if(ch == "L")
        {
            fout <<"L ";
            cout <<"L ";
            fin >> name;
            fout << name <<endl<<endl;
            cout << name <<endl<<endl;
            table.LookUp(name,fout);
        }
        else if(ch == "D")
        {
            fout <<"D ";
            cout <<"D ";
            fin >> name;
            fout <<name<<endl<<endl;
            cout <<name<<endl<<endl;
            table.Remove(name,fout);
        }
        else if(ch == "P")
        {
            fout << "P ";
            cout << "P ";
            fin >>ch;
            fout <<"A"<<endl<<endl;
            cout <<"A"<<endl<<endl;
            if(ch == "A") table.PrintAllScope(fout);
            else if(ch == "C") table.PrintCurrentScope(fout);
            //fout <<endl;
        }
        else if(ch == "S")
        {
            fout <<"S"<<endl<<endl;
            cout <<"S"<<endl<<endl;
            table.EnterScope(fout);
        }
        else if(ch == "E")
        {
            fout <<"E"<<endl<<endl;
            cout <<"E"<<endl<<endl;
            table.ExitScope(fout);
        }
    }

    return 0;
}

//int main()
//{
//    int Size;
//    cin >> Size;
//    SymbolTable table(Size);
//    string ch;
//    string name, type;
//    ofstream fout("output2.txt");
//    while(1)
//    {
//        cin >> ch;
//        if(ch == "I")
//        {
//
//            cin >> name >> type;
//            table.Insert(name, type,fout);
//        }
//
//        else if(ch == "L")
//        {
//            cin >> name;
//            SymbolInfo* item = table.LookUp(name,fout);
//            if( item == 0) cout << "not found"<<endl;
//            else cout<<"<"<<item->GetName()<<","<<item->GetType()<<">"<<endl;
//        }
//        else if(ch == "D")
//        {
//            cin >> name;
//            if(table.Remove(name,fout)) cout <<"Success"<<endl;
//            else cout << "failed"<<endl;
//        }
//        else if(ch == "P")
//        {
//            cin >>ch;
//            if (ch =="A") table.PrintAllScope(fout);
//            else if(ch == "C") table.PrintCurrentScope(fout);
//            else ;
//        }
//        else if(ch == "S") table.EnterScope(fout);
//        else if(ch == "E") table.ExitScope(fout);
//        else if(ch == "q") break;
//        cout <<"***************"<<endl;
//        fout <<"***************"<<endl;
//    }
//
//    return 0;
//}
