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

struct StateStr {
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
        Cell(std::vector<StateStr> states);
        void collapse(std::vector<std::string> neighbours, std::vector<std::vector<StateStr>> neighbourStateList);
        void setState();
        std::vector<StateStr> stateList;
};
    Cell::Cell(std::vector<StateStr> states){
        stateList = states;
    }

    void Cell::collapse(std::vector<std::string> neighbours, std::vector<std::vector<StateStr>> neighbourStateList){
        if (state != null){return;}
        std::vector<StateStr> possibleStates;
        for (StateStr i: stateList){
            bool validState = true;
            for (std::string j: neighbours){
                if (std::find(i.neighbours.begin(), i.neighbours.end(), j) == i.neighbours.end()){
                    validState = false;
                    break;
                }
            }
            if (validState){possibleStates.push_back(i);}
        }
        std::vector<StateStr> possibleStatesAfterPass;
        for (StateStr i: possibleStates){
            bool validState = true;
            for (std::vector<StateStr> j: neighbourStateList){
                bool pseudoValid = false;
                for (StateStr k: j){
                    if (std::find(i.neighbours.begin(), i.neighbours.end(), k.state) != i.neighbours.end()){
                        pseudoValid = true;
                        break;
                    }
                }
                if (!pseudoValid){
                    validState = false;
                    break;
                }
            }
            if (validState){possibleStatesAfterPass.push_back(i);}
        }
        stateList = possibleStatesAfterPass;
    }

    void Cell::setState(){
        state = stateList[rand() % stateList.size()].state;
    }

class Grid {
    public:
        std::vector<std::vector<Cell>> grid;
        int width;
        int height;
        Grid(int width, int height, std::vector<StateStr> stateList);
        void print();
        void generate();
    private:
        std::vector<Pos> visited;
        void propagate(Pos pos, bool optimisedProp);    
};
    Grid::Grid(int w, int h, std::vector<StateStr> stateList){
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
        std::vector<Pos> minPos = std::vector<Pos>();
        Pos pos = Pos(rand() % width, rand() % height);
        propagate(pos, true);
        bool genFin = false;
        while (!genFin){
            print();
            visited = std::vector<Pos>();
            genFin = true;
            int minStates = 999;
            minPos.clear();
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    if (grid[i][j].state == null){
                        genFin = false;
                        int states = grid[i][j].stateList.size();
                        if (states < minStates){
                            minStates = states;
                            minPos.clear();
                            minPos.push_back(Pos(j,i));
                        }
                    }
                }
            }
            if (genFin){break;}
            propagate(minPos[rand() % minPos.size()], true);
        }

    }

    void Grid::propagate(Pos xy, bool hard){
        for (int i = 0; i < visited.size(); i++){
            if (visited[i].x == xy.x && visited[i].y == xy.y){
                return;
            }
        }
        visited.push_back(xy);
        std::vector<std::string> neighbours;
        std::vector<Pos> neighbourPosList;
        std::vector<std::vector<StateStr>> neighbourStateList;
        for (auto i: offsets){
            std::string neighbourState = null;
            Pos neighbourPos = i.add(xy);
            if (neighbourPos.within(width,height)){
                neighbourState = grid[neighbourPos.y][neighbourPos.x].state;
                if (neighbourState == null){
                    neighbourPosList.push_back(neighbourPos);
                    neighbourStateList.push_back(grid[neighbourPos.y][neighbourPos.x].stateList);
                }
            }
            if (neighbourState != null){
                neighbours.push_back(neighbourState);
            }
        }
        int prevStateListSize = grid[xy.y][xy.x].stateList.size();
        grid[xy.y][xy.x].collapse(neighbours, neighbourStateList);
        if (hard){grid[xy.y][xy.x].setState();}
        if (!hard && prevStateListSize == grid[xy.y][xy.x].stateList.size()){return;}
        //print();
        for (auto i: neighbourPosList){
            propagate(i, false);
        }
    }
    
    void Grid::print(){
        std::string output;
        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                if (grid[i][j].state != null){output.append(grid[i][j].state + " ");}
                else {output.append("\x1b[0m" + std::to_string(grid[i][j].stateList.size()) + " ");}
            }
            output.append("\n");
        }
        std::cout << "\x1b[H\x1b[0J" << output << "\x1b[0m";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //std::cin.get();
    }


int main() {
    auto start = std::chrono::high_resolution_clock::now();
    srand(time(NULL));
    std::string s1 = "\x1b[1;34m~\x1b[22m";
    std::string s2 = "\x1b[33m=";
    std::string s3 = "\x1b[1;32m#\x1b[22m";
    std::string s4 = "\x1b[32m|";
    std::string s5 = "\x1b[37m^";
    std::vector<StateStr> stateList = {{s1,{s1,s2,s3}},{s2,{s3}},{s3,{s1,s3,s4,s5}},{s4,{s4,s3,s5}},{s5,{s4}}};

    Grid map(75,40,stateList);
    map.generate();
    map.print();

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

    for (int i = 0; i < stateList.size(); i++){std::cout << stateList[i].state << "\x1b[0m: " << ((double)totals[i] / (map.grid[0].size() * map.grid.size()) * 100.) << "% ";}
    std::cout << "\n\x1b[38;5;1mDone generating. Press enter to exit.\x1b[0m\nTime taken to complete: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms";
    std::cin.get();

	return 0;
}
