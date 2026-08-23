// #include "include/httplib.h"
#include "itdb.h"
#include "include/ipod_stuff.h"
#include "include/yt2ipod.h"
#include <iostream>

// For MP3s
#define DR_MP3_IMPLEMENTATION
#include "include/dr_mp3.h"

int main(int argc, char** argv) {

    if (argc == 1 || argc > 3) {
        std::cout << "usage: prog /ipod/mnt/point /path/to/song.mp3";
        return -1;
    }


    std::string strMountPoint { argv[1] };
    std::string strPathToSong { argv[2] };

    std::vector<std::unique_ptr<Track>> *tracks { nullptr };
    std::vector<std::unique_ptr<Playlist>> *playlists { nullptr };
    Itdb_iTunesDB *piTunesDB { nullptr };

    gboolean bSuccess { setup(strMountPoint, piTunesDB, playlists, tracks) };

    if (bSuccess) {
        while (true) {
            GError *pError { nullptr };

            Itdb_Track *pTrack { add_new_track(
                piTunesDB,
                strMountPoint,
                *(playlists->front()),
                *(tracks->front()),
                strPathToSong, 
                &pError
            )};


            break;
        }
    }

    shutdown(piTunesDB);

    return 0;
}