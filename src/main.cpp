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
    guint64 mpl_id { user_iPod->mpl_playlist_id() };
    std::cout << "hi playlist\n";

    // assert(pTracks && "pTracks in nullptr");
    // assert(pPlaylists && "pPlaylists is nullptr");
    // assert(piTunesDB && "iTunesDB is nullptr");

    std::array<std::string, 6> songPaths {
        "/home/philip-o/Desktop/temp for ipod/Music/F01/BFHC.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F01/YTUC.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F00/libgpod826620.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F00/WEUS.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F02/GAGF.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F02/NMBO.mp3"
    };

    std::string name { "name" };
    std::string artist { "artist"};
    std::string album { "album" };
    std::string genre { "genre "};

    for (int i { 0 }; i < songPaths.size(); i++) {
        std::cout << i << '\n';
        user_iPod->create_track(name, artist, album, genre, songPaths.at(i), mpl_id);
        std::cout << user_iPod->gpod_error_msg() << '\n';
        std::cout << i << '\n';
    }

    user_iPod->write_to_itunesdb();

    return 0;
}