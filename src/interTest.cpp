/** \file interTest.cpp
 *  \brief Definition of interactive testing module.
 *
 *  Interactivelly tests the Point Region Quadtree.
 */

#include "interTest.h"

#include "Quadtree.h"
#include <map>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;

#ifdef WIN
#   define CLEAR_SCR() system("CLS");
#elif defined UNIX
#   define CLEAR_SCR() system("CLEAR");
#else
#   error("Only support for windows and unix environments")
#endif

#define PAUSE()     { int c; c = getchar(); }

#define PRINT_TREE   1
#define ADD_PLACE    2
#define REMOVE_PLACE 3
#define MOVE_PLACE   4
#define PRINT_PLACES 5
#define GET_AT       6
#define GET_IN_RECT  7

#define EXIT_TEST    0

/** \class Place
 *  \brief Used in interactive testing.
 *
 * Stores a named place (point and string).
 */
class Place : public IRO_Point2D
{
    public:
        /**
         * \brief Creates a place.
         */
        Place(float x, float y, string name)
        : m_x(x), m_y(y), m_name(name)
        {}

        float getX() const  { return m_x; }     ///< Gets the x-coordinate.
        float getY() const  { return m_y; }     ///< Gets the y-coordinate.
        void setX(float x)  { m_x = x; }        ///< Sets the x-coordinate.
        void setY(float y)  { m_y = y; }        ///< Sets the y-coordinate.
        string getName()    { return m_name; }  ///< Returns the name.

    private:
        float m_x;
        float m_y;
        string m_name;
};

void startInteractiveTest()
{
    float l, w, d, h;
    int mD;

    cout << "\tQuadtree testing - Interactive test" << endl << endl
         << "Enter parameters for new tree (left, width, down, height, max depth)" << endl;
    cin >> l >> w >> d >> h >> mD;

    {
        Quadtree testTree(l, w, d, h, mD);
        map<string, Place *> places;
        int testOpt;

        CLEAR_SCR();
        try
        {
            do
            {
                cout << "\tQuadtree testing - Interactive test" << endl << endl
                     << "1. Print testing tree" << endl
                     << "2. Add new place" << endl
                     << "3. Remove place" << endl
                     << "4. Move place" << endl
                     << "5. Print places" << endl
                     << "6. Get place at coordinate" << endl
                     << "7. Get places in area" << endl << endl
                     << "0. Exit interactive test" << endl << endl
                     << "Enter option: ";
                cin >> testOpt;

                CLEAR_SCR();
                switch (testOpt)
                {
                    case PRINT_TREE:
                    {
                        cout << testTree;
                        PAUSE(); PAUSE(); //Needs two since "\n" is still in cin.
                        break;
                    }
                    case PRINT_PLACES:
                    {
                        cout << "Allocated places:" << endl;
                        for (map<string, Place *>::iterator it = places.begin();
                             it != places.end();
                             it++)
                        {
                            cout << "\t" << (*it).second->getName()
                                 << "\t(" << (*it).second->getX() << ", "
                                          << (*it).second->getY() << ")" << endl;
                        }
                        PAUSE(); PAUSE();
                        break;
                    }
                    case ADD_PLACE:
                    {
                        float x, y;
                        string name;

                        cout << "Enter name: ";
                        cin >> name;
                        cout << "Coordinate: ";
                        cin >> x >> y;

                        Place *newPlace = new Place(x, y, name);

                        testTree.addPos(newPlace);
                        places[name] = newPlace;
                        PAUSE(); PAUSE();
                        break;
                    }
                    case REMOVE_PLACE:
                    {
                        string name;
                        Place *place;

                        cout << "Enter name: ";
                        cin >> name;

                        place = places[name];
                        testTree.removePos(place);
                        places.erase(name);
                        PAUSE(); PAUSE();
                        break;
                    }
                    case MOVE_PLACE:
                    {
                        string name;
                        Place *place;
                        float x, y;

                        cout << "Enter name: ";
                        cin >> name;
                        cout << "Enter new coordinate: ";
                        cin >> x >> y;

                        place = places[name];
                        place->setX(x);
                        place->setY(y);

                        testTree.updatePos(place);

                        PAUSE(); PAUSE();
                        break;
                    }
                    case GET_AT:
                    {
                        float x, y;

                        cout << "Enter coordinate: ";
                        cin >> x >> y;

                        vector<IRO_Point2D *> points;
                        points = testTree.getContentAt(x, y);

                        cout << "Found: " << points.size() << " places." << endl;
                        for (vector<IRO_Point2D *>::iterator it = points.begin();
                             it != points.end();
                             it++)
                        {
                            cout << static_cast<Place *>(*it)->getName() << " ";
                        }
                        PAUSE(); PAUSE();
                        break;
                    }
                    case GET_IN_RECT:
                    {
                        float x1, y1, x2, y2;
                        vector<IRO_Point2D *> placesInRect;

                        cout << "Enter rectangle (left, down, right, up): ";
                        cin >> x1 >> y1 >> x2 >> y2;

                        placesInRect = testTree.getContentInRect(x1, y1, x2, y2);

                        cout << "Found: " << placesInRect.size() << " places." << endl;
                        for (vector<IRO_Point2D *>::iterator it = placesInRect.begin();
                             it != placesInRect.end();
                             it++)
                        {
                            cout << static_cast<Place *>(*it)->getName() << " ";
                        }
                        PAUSE(); PAUSE();
                        break;
                    }
                }
                CLEAR_SCR();
            }
            while (testOpt != EXIT_TEST);
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
            PAUSE();
            PAUSE();
        }

        for (map<string, Place *>::iterator it = places.begin();
             it != places.end();
             it++)
        {
            delete (*it).second;
        }
        places.clear();
    }
    PAUSE();
}
