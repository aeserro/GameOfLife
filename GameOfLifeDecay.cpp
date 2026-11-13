#include "GameOfLife.h"
#include <iostream>
GameOfLifeDecay::GameOfLifeDecay(int height, int width, std::string board, int buffer){
    GameOfLife::gheight = height;
    GameOfLife::gwidth = width;
    GameOfLife::gboard = board;
    GameOfLife::buffer = buffer;
    
}
GameOfLifeDecay::GameOfLifeDecay(){
    GameOfLife::gheight = 0;
    GameOfLife::gwidth = 0;
    GameOfLife::gboard = "";
}
void GameOfLifeDecay::next(){
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
        if((live < 2 || live > 3) && gboard[i] == aliveCell){
            newBoard.push_back(decayCell);
        }else if((live == 2 || live == 3) && (gboard[i] == aliveCell)){
            newBoard.push_back(aliveCell);
        } else if((live < 2 || live > 3) && gboard[i] == decayCell){
            newBoard.push_back(deadCell);
        }else if(live == 3 && (gboard[i] == decayCell)){
            newBoard.push_back(aliveCell);
        }else if(live == 2 && gboard[i] == decayCell){
            newBoard.push_back(decayCell);
        }else if(live == 3 && gboard[i] == deadCell){
            newBoard.push_back(decayCell);
        }else{
            newBoard.push_back(gboard[i]);
        }
    }
    GameOfLifeDecay::gboard = newBoard;
     if(GameOfLife::history.size() < static_cast<size_t>(buffer)){
        GameOfLife::history.push_back(newBoard);
    }else{
        GameOfLife::fullHistory(newBoard);
    }
    GameOfLifeDecay::gen++;

}
