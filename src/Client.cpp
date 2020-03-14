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
#include "Client.hpp"
#include <iostream>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>


Client::~Client() {
    board_name = "player_" + std::to_string(player) + ".action_board.json";
    remove(board_name.c_str());
}

void Client::initialize(unsigned int player, unsigned int board_size){
    this->player = player;
    this->board_size = board_size;
    vector< vector<int> > board;
    vector<int> temp_vec;
    ofstream action_board("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONOutputArchive archive( action_board );

    for (int x = 0; x < board_size; x++) {
        temp_vec.push_back(0);
    }
    for (int x = 0; x < board_size; x++) {
        board.push_back(temp_vec);
    }

    archive( CEREAL_NVP(board) );

    initialized = true;
}


void Client::fire(unsigned int x, unsigned int y) {
    ofstream file_name("player_" + std::to_string(player) + ".shot.json");
    cereal::JSONOutputArchive archive( file_name );

    archive( cereal::make_nvp("x", x), cereal::make_nvp("y", y) );
}


bool Client::result_available() {
    ifstream f("player_" + std::to_string(player) + ".result.json");
    return f.good();
}


int Client::get_result() {
    string file_name = "player_" + std::to_string(player) + ".result.json";
    ifstream file(file_name);
    cereal::JSONInputArchive archive(file);
    int result;

    archive(result);

    if (result != HIT && result != MISS && result != OUT_OF_BOUNDS){
        throw( ClientException("Unexpected result received.") );
    }

    remove(file_name.c_str());

    return result;

}



void Client::update_action_board(int result, unsigned int x, unsigned int y) {
    ifstream file("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONInputArchive read_action_board(file);
    vector< vector<int> > board;

    read_action_board(board);

    if (result == HIT) {
        board[x][y] = 1;
    } else if (result == MISS) {
        board[x][y] = -1;
    }

    ofstream action_board("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONOutputArchive archive(action_board);

    archive(CEREAL_NVP(board));
}

string Client::render_action_board(){
    ifstream file("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONInputArchive read_action_board(file);
    vector< vector<int> > board;
    string board_lines[board_size];
    string string_board;

    read_action_board( board );

    for(int i = 0; i < board_size; i++){
        for(int j = 0; j < board_size; j++){
            switch (board[j][i]) {
                case 0 : board_lines[i].append("0"); break;
                case 1 : board_lines[i].append("H"); break;
                case -1 : board_lines[i].append("M"); break;
                default : break;
            }
        }
        string_board.append(board_lines[i] + "\n");
    }

    return string_board;
}