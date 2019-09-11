
#ifndef PATTY_H
#define PATTY_H

/*
File:   Patty.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file declares the Patty data structure, used to store information about
hamburger patties, the methods used to manipulate patties described by that
data structure, as well as the Patty base recipe.
 */

#include "../DEBUG_PRINT.h"

#include <stdio.h>
#include <stdlib.h> /* abs() */
#include <unistd.h> /* sleep() */
#include <glib.h>

#include "Recipe.h"
#include "PattyFactory.hpp"

#include "../Mezzanine/Mezzanine.h"
#include "../RobotControl/RobotControl.h"

struct Patty
{
    gint    x;
    gint    y;
    gdouble temp;
};



struct Recipe * Patty_baseRecipe( void );
void            Patty_baseRecipe_free( void );
struct Patty *  Patty_new       ( gint _x, gint _y );

void            Patty_replaceWithNearest(   struct Patty *  patty,
                                            GSList *        pattyList   );

gboolean        Patty_isDone    (   struct Patty * patty      );
void            Patty_actionFlip(   struct Patty * patty      );
void            Patty_actionRemove( struct Patty * patty      );

#endif /* PATTY_H */

