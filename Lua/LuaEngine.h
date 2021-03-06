/*
 *  LuaEngine.h
 *  LusionEngine
 *
 *  Created by Erik Engheim on 3.1.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#pragma once

#include <Core/Core.h>

struct lua_State;

void initLua();
void initGame();
void closeLua();
void debugLua();

void   luaRenderFrame(real start_time);
void   luaSetNumberProperty(const char* key_path, double value);
double luaGetNumberProperty(const char* key_path);
void   luaSetEngineBoolean(const char* key, int int_key, bool value);
bool   luaGetEngineBoolean(const char* key, int int_key);
void   luaUpdate(real start_time);

// Accessors
lua_State* luaState();
