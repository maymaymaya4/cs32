#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <time.h>
#include <iostream>
#include <string>
#include <Stack>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
: Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
    // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
        {
            return false;
        }
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
    
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
    // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
    // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

Point pointget(int first, int second)
{
    do{
        getLineWithTwoIntegers(first, second);
    }
    
    while (!cin.good());
    Point p(first,second);
    return p;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.

//typedef AwfulPlayer HumanPlayer;
class HumanPlayer: public Player
{
public:
    HumanPlayer(string nm, const Game& g);
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board &b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
};

bool HumanPlayer::placeShips(Board &b)
{
    //prompt user where to place ships
    char dir = 'h'; int first = 0, second = 0; Direction d = HORIZONTAL;
    for(int i=0; i< game().nShips(); i++)
    {
        do
        {
            cout<< "Enter h or v for direction of " << game().shipName(i) << "(" << "length " << game().shipLength(i) << ")";
            cin >> dir;
            if(dir == 'v')
            {
                d = VERTICAL;
                cout << "Enter row and column of topmost cell (e.g. 3 5): ";
            }
            else if (dir == 'h')
            {
                d=HORIZONTAL;
                cout << "Enter row and column of leftmost cell (e.g. 3 5): ";
            }
            else cout <<"Direction must be h or v " <<endl;
        }
        while (dir!='h' && dir != 'v');
        
        
        bool placed = true;
        Point p = pointget(first, second);
        placed = b.placeShip(p, i, d);
        b.display(false);
        while(!placed)
        {
            cout << "The ship can not be placed there " << endl;
            if(d == VERTICAL)
            {
                cout << "Enter row and column of topmost cell (e.g. 3 5): ";
            }
            else if (d == HORIZONTAL)
            {
                cout << "Enter row and column of leftmost cell (e.g. 3 5): ";
            }
            
            Point r = pointget(first, second);
            placed = b.placeShip(r, i, d);
            b.display(false);
        }
        
    }
    return true;
}


Point HumanPlayer::recommendAttack()
{
    cout << endl;
    Board b(game()); bool shotHit = true; bool shipDestroyed = false;
    int first = 0; int second = 0;
    
    cout << "Enter the row and column to attack (e.g, 3 5): ";
    do{
        cin >> first; cin>>second;
        if (!(cin.good())) cout << "Not an integer.";
        cin.clear();
        cin.ignore();
    }
    while (!cin.good());
    int numb = 0;
    Point p (first,second);
    
    if(game().isValid(p))
    {
        b.attack(p, shotHit, shipDestroyed, numb);
    }
    else cout << name() << " wasted a shot at " << "(" << p.r << "," << p.c << ")" << endl;
    return p;
}

HumanPlayer::HumanPlayer(string nm, const Game& g)
: Player(nm, g)
{
    cout << nm << " must place " << game().nShips() << " ships" <<endl;
    Board b(game());
    b.display(true);
    cout << endl;
}

void HumanPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
    //need not do anything in human player
}

void HumanPlayer::recordAttackByOpponent(Point /* p */)
{
    //need not do anything in human player
}
//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer MediocrePlayer;

class MediocrePlayer:public Player{
public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    bool recursiveShips(Board &b, vector<int> &shipsToPlace, int index);
    void recordAttackByOpponent(Point p);
    void recordAttackResult(Point p, bool validShot, bool shotHit,
                            bool shipDestroyed, int shipId);
    bool ifused(Point p, vector<Point> v);
    
    Point recommendAttack();
private:
    int m_state;
    Point m_point; //has default constructor, doesn't need initialization list
    vector<Point>selected; //for state 1
    vector<Point>selected2; //for state 2
};
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

bool MediocrePlayer::ifused(Point p, vector<Point> v) //true if already used
{
    for(int i=0; i<v.size();i++)
    {
        if(v[i].r == p.r && v[i].c == p.c)
            return true;
    }
    return false;
}

MediocrePlayer::MediocrePlayer(string nm, const Game& g): Player(nm, g)
{
    m_state = 1; //state initially at 1
    Board b(game());
    b.display(false);
    cout << endl;
}

