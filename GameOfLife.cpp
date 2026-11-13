#include "GameOfLife.h"
#include <iostream>
#include <fstream>

GameOfLife::GameOfLife(int height, int width, std::string board, int buffer){
    GameOfLife::gheight = height;
    GameOfLife::gwidth = width;
    GameOfLife::gboard = board;
    GameOfLife::buffer = buffer;

}
GameOfLife::GameOfLife(){
    GameOfLife::gheight = 0;
    GameOfLife::gwidth = 0;
    GameOfLife::gboard = "";
}
std::vector<char> GameOfLife::returnNeighbors(int position){
    std::vector<char> neighbors;
    if(position == 0){
        neighbors.push_back(gboard[1]);
        neighbors.push_back(gboard[gwidth]);
        neighbors.push_back(gboard[gwidth + 1]);
    }else if(position == gwidth - 1){
        neighbors.push_back(gboard[gwidth - 2]);
        neighbors.push_back(gboard[(2*gwidth) - 1]);
        neighbors.push_back(gboard[(2 *gwidth) - 2]);
    }else if(position == (gwidth * (gheight - 1))){
        neighbors.push_back(gboard[(gwidth * (gheight - 2))]);
        neighbors.push_back(gboard[(gwidth * (gheight - 2) + 1)]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1))+ 1]);

    }else if(position == (gwidth * gheight) - 1){
        neighbors.push_back(gboard[(gwidth * gheight) - 2]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1)) - 2]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1)) - 1]);
    }else if(position < gwidth){
        neighbors.push_back(gboard[position + 1]);
        neighbors.push_back(gboard[position - 1]);
        neighbors.push_back(gboard[(position + gwidth)]);
        neighbors.push_back(gboard[(position + gwidth) + 1]);
        neighbors.push_back(gboard[(position + gwidth) - 1]);
    }else if((position%gwidth) == 0){
        neighbors.push_back(gboard[position - gwidth]);
        neighbors.push_back(gboard[(position - gwidth) + 1]);
        neighbors.push_back(gboard[position + 1]);
        neighbors.push_back(gboard[position + gwidth]);
        neighbors.push_back(gboard[(position + gwidth) + 1]);
    }else if((position%gwidth) == (gwidth - 1)){
        neighbors.push_back(gboard[(position - gwidth)]);
        neighbors.push_back(gboard[(position - gwidth) - 1]);
        neighbors.push_back(gboard[(position -1)]);
        neighbors.push_back(gboard[(position + gwidth)]);
        neighbors.push_back(gboard[(position + gwidth) - 1]);
    }else if(position > (gwidth * (gheight - 1)) && position < (gwidth * gheight) - 1){
        neighbors.push_back(gboard[(position + 1)]);
        neighbors.push_back(gboard[(position -1)]);
        neighbors.push_back(gboard[(position -gwidth)]);
        neighbors.push_back(gboard[(position - gwidth) - 1]);
        neighbors.push_back(gboard[(position - gwidth) + 1]);
    }else{
        neighbors.push_back(gboard[(position + gwidth)]);
        neighbors.push_back(gboard[(position + gwidth) + 1]);
        neighbors.push_back(gboard[(position + gwidth) - 1]);
        neighbors.push_back(gboard[(position - gwidth)]);
        neighbors.push_back(gboard[(position  -gwidth) + 1]);
        neighbors.push_back(gboard[(position - gwidth) - 1]);
        neighbors.push_back(gboard[(position + 1)]);
        neighbors.push_back(gboard[(position -1)]);
    }
    return neighbors;
}
void GameOfLife:: fullHistory(std::string newBoard){
    GameOfLife::history.erase(GameOfLife::history.begin());
    GameOfLife::history.push_back(newBoard);
}
void GameOfLife::next(){
    std::string newBoard;
    
    
    for(size_t i = 0; i< gboard.length(); i++){
        std:: vector<char> neighbors;
        neighbors = GameOfLife:: returnNeighbors(i);
        int live = 0;
    
        for(char j: neighbors){
            if(j == aliveCell ){
                live++;
            }
        }
        if(live < 2 && gboard[i] == aliveCell){
            newBoard.push_back(deadCell);
        }else if((live == 2 || live == 3) && gboard[i] == aliveCell){
            newBoard.push_back(aliveCell);
        } else if(live > 3 && gboard[i] == aliveCell){
            newBoard.push_back(deadCell);
        }else if(live == 3 && gboard[i] == deadCell){
            newBoard.push_back(aliveCell);
        }else{
            newBoard.push_back(gboard[i]);
        }
    }
    GameOfLife::gboard = newBoard;
    if(GameOfLife::history.size() < static_cast<size_t>(buffer)){
        GameOfLife::history.push_back(newBoard);
    }else{
        GameOfLife::fullHistory(newBoard);
    }
    GameOfLife::gen++;
}

