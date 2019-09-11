
#include "Recipe.h"


struct Recipe * Recipe_new( void )
{
    struct Recipe * _recipe;

    _recipe = g_new0(struct Recipe, 1);

    _recipe->ingredient = NULL;
    _recipe->steps = g_queue_new();

    return (_recipe);
}

void Recipe_free_full( struct Recipe * _recipe )
{
    if (NULL != _recipe)
    {
        g_free(_recipe->ingredient);
        g_queue_free_full(_recipe->steps, (GDestroyNotify) RecipeStep_destroy);
        g_free(_recipe);
    }
}

void Recipe_addStep(    struct Recipe *     _recipe,
                        struct RecipeStep * _recipeStep )
{
    g_queue_push_tail(_recipe->steps, _recipeStep);
}

static void deep_copy_steps(    gpointer _recipeStep,
                                gpointer _recipe        )
{
    Recipe_addStep(_recipe, RecipeStep_copy(_recipeStep));
}

struct Recipe * Recipe_copyFor( gpointer        _ingredient,
                                struct Recipe * _recipe     )
{
   struct Recipe * _recipeCopy;

   _recipeCopy = Recipe_new();

   _recipeCopy->ingredient = _ingredient;
   g_queue_foreach(_recipe->steps, deep_copy_steps, _recipeCopy);

   /* gross -- change (make an always finished step with 0 maxtime)*/
   RecipeStep_start(g_queue_peek_head(_recipeCopy->steps));

   return (_recipeCopy);
}

gboolean        Recipe_tryStep  (   struct Recipe * _recipe,
                                    const gchar **  name,
                                    gint *          reason      )
{
    struct RecipeStep * thisStep;
    gboolean isDone = FALSE;

    thisStep = g_queue_peek_head(_recipe->steps);
    if (NULL != thisStep)
    {
        *name = g_strdup_printf("%s:%p", thisStep->name,
                                _recipe->ingredient );
        isDone = RecipeStep_isDone(thisStep, _recipe->ingredient, reason);

        if (isDone)
        {
            RecipeStep_destroy(g_queue_pop_head(_recipe->steps));

            if (!g_queue_is_empty(_recipe->steps))
                RecipeStep_start(g_queue_peek_head(_recipe->steps));
        }
    }
    else
    {
        *name = NULL;
    }

    return (isDone);
}

gboolean Recipe_isDone( struct Recipe * _recipe )
{
    return (g_queue_is_empty(_recipe->steps));
}

