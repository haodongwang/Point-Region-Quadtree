/** \file autoTest.cpp
 *  \brief Definition of automatic testing module.
 *
 *  Tests the Point Region Quadtree with non-interactive functions.
 */

#include "autoTest.h"

#include "Quadtree.h"

#include <iostream>
using namespace std;

#include <cstdlib>
#include <cstdio>
#include <cmath>

#define SQUARE(a)   ( (a)*(a) )

#define PAUSE()     { int c; c = getchar(); }

/** \class Vector2
 *  \brief Standard 2D vector.
 *
 * Used in automated test.
 */
class Vector2 : public IRO_Point2D
{
    public:
        /**
         * Creates the vector with the initial value.
         */
        Vector2(float a, float b): x(a), y(b) {}

        ///Standard vector addition.
        const Vector2 &operator+(const Vector2 &vec) { return Vector2(x + vec.x, y + vec.y); }
        ///Standard vector substraction.
        const Vector2 &operator-(const Vector2 &vec) { return Vector2(x - vec.x, y - vec.y); }
        ///Scalar multiplication.
        const Vector2 &operator*(float f)            { return Vector2(x * f, y * f); }
        ///Scalar division.
        const Vector2 &operator/(float f)            { return Vector2(x / f, y / f); }
        ///Dot product.
        float operator*(const Vector2 &vec)          { return x * vec.x + y * vec.y; }

        const Vector2 &operator+=(const Vector2 &vec) { x += vec.x; y += vec.y; return *this; }
        const Vector2 &operator-=(const Vector2 &vec) { x -= vec.x; y -= vec.y; return *this; }
        const Vector2 &operator*=(float f)            { x *= f; y *= f;         return *this; }
        const Vector2 &operator/=(float f)            { x /= f; y /= f;         return *this; }

        ///Copy operator.
        const Vector2 &operator=(const Vector2 &vec)  { x = vec.x; y = vec.y; return *this; }

        ///Gets the squared length.
        float lenSquared() { return SQUARE(x) + SQUARE(y); }
        ///Gets the length.
        float len()        { return sqrt(SQUARE(x) + SQUARE(y)); }

        ///Normalizes the vector.
        void normalize()   { *this /= len(); }

        float getX() const { return x; }
        float getY() const { return y; }

        ///X-coordinate of vector.
        float x;
        ///Y-coordinate of vector.
        float y;
};

//Testing add and remove operations.
void testAddRemove()
{
    cout << "----Test \"Add / Remove\"---- BEGIN" << endl
         << "\tTesting add and remove operations." << endl << endl;

    {
        PAUSE();
        cout << "----> Test part 1: \"New tree\"" << endl
             << "\tShould show empty tree." << endl;
        PAUSE();

        Quadtree testTree(-15.0f, 50.0f, 8.0f, 120.5f, 6);    //Empty tree.
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 2: \"Adding one node (1.5, 10.0)\"" << endl
             << "\tNew vector should place itself in root." << endl;
        PAUSE();

        Vector2 pos(1.5f, 10.0f);
        testTree.addPos(&pos);      //Only root should exist.
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 3: \"Adding another node (3.5, 10.0)\"" << endl
             <<"\tNew vector should place itself in seperate node." << endl;
        PAUSE();

        Vector2 pos2(3.5f, 10.0f);
        testTree.addPos(&pos2);     //Should subdivide but be put in different regions.
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 4: \"Adding node on same location (3.5, 10.0)\"" << endl
             <<"\tNew vector should place itself at max depth (=6) in same node as previous (3.5, 10.0)." << endl;
        PAUSE();

        Vector2 pos3(3.5f, 10.0f);
        testTree.addPos(&pos3);     //Should subdivide to maximum.
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 5: \"Removing node last inserted node\"" << endl
             <<"\tShould produce same output as \"Test part 3\"." << endl;
        PAUSE();

        testTree.removePos(&pos3);
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 6: \"Removing node (3.5, 10.0)\"" << endl
             <<"\tShould produce same output as \"Test part 2\"." << endl;
        PAUSE();

        testTree.removePos(&pos2);
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 7: \"Removing node (1.5, 10.0)\"" << endl
             <<"\tShould produce same output as \"Test part 1\"." << endl;
        PAUSE();

        testTree.removePos(&pos);
        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 8: \"Trying to trigger exception\"" << endl
             <<"\tShould throw QE_outOfBound exception when adding outside region." << endl;
        PAUSE();

        pos = Vector2(-100, -100);
        try
        {
            testTree.addPos(&pos);
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
        }

        PAUSE();
        cout << "----> Test part 9: \"Trying to trigger exception\"" << endl
             <<"\tShould throw QE_badSearch exception when removing node not in tree." << endl;
        PAUSE();
        try
        {
            testTree.removePos(&pos2);
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
        }
    }

    cout << "----Test \"Add / Remove\"---- END" << endl;
    PAUSE();
}

