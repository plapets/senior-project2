
#ifndef RECIPE_H
#define RECIPE_H

/*
File:   Recipe.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file declares the Recipe data type.  A Recipe is composed of a queue
of RecipeSteps, and optionally an ingredient on which those steps might
be performed.

Using Recipe_new(), a Recipe is built with no referrent ingredient.  This
will describe the steps required to prepare an entire class of ingredients
(i.e. how to cook burger patties in the general).

Once an instance of an ingredient exists, a general Recipe may be applied
to cooking that ingredient with Recipe_copyFor().  This will use the methods
for cooking and checking doneness which are referenced in each RecipeStep on
the specific food item which is described by the ingredient data
(e.g. burger patty coordinates).  Calling this function will start the first
RecipeStep's timer.

To enact the Recipe, call Recipe_tryStep().  This will perform the step
pending in the queue.  If the step is completed, it will be removed from the
queue.  When the Recipe has no steps remaining, it is done, and calling
Recipe_isDone() will return TRUE.
*/

#include <glib.h>

#include "RecipeStep.h"

struct Recipe
{
    gpointer    ingredient; /* object for ingredient being prepared */
    GQueue *    steps;      /* holds the steps for the recipe       */
};

struct Recipe * Recipe_new      (   void    );
void            Recipe_free_full( struct Recipe * _recipe );

void            Recipe_addStep  (   struct Recipe *     _recipe,
                                    struct RecipeStep * _recipeStep );

struct Recipe * Recipe_copyFor  (   gpointer        _ingredient,
                                    struct Recipe * _recipe     );

gboolean        Recipe_tryStep  (   struct Recipe * _recipe,
                                    const gchar **  name,
                                    gint *          reason      );

gboolean        Recipe_isDone   (   struct Recipe * _recipe     );


#endif /* RECIPE_H */

