#include "freeglut.h"


typedef float type;

// Структура позиции
struct position {
   type x, y, z;

   position() {
      this->x = 0;
      this->y = 0;
      this->z = 0;
   }

   position(type x, type y, type z) {
      this->x = x;
      this->y = y;
      this->z = z;
   }

   position operator-(position A) {
      this->x = this->x - A.x;
      this->y = this->y - A.y;
      this->z = this->z - A.z;
      return *this;
   }

   position operator+(position A) {
      this->x = this->x + A.x;
      this->y = this->y + A.y;
      this->z = this->z + A.z;
   }

   template <typename t>
   position operator*(t k) {
      this->x = this->x * k;
      this->y = this->y * k;
      this->z = this->z * k;
      return *this;
   }

   template <typename t>
   position operator/(t k) {
      this->x = this->x / k;
      this->y = this->y / k;
      this->z = this->z / k;
   }
};

struct object_tetrahedron {
   position points[4];

   float tetrahedron[4][3] = {
        {0.0f, 1.0f, 0.0f}, 
      {0.943f, -0.333f, 0.0f}, 
      {-0.471f, -0.333f, 0.8165f}, 
      {-0.471f, -0.333f, -0.8165f} 
   };
   
   float materialAmbientColor[4][3] = {
           {1.0f, 0.5f, 0.0f},
      {1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 0.0f}
   };
   
   float materialSpecularColor[4][3] = {
           {1.0f, 0.5f, 0.0f}, 
      {1.0f, 1.0f, 1.0f}, 
      {1.0f, 0.0f, 0.0f},
           {0.0f, 0.0f, 0.0f} 
   };

   void draw_tetrahedron() {
      glutWireTetrahedron();

   }

};

struct scene {
   position center_of_system;
   bool show_cone;
   bool show_tetrahedron; 
   object_tetrahedron tetra;

   scene() {
      this->show_cone = 0;
      this->show_tetrahedron = 0;
   }

   void reinit_scene() {

   }

   void draw_scene() {
      if (this->show_tetrahedron) { this->tetra.draw_tetrahedron(); }

   }
} main_scene;