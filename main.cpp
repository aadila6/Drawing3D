

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "tk.h"
#endif

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include "vector.hpp"

typedef int OutCode;
constexpr int INSIDE = 0; // 0000
constexpr int LEFT = 1;   // 0001
constexpr int RIGHT = 2;  // 0010
constexpr int BOTTOM = 4; // 0100
constexpr int TOP = 8;    // 1000
/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;
/*Window information*/
int win_height;
int win_width;
float* Buffer;
struct Coordinate
{
    int x, y;
    Coordinate(int inputX, int inputY)
    {
        x = inputX;
        y = inputY;
    }
};

struct Polygon{
    //Center of mass is 0 respect to polygon itself
    //But position vector is the centroid from the viewPort
    int count;
    vmml::vector<3, float> position;
    std::vector<vmml::vector<3, float>> vertices;
    Polygon(std::vector<vmml::vector<3, float>> vertices, 
            vmml::vector<3, float> pos){
        position = pos;
        this->vertices = vertices;
        count = vertices.size();
    }
    Polygon(std::vector <Coordinate> vert){
        float xtotal,ytotal;
        count = vert.size();
        for(int i = 0; i<vert.size(); i++){
            xtotal +=vert[i].x;
            ytotal +=vert[i].y;
            vertices.push_back(
                vmml::vector<3, float>(vert[i].x,vert[i].y,1));
        }
        position = {xtotal/vert.size(),ytotal/vert.size(),1};
        for (int i = 0; i < vert.size(); i++){
            vertices[i] -= position;
            std::cout << vertices[i] << std::endl;
        }
    }
        
};
//Need to redefine my drawPix
void draw_pix(int x, int y)
{
    glBegin(GL_POINTS);
    glColor3f(.2, .2, 1.0);
    glVertex3f(x + .5, y + .5, 0);
    glEnd();
}
void display()
{
	//Misc.
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	//draws pixel on screen, width and height must match pixel buffer dimension
	//glDrawPixels(width, height, GL_RGB, GL_FLOAT, PixelBuffer);
	//window refresh
	glFlush();
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    //check();
}
void readinput(char *filename, std::vector<Polygon> polygons){
    //std::vector<PolygonBorderPixels> PolygonBorderPixelsList;
    std :: ifstream inputFile(filename);
    std :: string line;
    int count;
    inputFile >> count;
    getline(inputFile, line);
    getline(inputFile, line);
    for (int i=0; i< count; i++){
        int num;
        //std::vector<Coordinate> coorList;
        inputFile >> num;
        getline(inputFile, line);
        for (int j=0; i<num; j++){
            float x, y;
            std :: string inputX, inputY;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX, ' ');
            getline(record, inputY);
            x = stof(inputX);
            y = stof(inputX);
            //Coordinate point(x,y);
            //coorList.push_back(point);
        }
        //PolygonBorderPixels PolygonBorderPixels(num,coorList);
        //PolygonBorderPixelsList.push_back(PolygonBorderPixels);
        getline(inputFile, line);
    }
}
void drawLineDDA(vmml::vector<3, float> start, vmml::vector<3, float> end, bool* buffer )
{
    // Coordinate pixels[] = {start, end};
    //First figure out the direction
    int dx = -(start.x() - end.x());
    int dy = -(start.y() - end.y());
    float m = (float)dy / dx;
    float j = 0.0;
    float i = 0.0;

    if ((fabs(m)) < 1)
    { //Shallow, calculating Y
        if (m < 0)
        {
            if (start.y() < end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int in = start.x(); in > end.x(); in--)
            {
                j = (in - start.x()) * m + start.y();
                draw_pix(in, (int)j);
                //Coordinate point(in,(int)j);
                buffer[in * grid_width + (int)j]=true;
            }
        }
        else if (m == 0 || dy == 0)
        {
            if (start.x() > end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int inn = start.x(); inn < end.x(); inn++)
            {
                //j = (i-start.x)*m + start.y;
                draw_pix(inn, end.y());
                //Coordinate point(inn,end.y);
                //vertice.push_back(point);
                buffer[inn*grid_width+(int)j]=true;
            }
        }
        else
        {
            if (start.y() > end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int im = start.x(); im < end.x(); im++)
            {
                j = (im - start.x()) * m + start.y();
                draw_pix(im, (int)j);
                //Coordinate point(im,(int)j);
                //vertice.push_back(point);
                buffer[im* grid_width+(int)j]=true;
            }
        }
    }
    else
    { //Steep, calculating X
        m = (float)dx / dy;
        if (m < 0)
        {
            if (start.y() < end.y())
            {
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int jn = start.y(); jn >= end.y(); jn--)
            {
                i = start.x() + (jn - start.y()) * m;
                draw_pix((int)i, jn);
                //Coordinate point(i,(int)jn);
                //vertice.push_back(point); 
                buffer[(int)i*grid_width+jn]=true;
            }
        }
        else
        {
            if (start.y() > end.y()){
                int tempx = start.x();
                int tempy = start.y();
                start.x() = end.x();
                start.y() = end.y();
                end.x() = tempx;
                end.y() = tempy;
            }
            for (int jm = start.y(); jm < end.y(); jm++)
            {
                int ii = (int)(start.x() + (jm - start.y()) * m);
                draw_pix(ii, jm);
                //Coordinate point((int)i, jm);
                buffer[ii*grid_width + jm] = true;
                //vertice.push_back(point); 
            }
        }
    }
}
//Algorithm from class notes & textbook 
void drawLineBresenham(Coordinate start, Coordinate end)
{
    float m = (end.y - start.y) / (end.x - start.x);
    
    int x,y;
    if (fabs(m) < 1) {
        int dx = fabs(end.x - start.x),
            dy = fabs(end.y - start.y),
            p = 2 * dy - dx;
        if (start.x > end.x)
        {
            x = end.x;
            y = end.y;
            end.x = start.x;
        }else{
            x = start.x;
            y = start.y;
        }
        draw_pix(x, y);
        while (x < end.x)
        {
            x++;
            if (p < 0)
            {
                p = p + 2 * dy;
            }
            else
            {
                y++;
                p = p + 2 * dy - 2 * dx;
            }
            draw_pix(x, y);
        }
    } else if (fabs(m) >= 1) {
        int dx = fabs(end.x - start.x),
            dy = fabs(end.y - start.y),
            p = 2 * dx - dy;
        if (start.y > end.y)
        {
            x = end.x;
            y = end.y;
            end.y = start.y;
        }
        else
        {
            x = start.x;
            y = start.y;
        }
        while (y < end.y)
        {
            y++;
            if (p < 0)
            {
                p = p + (2 * dx);
            }else{
                if(m<0){
                    x--;
                }else{
                    x++;
                }
        
                p = p + (2 * dx) - (2 * dy);
            }
             draw_pix(x, y);
        }
     } 
}





//最后是main
int main(int argc, char *argv[])
{
    float angle;
    int iD;
    float translationX, translationY , sFactor;

    std::cout << "Please enter width of window: " ;
    std::cin>> win_width;
    std::cout << "Please enter height of window: ";
    std::cin>> win_height;
    std::vector<Polygon> polygonList;
    readinput(argv[1], polygonList);
    std::cout << "Please enter Polygon ID such as 0,1,2..: ";
    std::cin >>iD;
    std::cout << "Please enter rotation angle: ";
    std::cin>> angle;
    std::cout << "Please enter translation in x diretction: ";
    std::cin>> translationX;
    std::cout << "Please enter translation in y diretction: " ;
    std::cin>> translationY;
    std::cout << "Please enter scalling factor: " ;
    std::cin>> sFactor;
    Buffer = new float[win_width * win_height];
    


}