void GameOfLife:: nextNGen(int genToAdd){
    for(int i = 0; i < genToAdd;i++){
        GameOfLife::next();
    }
}

void GameOfLife:: printGame(){
    std::cout <<"Generation: " << GameOfLife:: gen << std::endl;
    if(GameOfLife:: gboard.length() < 1){

            return;
    }
    for(size_t i = 0; i< GameOfLife:: gboard.length(); i++){
       
        if(i%GameOfLife::gwidth == 0 && i > 0){
            std::cout<<std::endl;
        }
        std::cout << GameOfLife::gboard[i];
    }
    std::cout<<std::endl;
    
}
void GameOfLife:: printGame(std::ofstream& fout){
    fout <<"Generation: " << GameOfLife:: gen << std::endl;
    if(GameOfLife:: gboard.length() < 1){
            return;
    }
    for(size_t i = 0; i< GameOfLife:: gboard.length(); i++){
       
        if(i%GameOfLife::gwidth == 0 && i > 0){
            fout<<std::endl;
        }
        fout << GameOfLife::gboard[i];
    }
    fout<<std::endl;
    
}

void GameOfLife:: changeCells(){
    std::cout<<"Game Paused: What would you like to do?"<<std::endl;
    std::string input;
    while(true){
        int xPos =-1;
        int yPos =-1;
        int extraPos = -1;
        char cellType;
        std::cout<<"Current GameBoard: "<<std::endl;
        printGame();
        std::getline(std::cin, input);
        std::istringstream iss(input);
        iss >> cellType;
        iss >> xPos;
        iss >> yPos;
        iss >> extraPos;

       
        if(cellType != 'a'&&cellType != 'd'&&cellType != 'e'&&cellType != 'x' && cellType != 'r'){//Checks char validity
            std::cerr<<"Error: invalid input, expected a, d, e, r or x"<<std::endl;
            continue;
        }
        if(cellType == 'x'){
            input = "x";
            break;
        }
        if(cellType == 'r'){
            if((xPos > GameOfLife:: gen)|| (xPos > GameOfLife::buffer)||(yPos != -1)){
                std::cerr<<"Error: invalid input for rollback"<<std::endl; 
                continue;
            }else{
                
              
                GameOfLife::gboard = GameOfLife::history[((GameOfLife::history.size()) - (xPos))];
               
                //std::cout<<gen - xPos;
                //GameOfLife::history.erase(GameOfLife::history.begin() + (xPos + 1), GameOfLife::history.end());
                  GameOfLife:: gen = GameOfLife::gen - (xPos);
                continue;
            }
        }
        if(extraPos != -1){
            std::cerr<<"Error: Can not set mutliple cells to the same char"<<std::endl;
                    continue;
        }
        int positionToChange = -1;
        if(xPos != -1){//Whole chain of if statements take in coordinates, 1d or 2d, ensures in bounds
            if(yPos != -1){
                if((xPos > static_cast<int>(gwidth) -1 ||xPos< 0|| yPos > static_cast<int>(gheight) - 1)||yPos<0){
                    std::cerr<<"Error: position is out of bounds"<<std::endl;
                    continue;
                }else{
                    positionToChange = (gwidth * yPos) + xPos;
                }
            }else if(xPos> static_cast<int>(gboard.length()) - 1||xPos < 0){
               std::cerr<<"Error: position is out of bounds"<<std::endl;
               continue; 

            }else{
                positionToChange = xPos;
            }
           
        }else{
            std::cerr<<"Error: invalid coordinate"<<std::endl;
        }
        switch (cellType){
            case 'a':
                GameOfLife::gboard[positionToChange] = GameOfLife::aliveCell;
                break;
            case 'd':
                GameOfLife::gboard[positionToChange] = GameOfLife::deadCell;
                break;
            case 'e':
                GameOfLife::gboard[positionToChange] = GameOfLife::decayCell;
                break;
        }


        
    }
    
}