Point MediocrePlayer::recommendAttack()
{
    Point lastresort;
    srand (time(NULL));
    //based on mediocreplayer's state
    if(m_state == 1) //returns a unique random point
        
    {
        Point p;
        p = game().randomPoint();
        while(ifused(p, selected))
        {
            p = game().randomPoint();
        }
        selected.push_back(p);
        return p;
        
    }
    
    else if(m_state == 2)
    {
        Point p = m_point;
        vector<Point> pointy;
        //create all the rand points, push into vector. every time chosen, delete and choose from the rest
        
        //NORTH
        for(int i=1; i<=4; i++)
        {
            Point new_point(p.r-i,p.c );
            if(game().isValid(new_point))
                pointy.push_back(new_point);
        }
        
        //EAST
        for(int i=1; i<=4; i++)
        {
            Point new_point(p.r,p.c+i );
            if(game().isValid(new_point))
                pointy.push_back(new_point);
        }
        
        //SOUTH
        for(int i=1; i<=4; i++)
        {
            Point new_point(p.r+i,p.c );
            if(game().isValid(new_point))
                pointy.push_back(new_point);
        }
        
        //WEST
        for(int i=1; i<=4; i++)
        {
            Point new_point(p.r,p.c-i );
            if(game().isValid(new_point))
                pointy.push_back(new_point);
        }
        
        int index = randInt(pointy.size()); //randomly generate an index
        Point on_cross = pointy[index];
        
        while(ifused(on_cross,selected2))
        {
            index = randInt(pointy.size()); //randomly generate an index
            on_cross = pointy[index];
        }
        selected2.push_back(on_cross);
        return on_cross;
    }
    return lastresort;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                        bool shipDestroyed, int shipId)
{
    Board b(game());
    //switch states here, recommendattack() based on states
    if(m_state == 1)
    {
        //         b.attack(p, shotHit,shipDestroyed , shipId);
        if(!shotHit)
        {
            m_state = 1; //attack missed, player stays in state 1
        }
        
        else if(shotHit)
        {
            if(shipDestroyed)
            {
                m_state = 1;
            }
            else if (!shipDestroyed)
            {
                cerr << "switch";
                m_point = p;
                m_state = 2;
            }
        }
        
    }
    
    else if (m_state ==2)
    {
        if(!shotHit)
        {
            m_state=2; //player stays in state 2
        }
        
        else
            //            ((b.attack(m_point, shotHit, shipDestroyed, shipId)))
        {
            if(!shipDestroyed)
            {
                m_state=2; //ship not destroyed, player stays in state 2
            }
            else  //ship destroyed
            {
                m_state = 1;
                m_point = p;//transition to state 1
            }
        }
    }
    
    
}

void MediocrePlayer::recordAttackByOpponent(Point /* p */)
{
    //need not do anything in mediocre player
}

bool MediocrePlayer::placeShips(Board& b)
{
    // Order ships by size in decreasing order
    
    vector<int> sizes;
    for (int i = 0; i < game().nShips(); i++)
    {
        if (i == 0) {
            sizes.push_back(i);
            continue;
        }
        
        bool poot = false;
        for (auto it = sizes.end()-1; it != sizes.begin(); it--) {
            
            if (game().shipLength(i) < game().shipLength(*it)) {
                
                sizes.insert(it, i);
                poot = true;
                break;
                
            }
        }
        if (!poot) {
            sizes.insert(sizes.begin(), i);
        }
        
    }
    
    for (int i = 0; i < 50; i++) {
        
        b.block();
        bool success = recursiveShips(b, sizes, 0);
        b.unblock();
        if (success)
            return true;
        
    }
    
    return false;
}

bool MediocrePlayer::recursiveShips(Board &b, vector<int> &v, int i) {
    
    if (i == v.size())
        return true;
    
    Point put;
    Direction dir = HORIZONTAL;
    int len = game().shipLength(v[i]);
    while (!b.placeShip(put, v[i], dir)) {
    here:
        if (dir == HORIZONTAL) {
            
            if (put.r == game().rows()-1 && put.c == game().cols()-1-len) {//If at an edge, place vertically
                dir = VERTICAL;
                put.r = 0;
                put.c = 0;
                continue;
            }
            
            if (put.c <= game().cols()-len) //if can move to the right, do
                put.c++;
            else {
                put.c = 0;
                put.r++; //move down
            }
            
        }
        
        else if (dir ==VERTICAL){
            
            if (put.r == game().rows()-1-len && put.c == game().cols()-1) { //if at edge, false
                                return false;
                
            }
            
            if (put.r <= game().rows()-len) //move down
                put.r++;
            else {
                put.r = 0;
                put.c++; //move right
            }
            
        }
        
    }
    
    bool cursed = recursiveShips(b, v, i+1);
    
    if (!cursed) { //if unable to place, unplace the previous ship
        b.unplaceShip(put, v[i], dir);
        goto here;
    }
    
    else return cursed;
    
}
//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer GoodPlayer;
enum Compass { NORTH, EAST, SOUTH, WEST };
struct loc {
    Point m_p;
    Compass m_c;
};
struct upgradeinfo {
    Point m_point;
    Direction m_dir;
    int m_shipID;
};

