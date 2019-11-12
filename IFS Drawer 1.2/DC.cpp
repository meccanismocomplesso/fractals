/* 
   Dc.cpp

   Copyright (C) 2002-2004 Ren� Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren� Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/


#include "Dc.h"
#include "Color.h"

#include "Pen.h"

#include <sstream>

Dc::Dc(HDC dc) : dc_(dc) {

}

Dc::~Dc() {

}

void Dc::Text(int x, int y, const STD_STR& text) {
  ::TextOut(dc_, x, y, text.c_str(), text.length());
}

void Dc::Text(int x, int y, const STD_STR& text, const Color& color) {
  Color oldColor=TextColor();
  TextColor(color);
  ::TextOut(dc_, x, y, text.c_str(), text.length());
  TextColor(oldColor);
}

void Dc::Text(int x, int y, int i) {
  STD_SSTR s;
  s << i;
  Text(x, y, s.str().c_str());
}

Dc::operator HDC () const {
  return dc_;
}

void Dc::BackgroundColor(unsigned char red, unsigned char green, unsigned char blue) {
  if (::SetBkColor(dc_, RGB(red, green, blue)) == CLR_INVALID) {
    throw "SetBkColor";
  }
}

void Dc::DrawOpaque(bool b) {
  if (b) {
    ::SetBkMode(dc_, OPAQUE);
  }
  else {
    ::SetBkMode(dc_, TRANSPARENT);
  }
}

void Dc::Line(int xTo, int yTo) {
  ::LineTo(dc_, xTo, yTo);
}

void Dc::Move(int x, int y) {
  ::MoveToEx(dc_, x, y, 0);
}

Pen Dc::Select(Pen& pen) {
  Pen ret((HPEN) ::SelectObject(dc_, pen));
  return ret;
}

void Dc::TextAlign(v_textalignment va, h_textalignment ha) {
  ::SetTextAlign(dc_, va | ha);
}

void Dc::TextColor(const Color& color) {
  ::SetTextColor(dc_, color.color_);
}

Color Dc::TextColor() const {
  return Color(::GetTextColor(dc_));
}

