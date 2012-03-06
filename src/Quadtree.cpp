/** \file Quadtree.cpp
 *  \brief Core file defining the data structure.
 *
 * File containing definition of \link Quadtree_node \endlink and \link Quadtree \endlink.
 */

#include "Quadtree.h"

#ifndef _DEBUG_QUADTREE
#   define NDEBUG
#endif

#include <cassert>

const QuadtreeException QuadtreeException::QE_outOfBound
("QuadtreeException (OutOfBound):\
 Trying to access position not in tree!\n");
const QuadtreeException QuadtreeException::QE_badSearch
("QuadtreeException (BadSearch):\
 Cannot find point! (Did you forget to update tree directly after moving point?)");
const QuadtreeException QuadtreeException::QE_badRect
("QuadtreeException (BadRect):\
 Search rectangle is incorrectly defined! (Format is (left, down, right, up))");


/** \class Quadtree_node
 *  \brief Node class of the tree.
 *
 * The user can't access the node class directly, instead use the \link Quadtree \endlink class.
 */
class Quadtree_node
{
    public:
        /**
         * Constructor of root.
         *
         * @param l     Left x-coordinate.
         * @param w     Width of scene.
         * @param d     Down y-coordinate.
         * @param h     Height of scene.
         *
         * @see Quadtree
         */
        Quadtree_node(float, float, float, float); //Public ctor to create root node.
        /**
         * Destructor of node.
         * Deletes sub nodes recursivelly.
         */
        ~Quadtree_node();


        float getDown()   const { return down;   }  ///< @return The down y-coordinate of region.
        float getLeft()   const { return left;   }  ///< @return The left x-coordinate of region.
        float getWidth()  const { return width;  }  ///< @return The width of region.
        float getHeigth() const { return height; }  ///< @return The height of region.
        float getDepth()  const { return depth;  }  ///< @return The depth of region.

        /**
         * Gets the center of the region.
         *
         * @param [out] x X-coordinate of center.
         * @param [out] y Y-coordinate of center.
         */
        void  getCenter(float &x, float &y) const
        { x = left + width / 2.0f; y = down + height / 2.0f; }

        static const int START_CHILD = 0; ///< Enumeration of first child.
        static const int NE = 0;          ///< Enumeration of North East child.
        static const int NW = 1;          ///< Enumeration of North West child.
        static const int SW = 2;          ///< Enumeration of South West child.
        static const int SE = 3;          ///< Enumeration of South East child.
        static const int END_CHILD = 3;   ///< Enumeration of last child.

        /**
         * Gets a child of a node.
         *
         * @param e Enumeration of child.
         * @return  The child.
         */
        Quadtree_node *getChild(int e) const { assert( !isLeaf ); return child[e]; }

        /**
         * Checks if node is leaf.
         *
         * @return True if node is not leaf.
         */
        bool hasChildren()                   const { return !isLeaf; }

        /**
         * Checks for data in node.
         *
         * @return True if data is in node, else false.
         */
        bool isInNode(IRO_Point2D *)         const;

        /**
         * Checks if coordinate is inside region.
         *
         * @return True if point is inside region, else false.
         */
        bool isInRegion(float, float)        const;
        /**
         * Checks if y-coordinate is in region.
         *
         * @return True if y-coordinate is inside, else false.
         */
        bool isInInterY(float)               const;
        /**
         * Checks if x-coordinate is in region.
         *
         * @return True if x-coordinate is inside, else false.
         */
        bool isInInterX(float)               const;

        /**
         * Subdivides the node and distributes any data stored.
         */
        void subdivide();
        /**
         * Merges the children of this node recursivelly.
         */
        void merge();

        /**
         * Adds data to the node.
         *
         * @param posPtr Point to be added.
         */
        void addValue(IRO_Point2D *);
        /**
         * Removes data from the node.
         *
         * @param posPtr Point to be removed.
         */
        void removeValue(IRO_Point2D *);
        /**
         * Gets the data stored as a dynamic array.
         *
         * @return The data stored.
         */
        IRO_Point2D **getValues() const { assert( isLeaf ); return val; }

