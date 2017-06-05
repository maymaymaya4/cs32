#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;
    
private:
    // TODO:  Decide what private members you need.  Here's one
    char m_grid[MAXROWS][MAXCOLS];
    int nRows;
    int nCols;
    const Game& m_game;
};


BoardImpl::BoardImpl(const Game& g)
: m_game(g)
{
    nRows = g.rows();
    nCols = g.cols();
    for (int r = 0; r < g.rows(); r++)
        for (int c = 0; c < g.cols(); c++)
            m_grid[r][c] = '.';
    //Initialize a board. The parameter g refers to the game the board is being used in. Board
    //member functions that need to know the number of rows or columns or the characteristics
    //of ships will presumably find out from the game object the board was constructed with.
    
}

void BoardImpl::clear()
{
    //This function clears the board so it is empty, ready to be populated with ships
    
    for (int r = 0; r < nRows; r++)
        for (int c = 0; c < nCols; c++)
            m_grid[r][c] = '.';
}

void BoardImpl::block()
{
    //    This function blocks about half the positions on the board. This function will be used
    //    only by the MediocrePlayer when placing ships on the board at the start of play. If a
    //    position is blocked, then that player may not place a ship that occupies that location.
    // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                //blocked cell has '#'
                m_grid[r][c] = '#'; //  code to block cell (r,c)
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(m_grid[r][c] == '#')
                m_grid[r][c] = '.';  ; // code to unblock cell (r,c) if blocked
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    //if false, board must remain unchanged
    //if true, ship successfully placed
    // The integers from 0 to nships()−1 will be the ship ID
    
    if (shipId<0 || shipId > m_game.nShips()-1) return false; //Case 1: The shipId is invalid
    if(!(m_game.isValid(topOrLeft))) return false;
    int length = m_game.shipLength(shipId);
    if(dir== HORIZONTAL) //Case 2: The  ship would be partly or fully outside the board.
    {
        for(int i=0; i<length;i++)
        {
            //check each coord of length of the ship
            Point temp = Point(topOrLeft.r , topOrLeft.c+i);
            
            //            if(temp.r<0 || temp.r >m_game.rows() || temp.c <0 || temp.c > m_game.cols())
            //                return false;
            if(!(m_game.isValid(temp))) return false;
            //Case 3,4: Check that ship does not overlap another ship & ship not on blocked
            if(m_grid[temp.r][temp.c] != '.') return false;
        }
        
    }
    
    if(dir== VERTICAL) //Case 2: Vertical
    {
        for(int i=0; i<length;i++)
        {
            //check each coord of length of the ship
            Point temp = Point(topOrLeft.r+i , topOrLeft.c);
            if(!(m_game.isValid(temp))) return false;
            //Case 3,4: Check that ship does not overlap another ship & ship not on blocked
            if(m_grid[temp.r][temp.c] != '.') return false;
        }
        
        
    }
    
    // Case 5. The ship with that ship ID has previously been placed on this Board and not
    // yet been unplaced since its most recent placement
    char tofind = m_game.shipSymbol(shipId);
    //Look for that symbol in game for shipId If found, ship already placed and return false
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(m_grid[r][c] == tofind)
                return false;         }
    
    //Now true, place the ship on the board
    if(dir == HORIZONTAL)
    {
        for(int i=0; i<length;i++)
        {
            m_grid[topOrLeft.r][topOrLeft.c+i] = tofind;
        }
    }
    
    if(dir == VERTICAL)
    {
        for(int i=0; i<length;i++)
        {
            m_grid[topOrLeft.r+i][topOrLeft.c] = tofind;
        }
    }
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    //remove the specified ship from the board, so the positions it
    //occupied may be used to place other ships. Make it '.'
    
    if (shipId<0 || shipId > m_game.nShips()-1) return false;  //shipId is invalid
    int length = m_game.shipLength(shipId);
    char tofind = m_game.shipSymbol(shipId);
    
    //Case 2: 2. The board does not contain the entire ship at the indicated locations
    if(m_grid[topOrLeft.r][topOrLeft.c] != tofind) return false;
    
    if(dir== VERTICAL)
    {
        for(int i=0; i<length;i++)
        {
            //check each coord of length of the ship
            Point temp = Point(topOrLeft.r+i , topOrLeft.c);
            if(!(m_game.isValid(temp))) return false;
            if(m_grid[temp.r][temp.c] != tofind) return false;
        }
    }
    
    if(dir== HORIZONTAL) //Case 2: The board does not contain the ship at indicated locations
    {
        for(int i=0; i<length;i++)
        {
            //check each coord of length of the ship
            Point temp = Point(topOrLeft.r , topOrLeft.c+i);
            if(!(m_game.isValid(temp))) return false;
            if(m_grid[temp.r][temp.c] != tofind) return false;
        }
    }
    
    //clear area
    if(dir== VERTICAL)
    {
        for(int i=0; i<length;i++)
        {
            //check each coord of length of the ship
            Point temp = Point(topOrLeft.r+i , topOrLeft.c);
            m_grid[temp.r][temp.c] = '.';
        }
    }
    
    //clear area
    if(dir== HORIZONTAL)
    {
        for(int i=0; i<length;i++)
        {
            //check each coord of length of the ship
            Point temp = Point(topOrLeft.r , topOrLeft.c +i);
            m_grid[temp.r][temp.c] = '.';
        }
    }
    
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    //display part
    if(!(shotsOnly))
    {
        cout << "  ";
        for (int c = 0; c < nCols; c++)
            cout << c;
        for (int r = 0; r < nRows; r++)
        {
            cout << endl << r<< " " ;
            for (int c = 0; c < nCols; c++)
            {
                cout << m_grid[r][c];
            }
            
        }
        cout << endl;
    }
    
    else
    {
        cout << "  ";
        for (int c = 0; c < nCols; c++)
            cout << c;
        for (int r = 0; r < nRows; r++)
        {
            cout << endl << r<< " " ;
            for (int c = 0; c < nCols; c++)
            {
                if(m_grid[r][c] != 'X' && m_grid[r][c] != 'o' && m_grid[r][c]!='.')
                    cout << '.';
                else
                    cout <<m_grid[r][c];
            }
        }
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    //might have to do something with shipId
    
    if(!(m_game.isValid(p))) return false; //point outside the board
    if(m_grid[p.r][p.c] == 'o' || m_grid[p.r][p.c] == 'X') return false; //previously attacked location
    if(m_grid[p.r][p.c] == '.')
    {
        shotHit = false;
        shipDestroyed=false;
        m_grid[p.r][p.c] = 'o';
    }
    else //there is a ship at location
    {
        
        for(int i=0; i<m_game.nShips();i++) //find shipID of ship at that location
        {
            if(m_grid[p.r][p.c] == m_game.shipSymbol(i))
            {
                shipId = i;
            }
        }
        char shipchar = m_game.shipSymbol(shipId);
        int shiplength = m_game.shipLength(shipId);
        
        m_grid[p.r][p.c] = 'X';  //damaged ship segment
        shotHit = true;
        
        //check if it's the last undamaged segment
        //use shipId, check it's components
        //check length of NESW for ship char, if none, boat destroyed, shipDestroyed set true
        shipDestroyed = true;
        for(int i=0; i<shiplength; i++)
        {
            
            for (int r = 0; r < nRows; r++) //check board for char, if there, ship not destroyed
                for (int c = 0; c < nCols; c++)
                    if(m_grid[r][c] == shipchar)
                    {
                        shipDestroyed = false;
                    }
        }
        
    }
    return true; // one I had to change
}

bool BoardImpl::allShipsDestroyed() const
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(m_grid[r][c] != '.'&& m_grid[r][c] != 'X' && m_grid[r][c] != 'o' && m_grid[r][c] != ' ')
                return false;
        }
    }
    
    return true; // This compiles, but may not be correct
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
