#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "GameOfLife.h"
#include <unistd.h> 
#include <algorithm>
#include <sstream>
std:: vector<std::string> readLines(std::ifstream& file, char deadChar, char aliveChar){
    int height,width;
    std:: vector<std::string> finalVect;
    std::string finalString;
    if (file >> height >> width) {  //reads two integers and stores them in height and width
        std:: string dummy;
        std::getline(file, dummy); //this just makes sure that it finished the line and all trailing whitespace
        if(!dummy.empty() && dummy.back() == '\r'){
            dummy.pop_back();
        }
        if(!dummy.empty()){
            std::cerr <<"Error: overfull header" << std::endl;
            finalVect.push_back("Error");
            finalVect.push_back("1");
            return finalVect;
        }
    } else {
        std::cerr << "Error: invalid height and width" << std::endl;
            finalVect.push_back("Error");
            finalVect.push_back("1");
            return finalVect;
    }

        std::string line;
    int row = 1;
    while (std::getline(file, line)) {
       if(!line.empty() && (line.back() == '\r')){
            line.pop_back();
        }
        
        row = row + 1;
        if(line.empty()){
            break;
        }
        int rowChecker = row;
        if(rowChecker - 1> height){
            std::cerr <<"Error: Invalid height on row " << row <<  std::endl;
            std::cerr<<"Return code: " << rowChecker + 1<<std::endl;
            finalVect.push_back("Error");
            finalVect.push_back(std::to_string(row + 1));
            return finalVect;
        }
        int lineLength = 0;
        for(char c: line){
            if(c != ' '){
                lineLength++;
            }else{
                continue;
            }
        }
        if(lineLength != width){
            std::cerr <<"Error: Invalid length on row " << row << " length is "<<lineLength<<" but expected was: " << width<< std::endl;
            std::cerr<<"Return code: " << rowChecker + 1<<std::endl;
            finalVect.push_back("Error");
            finalVect.push_back(std::to_string(row + 1));
            return finalVect;
        }
        for(char c: line){
            if(c == ' ')continue;
            if(c != 'X' && c != 'O' && c != '\r'){
                std::cerr<<"Error: Invalid character on row " << row << ". Character is " << c << std::endl;
                std::cerr<<"Return code: " << rowChecker + 1 <<std::endl;
                finalVect.push_back("Error");
                finalVect.push_back(std::to_string(row + 1));
                return finalVect;
            }else{
                if(c == 'X'){
                    finalString.push_back(deadChar);
                }else if( c == 'O'){
                    finalString.push_back(aliveChar);
                }
            }
        }

    }
   
    if(row -1 < height){
                std::cerr <<"Error: Invalid height, too short on row "<< row<< std::endl;
                std::cerr<<"Return code: " << row <<std::endl;
                finalVect.push_back("Error");
                finalVect.push_back(std::to_string(row + 1));
                return finalVect;
    }
            
   /* for(std::vector<int> vectorPrint: lastVector){
        for(int element: vectorPrint){
            std::cout<<element;
        }
    }
    std::cout<<std::endl;
    for(std::vector<int> vectorPrint: lastVector){
        for(int element: vectorPrint){
            std::cout<<element;
        }
         std::cout<<std::endl;
    }*/
    finalVect.push_back(std::to_string(height));
    finalVect.push_back(std::to_string(width));
    finalVect.push_back(finalString);
    return finalVect;
    
       
}

int main(int argc, char* argv[]){
   std::string fileName;
   std::string output = "random.txt";
   std::vector<std::string> gameData;
    int generations = 10;
    int delay = 1;
    int buffer = 100;
    bool decay = false;
    bool wrap = false;
    bool needOutputFile = false;
    int opt;
    int pauseInterval = -1;
    char aliveChar = '1';
    char decayChar = 'D';
    char deadChar = '0';
    
    // Option string: f,g,p require args; s,w are flags
    while ((opt = getopt(argc, argv, "f:g:p:r:x:o:d:e:a:sw")) != -1) {
        switch (opt) {
            case 'f':
                fileName = optarg;
                break;
            case 'o':
                output = optarg;
                needOutputFile = true;
                break;
            case 'a':
                aliveChar = optarg[0];
                break;
            case 'd':
                deadChar = optarg[0];
                break;
            case 'e':
                decayChar = optarg[0];
                break;
            case 'g':
                generations = std::stoi(optarg);
                break;
            case 'x':
                pauseInterval = std::stoi(optarg);
                break;
            case 'p':
                delay = std::stoi(optarg);
                break;
            case 'r':
                buffer = std::stoi(optarg);
                break;
            case 's':
                decay = true;
                break;
            case 'w':
                wrap = true;
                break;
            case '?':
            default:
                std::cerr << "Error: Unknown option or missing argument.\n";
                return 1;
        }
    }
   
    if(fileName.empty()){
        std::cerr << "Error: filename is empty" << std::endl;
        return 1;
    }
    std::ifstream file(fileName);
    if (!file) {
        std::cerr <<"Error: could not open file " << fileName;
        std::cerr<<"Return code: " << 1;
        return 1;
    }
    gameData = readLines(file, deadChar, aliveChar);
    GameOfLife* newBoard;
    if((wrap == true && decay == true)){
        std::cerr << "Error: two game types can not be given, both -w and -s were given" << std::endl;
        return 1;
    }else if(wrap == true){
         newBoard = new GameOfLifeWrap(std::stoi(gameData[0]), std::stoi(gameData[1]), gameData[2], buffer);
    }else if(decay == true){
         newBoard = new GameOfLifeDecay(std::stoi(gameData[0]), std::stoi(gameData[1]), gameData[2], buffer);
    }else{
         newBoard = new GameOfLife(std::stoi(gameData[0]), std::stoi(gameData[1]), gameData[2], buffer);
    }
    if(aliveChar == deadChar||aliveChar ==  decayChar|| deadChar == decayChar){
         std::cerr<<"Error: Can not set multiple cells to the same char"<<std::endl;
         return 1;
    }else{
        newBoard->aliveCell = aliveChar;
        newBoard->deadCell = deadChar;
        newBoard->decayCell = decayChar;
    }
    newBoard->history.push_back(gameData[2]);
     std::ofstream fout(output); // creates file if it doesn't exist
    if (!fout) {
        std::cerr << "Error: Failed to open output file" << std::endl;
        return 1;
    }
    if(needOutputFile == true){
                newBoard->printGame(fout);
                 fout << std::endl;
    }else{
                newBoard->printGame();
                std::cout<<std::endl;
              
    }
  
    for(int i = 1; i < generations + 1; i++){
         newBoard->next();
        if(pauseInterval != -1 && (i % pauseInterval == 0) && (i != generations)){
            newBoard->changeCells();
            /*if(needOutputFile == true){
                newBoard->printGame(fout);
                fout << std::endl;
                continue;
            }else{
                newBoard->printGame();
                 std::cout << std::endl;
                 continue;
            }*/
         
        }
       
        if(i % delay == 0){
            if(needOutputFile == true){
                newBoard->printGame(fout);
                fout << std::endl;
            }else{
                newBoard->printGame();
                 std::cout << std::endl;
            }
           
        }
    }
    delete newBoard;
 return 0;
}