// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $
// Daniel Lavell dlavell@ucsc.edu
// Gavin Mack gmmack@ucsc.edu

#include "commands.h"
#include "debug.h"

commands::commands(): map ({
   {"#"     , fn_comment},
   {"cat"   , fn_cat    },
   {"cd"    , fn_cd     },
   {"echo"  , fn_echo   },
   {"exit"  , fn_exit   },
   {"ls"    , fn_ls     },
   {"lsr"   , fn_lsr    },
   {"make"  , fn_make   },
   {"mkdir" , fn_mkdir  },
   {"prompt", fn_prompt },
   {"pwd"   , fn_pwd    },
   {"rm"    , fn_rm     },
}){}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}

void fn_comment(inode_state&, const wordvec&){}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr curr;
   for (unsigned int i=1; i<words.size(); i++) {
      curr = state.get_cwd();
      curr = curr->get_child (words.at(i));
      if (curr->is_a_dir()) {
         throw yshell_exn (words.at(i)+ " is a directory");
      }else {
         cout << curr->readfile() <<endl;
      }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr curr = state.get_cwd();
   state.set_cwd (curr->get_child (words.at(1)));
   state.update_pwd (words.at(1));
}

void fn_echo (inode_state& state, const wordvec& words){
   for (unsigned int i = 1; i<words.size(); i++) {
      cout << words.at(i) + " ";
   }
   cout << endl;
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() < 2) exit(0);
   int exit_status = stoi (words.at(1), NULL, 10);
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   exit (exit_status);
   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr tmp = state.get_cwd();
   if (words.size() == 1) {
      state.get_cwd()->print_stats();
      return;
   }else {
      for (unsigned int i=1; i<words.size(); i++) {
         if (words.at(i).compare("..") == 0) {
            tmp = tmp->get_child("..");
            tmp->print_stats();
         }else if (words.at(i).compare(".") == 0) {
            tmp->print_stats();
         }else {
            if (words.at(i)[0] == '/') {
               tmp = state.get_root();
            }
            wordvec path = split (words.at(i), "/");
            unsigned int j=0;
            for (; j<path.size(); j++) {
               tmp = tmp->get_child(path.at(j));
            }
            tmp->print_stats();
            if (!(tmp->is_a_dir())) {
               cout << path.at(j-1) << endl;
            }
         }
      }
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   //if(node == nullptr) return;
   // print 
   // fn_lsr(node.left)
   // fn_lsr(node.right)



   //make a function for this!
   //
   //split words by /
   //check children of first word for next word
   //so on until at last directory...
   //




}


void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr curr;
   if (words.at(1)[0] != '/') {
      curr = state.get_cwd();
      inode_ptr newnode = make_shared<inode>(PLAIN_INODE);
      newnode->writefile(words);
      curr->put_dir(words.at(1), newnode);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1) {
      throw yshell_exn ("Error missing filename!");
   }
   inode_ptr curr;
   if (words.at(1)[0] != '/') {
      curr = state.get_cwd();
      inode_ptr newnode = make_shared<inode>(DIR_INODE);
      newnode->put_dir(".", newnode);
      newnode->put_dir("..", curr);
      curr->put_dir(words.at(1), newnode);
   }
}

void fn_prompt (inode_state& state, const wordvec& words){
   state.change_prompt (wtos (words, 1));
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_pwd()->at(0);
   for (unsigned int i=1; i<state.get_pwd()->size(); i++) {
      cout << state.get_pwd()->at(i) + "/";
   }
   cout << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr curr = state.get_cwd();
   curr->remove(words.at(1));
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

