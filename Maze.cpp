#include "Maze.hpp"
#include <random>



Maze::Maze(int rows , int cols ): rows(rows), cols(cols){
    size = cols*rows;
    maze = new (nothrow) cell**[rows];
    if ( maze == 0 ) {
        cerr<<"maze is null "<<endl;
        exit(1);
    }

    for (size_t i = 0; i < rows; i++)
    {
        maze[i] = new cell*[cols];  
        for (size_t j = 0; j < cols ; j++)
        {
            if( i==0 || j == 0 || i == rows -1 || j == cols - 1) {       // borders of maze should be walls
                maze[i][j] = new cell(j,i,1);
            }else{
                maze[i][j] = new cell(j,i);
            }
        }
    }
}

void Maze::display(sf::RenderWindow& window , sf::CircleShape& playerCircle ) {

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::RectangleShape cellShape(sf::Vector2f(cellSize, cellSize));
            cellShape.setPosition(j * cellSize, i * cellSize);
            if (i == start_y && j == start_x) {
                cellShape.setFillColor(sf::Color::Green);
            } else if (i == end_y && j == end_x) {
                cellShape.setFillColor(sf::Color::Red);
            } else {
                cellShape.setFillColor(maze[i][j]->issolid() ? sf::Color::Black : sf::Color::White);
            }
            window.draw(cellShape);
        }
    }
    playerCircle.setPosition(cellSize*curr_x,cellSize*curr_y);   
}

cell* Maze::found_cell(int x,int y){
    if( x > cols || y > rows){
        cerr<<"cell not found invalid index"<<endl;
        return 0;
    }
    return maze[y][x];
}

bool Maze::isWall(int x, int y) {
    if( found_cell(x,y) == 0) return 0;
    return found_cell(x,y)->issolid();
}


void Maze::toggleWall(int x, int y){
    if( found_cell(x,y) == 0 || ( x == start_x &&  y == end_y )|| ( x == end_x && y == end_y )) return;
    found_cell(x,y)->toggle_wall(); 
}


Maze::~Maze() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            delete maze[i][j];
        }
        delete[] maze[i];
    }
    delete[] maze;
}

void Maze::setStartPoint(int x, int y) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        start_x = x;
        start_y = y;
        curr_x = start_x;
        curr_y = start_y;
    }
    cell *c = found_cell(x,y); 
    if( c->issolid() ) c->toggle_wall();
}

void Maze::setEndPoint(int x, int y) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        end_x = x;
        end_y = y;
    }
    cell *c = found_cell(x,y); 
    if( c->issolid() ) c->toggle_wall();
}


void Maze::solve(sf::CircleShape& playerCircle){
    int ran,i=0;
    Hash_map h_map(10*rows + cols);
    cell* c;
    char pre;

    while ( curr_x != end_x && curr_y != end_y )
    {
        set_direc(curr_x,curr_y);
        ran = rand()% 4;
        i=0;

        while( arr_dir[ran] == 0 && i < 3 ){ 
            i++;
        }
        if( arr_dir[ran] == 0 )
        {
            cerr<<"Maze unsolvable :) , make some changes "<<endl;
            return;
        }else{
            c = ( arr_dir[ran] == 'r' )? &r : ( arr_dir[ran] == 'l')? &l : (arr_dir[ran] == 'n')? &n : (arr_dir[ran] == 's')? &s : nullptr;
            if( h_map.isfound(maze[curr_x+c->get_x()][curr_y+c->get_Y()]) ){
                for (size_t i = 0; i < 4 ; i++){
                    c = ( arr_dir[i] == 'r' )? &r : ( arr_dir[i] == 'l')? &l : (arr_dir[i] == 'n')? &n : (arr_dir[i] == 's')? &s : nullptr;
                    if( !h_map.isfound(maze[curr_x+c->get_x()][curr_y+c->get_Y()]) ){
                        move(arr_dir[i]);
                        h_map.insert(maze[curr_x+c->get_x()][curr_y+c->get_Y()]);
                        stack.push(arr_dir[i]);
                        break;
                    }
                }
                if(ran == 4){                  // all avaialbe directions stored in hash_table
                    pre = stack.top();
                    stack.pop();
                    bool check_return = ( pre == 'r' )? move('l'): ( pre == 'l')? move('r') : (pre == 's')? move('n') : ( pre == 'n')? move('s'): 0;
                    if( check_return == 0 ){
                        cerr<<"Error : didn't return "<<endl;
                        exit(1);
                    }
                }
            }else{
                h_map.insert(maze[curr_x+c->get_x()][curr_y+c->get_Y()]);
                stack.push(arr_dir[ran]);
                move(arr_dir[ran]);
            }
        }
    }
}
    

bool Maze::move( char e ) {
    //cout<<e<<endl;
    //cout<<curr_x<<endl;
    bool moved = 0;
    if (e == 'r' && curr_x + 1 < cols && !maze[curr_x + 1][curr_y]->issolid()) {
        curr_x++;
        moved =1;
    } else if ( e == 'l' && curr_x - 1 > 0 && !maze[curr_x - 1][curr_y]->issolid()) {
        curr_x--;
        moved =1;
    } else if ( e == 's' && curr_y + 1 < rows && !maze[curr_x][curr_y + 1]->issolid()) {
        curr_y++;
        moved =1;
    } else if ( e == 'n' && curr_y - 1 > 0 && !maze[curr_x][curr_y - 1]->issolid()) {
        curr_y--;
        moved =1;
    }

    return moved;
}


void Maze::set_direc(int x, int y ){
    ( x+1 < cols && !maze[x+1][y]->issolid() )? arr_dir[0] = 'r' : arr_dir[0] = 0;
    ( y+1 < rows && !maze[x][y+1]->issolid())? arr_dir[1] = 's' : arr_dir[1] = 0;
    ( x-1 > 0 && !maze[x-1][y]->issolid())? arr_dir[2] = 'l' : arr_dir[2] = 0;
    ( y-1 > 0 && !maze[x][y-1]->issolid())? arr_dir[3] = 'n' : arr_dir[3] = 0; 
}