        /**
         * Gets the total amount of points inside region (adds from children recursivelly).
         *
         * @return The amount of points inside the region.
         */
        int getTotalLen() const;  //Recursive method to find out how many values (points) that are in region.
        /**
         * Gets the amount of points in this region (must be leaf).
         *
         * @return The amount of points in the region.
         */
        int getLen() const { assert( isLeaf ); return len; }

        friend std::ostream &operator<<(std::ostream &, const Quadtree_node &);

    private:
        /**
         * Private constructor to create non-root node.
         *
         * @param de    Depth of node.
         * @param l     Left x-coordinate.
         * @param w     Width of scene.
         * @param d     Down y-coordinate.
         * @param h     Height of scene.
         */
        Quadtree_node(int, float, float, float, float);

        /**
         * Stores the node type.
         * True if leaf node, false if interleaved node.
         */
        bool        isLeaf;                     //Leaves store val and len, others store child[4].
        /**
         * Depth of node.
         * Is in range [0, maxDepth].
         */
        const int   depth;                      //Distance from root.
        /**
         * Bounds of region.
         */
        const float left, down, width, height;  //Defines the region rectangle.

        union
        {
            struct
            {
                /**
                 * Children of node.
                 */
                Quadtree_node *child[4];
            };
            struct
            {
                /**
                 * Data stored in leaf.
                 */
                IRO_Point2D **val; //Dynamic array of point pointers.
                /**
                 * Number of data stored in leaf.
                 */
                int len;
            };
        };
};

//Public ctor, creating root.
Quadtree_node::Quadtree_node(float l, float w, float d, float h)
:   left(l), width(w), down(d), height(h), isLeaf(true), depth(0), len(0)
{
#   ifdef _DEBUG_QUADTREE
        cout << "Creating root node" << this << endl;
#   endif
}

//Private ctor, creating node.
Quadtree_node::Quadtree_node(int de, float l, float w, float d, float h)
:   left(l), width(w), down(d), height(h), isLeaf(true), depth(de), len(0)
{
#   ifdef _DEBUG_QUADTREE
        cout << "Creating node " << this << endl;
#   endif
}

//Recursive destructor.
Quadtree_node::~Quadtree_node()
{
#   ifdef _DEBUG_QUADTREE
        cout << "Destroying node " << this << endl;
#   endif

    if ( !isLeaf )
    {
        for (int e = START_CHILD; e <= END_CHILD; e++)
            delete child[e];
    }
    else
    {
        if (len)
            delete[] val;
    }

}

//Checks if point is in node.
bool Quadtree_node::isInNode(IRO_Point2D *posPtr) const
{
    assert( isLeaf );

    for (int i = 0; i < len; i++)
    {
        if (val[i] == posPtr)
            return true;
    }

    return false;
}

//Checks if point (x, y) is inside region.
bool Quadtree_node::isInRegion(float x, float y) const
{
    return ( (x >= left) &&  (x < left + width) && (y >= down) && (y < down + height) );
}

//Checks if x is inside x-intervall of region.
bool Quadtree_node::isInInterX(float x) const
{
    return ( (x >= left) && (x < left + width) );
}

//Checks if y is inside y-intervall of region.
bool Quadtree_node::isInInterY(float y) const
{
    return ( (y >= down) && (y < down + height) );
}

//Adds a point to node. Does not subdivide.
void Quadtree_node::addValue(IRO_Point2D *posPtr)
{
    assert( isLeaf );

#   ifdef _DEBUG_QUADTREE
        cout << "Adding value to node " << this << endl;
#   endif

    IRO_Point2D **tempVal = new IRO_Point2D *[len + 1];

    for (int i = 0; i < len; i++)
        tempVal[i] = val[i];

    tempVal[len] = posPtr;

    if ( len > 0 )
        delete[] val;

    val = tempVal;
    len++;
}

