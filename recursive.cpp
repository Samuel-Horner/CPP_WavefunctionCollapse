#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <time.h>
#include <chrono>
#include <thread>

const std::string null = "\x1b[0m0";

class Pos {
    public:
        int x;
        int y;
        Pos(int xin, int yin);
        Pos add(Pos a);
        bool within(int maxx, int maxy);
};
    Pos::Pos(int xin, int yin){
        x = xin;
        y = yin;
    }
    Pos Pos::add(Pos a){
        return Pos(x + a.x, y + a.y);
    }
    bool Pos::within(int maxx, int maxy){
        return ((x < maxx) && (x >= 0) && (y < maxy) && (y >= 0));
    }

struct stateStr {
    std::string state;
    std::vector<std::string> neighbours;
};

const Pos offsets[4] = {Pos(0,1),Pos(0,-1),Pos(1,0),Pos(-1,0)};

std::vector<Pos> shuffle(std::vector<Pos> v) {
	for(int i = v.size()-1; i >= 0; i--) {
		int j = rand() % (i + 1);

		std::swap(v[i], v[j]);
	}
	return v;
}

class Cell {
    public:
        std::string state = null;
        Cell(std::vector<stateStr> states);
        void collapse(std::vector<std::string> neighbours);
    private:
       std::vector<stateStr> stateList;
};
    Cell::Cell(std::vector<stateStr> states){
        stateList = states;
    }

    void Cell::collapse(std::vector<std::string> neighbours){
        std::vector<std::string> possibleStates;
        for (stateStr i: stateList){
            bool validState = true;
            for (std::string j: neighbours){
                if (find(i.neighbours.begin(), i.neighbours.end(), j) == i.neighbours.end()){
                    validState = false;
                    break;
                }
            }
            if (validState){possibleStates.push_back(i.state);}
        }
        state = possibleStates[rand() % possibleStates.size()];
    }

class Grid {
    public:
        std::vector<std::vector<Cell>> grid;
        int width;
        int height;
        Grid(int width, int height, std::vector<stateStr> stateList);
        void print();
        void generate();
    private:
        void propagate(Pos pos);
};
    Grid::Grid(int w, int h, std::vector<stateStr> stateList){
        std::vector<Cell> gridRow;
        for (int i = 0; i < h; i++){
            gridRow = {};
            for (int j = 0; j < w; j++){
                gridRow.push_back(Cell(stateList));
            }
            grid.push_back(gridRow);
        }
        width = w;
        height = h;
    }

    void Grid::generate(){
        propagate(Pos(rand() % width, rand() % height));
    }

    void Grid::propagate(Pos xy){
        if (grid[xy.y][xy.x].state != null){return;}
        std::vector<std::string> neighbours;
        std::vector<Pos> neighbourPosList;
        for (auto i: offsets){
            std::string neighbourState = null;
            Pos neighbourPos = i.add(xy);
            if (neighbourPos.within(grid[0].size(),grid.size())){
                neighbourState = grid[neighbourPos.y][neighbourPos.x].state;
                if (neighbourState == null){
                    neighbourPosList.push_back(neighbourPos);
                }
            }
            if (neighbourState != null){
                neighbours.push_back(neighbourState);
            }
        }
        grid[xy.y][xy.x].collapse(neighbours);
        print();
        neighbourPosList = shuffle(neighbourPosList);
        for (auto i: neighbourPosList){
            propagate(i);
        }
    }
    
    void Grid::print(){
        std::string output;
        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                output.append(grid[i][j].state + " ");
            }
            output.append("\n");
        }
        std::cout << "\x1b[H\x1b[0J" << output << "\x1b[0m";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }


int main() {
    auto start = std::chrono::high_resolution_clock::now();
    srand(time(NULL));
    std::string s1 = "\x1b[1;34m~\x1b[22m";
    std::string s2 = "\x1b[33m=";
    std::string s3 = "\x1b[1;32m#\x1b[22m";
    std::string s4 = "\x1b[32m|";
    std::string s5 = "\x1b[37m^";
    std::vector<stateStr> stateList = {{s1,{s1,s2}},{s2,{s2,s1,s3}},{s3,{s3,s2,s1,s4,s5}},{s4,{s4,s3,s5}},{s5,{s4,s5}}};

    Grid map(75,40,stateList);
    map.generate();

    // Debug info
    auto stop = std::chrono::high_resolution_clock::now();

    int totals[5] = {0,0,0,0,0};
    for (auto i: map.grid){
        for (auto j: i){
            for (int k = 0; k < stateList.size(); k++){
                if (stateList[k].state == j.state){
                    totals[k] += 1;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < stateList.size(); i++){std::cout << stateList[i].state << "\x1b[0m: " << ((double)totals[i] / (75. * 40.) * 100.) << "% ";}
    std::cout << "\n\x1b[38;5;1mDone generating. Press enter to exit.\x1b[0m\nTime taken to complete: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms";
    std::cin.get();

	return 0;
}
