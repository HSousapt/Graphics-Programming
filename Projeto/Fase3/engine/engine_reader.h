#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

using namespace std;

//Struct that represents a point
struct Point
{
    float x;
    float y;
    float z;
};

//Geometric Transformations types
enum type
{
    ROTATE,
    TRANSLATE,
    SCALE,
    ROTATE_ANIM,
    TRANSLATE_ANIM
};

//Struct that represents a Geometric Transformation
struct gt
{
    struct Point p;
    enum type gt_type;
    float r_angle;
    float rotate_time;
    float translate_time;
    vector<Point> control_points;
};

//Struct that represents a group and its subgroups of models and their associated geometric transformations
struct group
{
    vector<struct gt> gt;
    vector<vector<struct Point>> models;
    vector<struct group> child;
    vector<struct Point> colors;
};

//Struct that represents a scene as a vector of groups and and int that tells us how many models the scene has
struct scene
{
    vector<struct group> groups;
    int nModels;
};

//Fuction that draws the geometric transformations
void draw_gt(struct group group, int);

//Loads the data from the .xml file and stores it in the struct scene
void load_scene(struct scene *, TiXmlElement *);

//Draws the models without VBO's
void draw_models(struct group);

//Draws the color
void draw_color(struct Point);