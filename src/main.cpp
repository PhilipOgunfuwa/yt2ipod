#include "itdb.h"
#include <iostream>
#include <cassert>
#include <array>
#include "include/iPod.h"
#include "include/Server.h"

// For MP3s
#define DR_MP3_IMPLEMENTATION
#include "include/dr_mp3.h"

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "usage: prog /ipod/mnt/point /path/to/song.mp3";
        return -1;
    }

    Server svr {};
    svr.listen();

    return 0;
}