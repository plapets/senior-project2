
#include "PattyFactory.hpp"


using namespace cv;

static VideoCapture g_cam;

static Mat g_current_frame;

static Mat g_bg;
static Mat g_fg;

static Mat g_src;

static MatND            g_hist;
static int              g_histSize = 32;
static float            HUE_RANGES[] = { 0.0, 180.0 };
static const float *    g_ranges = { HUE_RANGES };

static Mat g_ui_pre;
static Mat g_ui_post;

static bool PattyFactory_decimateMatFromFile( Mat & dst, const gchar * filename )
{
    Mat         src;
    bool        succeeded   = false;
    std::string fname       = std::string(filename);

    src = imread(fname);
    succeeded = (false == src.empty());

    if (succeeded)
    {
        resize(src, dst, Size(), DECIMATION_FACTOR, DECIMATION_FACTOR,
                INTER_NEAREST);
    }

    return (succeeded);
}

static GSList * PattyFactory_getPattyListFromBlobs( Mat & binary )
{
    GSList * pattyList = NULL;
    std::vector<std::vector<Point> > contours;
    int xOffset = binary.rows / 2;
    int yOffset = binary.cols / 2;

    // find contours of the objects
    puts("        find contours");
    findContours( binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    printf("        contour size: %d\n", contours.size());

    // find a bounding box for each found object
    for ( size_t i = 0; i < contours.size(); ++i )
    {
        puts("        get bounding rect and center point");
        Rect bb = boundingRect( Mat(contours[i]) );
        Point center = Point(   bb.x + bb.width / 2,
                                bb.y + bb.height / 2 );

        puts("        draw rect and markers");
        rectangle(g_ui_post, bb.tl(), bb.br(), Scalar(0, 255, 0), 4);
        drawMarker(g_ui_post,   center,
                                Scalar(255, 0, 255),
                                MARKER_TILTED_CROSS, 16, 2  );

        puts("        make patty");
        Patty * foundPatty = Patty_new((center.x - xOffset) * MM_PER / PX_PER,
                                       (center.y - yOffset) * MM_PER / PX_PER);

        puts("        add to pattylist");
        pattyList = g_slist_prepend(pattyList, foundPatty);
    }

    return (pattyList);
}

static Mat PattyFactory_hueFromRgb(Mat & rgb)
{
    Mat hsv;
    Mat hue;
    int ch[] = { 0, 0 };

    // convert to hsv
    cvtColor(rgb, hsv, COLOR_BGR2HSV);

    // get the hue channel only from the HSV image
    hue.create(hsv.size(), hsv.depth());
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );

    return (hue);
}

/* initializes the camera interface, etc. */
int PattyFactory_init( void )
{
    g_cam = VideoCapture(0);

    if (!g_cam.isOpened())  // check if we succeeded
        return -1;

    return (0);
}

/* loads the background image from the provided filename with decimation */
void PattyFactory_setBgFromFile( const gchar * filename )
{
    PattyFactory_decimateMatFromFile(g_bg, filename);
}

/* loads the foreground image from the provided filename with decimation */
void PattyFactory_setFgFromFile( const gchar * filename )
{
    PattyFactory_decimateMatFromFile(g_fg, filename);
}

/* loads the back-projection source from the provided filename with decimation */
void PattyFactory_setBackProjFromFile( const gchar * filename )
{
    PattyFactory_decimateMatFromFile(g_src, filename);
}

void PattyFactory_updateFrame( void )
{
    Mat temp;
    
    g_cam >> temp;
    resize(temp, g_current_frame, Size(), DECIMATION_FACTOR, DECIMATION_FACTOR,
            INTER_NEAREST);
}

void PattyFactory_setBgFromCam ( void )
{
    g_bg = g_current_frame.clone();
}

void PattyFactory_setFgFromCam ( void )
{
    g_fg = g_current_frame.clone();
}

void PattyFactory_setBackProjFromCam ( void )
{
    g_src = g_current_frame.clone();
}

/* Warning: vomit-inducing mixture of C and C++                 */
/* programming in the problem domain is for eggheads anyways... */
static Mat PattyFactory_getBlobs_bg_subtract( void )
{
    Mat diff;
    Mat binary;

    // get absolute value difference between background and foreground
    absdiff(g_fg, g_bg, diff);

    // convert result to grayscale
    cvtColor(diff, diff, COLOR_BGR2GRAY);

    // reject high-frequency content
    GaussianBlur(diff, diff, Size(), BG_SUBTRACT_BLUR_SIGMA * DECIMATION_FACTOR);

    // find pronounced differences in the image
    threshold(diff, binary, BG_SUBTRACT_THRESHOLD, 255.0, THRESH_BINARY);

    return (binary);
}

void PattyFactory_setHistFromFile( const gchar * filename )
{
    Mat src;
    Mat hue;

    // read image from file
    PattyFactory_decimateMatFromFile(src, filename);

    // convert to HSV and extract Hue only
    hue = PattyFactory_hueFromRgb(src);

    // calculate the histogram
    calcHist(&hue, 1, 0, Mat(), g_hist, 1, &g_histSize, &g_ranges, true,
            false);
    normalize(g_hist, g_hist, 0, 255, NORM_MINMAX, -1, Mat());
}

static Mat PattyFactory_getBlobs_back_project( void )
{
    Mat hue;
    Mat backproj;

    // convert to HSV and extract Hue only
    hue = PattyFactory_hueFromRgb(g_src);

    // perform back-projection
    calcBackProject(&hue, 1, 0, g_hist, backproj, &g_ranges, 1, true);

    // reject high-frequency content
    GaussianBlur(backproj, backproj, Size(), BACK_PROJECT_BLUR_SIGMA);

    // find pronounced differences in the image
    threshold(backproj, backproj, BACK_PROJECT_THRESHOLD, 255.0, THRESH_BINARY);

    return (backproj);
}

GSList * PattyFactory_getPattyList( enum DETECTION_METHOD method )
{
    Mat pattyBlobs;
    GSList * pattyList = NULL;

    puts("applying method");
    switch (method)
    {
        case BG_SUBTRACT:
            g_ui_post = g_fg.clone();
            pattyBlobs = PattyFactory_getBlobs_bg_subtract();
            break;

        case BACK_PROJECT:
            g_ui_post = g_src.clone();
            pattyBlobs = PattyFactory_getBlobs_back_project();
            break;
    }

    // show the blobs as "pre"
    puts("converting blobs to bgr");
    cvtColor(pattyBlobs, g_ui_post, COLOR_GRAY2BGR);

    // create patties for each blob in the image
    puts("getting list from blobs");
    pattyList = PattyFactory_getPattyListFromBlobs(pattyBlobs);

    return (pattyList);
}

Mat PattyFactory_getPreImage( void )
{
    return (g_current_frame);
}

Mat PattyFactory_getPostImage( void )
{
    return (g_ui_post);
}