//Removes a point from node. Does not merge. Does not check if param is in node!
void Quadtree_node::removeValue(IRO_Point2D *posPtr)
{
#   ifdef _DEBUG_QUADTREE
        cout << "Removing value from " << this << endl;
#   endif

    assert( isLeaf );
    assert( len > 0 );

    if (len == 1)
    {
        assert( val[0] == posPtr );
        delete[] val;
        len--;
        return;
    }

    IRO_Point2D **tempVal = new IRO_Point2D *[len - 1];

    int j = 0;
    for (int i = 0; i < len; i++)
    {
        if (val[i] != posPtr)
            tempVal[j++] = val[i];  //SEGFAULT if posPtr is not in node.
    }

    delete[] val;

    val = tempVal;
    len--;
}


//Subdivides the region and puts the corresponding values in children's region.
void Quadtree_node::subdivide()
{
#   ifdef _DEBUG_QUADTREE
        cout << "Subdividing " << this << endl;
#   endif

    //Observe: Children are not accessed before turning node to interleaf (isLeaf = false).
    //         If child field would have been accessed before, then fields len and val would
    //         be lost (node is union!).

    Quadtree_node *newChild[4] =
    {
        //NE + +
        new Quadtree_node(depth + 1,
                          left + width  / 2.0f, width  / 2.0f,
                          down + height / 2.0f, height / 2.0f),
        //NW - +
        new Quadtree_node(depth + 1,
                          left                , width  / 2.0f,
                          down + height / 2.0f, height / 2.0f),
        //SW - -
        new Quadtree_node(depth + 1,
                          left,                 width  / 2.0f,
                          down,                 height / 2.0f),
        //SE + -
        new Quadtree_node(depth + 1,
                          left + width  / 2.0f, width  / 2.0f,
                          down,                 height / 2.0f)
    };

    for (int i = 0; i < len; i++)
    {
        for (int e = START_CHILD; e <= END_CHILD; e++)
            if ( newChild[e]->isInRegion(val[i]->getX(), val[i]->getY()) )
                newChild[e]->addValue(val[i]);

    }

    //All values are copied, remove original values.
    if (len)
        delete[] val;

    isLeaf = false;

    for (int e = START_CHILD; e <= END_CHILD; e++)
        child[e] = newChild[e];
}

//Merging child nodes to their parents.
//Is recursive and somewhat slow since the data stored is immutable. One could determine the size of the
//total data in region (total number of points stored inside this region) and do only one new allocation
//of data, but this would be structurally diffucult compared to this simple recursion.
void Quadtree_node::merge()
{
#   ifdef _DEBUG_QUADTREE
        cout << "Merging " << this << endl;
#   endif

    //Slow, reallocating data for every child to be merged.

    //Merging leaves does nothing.
    if (isLeaf)
        return;

    //If leaf, copy values to parent (this). Else do recursion (merge lower regions before merging this).
    for (int e = START_CHILD; e <= END_CHILD; e++)
        if ( !child[e]->isLeaf )
            child[e]->merge();

    //All children are now leaves.
    int nValues = 0;
    for (int e = START_CHILD; e <= END_CHILD; e++)
        nValues += child[e]->len;

    IRO_Point2D **tempVal = new IRO_Point2D *[nValues];

    //Copy values from leaves.
    {
        int j = 0; //Index for new data.
        for (int e = START_CHILD; e <= END_CHILD; e++)
            for (int i = 0; i < child[e]->len; i++)
                tempVal[j++] = child[e]->val[i]; //Reads j before incrementing.
    }

    for (int e = START_CHILD; e <= END_CHILD; e++)
        delete child[e];

    isLeaf = true;
    val = tempVal;
    len = nValues;
}

//Returns the total number of points inside region.
int Quadtree_node::getTotalLen() const
{
    if ( isLeaf )
        return len;
    else
    {
        int rVal = 0;
        for (int e = START_CHILD; e <= END_CHILD; e++)
            rVal += child[e]->getTotalLen();

        return rVal;
    }
}

//----Quadtree entry----

