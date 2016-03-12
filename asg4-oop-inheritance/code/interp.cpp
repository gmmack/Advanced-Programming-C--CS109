// $Id: interp.cpp,v 1.15 2014-07-22 20:03:19-07 - - $
// Daniel Lavell dlavell@ucsc.edu
// Gavin Mack gmmack@ucsc.edu

#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"          , &interpreter::make_text          },
   {"ellipse"       , &interpreter::make_ellipse       },
   {"circle"        , &interpreter::make_circle        },
   {"polygon"       , &interpreter::make_polygon       },
   {"rectangle"     , &interpreter::make_rectangle     },
   {"square"        , &interpreter::make_square        },
   {"diamond"       , &interpreter::make_diamond       },
   {"triangle"      , &interpreter::make_triangle      },
   {"right_triangle", &interpreter::make_right_triangle},
   {"isosceles"     , &interpreter::make_isosceles     },
   {"equilateral"   , &interpreter::make_equilateral   },
};



static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};




interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   auto test = factory_map.find (*++begin);
   if (test == factory_map.end()) return;
   objmap.insert ({name, make_shape (begin, end)});
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      return;
      //fprintf (stderr, name + ": no such shape");
      //throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   //itor->second->draw (where, color);
   object newobj (itor->second, where, color);
   window::push_back (newobj);
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   window::to_move = stof (*begin);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      //fprintf (stderr, type + ": no such shape");
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string words;
   string font = *begin;
   auto found = fontcode.find(font);
   auto itor = ++begin;
   while(itor != end){
      words += *itor + " ";
      ++itor; 
   } 
   return make_shared<text> (found->second, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = stof(*begin++);
   GLfloat h = stof(*begin);
   return make_shared<ellipse> (w,h);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat diam = stof(*begin);
   return make_shared<circle> (diam);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vlist;
   GLfloat deltax, deltay;
   int numverts=0;
   auto itor = begin;
   for (;itor != end;) {
      deltax += stof(*itor++);
      if (itor == end) {
         throw runtime_error ("Incorrect number of polygon vertices");
      }
      deltay += stof(*itor++);
      ++numverts;
   }
   GLfloat avgx = deltax/numverts;
   GLfloat avgy = deltay/numverts;
   //cout << "CENTER: " << avgx << ", " << avgy << endl;
   itor = begin;
   for (;itor!=end;) {
      GLfloat posx = stof (*itor++) - avgx;
      GLfloat posy = stof (*itor++) - avgy;
      vertex vert {posx, posy};
      vlist.push_back (vert);
   }
   return make_shared<polygon> (vlist);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = stof(*begin++);
   GLfloat l = stof(*begin);
   return make_shared<rectangle> (w, l);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat w = stof(*begin);
   return make_shared<square> (w);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   GLfloat w = stof(*begin++);
   GLfloat h = stof(*begin);
   return make_shared<diamond> (w,h);
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   if(end - begin != 6){
      throw runtime_error("Not a triangle.");
   }
   vertex_list vlist;
   GLfloat deltax, deltay;
   auto itor = begin;
   while(itor != end){
      deltax += stof(*itor++);
      deltay += stof(*itor++);
   }
   GLfloat avgx = deltax/3;
   GLfloat avgy = deltay/3;
   itor = begin;
   cout << "trying to add triangle" << endl;
   while(itor != end){
      GLfloat x = stof(*itor++) - avgx;
      GLfloat y = stof(*itor++) - avgy;
      vertex vert {x, y};
      vlist.push_back(vert);
   }
   return make_shared<triangle> (vlist);
}

shape_ptr interpreter::make_right_triangle (param begin, param end) {
   GLfloat w = stof(*begin++);
   GLfloat h = stof(*begin);
   return make_shared<right_triangle> (w, h);
}

shape_ptr interpreter::make_isosceles (param begin, param end) {
   GLfloat w = stof(*begin++);
   GLfloat h = stof(*begin);
   return make_shared<isosceles> (w, h);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   GLfloat w = stof(*begin);
   return make_shared<equilateral> (w);
}

