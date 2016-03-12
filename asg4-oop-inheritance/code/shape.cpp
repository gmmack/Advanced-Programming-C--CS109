// $Id: shape.cpp,v 1.7 2014-05-08 18:32:56-07 - - $
// Daniel Lavell dlavell@ucsc.edu
// Gavin Mack gmmack@ucsc.edu

#include <cmath>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
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

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{-width/2,-height/2},{width/2,-height/2},
                     {width/2,height/2},{-width/2,height/2}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
            polygon({{-width/2,0},{0,-height/2},
                     {width/2,0},{0,height/2}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

triangle::triangle (const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);
}

right_triangle::right_triangle (GLfloat width, GLfloat height):
               triangle ({{-width/3, -height/3},
               {(2*width)/3, -height/3},{-width/3, (2*height)/3}}) {
   DEBUGF ('c', this);
}

isosceles::isosceles (GLfloat width, GLfloat height): 
               triangle ({{-(width/2)  ,-((width/2)*tan(M_PI/6))},
               {(width/2)  ,-((width/2)*tan(M_PI/6))},
               {0,height-(width/2)*tan(M_PI/6)}}) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width): 
               triangle ({{-(width/2)  ,-((width/2)*tan(M_PI/6))}
               ,{(width/2)  ,-((width/2)*tan(M_PI/6))},
               {0,(width*sin(M_PI/3))-(width/2)*tan(M_PI/6)}}) {
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   GLfloat str_width = glutBitmapLength (glut_bitmap_font, 
                                        (GLubyte*) textdata.c_str());
   if (str_width == 0) throw runtime_error ("Invalid text input");
   GLfloat str_height = glutBitmapHeight (glut_bitmap_font);
   rgbcolor colors = color;
   glColor3ubv (colors.ubvec3());
   GLfloat posx = center.xpos;// - str_width / 2;
   GLfloat posy = center.ypos;// - str_height / 4;
   glRasterPos2f (posx, posy);
   for (auto itor: textdata)
      glutBitmapCharacter (glut_bitmap_font,itor);
   glutSwapBuffers();
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   rgbcolor colors = color;
   glColor3ubv (colors.ubvec3());
   const GLfloat delta = 2 * M_PI / 32;
   GLfloat width = this->dimension.xpos;
   GLfloat height = this->dimension.ypos;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = width * cos (theta) + center.xpos;
      GLfloat ypos = height * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
/*
   if(border){
      glBegin (GL_LINE);
      glEnable (GL_LINE_SMOOTH);
      rgbcolor bord_color = border_color;
      glColor3ubv (bord_color.ubvec3());///////////////////////////
      for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
         GLfloat xpos = width * cos (theta) + center.xpos;
         GLfloat ypos = height * sin (theta) + center.ypos;
         glVertex2f (xpos, ypos);
      }
      glEnd();
   }
*/
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   rgbcolor colors = color;
   glColor3ubv (colors.ubvec3());
   for (auto vert: vertices) {
      glVertex2f (vert.xpos + center.xpos, vert.ypos + center.ypos);
   }
   glEnd();

}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

