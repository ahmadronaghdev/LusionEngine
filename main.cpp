/*
	LusionEngine- 2D game engine written in C++ with Lua interface.
	Copyright (C) 2006  Erik Engheim

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "Engine.h"

#include <QtGui/QApplication>

#include <Gui/MainForm.h>

#include <Core/AutoreleasePool.hpp>

int main(int argc, char *argv[])
{  
  AutoreleasePool::begin();
    QApplication a(argc, argv);
  
    QWidget* w = new MainForm;
    w->resize(640, 480);
    a.setActiveWindow( w );
    w->show();
    
    int result = a.exec();
    delete w;
  AutoreleasePool::end();
          
  return result;
}