Quadtree::Quadtree(float left, float width, float down, float height, int maxDepth)
:   m_maxDepth(maxDepth), m_root(new Quadtree_node(left, width, down, height))
{

}

Quadtree::~Quadtree()
{
    delete m_root;
}

//Private.
//Returns the leaf that has the point (x, y) inside region.
//This is a directed search, we will never need to consider all nodes in the tree.
Quadtree_node *Quadtree::getLeafAt(float x, float y) const
{
#   ifdef _DEBUG_QUADTREE
        cout << "Finding leaf, (x, y) = (" << x << ", " << y << ")" << endl;
#   endif

    Quadtree_node *curNode = m_root;

    if ( !curNode->isInRegion(x, y) )
        throw QuadtreeException::QE_outOfBound;

    while ( curNode->hasChildren() )
    {
#       ifdef _DEBUG_QUADTREE
            bool noBreak = true;
#       endif
        for (int e = Quadtree_node::START_CHILD;
             e <= Quadtree_node::END_CHILD;
             e++)
        {
            if ( curNode->getChild(e)->isInRegion(x, y) )
            {
#               ifdef _DEBUG_QUADTREE
                    noBreak = false;
#               endif
                curNode = curNode->getChild(e);
                break;
            }
        }
#       ifdef _DEBUG_QUADTREE
            assert( !noBreak ); //Should never trigger assertion.
#       endif
    }

    return curNode;
}

//Private.
//Does a directed search to find the parent of a node. Return 0 (NULL) if looking for parent of root.
Quadtree_node *Quadtree::getParent(Quadtree_node *node) const
{
#   ifdef _DEBUG_QUADTREE
        cout << "Finding parent" << endl;
#   endif

    Quadtree_node *curNode = m_root;

    if (node == curNode)
        return 0;

    //Searching point.
    float x, y;
    node->getCenter(x, y);

    while ( curNode->hasChildren() )
    {
        for (int e = Quadtree_node::START_CHILD;
             e <= Quadtree_node::END_CHILD;
             e++)
        {
            if ( curNode->getChild(e)->isInRegion(x, y) )
            {
                if ( curNode->getChild(e) == node )
                {
                    return curNode; //Found parent.
                }
                else
                {
                    //Found anchestor.
                    curNode = curNode->getChild(e);
                    break;
                }
            }
        }
    }

    assert( 0 );
}

#include <list> //Used as a dynamic stack.

//Private.
//Does an undirected search to find a point.
//This is called when the value of the point has changed and
//thus the new position cannot be guaranteed to be inside same region (see updatePos).
Quadtree_node *Quadtree::find(IRO_Point2D *posPtr) const //Depth-First-Search.
{
#   ifdef _DEBUG_QUADTREE
        cout << "Searching" << endl;
#   endif

    std::list<Quadtree_node *> searchStack;

    searchStack.push_back(m_root);

    while ( !searchStack.empty() )
    {
        Quadtree_node *curNode;

        curNode = searchStack.back();
        searchStack.pop_back();

        if ( !curNode->hasChildren() )
        {
            if ( curNode->isInNode(posPtr) )
                return curNode;
        }
        else
        {
            for (int e = Quadtree_node::START_CHILD;
                 e <= Quadtree_node::END_CHILD;
                 e++)
            {
                searchStack.push_back( curNode->getChild(e) );
            }
        }
    }

    return 0; //Did not find point.
}

//Public.
//Adds a point and subdivides the region if not max depth has been reached.
//Subdivision is done iterativelly.
void Quadtree::addPos(IRO_Point2D *posPtr)
{
#   ifdef _DEBUG_QUADTREE
        cout << "Adding pos" << endl;
#   endif

    Quadtree_node *curNode = getLeafAt(posPtr->getX(), posPtr->getY());

    curNode->addValue(posPtr);

    if (curNode->getDepth() >= m_maxDepth)
        return;


    std::list<Quadtree_node *> divideStack;

    divideStack.push_back(curNode);

    //If more than the just added value is in leaf, then subdivide (if depth < maxDepth).
    //If the just added value is the only value in leaf, then do nothing.

    while ( !divideStack.empty() )
    {
        curNode = divideStack.back();
        divideStack.pop_back();

        if ( (curNode->getLen() > 1) && (curNode->getDepth() < m_maxDepth) )
        {
            curNode->subdivide(); //Will distribute points to new leaves.
            for (int e = Quadtree_node::START_CHILD;
                 e <= Quadtree_node::END_CHILD;
                 e++)
            {
                divideStack.push_back( curNode->getChild(e) );
            }
        }
    }
}