//Testing moving vector and updating tree.
void testMove()
{
    cout << "----Test \"Move\"---- BEGIN" << endl
         << "\tTesting updating operations." << endl << endl;

    {
        Quadtree testTree(-10, 20, -10, 20, 5);

        Vector2 pos1(-1, -1), pos2(0, 0), pos3(1, 1);

        testTree.addPos(&pos1);
        testTree.addPos(&pos2);
        testTree.addPos(&pos3);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 1: \"Move from one region to another\"" << endl
             << "\tShould change the structure of the tree but not merge old branch or subdivide new." << endl
             << "\t(-1, -1) -> (1, 0)" << endl;
        PAUSE();

        pos1 += Vector2(2, 1);
        testTree.updatePos(&pos1);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 2: \"Move within same region\"" << endl
             << "\tShould not change the structure of the tree." << endl
             << "\t(1, 0) -> (1.1, 0)" << endl;
        PAUSE();

        //No updating needed move.
        pos1 += Vector2(.1f, 0);
        testTree.updatePos(&pos1);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 3: \"Move to merge old branch and subdivide new\"" << endl
             << "\tShould merge current branch and subdivide a new" << endl
             << "\t(0, 0) -> (-5, -5)" << endl
             << "\t(1.1, 0) -> (-5, 7)" << endl
             << "\t(1, 1) -> (-5, 7.01)" << endl;
        PAUSE();

        // !!!YOU MUST UPDATE THE TREE DIRECTLY AFTER CHANGING VECTOR!!!
        // !!!   IF NOT THE findLeaf METHOD MIGHT NOT LOCATE VECTOR  !!!

        pos1 = Vector2(-5, 7);      testTree.updatePos(&pos1);
        pos2 = Vector2(-5, -5);     testTree.updatePos(&pos2);
        pos3 = Vector2(-5, 7.01);   testTree.updatePos(&pos3);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 4: \"Move back to original positions\"" << endl
             << "\tShould produce same output as in beginning of test." << endl;
        PAUSE();

        pos1 = Vector2(-1, -1);  testTree.updatePos(&pos1);
        pos2 = Vector2( 0,  0);  testTree.updatePos(&pos2);
        pos3 = Vector2( 1,  1);  testTree.updatePos(&pos3);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 5: \"Trying to trigger exception\"" << endl
             <<"\tShould throw QE_outOfBound exception when updating outside region." << endl;
        PAUSE();

        pos1 = Vector2(-100, -100);
        try
        {
            testTree.addPos(&pos1);
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
        }

        PAUSE();
        cout << "----> Test part 6: \"Trying to trigger exception\"" << endl
             <<"\tShould throw QE_badSearch exception when updating node not in tree." << endl;
        PAUSE();
        try
        {
            Vector2 tempPos(1, 1);
            testTree.removePos(&tempPos);
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
        }
    }
    cout << "----Test \"Move\"---- END" << endl;
    PAUSE();
}

