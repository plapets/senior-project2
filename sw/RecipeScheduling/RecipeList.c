
#include "RecipeList.h"

/*
File:   RecipeList.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file defines utility functions for creating a GSList of Recipe structs
from a GSList of Patty structs and performing operations on it.
 */

static void RecipeList_foreach_build( gpointer data, gpointer recipes )
{
    gpointer temp;

    temp = Recipe_copyFor(data, Patty_baseRecipe());
    *((GSList **) recipes) = g_slist_prepend(*((GSList **) recipes), temp);
}

static void RecipeList_foreach_run( gpointer data, gpointer count )
{
    gboolean        success;
    const gchar *   name;
    gint            reason;

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG, "Processing recipe %d...\n", *((gint *) count));

    DEBUG_PRINT_LEVEL_ENTER();
    success = Recipe_tryStep((struct Recipe *) data, &name, &reason);
    if (success)
    {
        DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
        fprintf(G_SYSTEM_LOG,   "Recipe %d: step '%s' finished because %s\n",
                                *((gint *) count),
                                name,
                                reason ? "time expired": "food was done");
    }
    DEBUG_PRINT_LEVEL_EXIT();

    (*(gint *) count)++;

    g_free((gpointer) name);
}

static gint RecipeList_foreach_findDone( gconstpointer recipe, gconstpointer dontcare )
{
    /* if recipe is done, return 0 */
    return (!Recipe_isDone((struct Recipe *) recipe));
}

void RecipeList_buildFromPattyList(GSList * patties, GSList ** recipes)
{
    g_slist_foreach(patties, RecipeList_foreach_build, recipes);
}

void RecipeList_tryAll(GSList * recipes)
{
    gint i = 0;
    g_slist_foreach(recipes, RecipeList_foreach_run, &i);
}

void RecipeList_removeDone(GSList ** recipes)
{
    GSList * doneRecipeLink = NULL;

    while ( NULL != (doneRecipeLink = g_slist_find_custom(*recipes, NULL,
                                            RecipeList_foreach_findDone))
          )
    {
        Recipe_free_full((struct Recipe *) doneRecipeLink->data);
        doneRecipeLink->data = NULL;
        *recipes = g_slist_remove(*recipes, NULL);
    }
}