//Public.
//Does a directed search to find leaf node that contains point.
void Quadtree::removePos(IRO_Point2D *posPtr)
{
#   ifdef _DEBUG_QUADTREE
        cout << "Removing pos" << endl;
#   endif

    Quadtree_node *curNode = getLeafAt(posPtr->getX(), posPtr->getY());

    if ( !curNode->isInNode(posPtr) )
        throw QuadtreeException::QE_badSearch;

    curNode->removeValue(posPtr);

    //Keep the branches as small as possible.
    while (curNode->len <= 1)
    {
        if ( !(curNode = getParent(curNode)) ) //If curNode is root, no parent. Quadtree is now empty.
            return;

        if (curNode->getTotalLen() <= 1)
            curNode->merge();
    }
}

//Public.
//Tells the tree that the point has changed position.
void Quadtree::updatePos(IRO_Point2D *posPtr)
{
#   ifdef _DEBUG_QUADTREE
        cout << "Updating pos" << endl;
#   endif

    Quadtree_node *curNode = getLeafAt(posPtr->getX(), posPtr->getY());

    if ( !curNode->isInNode(posPtr) ) //If posPtr is no longer in region, then move posPtr (early escape test).
    {
        //Find the old node where posPtr was, then remove it.
        Quadtree_node *oldNode = find(posPtr);

        if ( !oldNode )
            throw QuadtreeException::QE_badSearch; //Trying to update a point not in tree.

        oldNode->removeValue(posPtr);

        //Adds point to tree again.
        //Must add point again before removing old one!!!
        //If not, tree might be empty and oldNode will become parent of root (and trigger assertion).
        addPos(posPtr);

        //(see removePos)
        //Cannot use removePos since (x, y) is not its position in tree according to if-statement.
        while (oldNode->len <= 1)
        {
            oldNode = getParent(oldNode);
            assert( oldNode );

            if (oldNode->getTotalLen() <= 1)
                oldNode->merge();
        }
    }
    //If point is in same region as before, then do nothing.
}

//Public.
//Returns the point(s) in smallest region that contains (x, y).
std::vector<IRO_Point2D *> Quadtree::getContentAt(float x, float y) const
{
#   ifdef _DEBUG_QUADTREE
        cout << "Getting at (x, y) = (" << x << ", " << y << ")" << endl;
#   endif

    Quadtree_node *curNode = getLeafAt(x, y);

    std::vector<IRO_Point2D *> rVal;
    IRO_Point2D **temp = curNode->getValues();

#   ifdef _DEBUG_QUADTREE
        cout << "Found " << curNode->getLen() << " data" << endl;
#   endif

    for (int i = 0; i < curNode->getLen(); i++)
    {
        rVal.push_back(temp[i]);
    }

#   ifdef _DEBUG_QUADTREE
        for (int i = 0; i < curNode->getLen(); i++)
        {
            cout << rVal[i] << endl;
        }
#   endif

    return rVal; //If region is empty, so will rVal be.
}

