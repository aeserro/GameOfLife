#include "GameOfLife.h"
#include <iostream>

GameOfLifeWrap::GameOfLifeWrap(int height, int width, std::string board, int buffer){
    GameOfLife::gheight = height;
    GameOfLife::gwidth = width;
    GameOfLife::gboard = board;
    GameOfLife::buffer = buffer;
}
GameOfLifeWrap::GameOfLifeWrap(){
    GameOfLife::gheight = 0;
    GameOfLife::gwidth = 0;
    GameOfLife::gboard = "";
}


std::vector<char> GameOfLifeWrap::returnNeighbors(int position){
    std::vector<char> neighbors;
    /*if(position == 0){ // top left corner
        neighbors.push_back(gboard[1]);
        neighbors.push_back(gboard[gwidth]);
        neighbors.push_back(gboard[gwidth + 1]);
        neighbors.push_back(gboard[gwidth - 1]);
        neighbors.push_back(gboard[(2*gwidth) - 1]);
    }else if(position == gwidth - 1){// top right corner
        neighbors.push_back(gboard[gwidth - 2]);
        neighbors.push_back(gboard[(2*gwidth) - 1]);
        neighbors.push_back(gboard[(2 *gwidth) - 2]);
        neighbors.push_back(gboard[0]);
        neighbors.push_back(gboard[gwidth]);
    }else if(position == (gwidth * (gheight - 1))){ // bottom left corner
        neighbors.push_back(gboard[(gwidth * (gheight - 2))]);
        neighbors.push_back(gboard[(gwidth * (gheight - 2) + 1)]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1))+ 1]);
        neighbors.push_back(gboard[(gwidth * gheight) - 1]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1)) - 1]);
    }else if(position == (gwidth * gheight) - 1){ // bottom right corner
        neighbors.push_back(gboard[(gwidth * gheight) - 2]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1)) - 2]);
        neighbors.push_back(gboard[(gwidth * (gheight - 1)) - 1]);
        neighbors.push_back(gboard[gwidth * (gheight - 1)]);
        neighbors.push_back(gboard[(gwidth * (gheight - 2))]);
    }else if(position < gwidth){ //top edge
        neighbors.push_back(gboard[position + 1]);
        neighbors.push_back(gboard[position - 1]);
        neighbors.push_back(gboard[(position + gwidth)]);
        neighbors.push_back(gboard[(position + gwidth) + 1]);
        neighbors.push_back(gboard[(position + gwidth) - 1]);
    }else if((position%gwidth) == 0){ // left edge
        neighbors.push_back(gboard[position - gwidth]);
        neighbors.push_back(gboard[(position - gwidth) + 1]);
        neighbors.push_back(gboard[position + 1]);
        neighbors.push_back(gboard[position + gwidth]);
        neighbors.push_back(gboard[(position + gwidth) + 1]);
        neighbors.push_back(gboard[position + (gwidth - 1)]);
        neighbors.push_back(gboard[position + ((2 *gwidth) - 1)]);
        neighbors.push_back(gboard[position - 1]);
    }else if((position%gwidth) == (gwidth - 1)){ // right edge
        neighbors.push_back(gboard[(position - gwidth)]);
        neighbors.push_back(gboard[(position - gwidth) - 1]);
        neighbors.push_back(gboard[(position -1)]);
        neighbors.push_back(gboard[(position + gwidth)]);
        neighbors.push_back(gboard[(position + gwidth) - 1]);
        neighbors.push_back(gboard[position - (gwidth - 1)]);
        neighbors.push_back(gboard[position - ((2 *gwidth) - 1)]);
        neighbors.push_back(gboard[position + 1]);
    }else if(position > (gwidth * (gheight - 1)) && position < (gwidth * gheight) - 1){ // bottom edge
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
    }*/
    int row = position / gwidth;
    int col = position % gwidth;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // skip the cell itself

            int neighborRow = (row + i + gheight) % gheight; //formula given in class to get neighbor position in wrap around board
            int neighborCol = (col + j + gwidth) % gwidth;

            int neighborPos = (neighborRow * gwidth) + neighborCol; //finds exact neighbor position.

            neighbors.push_back(gboard[neighborPos]); 
        }
    }

    
    return neighbors;
}
void GameOfLifeWrap::next(){
    std::string newBoard;
    
    for(size_t i = 0; i< gboard.length(); i++){
        std:: vector<char> neighbors;
        neighbors = GameOfLifeWrap:: returnNeighbors(i);
        int live = 0;
    
        for(char j: neighbors){
            if(j == aliveCell){
                live++;
            }
        }
        if(live < 2 && gboard[i] == aliveCell){
            newBoard.push_back(deadCell);
        }else if((live == 2 || live == 3) && gboard[i] == aliveCell ){
            newBoard.push_back(aliveCell);
        } else if(live > 3 && gboard[i] == aliveCell){
            newBoard.push_back(deadCell);
        }else if(live == 3 && gboard[i] == deadCell){
            newBoard.push_back(aliveCell);
        }else{
            newBoard.push_back(gboard[i]);
        }
    }
    GameOfLifeWrap::gboard = newBoard;
     if(GameOfLife::history.size() < static_cast<size_t>(buffer)){
        GameOfLife::history.push_back(newBoard);
    }else{
        GameOfLife::fullHistory(newBoard);
    }
    GameOfLifeWrap::gen++;

}