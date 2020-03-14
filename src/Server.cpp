//    Battleship game assignment for MSU CSCI 366
//    Copyright (C) 2020    Mike P. Wittie
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "common.hpp"
#include "Server.hpp"


/**
 * Calculate the length of a file (helper function)
 *
 * @param file - the file whose length we want to query
 * @return length of the file in bytes
 */
int get_file_length(ifstream *file){
    int length;

    file->seekg (0, file->end);
    length = file->tellg();
    length -= BOARD_SIZE;

    return length;
}


void Server::initialize(unsigned int board_size, string p1_setup_board, string p2_setup_board){
    int board_length = board_size*board_size;
    this->board_size = board_size;
    std::ifstream p1_board(p1_setup_board);
    std::ifstream p2_board(p2_setup_board);

    if (get_file_length(&p1_board) != board_length || get_file_length(&p2_board) != board_length){
        throw(ServerException("Player's set up board out of bounds."));
    }

}


int Server::evaluate_shot(unsigned int player, unsigned int x, unsigned int y) {
    if (player < 1 || player > MAX_PLAYERS) {
        throw(ServerException("Player number out of bounds"));
    }

    string file_name;
    if (player == 1) {
        file_name = "player_2.setup_board.txt";
    }
    else {
        file_name = "player_1.setup_board.txt";
    }
    std::ifstream board(file_name);

    if (x < 0 || y < 0 || x >= board_size || y >= board_size){
        return OUT_OF_BOUNDS;
    }

    string line;
    char position;
    int count = 0;
    while( std::getline(board, line) && count <= y){
        position = line.at(x);
        count++;
    }

    if (position == 'C' || position == 'B' || position == 'R' || position == 'S' || position == 'D') {
        return HIT;
    }
    else {
        return MISS;
    }
}

int Server::process_shot(unsigned int player) {
    if (player < 1 || player > MAX_PLAYERS) {
        throw(ServerException("Player number out of bounds"));
    }

    string shot_file_name = "player_" + std::to_string(player) + ".shot.json";
    ifstream shot_file(shot_file_name);
    if (!shot_file.good()){
        return NO_SHOT_FILE;
    }
    cereal::JSONInputArchive shot(shot_file);
    int x;
    int y;

    shot( CEREAL_NVP(x), CEREAL_NVP(y) );

    ofstream file_name("player_" + std::to_string(player) + ".result.json");
    cereal::JSONOutputArchive result( file_name );

    result( cereal::make_nvp("result", evaluate_shot(player, x, y)) );

    remove(shot_file_name.c_str());

    return SHOT_FILE_PROCESSED;
}