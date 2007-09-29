/*
 *  LuaTrapezoidNode2.h
 *  LusionEngine
 *
 *  Created by Erik Engheim on 19.1.07.
 *  Copyright 2007 Translusion. All rights reserved.
 *
 */

#pragma once

struct lua_State;
class TrapezoidNode2;

void initLuaTrapezoidNode2(lua_State *L);
TrapezoidNode2  *checkTrapezoidNode2(lua_State* L, int index = 1);