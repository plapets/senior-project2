
#ifndef RECIPESTEP_H
#define RECIPESTEP_H

/*
File:   RecipeStep.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file declares the RecipeStep data type.  A RecipeStep provides a
means of specifying a cooking method for a particular kind of food, and
of performing this method on the food when either sufficient time has
passed or a food-specific method has determined that the food is done.

Such a separation of process and methods allows for a system which is
easily extendable to handle new kinds of food.
*/

/* the only burgers cooked to GOST standards! */

#include <glib.h>

#define RECIPESTEP_REASON_DONE      0
#define RECIPESTEP_REASON_MAXTIME   1

/* done action */
/* fn pointer to action to perform on ingredient
 * when done.
 */
typedef void        (*DoneAction)   (gpointer);

/* doneness critereon */
/* fn pointer to ingredient's done checker function.
 * is passed the ingredient itself.
 */
typedef gboolean    (*DoneChecker)  (gpointer);

struct RecipeStep
{
    gchar *         name;           /* what to call this step           */
    DoneAction      Action;         /* fn * to ingredient's step        */
    DoneChecker     DoneCheck;      /* fn * to ingredient's doneness    */
    GTimer *        timer;          /* starts on RecipeStep_start       */
    gdouble         maximumTime;    /* step assumed done after this time*/
};


struct RecipeStep * RecipeStep_new(     const gchar *   _name,
                                        DoneAction      _Action,
                                        DoneChecker     _DoneCheck,
                                        gdouble         _maximumTime    );
struct RecipeStep * RecipeStep_copy(    struct RecipeStep * _recipeStep );
struct RecipeStep * RecipeStep_copyAs(  struct RecipeStep * _recipeStep,
                                        const gchar *       _newName    );
void                RecipeStep_destroy( struct RecipeStep * _recipeStep );

void                RecipeStep_start(   struct RecipeStep * r           );
gboolean            RecipeStep_isDone(  struct RecipeStep * r,
                                        gpointer            ingredient,
                                        gint *              reason      );

gboolean Checker_NeverDone( gpointer dontcare );
gboolean Checker_AlwaysDone( gpointer dontcare );

#endif /* RECIPE_STEP */

