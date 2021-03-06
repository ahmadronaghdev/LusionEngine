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

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


#include "Base/Sprite.h"
#include "Lua/Base/LuaSprite.h"
#include "LuaEngine.h"
//#include "Base/ImageView.h"
#include "Base/Group.h"
#include "Base/CircleShape.h"
#include "Base/RectShape2.h"
#include "Base/SegmentShape2.h"
#include "Base/PolygonView.h"
#include "Utils/PolygonUtils.h"
#include "Utils/GLUtils.h"

#include <Core/SharedObject.hpp>
#include <Core/AutoreleasePool.hpp>

#include <vector>
#include <iostream>
#include <algorithm>

// #define PRINT_AVG_LOOP_TIME

using namespace std;

// private member variables
// Timer
static int gStartTime = 0;

static bool       gDone;

// Viewport
static int gViewportWidth = 640;
static int gViewportHeight = 640;

// View
static Rect2  gView(-20.0, -20.0, 20.0, 20.0);

static bool gGLInitialized = false;

// private functions
template <typename ForwardIterator>
void deleteAll(ForwardIterator begin, ForwardIterator end)
{
    while (begin != end) {
        delete *begin;
        ++begin;
    }    
}

template <typename Container>
inline void deleteAll(const Container &c)
{
    deleteAll(c.begin(), c.end());
}

// Game related functions
void renderFrame(real /*start_time*/)
{
    glClear(GL_COLOR_BUFFER_BIT);
    renderGroup()->draw(worldView());
}

void engineEndLoop(real secs)
{
  luaUpdate(secs);  
  AutoreleasePool::currentPool()->releasePool();
}


static void initGL()
{
#ifdef USE_TEXTURES
  glEnable( GL_TEXTURE_2D );                // Enable texture mapping
  glShadeModel( GL_SMOOTH );                // Enable smooth shading
  // glEnable( GL_DEPTH_TEST );                // Enable depth testing
  // glDepthFunc( GL_LEQUAL );                 // Type of depth test to do
  glBlendFunc( GL_SRC_ALPHA, GL_ONE );
  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );  
#endif
  glClearColor( 0.0, 0.0, 0.0, 1.0 );   // Black background  

  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(gView.left(), gView.right(), gView.bottom(), gView.top());  
  glMatrixMode(GL_MODELVIEW);    

  gGLInitialized  = true;
}

void engineInit()
{   
  gDone = false;
  initLua();  
  initGL();
#ifdef USE_TEXTURES  
  initTextures();  
#endif
  initGame();
  
  AutoreleasePool::currentPool()->releasePool();  
}

// Constructors
Sprite* createSprite() 
{
  Sprite* sprite = new Sprite;
  
  // all sprites should be in this since they need to be rendered      
  renderGroup()->addKid(sprite); 
  return sprite;  
}

Sprite* createSprite(View* view)
{
  Sprite* sprite = new Sprite(view);
  
  // all sprites should be in this since they need to be rendered      
  renderGroup()->addKid(sprite); 
  return sprite;
}


Sprite* createSprite(const Point2& aPos, real aDir, real aSpeed)
{
  Sprite* sprite = new Sprite(aPos, aDir, aSpeed);
  
  // all sprites should be in this since they need to be rendered      
  renderGroup()->addKid(sprite); 
  return sprite;
}

CircleShape* createCircle(const Circle& circle)
{
  CircleShape* shape = new CircleShape(circle);
  renderGroup()->addKid(shape);     
  return shape;
}

RectShape2* createRect2(const Rect2& rect)
{
  RectShape2* shape = new RectShape2(rect);
  renderGroup()->addKid(shape);   
  return shape;
}

SegmentShape2* createSegment2(const Segment2& seg)
{
  SegmentShape2* shape = new SegmentShape2(seg);
  renderGroup()->addKid(shape);     
  return shape;
}



// Accessors
bool isDone() {
  return gDone;
}

void setDone(bool done) {
  gDone = done;
}

void setViewportHeight(int height)
{
  gViewportHeight = height;
}

void setViewportWidth(int width)
{
  gViewportWidth = width;
}

void setViewport(int width, int height)
{
  gViewportWidth = width;
  gViewportHeight = height;
}

void setViewport(const QSize size)
{
  gViewportWidth = size.width();
  gViewportHeight = size.height();
}

QSize getViewport()
{
  return QSize(gViewportWidth, gViewportHeight);
}


void setWorldView(const Rect2& rect)
{
  gView = rect;
  
  if (gGLInitialized) {
    glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(gView.left(), gView.right(), gView.bottom(), gView.top());  
    glMatrixMode(GL_MODELVIEW);    
  }
}

void lookAt(const Point2& p)
{
  gView.moveCenter(p);
  if (gGLInitialized) {
    glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(gView.left(), gView.right(), gView.bottom(), gView.top());  
    glMatrixMode(GL_MODELVIEW);    
  }
}

Rect2 worldView()
{
  return gView;
}


int ticksLeft(int start_ticks)
{
  return ticksPerFrame() - (getTicks()-start_ticks);
}




void startTimer()
{
  gStartTime = getTicks();
}

int stopTimer()
{
  return getTicks()-gStartTime;
}