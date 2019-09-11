
#include "Patty.h"

static struct Recipe * base = NULL;

struct Recipe * Patty_baseRecipe( void )
{
    if (NULL == base)
    {
        base = Recipe_new();

        Recipe_addStep(base, RecipeStep_new("flip",
                                        (DoneAction) Patty_actionFlip, 
                                        (DoneChecker) Patty_isDone, 
                                        60.0                ));

        Recipe_addStep(base, RecipeStep_new("remove",
                                        (DoneAction) Patty_actionRemove, 
                                        Checker_NeverDone,
                                        5.0                 ));
    }

    return (base);
}

void Patty_baseRecipe_free( void )
{
    Recipe_free_full(base);
}

struct Patty * Patty_new( gint _x, gint _y )
{
    struct Patty * _patty;

    _patty = g_new0(struct Patty, 1);

    _patty->x = _x;
    _patty->y = _y;
    _patty->temp = 0.0;

    return (_patty);
}

static gint Patty_distanceBetween_fast( struct Patty * p1,
                                        struct Patty * p2   )
{
    return (abs(p1->x - p2->x) + abs(p1->y - p2->y));
}

static void Patty_foreach_getClosest( gpointer data, gpointer pattyArgs )
{
#define PATTY_REFERENCE (*((struct Patty * (*)[2])pattyArgs))[0]
#define PATTY_NEAREST   (*((struct Patty * (*)[2])pattyArgs))[1]
#define PATTY_CURRENT   ((struct Patty *) data)
    gint distNearest;
    gint distCurrent;

    distNearest = Patty_distanceBetween_fast(   PATTY_REFERENCE,
                                                PATTY_NEAREST   );

    distCurrent = Patty_distanceBetween_fast(   PATTY_REFERENCE,
                                                PATTY_CURRENT   );
    
    if (distCurrent < distNearest)
    {
        PATTY_NEAREST->x = PATTY_CURRENT->x;
        PATTY_NEAREST->y = PATTY_CURRENT->y;
    }
#undef PATTY_REFERENCE
#undef PATTY_NEAREST
#undef PATTY_CURRENT
}

void Patty_replaceWithNearest( struct Patty * patty, GSList * pattyList )
{
    struct Patty    tempPatty;
    struct Patty *  pattyArgs[] = { patty, &tempPatty };
    
    tempPatty.x = ((struct Patty *) pattyList->data)->x;
    tempPatty.y = ((struct Patty *) pattyList->data)->y;
    
    g_slist_foreach(pattyList, Patty_foreach_getClosest, pattyArgs);
    
    patty->x = tempPatty.x;
    patty->y = tempPatty.y;
}

gboolean Patty_isDone( struct Patty * patty )
{
    struct ROBOT_POSE_3D pose = { 0, 0, 0 };
    gboolean isDone;

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Performing patty done check.\n");
    RobotControl_Home();

    pose.x = patty->x;
    pose.y = patty->y;

    RobotControl_Temp(&pose);
    Mezzanine_TempSensorGetTObject(&(patty->temp));
    RobotControl_Home();

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG, "Mezzanine: Got patty temperature: %lf\n", patty->temp);

    isDone = (patty->temp > 27.0);

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG, "Patty is %sdone.\n", isDone ? "" : "not ");

    return (isDone);
}

void Patty_actionFlip( struct Patty * patty )
{
    struct ROBOT_POSE_3D pose = { 0, 0, 0 };
    GSList * pattyList = NULL;

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Performing patty flip.\n");

    RobotControl_Home();

    pose.x = patty->x;
    pose.y = patty->y;

    RobotControl_Flip(&pose);

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Updating patty location.\n");

    //PattyFactory_setBackProjFromFile("./im/h15-bi.jpg");

    RobotControl_Home();
    RobotControl_Photo();
    PattyFactory_updateFrame();
    PattyFactory_setBackProjFromCam();
    RobotControl_Home();

    pattyList = PattyFactory_getPattyList(BACK_PROJECT);

    DEBUG_PRINT_LEVEL_ENTER();
    if (NULL != pattyList)
    {
        DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
        fprintf(G_SYSTEM_LOG, "Found %d candidates", g_slist_length(pattyList));

        Patty_replaceWithNearest(patty, pattyList);
        g_slist_free(pattyList);

        DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
        fprintf(G_SYSTEM_LOG, "Patty found at (%d, %d).\n", patty->x, patty->y);
    }
    else
    {
        DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Patty not found!\n");
    }
    DEBUG_PRINT_LEVEL_EXIT();
}

void Patty_actionRemove( struct Patty * patty )
{
    struct ROBOT_POSE_3D pose = { 0, 0, 0 };

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Performing patty remove.\n");

    RobotControl_Home();

    pose.x = patty->x;
    pose.y = patty->y;

    RobotControl_Deposit(&pose);
    RobotControl_Home();

    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Advancing conveyor.\n");
    Mezzanine_ConveyorSetState(CONVEYOR_FORWARD);
    sleep(3);
    Mezzanine_ConveyorSetState(CONVEYOR_STOPPED);
}

