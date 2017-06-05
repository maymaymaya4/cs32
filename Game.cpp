#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

struct Ship
{
    Ship(int length, char symbol, string name);
    char symbol;
    int length;
    string name;
    Direction dir;
};

Ship::Ship(int l, char s, string n){
    symbol = s;
    length = l;
    name = n;
}
class GameImpl
{
public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    Ship get_ship(char c);
private:
    int m_rows;
    int m_cols;
    vector<Ship> shiptypes;
};



Ship GameImpl::get_ship(char c)
{
    for(int i=0; i<shiptypes.size(); i++)
    {
        if(shiptypes[i].symbol == c)
        {
            return shiptypes[i];
        }
    }
    return Ship(0, '0', "0");
    
}

void waitForEnter()
{
    cout << endl;
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    m_rows =  nRows;
    m_cols = nCols;
    // This compiles but may not be correct
}

int GameImpl::rows() const
{
    //byme
    return m_rows;  // This compiles but may not be correct
}

int GameImpl::cols() const
{
    //byme
    return m_cols;  // This compiles but may not be correct
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    //all conditions checked in Game::addship(..)
    //just need to add ship
    for(int i=0; i<shiptypes.size();i++)
    {
        if (shiptypes[i].symbol == symbol)
        {
            cout << "Ship with symbol already exists " << endl;
            return false;
        }
    }
    Ship temp = Ship(length, symbol, name);
    shiptypes.push_back(temp);
    return true;
}

int GameImpl::nShips() const
{
    return ((int)shiptypes.size()); // This compiles but may not be correct
}

int GameImpl::shipLength(int shipId) const
{
    //checks asserted in Game::shipLength
    return shiptypes[shipId].length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return shiptypes[shipId].symbol;
}

string GameImpl::shipName(int shipId) const
{
    return shiptypes[shipId].name;  }

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause = true)
{
    if (!p1->placeShips(b1) || !p2->placeShips(b2))
        return nullptr;
    
    while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
    {
        cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
        
        b2.display(p1->isHuman()); //show board
        
        Point toattack1 = p1->recommendAttack(); //get pt to attack
        bool shotHit1, shipDestroyed1; int si1;
        bool attacked1 = b2.attack(toattack1, shotHit1, shipDestroyed1, si1); //attack
        p1->recordAttackResult(toattack1, attacked1, shotHit1, shipDestroyed1, si1);
        
        string _attack;
        if (shotHit1)_attack = "hit something";
        else _attack = "missed";
        if (shipDestroyed1)
            _attack = "destroyed the " + shipName(si1);
        
        cout << p1->name() << " attacked (" << toattack1.r << "," << toattack1.c << ") and " << _attack << ", resulting in: " << endl;
        
        if (p1->isHuman())
            b2.display(true); //displaying result
        else
            b2.display(false);
        
        if (b2.allShipsDestroyed())
        {
            cout <<endl<< p1->name() << " wins!" << endl;
        }
        
        if (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
        {
            if (shouldPause)
            {
                waitForEnter();
            }
        }
        
        if (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
        {
            cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
            //switch to p2
            bool disp = p2->isHuman();
            b1.display(disp);
            
            Point toattack2 = p2->recommendAttack();
            bool attacked2, shipDestroyed2;
            int si2;
            bool v2 = b1.attack(toattack2, attacked2, shipDestroyed2, si2);
            p2->recordAttackResult(toattack2, v2, attacked2, shipDestroyed2, si2);
            
            if (attacked2) _attack = "hit something";
            else _attack = "missed";
            if (shipDestroyed2)
                _attack = "destroyed the " + shipName(si2);
        
            cout << p2->name() << " attacked (" << toattack2.r << "," << toattack2.c << ") and " << _attack << ", resulting in: " << endl;
            
            if (p2->isHuman())
                b1.display(true); //displaying result
            else
                b1.display(false);
            
            if (b1.allShipsDestroyed())
                cout << endl<< p2->name() << " wins!" << endl;
            
            if (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
            {
                if (shouldPause)
                {
                    waitForEnter();
                }
            }
        }
        
    }
    if (b1.allShipsDestroyed()) return p2;
    return p1;
}



//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
        << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
        << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
        << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
            << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}
