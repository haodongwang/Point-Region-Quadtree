/** \file main.cpp
 *  \brief Control file for testing.
 */

#include "interTest.h"
#include "autoTest.h"

#include <iostream>
#include <cstdlib>
using namespace std;

#define EXIT_TEST  0
#define AUTO_TEST  1
#define INTER_TEST 2

#ifdef WIN
#   define CLEAR_SCR() system("CLS");
#elif defined UNIX
#   define CLEAR_SCR() system("CLEAR");
#else
#   error("Only support for windows and unix environments")
#endif

/**
 * \brief Start of program.
 */
int main()
{
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);

    int testOpt;

    do
    {
        cout << "\tQuadtree testing" << endl << endl
             << "1. Start automated test" << endl
             << "2. Start interactive test" << endl << endl
             << "0. To exit" << endl << endl
             << "Enter option: ";
        cin >> testOpt;

        switch (testOpt)
        {
            case AUTO_TEST:
                CLEAR_SCR();
                testAddRemove();
                testMove();
                testGet();
                testGetRect();
                break;

            case INTER_TEST:
                CLEAR_SCR();
                startInteractiveTest();
                break;
        }
        CLEAR_SCR();
    }
    while (testOpt != EXIT_TEST);

    return 0;
}