class GoodPlayer : public Player  //use probability density fxn? Maybe too difficult
{
public:
    GoodPlayer(string nm, const Game& g) : Player(nm, g), m_state(1), firstHit(Point(0, 0))
    {
        for (int i = 0; i<game().rows(); i++)
            for (int j = 0; j<game().cols(); j++)
            {
                grid[i][j] = 0;
            }
    }
    virtual bool placeShips(Board& b);
    bool plattempt(Board &b, Point p, int shipID, Direction d);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p) {} //define just so not an abstract class
private:
    int grid[MAXROWS][MAXCOLS];
    int m_state;
    Point firstHit;
    vector <upgradeinfo> fleet;
    stack <loc> attacked;

};


bool GoodPlayer::placeShips(Board& b)
{
    //same as mediocre player
    int tried = 0; bool placed = false;
    while (tried < 50 && !placed)
    {
        b.block();
        placed = plattempt(b, Point(0, 0), 0, VERTICAL); //if not placed, do again
        tried++;
        b.unblock(); //call unblock just before returning
    }

    return placed;

}

bool GoodPlayer::plattempt(Board &b, Point p, int shipID, Direction d) //recursive fxn
{
    if (shipID == game().nShips())
        return true;
    if (p.r == game().rows() - 1 && p.c == game().cols() - 1 && d == HORIZONTAL)
    {
        if (!fleet.empty())  //while more ships to place
        {
            p = fleet[fleet.size() - 1].m_point;
            shipID = fleet[fleet.size() - 1].m_shipID;
            d = fleet[fleet.size() - 1].m_dir;
            b.unplaceShip(p, shipID, d);
            fleet.erase(fleet.begin() + fleet.size() - 1);
            if (p.c + 1 == game().cols())
                return plattempt(b, Point(p.r + 1, 0), shipID, d);
            return plattempt(b, Point(p.r, p.c + 1), shipID, d);
        }
        else
            return false;

    }
    if (p.r == game().rows() - 1 && p.c == game().cols() - 1 && d == VERTICAL)
    {
        d = HORIZONTAL;
        p.r = 0;
        p.c = 0;
    }


    if (!(p.r == game().rows() - 1 && p.c == game().cols() - 1))
    {
        bool canPlace = b.placeShip(p, shipID, d);
        if (canPlace)
        {
            upgradeinfo ship;
            ship.m_dir = d;
            ship.m_point = p;
            ship.m_shipID = shipID;
            fleet.push_back(ship);
            return plattempt(b, Point(0, 0), shipID + 1, d);
        }
        if (!canPlace)
        {
            if (p.c < game().cols() - 1)
                return plattempt(b, Point(p.r, p.c + 1), shipID, d); //next column

            return plattempt(b, Point(p.r + 1, 0), shipID, d); //next row
        }
    }

    return false;
}

Point GoodPlayer::recommendAttack()//same as mediocre player w/states
{
    switch (m_state)
    {
        case 1:
        {
            int count = 0;
            bool shotHit = false;
            while (!shotHit)
            {
                Point p = game().randomPoint();
                if ((p.r + 1 + p.c) % 2 == 1) //attack odd coordinates, creating a "checkerboard" effect
                {
                    if (grid[p.r][p.c] == 0)
                    {
                        grid[p.r][p.c] = 1;
                        return p;
                    }
                }
                count++;
                if (count == 200)
                { //same as MEDIOCREPLAYER
                    while (!shotHit)
                    {
                        p = game().randomPoint();
                        if (grid[p.r][p.c] == 0)
                        {
                            grid[p.r][p.c] = 1; //stay in state 1
                            return p;
                        }
                    }
                }

            }
            break;
        }
        case 2:
        {
            if (!attacked.empty()) //if still spots to attack from directions, return it so that it can be attacked
            {
                loc tacd = attacked.top();
                grid[tacd.m_p.r][tacd.m_p.c] = 1;
                return tacd.m_p;
            }
            else //if empty
            {
                cerr << "goodswitch" <<endl;
                m_state = 1; //back to state 1
                int count = 0;
                bool toattack = false;
                while (!toattack)
                {
                    Point p = game().randomPoint(); //create a random point
                    if ((p.r + 1 + p.c) % 2 == 1) //attack odd coords for checkering
                    {
                        if (grid[p.r][p.c] == 0)
                        {
                            grid[p.r][p.c] = 1;
                            return p;
                        }
                    }
                    count++;
                    if (count == 100)
                    {
                        while (!toattack)
                        {
                            p = game().randomPoint();
                            if (grid[p.r][p.c] == 0)
                            {
                                grid[p.r][p.c] = 1;
                                return p;
                            }
                        }
                    }

                }
            }
            break;
        }
    }
    Point p;
    return p;
}



