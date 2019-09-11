
#ifndef RECIPELIST_H
#define RECIPELIST_H

/*
File:   RecipeList.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file declares utility functions for creating a GSList of Recipe structs
from a GSList of Patty structs and performing operations on it.
 */

#include "../DEBUG_PRINT.h"

#include "Patty.h"
#include "Recipe.h"

void RecipeList_buildFromPattyList(GSList * patties, GSList ** recipes);
void RecipeList_tryAll(GSList * recipes);
void RecipeList_removeDone(GSList ** recipes);

#endif /*RECIPELIST_H*/

