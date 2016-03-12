// $Id: inode.cpp,v 1.11 2014-06-20 14:03:53-07 - - $
// Daniel Lavell   dlavell@ucsc.edu
// Gavin Mack      gmmack@ucsc.edu

#include <iostream>
#include <stdexcept>
#include <iomanip>
using namespace std;

#include <cassert>
#include "debug.h"
#include "inode.h"

// initializes nex_inode_nr to 1
int inode::next_inode_nr {1};

// constructor for inode()
inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();           
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

// caller function that returns the inode_nr
int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

// operator= function.  allows inodes to be equated.
inode &inode::operator= (const inode &that) {
      inode_nr = that.inode_nr;
      type = that.type;
      contents = that.contents;
   return *this;
}

// function to add a directory inode to the map
void inode::put_dir(const string& dirname, inode_ptr child){
   directory_ptr_of(contents)
      ->dirents.insert(make_pair(dirname, child));
}

// caller function to find and return the child desired
inode_ptr inode::get_child(const string &key){
   auto found = directory_ptr_of(contents)->find_key(key);
   if(found == directory_ptr_of(contents)->getEnd()){
      throw yshell_exn(key + " not found");
   }else{
      return found->second;
   }
}

// caller function to return the map
map<string, inode_ptr> directory::getMap(){
   return this->dirents;
}

// operator= function. allows plain files to be equated
plain_file &plain_file::operator= (const plain_file &that) {
   if (this != &that) {
      data = that.data;
   }
   return *this;
}

// function to convert file_base_ptr to plain_file_ptr
plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

// function to convert file_base_ptr to directory_ptr
directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");//
   return dirptr;
}

// caller function to return the size of the plain file
size_t plain_file::size() const {
   size_t _size {0};
   for(size_t i = 0; i < data.size(); i++){
      _size += data[i].size();
   }
   _size += data.size();
   DEBUGF ('i', "size = " << _size);
   return _size;
}

// caller function to access the private data of a file
wordvec inode::readfile(){
   assert(type == PLAIN_INODE);
   return plain_file_ptr_of(this->contents)->data;
}

// function used to add text to a plain_file
void inode::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   wordvec* copy = new wordvec();
   for(unsigned int i = 2; i < words.size(); i++){
      copy->push_back(words.at(i));
   }
   plain_file_ptr_of(this->contents)->data = *copy;
}

// caller function to return the size of a directory
size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

// function to remove a file or directory from the map
void inode::remove (const string& filename) {
   DEBUGF ('i', filename);
   inode_ptr remove = 
         directory_ptr_of(contents)->find_key(filename)->second;
   if(remove->type == DIR_INODE && 
      directory_ptr_of(remove->contents)->getSize() > 2){
         throw yshell_exn("Can't remove Directory thats not empty!");
   }
   if(directory_ptr_of(this->contents)->dirents.erase(filename) == 0){
      throw yshell_exn(filename + "is not a file");
   }
}

// Constructor for inode_state()
inode_state::inode_state(){
   root = make_shared<inode>(DIR_INODE);
   cwd = root;
   root->put_dir(".", root);
   root->put_dir("..", root);
   pwd = new wordvec;
   pwd->push_back("/");
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}

// function used to find a child inside the map
map<string, inode_ptr>::iterator directory::find_key(const string &key){
   return dirents.find(key);
}

// caller function to get hte beginning itor of the map
map<string, inode_ptr>::iterator directory::getBegin(){
   return dirents.begin();
}

// caller function to get the end itor of the map
map<string, inode_ptr>::iterator directory::getEnd(){
   return dirents.end();
}

// caller function that returns the size of the directory
size_t directory::getSize(){
   return dirents.size();
}

// caller function to get the data from a file
wordvec plain_file::getData(){
   return data;
}

// check to see if its a directory
bool inode::is_a_dir(){
   return this->type == DIR_INODE;
}

// check to see if its a plin_inode
bool inode::is_a_file(){
   return this->type == PLAIN_INODE;
}

// prints the stats of the inode which calls the function
void inode::print_stats(){
   if(this->type == DIR_INODE){
      map<string, inode_ptr>::iterator  it;
      it = directory_ptr_of(this->contents)->getBegin();
      for(;it != directory_ptr_of(this->contents)->getEnd(); it++){
         cout << "      "  << setw(8) <<
                        it->second->inode_nr << setw(10);
         if(it->second->type == DIR_INODE){
            cout << directory_ptr_of(it->second->contents)->getSize();;
         }else{
            cout << wtos(plain_file_ptr_of(it->second->contents)
                                             ->getData(),0).size() - 1;
         }
         cout << left << setw(3) << it->first;
         if(it->second->type == DIR_INODE 
                && (it->first.compare(".") != 0
                && (it->first.compare("..") != 0))){
            cout << "/";
         }
         cout << endl;
      }
   }else{
      cout << "      " << setw(8) << inode_nr << setw(10) 
                  << wtos(plain_file_ptr_of(contents)
                                            ->getData(),0).size() - 1;
   }
}

// changes the current prompt being used
void inode_state::change_prompt (const string newprompt) {
   this->prompt = newprompt;
}

// retunrs the current set prompt
string inode_state::get_prompt() {
   return this->prompt;
}

// check to see if we are currently at the root
bool inode_state::at_root(){
   return root == cwd;
}

// function to update pwd
void inode_state::update_pwd(const string& dir){
   //if already at the current working directory
   if(dir.compare(".") == 0){
      return;
   //appends dir
   }else if(dir.compare("..") != 0){
      pwd->push_back(dir);
   //removes last item
   }else if(pwd->size()>1){
      pwd->pop_back();
   }
}

// simply returns pwd
wordvec* inode_state::get_pwd(){
   return pwd;
}

// simply returns root
inode_ptr inode_state::get_root(){
   return root;
}

// simply returns cwd
inode_ptr inode_state::get_cwd(){
   return cwd;
}

// sets the cwd to new_cwd
void inode_state::set_cwd(inode_ptr new_cwd){
   cwd = new_cwd;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

