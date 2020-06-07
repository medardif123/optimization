#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#define Width 1400
#define Height 1000
#define scaling1 true
#define scaling2 false
int n = 1;
float beta = 0.9;
float stayblinskiTank(std::vector <float> x) 
{
    float f=0.0;
    for (int i = 0; i < n; i++) {
        f +=glm::pow(x[i], 4) - 16 * glm::pow(x[i], 2) - 5 * x[i];
    }
    return 0.5*f;
}
std::vector<float> grad(std::vector <float> x) 
{
    std::vector<float> g;
    for (int i = 0; i < n; i++) 
    {
        g.push_back(0.5 * (4 * glm::pow(x[i], 3) - 32 * x[i] - 5));
    }
    return g;
}
std::vector <float> multi(std::vector <float> x, float coeff) 
{
    std::vector <float> mult;
    for (int i = 0; i < n; i++) 
    {
        mult.push_back(x[i]*coeff);
    }
    return mult;
}
std::vector <float> sum(std::vector <float> x, std::vector <float> y)
{
    std::vector <float> s;
    for (int i = 0; i < n; i++) 
    {
        s.push_back(x[i]+y[i]);
    }
    return s;
}
float vectMult(std::vector<float> vec1, std::vector<float> vec2) 
{
    float mult=0;
    for (int i = 0; i < n; i++) 
    {
        mult += vec1[i] * vec2[i];
    }
    return mult;
}
float armijo(std::vector<float> x, float alpha, std::vector<float> g0, std::vector<float> d) 
{
    float f0 = stayblinskiTank(x);
    float f1 = stayblinskiTank(sum(x, multi(d, alpha)));
    if (f1 < f0 + alpha * beta * vectMult(g0, d)) 
    {
        while(f1 < f0 + alpha * beta * vectMult(g0, d))
        {
            alpha *= 2.0;
            std::cout << alpha << std::endl;
            f1 = stayblinskiTank(sum(x, multi(d, alpha)));
        }
    }
    else if (f1 > f0 + alpha * beta * vectMult(g0, d)) 
    {
        while (f1 > f0 + alpha * beta * vectMult(g0, d)) 
        {
            alpha /= 2.0;
            std::cout << alpha << std::endl;
            f1 = stayblinskiTank(sum(x, multi(d, alpha)));
        }
    }
    return alpha;
}

float xCoords(float x) 
{
    return x/20;
}
float yCoords(float y)
{
    return y/45;
}
float yMax = 100;
float yMin = -100;
float yTransform(float x) 
{
    return (2 * x / (yMax - yMin)) - ((yMax + yMin) / (yMax - yMin));
}

float xMin = -100;
float xMax = 100;
float xTransform(float x)
{
    return (2 * x / (xMax - xMin)) - ((xMax + xMin) / (xMax - xMin));
}

#define c(a) xCoords(a)
#define d(a) yCoords(a)

int main(void)
{
    float alpha = 1.0;
    float epsilon = 0.01;
    std::vector<float> x;
    x.push_back(0);
    std::vector<float> d;
    d = multi(grad(x), -1);
    std::vector<float> g0;
    g0 = grad(x);
    alpha = armijo(x, alpha, g0, d);
    std::vector<std::vector<float>> points;
    points.push_back(x);
    while (vectMult(d, d)>epsilon) 
    {
        for(int i=0;i<n;i++)
        {
            x[i] += alpha * d[i];
            std::cout << x[i] << std::endl;
        }
        points.push_back(x);
        d = multi(grad(x), -1);
        g0 = grad(x);
        alpha = armijo(x, alpha, g0, d);
    }
    for(int i=0;i<n;i++)
    {
        std::cout << x[i] << std::endl;
    }
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Width, Height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "error occured while initializing the glew library" << std::endl;
    }
    std::cout << xTransform(-100) << std::endl;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, Width,Height);
        glLineWidth(1);
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, -1.0, 0.0);
        glVertex3f(0.0,1.0,0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glVertex3f(-1.0, 0.0, 0.0);
        glEnd();
        std::vector<glm::fvec2> x;
        std::vector<float> x1;
        if (scaling2) 
        {
            for (float i = xMin; i < xMax; i += 0.001)
            {
                //x1.push_back(i);
                float y = glm::acos(i);
                if (y > yMax)
                    yMax = y;
                if (y < yMin)
                    yMin = y;
                x.push_back(glm::fvec2(i, y));
                //x1.pop_back();
                //glVertex3f(c(i), d(glm::cos(i)), 0.0);
                //glVertex3f(c(i), d(glm::sin(i)), 0.0);
            }
            //std::cout << yMin << std::endl;
            glPointSize(2);
            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_POINTS);
            for (int i = 0; i < x.size(); i++)
            {
                glVertex3f(xTransform(x[i].x), yTransform(x[i].y), 0.0);
            }
            glEnd();
        }
        if (scaling1) 
        {
            glPointSize(6);
            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_POINTS);
            for (float i = -100; i < 100;i+=0.0001) 
            {
                x1.push_back(i);
                glVertex3f(c(i), d(stayblinskiTank(x1)), 0.0);
                x1.pop_back();
            }
            glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_POINTS);
            glPointSize(20);
            for (int i = 0; i < points.size(); i++) {
                std::vector<float> point;
                point.push_back(points[i][0]);
                glVertex3f(c(points[i][0]), d(stayblinskiTank(point)), 0.0);
                point.pop_back();
            }
            glEnd();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}