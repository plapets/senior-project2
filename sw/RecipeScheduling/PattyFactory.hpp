
#ifndef PATTYFACTORY_HPP
#define PATTYFACTORY_HPP

/*
File:   PattyFactory.hpp
Date:   2019-04-30
Author: Peter Lapets

Description:
This file defines a C/C++ interface for running image processing algorithms
to detect patties.  Functions are called to retrieve camera frames and set
analysis images.  This is setup in a way which hides OpenCV details from
the calling code.

When all the desired parameters are set, call PattyFactory_getPattyList()
and specify the DETECTION_METHOD.  The returned value will be a GSList *
containing a Patty object for each found patty.

Note that it is necessary to free this list yourself;  however, to use it
with RecipeList_buildFromPattyList() as intended, you must free the list
nodes WITHOUT freeing the contained Patty structs (refer to the GLib
reference).  These will be freed when you call RecipeList_removeDone().
 */

#ifdef __cplusplus
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"

extern "C"
{
    cv::Mat PattyFactory_getPreImage( void );
    cv::Mat PattyFactory_getPostImage( void );
#endif
    #include <glib.h>
    #include "Patty.h"
    #include <stdio.h>

    enum DETECTION_METHOD
    {
        BG_SUBTRACT,
        BACK_PROJECT
    };

    int     PattyFactory_init( void );
    
    void    PattyFactory_setBgFromFile      ( const gchar * filename );
    void    PattyFactory_setFgFromFile      ( const gchar * filename );
    void    PattyFactory_setBackProjFromFile( const gchar * filename );
    
    void    PattyFactory_updateFrame        ( void );
    
    void    PattyFactory_setBgFromCam       ( void );
    void    PattyFactory_setFgFromCam       ( void );
    void    PattyFactory_setBackProjFromCam ( void );

    void    PattyFactory_setHistFromFile    ( const gchar * filename );

    GSList * PattyFactory_getPattyList      ( enum DETECTION_METHOD method );
#ifdef __cplusplus
} /* extern "C" */
#endif

#define MM_PER  30
#define PX_PER  51

#define DECIMATION_FACTOR       1

#define BG_SUBTRACT_BLUR_SIGMA  30.0
#define BG_SUBTRACT_THRESHOLD   40.0

#define BACK_PROJECT_BLUR_SIGMA 5.0
#define BACK_PROJECT_THRESHOLD  200.0

#endif /* PATTYFACTORY_HPP */

