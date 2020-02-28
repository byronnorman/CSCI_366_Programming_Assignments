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
#include <cereal/archives/json.hpp>
#include <fstream>

//To-do handle exceptions
Client::~Client() {
    board_name = "player_" + std::to_string(player) + ".action_board.json";
    remove(board_name.c_str());
}

void Client::initialize(unsigned int player, unsigned int board_size){
    this->player = player;
    this->board_size = board_size;
    std::string blank = "_";
    ofstream action_board("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONOutputArchive archive( action_board );

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            archive( cereal::make_nvp(std::to_string(i) + std::to_string(j), blank) );
        }
    }

    action_board.close();
    initialized = true;
}


void Client::fire(unsigned int x, unsigned int y) {
    ofstream file_name("player_" + std::to_string(player) + ".shot.json");
    std::string shot = std::to_string(x) + ", " + std::to_string(y);
    cereal::JSONOutputArchive archive( file_name );

    archive(CEREAL_NVP(shot));
}


bool Client::result_available() {
    ifstream f("player_" + std::to_string(player) + ".result.json");
    return f.good();
}


int Client::get_result() {
    ifstream file("player_" + std::to_string(player) + ".result.json");
    cereal::JSONInputArchive archive(file);
    int result;

    archive(result);

    if (result == 0) {
        return MISS;
    }
    else if (result == 1){
        return HIT;
    }
    else {
        return OUT_OF_BOUNDS;
    }
}



void Client::update_action_board(int result, unsigned int x, unsigned int y) {
    ofstream action_board("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONOutputArchive archive( action_board );

    if (result == HIT) {
        archive( cereal::make_nvp(std::to_string(x) + std::to_string(y), "H") );
    }
    else {
        archive( cereal::make_nvp(std::to_string(x) + std::to_string(y), "M") );
    }
}


string Client::render_action_board(){
    ifstream file("player_" + std::to_string(player) + ".action_board.json");
    cereal::JSONInputArchive archive(file);
    string board_lines [board_size];
    string board;

    for(int i = 0; i < board_size; i++){
        for(int j = 0; j < board_size; j++){
            string s;
            archive( cereal::make_nvp(std::to_string(i) + std::to_string(j), s) );
            board_lines[i].append(s);
        }
        board.append(board_lines[i] + "\n");
    }

    return board;
}