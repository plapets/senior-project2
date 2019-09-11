
#include "RecipeStep.h"

struct RecipeStep * RecipeStep_new(     const gchar *   _name,
                                        DoneAction      _Action,
                                        DoneChecker     _DoneCheck,
                                        gdouble         _maximumTime    )
{
    struct RecipeStep * _recipeStep;

    _recipeStep = g_new0(struct RecipeStep, 1);

    _recipeStep->name = g_strdup(_name);
    _recipeStep->Action = _Action;
    _recipeStep->DoneCheck = _DoneCheck;
    _recipeStep->timer = g_timer_new();
    _recipeStep->maximumTime = _maximumTime;

    return (_recipeStep);
}

struct RecipeStep * RecipeStep_copy( struct RecipeStep *    _recipeStep )
{
    struct RecipeStep * _recipeStepCopy;

    _recipeStepCopy = RecipeStep_copyAs(_recipeStep, _recipeStep->name); 

    return (_recipeStepCopy);
}

struct RecipeStep * RecipeStep_copyAs(  struct RecipeStep * _recipeStep,
                                        const gchar *       _newName    )
{
    struct RecipeStep * _recipeStepCopy;

    _recipeStepCopy = RecipeStep_new(   _newName,
                                        _recipeStep->Action,
                                        _recipeStep->DoneCheck,
                                        _recipeStep->maximumTime );

    return (_recipeStepCopy);
}

void RecipeStep_destroy( struct RecipeStep * _recipeStep )
{
    g_free(_recipeStep->name);
    g_timer_destroy(_recipeStep->timer);
    g_free(_recipeStep);
}

void RecipeStep_start( struct RecipeStep * r )
{
    g_timer_start(r->timer);
}

gboolean RecipeStep_isDone( struct RecipeStep * r,
                            gpointer            ingredient,
                            gint *              reason      )
{
    gboolean isDone         = FALSE;

    if (g_timer_elapsed(r->timer, NULL) >= r->maximumTime)
    {
        isDone = TRUE;
        if (NULL != reason) *reason = RECIPESTEP_REASON_MAXTIME;
    }
    else if (r->DoneCheck(ingredient))
    {
        isDone = TRUE;
        if (NULL != reason) *reason = RECIPESTEP_REASON_DONE;
    }

    if (isDone)
        r->Action(ingredient);

    return (isDone);
}

gboolean Checker_NeverDone( gpointer dontcare )
{
    return (FALSE);
}

gboolean Checker_AlwaysDone( gpointer dontcare )
{
    return (TRUE);
}

