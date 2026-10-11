// #include "include/httplib.h"
#include "itdb.h"
// #include "include/ipod_management.h"
// #include "include/yt2ipod.h"
#include <iostream>
#include <cassert>
#include <array>
#include "include/iPod.h"

// For MP3s
#define DR_MP3_IMPLEMENTATION
#include "include/dr_mp3.h"

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "usage: prog /ipod/mnt/point /path/to/song.mp3";
        return -1;
    }


    std::string strMountPoint { argv[1] };
    std::string strPathToSong { argv[2] };

    std::cout << "testing :)\n";
    std::unique_ptr<iPod> user_iPod { std::make_unique<iPod>(strMountPoint.c_str()) };
    std::cout << "hi playlist\n";

    // assert(pTracks && "pTracks in nullptr");
    // assert(pPlaylists && "pPlaylists is nullptr");
    // assert(piTunesDB && "iTunesDB is nullptr");

    std::array<std::string, 2> songPaths {
        "/home/philip-o/Desktop/ipod stuff/Malcolm Todd - Gun To My Head (Official Visualizer) [youaretube.com].mp3",
        "/home/philip-o/Desktop/ipod stuff/Malcolm Todd - Gun To My Head (Official Visualizer) [youaretube.com].mp3"
    };

    std::string name { "name" };
    std::string artist { "artist"};
    std::string album { "album" };
    std::string genre { "genre "};

    guint64 pl_id { 0 };

    for (int i { 0 }; i < 100; i++) user_iPod->create_playlist(name, FALSE);
    user_iPod->write_to_itunesdb();
    

    return 0;
}