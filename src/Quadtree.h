/** \file Quadtree.h
 *  \brief Core file declaring the data structure.
 *
 * File containing declaration of \link IRO_Point2D \endlink,
 * \link QuadtreeException \endlink and \link Quadtree \endlink.
 *
 * The \link Quadtree_node \endlink is defined in \link Quadtree.cpp \endlink to
 * make it invisible to the user.
 */

/** \mainpage Point Region Quadtree
 *
 * Author Jens Åkerblom \n
 * Implemented as a part of the Datastructure Project course spring 2011 at the University of
 * Helsinki, faculty of Science \n
 * Project supervisor: Henning Lübbers \n
 * \section intro_sec Introduction
 *   This is the doxygen generated API for my Point Region Quadtree.
 *   For full documentation, read the corresponding document.
 * \section install_sec Installing
 *   In this package you should have the core files Quadtree.cpp and Quadtree.h,
 *   the files for testing and the make files.
 *   \subsection install_sec_ide Installing with Code::Blocks
 *     To install this package with Code::Blocks, use the
 *     Code::Blocks project file and set the build target to be compatible with your machine
 *     (targets are WinDebug, WinRelease, UnixDebug, UnixRelease). The debug targets
 *     will show additional debugging information when using the tree, since this might
 *     flush the console output with alot of information you can compile it using a release
 *     target. The debug targets have additional safety checks, such as asserts. The exceptions
 *     will be thrown in all build targets.
 *  \subsection install_sec_con Installing using make
 *     If you don't have Code::Blocks you can build the application using one of the make files.
 *     Choose the one compatible with your system.
 *
 */

#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector> //Used ONLY for returning data, data is stored in C-style arrays (see Quadtree_node).

/** \class IRO_Point2D
 *  \brief Interface for Read-Only 2D point.
 *
 * All data stored must implement this interface.
 */
class IRO_Point2D
{
    public:
        virtual float getX() const = 0;
        virtual float getY() const = 0;
};

class Quadtree_node; //Defined inside implementation.

#ifdef _DEBUG //General debugging.
#   include <iostream>
    using namespace std;
#   define _DEBUG_QUADTREE //Quadtree debugging.
#endif

#include <ostream>
#include <string>
#include <exception>

/** \class QuadtreeException
 *  \brief Exception class used by \link Quadtree \endlink.
 */
class QuadtreeException : public std::exception
{
    public:
        /**
         * Creates the exception.
         * @param mess The exception message.
         */
        QuadtreeException(std::string mess) throw()
        :   m_mess(mess) {}
        ~QuadtreeException() throw() {}

        const char *what() const throw()
        { return m_mess.c_str(); }

        /**
         * Thrown when trying to access something outside the scene.
         */
        static const QuadtreeException QE_outOfBound;
        /**
         * Thrown when the tree can't find a node that's supposed to be in it.
         */
        static const QuadtreeException QE_badSearch;
        /**
         * Thrown when the search rectangle is defined wrongly.
         */
        static const QuadtreeException QE_badRect;

    private:
        const std::string m_mess;
};

/** \class Quadtree
 *  \brief Main class of project.
 *
 * This is the Point Region Quadtree.
 */
class Quadtree
{
    public:
        /**
         * Creates the tree.
         * Once the tree is created the dimensions can't be changed.
         *
         * @param left      Left x-coordinate.
         * @param width     Width of scene.
         * @param down      Down y-coordinate.
         * @param height    Height of scene.
         * @param maxDepth  Max depth of each node (maximum subdivisions of root region).
         */
        Quadtree(float, float, float, float, int);
        /**
         * Destructor.
         * Deallocates the tree and all of its nodes (but not the data).
         */
        ~Quadtree();

        /**
         * Adds a point to the scene.
         *
         * @param posPtr Point to be added.
         */
        void addPos(IRO_Point2D *);
        /**
         * Removes a point from the scene.
         * Will throw \link QuadtreeException::QE_badSearch \endlink if it cannot find point
         * where it's supposed to be.
         *
         * @param posPtr Position to be removed.
         */
        void removePos(IRO_Point2D *);
        /**
         * Updates a point, must be called directly after change in position.
         * It is faster to remove a point, move the point and then add the point
         * to the scene again.
         *
         * @param posPtr Point to be updated.
         */
        void updatePos(IRO_Point2D *);

        /**
         * Returning content in smalles region containing the point.
         * Not very usefull method since it requires the user to
         * understand how the tree subdivied the scene.
         *
         * @param x X-coordinate.
         * @param y Y-coordinate.
         * @return  The content at the smallest subregion of point.
         */
        std::vector<IRO_Point2D *> getContentAt(float, float)                     const;

        /**
         * Returning content in a rectangular area.
         *
         * @param left  Left x-coordinate of rectangle.
         * @param down  Down y-coordinate of rectangle.
         * @param right Right x-coordinate of rectangle.
         * @param up    Up y-coordinate of rectangle.
         * @return      The content inside the rectangle.
         */
        std::vector<IRO_Point2D *> getContentInRect(float, float, float, float)   const;

        friend std::ostream &operator<<(std::ostream &, const Quadtree &);

    private:
        /**
         * Returns the node at the specified location.
         *
         * @param x X-coordinate of location.
         * @param y Y-coordinate of location.
         * @return  The node at the specified location.
         */
        Quadtree_node *getLeafAt(float, float)      const;
        /**
         * Returns the parent of the specified node.
         *
         * @param node The node.
         * @return     The parent of the node.
         */
        Quadtree_node *getParent(Quadtree_node *)   const;
        /**
         * Does a tree search to find a node.
         * The implemented search is depth first.
         *
         * @return The node if found, else null (0).
         */
        Quadtree_node *find(IRO_Point2D *)          const;

        /**
         * A link to the root of the tree.
         */
        Quadtree_node *m_root;
        /**
         * Maximum subdivisions of the tree.
         */
        const int      m_maxDepth;
};

std::ostream &operator<<(std::ostream &, const Quadtree &);

#endif