//Testing Quadtree::getContentAt(float, float).
void testGet()
{
    cout << "----Test \"Get\"---- BEGIN" << endl
         << "\tTesting getting vector based on smalles region extraction (getAt(x, y))." << endl << endl;
    {
        vector<IRO_Point2D *> posVec;

        Quadtree testTree(-10, 20, -10, 20, 5);
        Vector2 pos1(.1, .1), pos2(5, 5), pos3(5.01, 5);

        testTree.addPos(&pos1);
        testTree.addPos(&pos2);
        testTree.addPos(&pos3);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 1: \"Get from empty region\"" << endl
             << "\tShould return an empty vector." << endl
             << "\tGetting at (-0.9, -0.9)" << endl;
        PAUSE();

        posVec = testTree.getContentAt(-.9, -.9);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 2: \"Get from occupied region\"" << endl
             << "\tShould return an vector containing Vector2 at (0.1, 0.1)." << endl;
        PAUSE();

        posVec = testTree.getContentAt(.1f, .1f);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 3: \"Get from occupied region\"" << endl
             << "\tShould return an vector containing two Vector2." << endl;
        PAUSE();

        posVec = testTree.getContentAt(5, 5);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;
    }
    cout << "----Test \"Get\"---- END" << endl;
    PAUSE();
}

//Testing Quadtree::getContentInRect(float, float, float, float);
void testGetRect()
{
    cout << "----Test \"Get in rectangle\"---- BEGIN" << endl
         << "\tTesting getting vectors from an arbitrary rectangular area." << endl << endl;
    {
        vector<IRO_Point2D *> posVec;

        Quadtree testTree(-10, 20, -10, 20, 5);

        Vector2 pos1(-5, -5), pos2(0, 0), pos3(5, 5);

        testTree.addPos(&pos1);
        testTree.addPos(&pos2);
        testTree.addPos(&pos3);

        cout << testTree << endl;

        PAUSE();
        cout << "----> Test part 1: \"Get from empty region\"" << endl
             << "\tShould return an empty vector." << endl
             << "\tGetting at (-4, 1, -1, 9)" << endl;
        PAUSE();

        posVec = testTree.getContentInRect(-4, 1, -1, 9);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 2: \"Get from partial regions\"" << endl
             << "\tShould return a vector at (-5, -5)." << endl
             << "\tGetting at (-9, -8, -1, 9)" << endl;
        PAUSE();

        posVec = testTree.getContentInRect(-9, -8, -1, 9);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 3: \"Get from mixed regions\"" << endl
             << "\tShould return vectors (0, 0) and (5, 5)." << endl
             << "\tGetting at (-1, -1, 6, 6)" << endl;
        PAUSE();

        posVec = testTree.getContentInRect(-1, -1, 6, 6);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 4: \"Getting whole scene (with marginals)\"" << endl
             << "\tShould return all vectors." << endl
             << "\tGetting at (-11, -11, 11, 11)" << endl;
        PAUSE();

        posVec = testTree.getContentInRect(-11, -11, 11, 11);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 5: \"Getting outside scene\"" << endl
             << "\tShould return no vectors." << endl
             << "\tGetting at (11, 11, 20, 20)" << endl;
        PAUSE();

        posVec = testTree.getContentInRect(11, 11, 20, 20);

        cout << "Content: \"";
        for (int i = 0; i < posVec.size(); i++)
            cout << "(" << posVec[i]->getX() << ", " << posVec[i]->getY() << ") ";
        cout << "\"" << endl;

        PAUSE();
        cout << "----> Test part 6: \"Trying to trigger exception\"" << endl
             << "\tShould throw QE_badRect exception." << endl;
        PAUSE();

        try
        {
            posVec = testTree.getContentInRect(-4, -8, -8, 9);
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
        }
    }
    cout << "----Test \"Get in rectangle\"---- END" << endl;
    PAUSE();
}
