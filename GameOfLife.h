#include <string>
#include <vector>
#include <sstream>
class GameOfLife{
    public: 
        std:: string gboard;
        std::vector<std::string> history;
        int buffer;
        int gheight;
        int gwidth;
        int gen = 0;
        char deadCell;
        char decayCell;
        char aliveCell;
        GameOfLife(int height, int width, std::string board, int buffer);
        GameOfLife();
        virtual void next();
        virtual void nextNGen(int genToAdd);
        virtual std:: vector<char> returnNeighbors(int position);
        void printGame();
        void printGame(std::ofstream&);
        virtual ~GameOfLife() {}
        void changeCells();
        void fullHistory(std::string board);
        
        
        

};
class GameOfLifeDecay : public GameOfLife{
    public:
        GameOfLifeDecay(int height, int width, std::string board, int buffer);
        GameOfLifeDecay();
        void next() override;


};
class GameOfLifeWrap : public GameOfLife{
    
    public:
        GameOfLifeWrap(int height, int width, std::string board, int buffer);
        GameOfLifeWrap();
        std:: vector<char> returnNeighbors(int position) override;
        void next() override;

};