//Public.
//Returning points in rectangular region.
std::vector<IRO_Point2D *> Quadtree::getContentInRect(float left, float down, float right, float up) const
{
    if ( (left > right) || (down > up) )
        throw QuadtreeException::QE_badRect;

#   ifdef _DEBUG_QUADTREE
        cout << "Getting at rect area" << endl;
#   endif

    Quadtree_node *curNode = m_root;

    std::vector<IRO_Point2D *> rVec;
    std::list<Quadtree_node *> evalPartialList;     //List will contain all leaves partially inside rectangle.
    std::list<Quadtree_node *> evalCompleteList;    //List will contain all leaves completely inside rectangle.
    std::list<Quadtree_node *> searchStack;         //See method find.

    searchStack.push_back(curNode);

    while ( !searchStack.empty() )
    {
        curNode = searchStack.back();
        searchStack.pop_back();

        if ( curNode->hasChildren() )
        {
            for (int e = Quadtree_node::START_CHILD;
                 e <= Quadtree_node::END_CHILD;
                 e++)
            {
                Quadtree_node *curChild = curNode->getChild(e);
                if ( (curChild->getLeft() <= right) &&
                     (curChild->getLeft() + curChild->getWidth() > left) &&
                     (curChild->getDown() <= up) &&
                     (curChild->getDown() + curChild->getHeigth() > down) )
                {
                    //At least part of curChild is in rectangle.
                    searchStack.push_back(curChild);
                }
            }
        }
        else //curNode is leaf and at least part of rectangle.
        {
            if ( (curNode->getLeft() >= left) &&
                 (curNode->getLeft() + curNode->getWidth() < right) &&
                 (curNode->getDown() >= down) &&
                 (curNode->getDown() + curNode->getHeigth() < up) )
            {
                evalCompleteList.push_back(curNode);
            }
            else
            {
                evalPartialList.push_back(curNode);
            }
        }
    }

    //Evaluation lists are completed, copy data from evalCompleteList to return point and process partials.
    for (std::list<Quadtree_node *>::iterator it = evalCompleteList.begin();
         it != evalCompleteList.end();
         it++)
    {
        IRO_Point2D **data = (*it)->getValues();
        for (int i = 0; i < (*it)->getLen(); i++)
            rVec.push_back(data[i]);
    }

    for (std::list<Quadtree_node *>::iterator it = evalPartialList.begin();
         it != evalPartialList.end();
         it++)
    {
        IRO_Point2D **data = (*it)->getValues();
        for (int i = 0; i < (*it)->getLen(); i++)
        {
            if ( (data[i]->getX() >= left) &&
                 (data[i]->getX() < right) &&
                 (data[i]->getY() >= down) &&
                 (data[i]->getY() < up) )
            {
                rVec.push_back(data[i]);
            }
        }
    }

#   ifdef _DEBUG_QUADTREE
        cout << "Found " << evalCompleteList.size() << " region(s) completely inside and "
             << evalPartialList.size() << " region(s) partially inside." << endl;
#   endif

    return rVec;
}

#include <string>

std::ostream &operator<<(std::ostream &out, const Quadtree_node &node)
{
    std::string tabber;
    for (int i = 0; i < node.depth; i++)
        tabber += "\t";

    out << tabber << "[" << std::endl
        << tabber << " " << &node << std::endl
        << tabber << " left   = " << node.left << std::endl
        << tabber << " width  = " << node.width <<  std::endl
        << tabber << " down   = " << node.down << std::endl
        << tabber << " height = " << node.height;

    if (node.isLeaf)
    {
        if (node.len)
        {
            out << std::endl << tabber << " -- ";

            for (int i = 0; i < node.len - 1; i++)
            {
                out << "(" << node.val[i]->getX() << ", "
                    << node.val[i]->getY() << "), ";
            }
            out << "(" << node.val[node.len - 1]->getX() << ", "
                << node.val[node.len - 1]->getY() << ")" << "-- " << std::endl
                << tabber << "]" << std::endl;
        }
        else
        {
            out << std::endl << tabber << " --no content--" << std::endl
                             << tabber << "]" << std::endl;
        }
    }
    else
    {
        out << std::endl << tabber << "]" << std::endl;
        for (int e = Quadtree_node::START_CHILD;
             e <= Quadtree_node::END_CHILD;
             e++)
        {
            out << *node.getChild(e);
        }
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const Quadtree &tree)
{
    out << std::endl << *tree.m_root << std::endl;
    return out;
}