void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId)
{
    switch (m_state)
    {
        case 1:
        {
            if (shotHit)
            {
                m_state = 2;
                if (shipDestroyed)
                    m_state = 1;
                else
                {
                    firstHit = p;

                    Point west = Point(p.r, p.c - 1);
                    Point south = Point(p.r+1, p.c);
                    Point east = Point(p.r, p.c + 1);
                    Point north = Point(p.r-1, p.c);

                    if (game().isValid(Point(north)) && grid[north.r][north.c] != 1) //not equal 1: attacking only checkered spots
                    {
                        loc norInfo;
                        norInfo.m_p = north;
                        norInfo.m_c = NORTH;
                        attacked.push(norInfo);
                    }
                    if (game().isValid(Point(east)) && grid[east.r][east.c] != 1)
                    {
                        loc eastInfo;
                        eastInfo.m_p = east;
                        eastInfo.m_c = EAST;
                        attacked.push(eastInfo);
                    }
                    if (game().isValid(Point(south)) && grid[south.r][south.c] != 1)
                    {
                        loc southInfo;
                        southInfo.m_p = south;
                        southInfo.m_c = SOUTH;
                        attacked.push(southInfo);
                    }
                    if (game().isValid(Point(west)) && grid[west.r][west.c] != 1)
                    {
                        loc westInfo;
                        westInfo.m_p = west;
                        westInfo.m_c = WEST;
                        attacked.push(westInfo);
                    }
                }
            }
            break;
        }
        case 2:
        {
            if (shotHit)
            {
                if (shipDestroyed)
                    m_state = 1;
                else
                {
                    bool f = false;
                    loc stackattack = attacked.top();
                    while (!f)
                    {
                        bool beast = false;
                        bool bnorth = false;
                        bool bsouth = false;
                        bool bwest = false;

                        switch (stackattack.m_c)
                        {
                            case WEST:
                            {
                                if (game().isValid(Point(stackattack.m_p.r, stackattack.m_p.c - 1)) && grid[stackattack.m_p.r][stackattack.m_p.c - 1] != 1)
                                {
                                    grid[stackattack.m_p.r][stackattack.m_p.c - 1] = 1;
                                    stackattack.m_p = Point(stackattack.m_p.r, stackattack.m_p.c - 1);
                                    attacked.pop();
                                    attacked.push(stackattack);
                                    f = true;
                                    break;
                                }
                                else
                                {
                                    stackattack.m_c = EAST;
                                    beast = true;
                                }
                            }
                            case EAST:
                            {
                                if (game().isValid(Point(stackattack.m_p.r, stackattack.m_p.c + 1)) && grid[stackattack.m_p.r][stackattack.m_p.c + 1] != 1)
                                {
                                    grid[stackattack.m_p.r][stackattack.m_p.c + 1] = 1;
                                    stackattack.m_p = Point(stackattack.m_p.r, stackattack.m_p.c + 1);
                                    attacked.pop();
                                    attacked.push(stackattack);
                                    f = true;
                                    break;
                                }
                                else
                                {
                                    stackattack.m_c = NORTH;
                                    bnorth = true;
                                }
                            }
                            case NORTH:
                            {
                                if (game().isValid(Point(stackattack.m_p.r-1, stackattack.m_p.c)) && grid[stackattack.m_p.r-1][stackattack.m_p.c] != 1)
                                {
                                    grid[stackattack.m_p.r-1][stackattack.m_p.c] = 1;
                                    stackattack.m_p = Point(stackattack.m_p.r-1, stackattack.m_p.c );
                                    attacked.pop();
                                    attacked.push(stackattack);
                                    f = true;
                                    break;
                                }
                                else
                                {
                                    stackattack.m_c = SOUTH;
                                    bsouth = true;
                                }
                            }
                            case SOUTH:
                            {
                                if (game().isValid(Point(stackattack.m_p.r+1, stackattack.m_p.c)) && grid[stackattack.m_p.r+1][stackattack.m_p.c] != 1)
                                {
                                    grid[stackattack.m_p.r+1][stackattack.m_p.c] = 1;
                                    stackattack.m_p = Point(stackattack.m_p.r+1, stackattack.m_p.c);
                                    attacked.pop();
                                    attacked.push(stackattack);
                                    f = true;
                                    break;
                                }
                                else
                                {
                                    stackattack.m_c = WEST;
                                    bwest = true;
                                }
                            }
                        }
                        if (bsouth && bwest && beast && bnorth) //once all pushed onto stack
                        {
                            f = true;
                            attacked.pop();
                        }
                    }
                }
            }
            else {
                attacked.pop();
            }
        }
    }

}


//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
         type != types[pos]; pos++)
        ;
    switch (pos)
    {
        case 0:  return new HumanPlayer(nm, g);
        case 1:  return new AwfulPlayer(nm, g);
        case 2:  return new MediocrePlayer(nm, g);
        case 3:  return new GoodPlayer(nm, g);
        default: return nullptr;
    }
}
