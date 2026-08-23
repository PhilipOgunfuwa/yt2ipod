// #include "include/httplib.h"
#include "itdb.h"
#include "include/ipod_stuff.h"
#include "include/yt2ipod.h"
#include <iostream>

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

    std::vector<std::unique_ptr<Track>> *pTracks { nullptr };
    std::vector<std::unique_ptr<Playlist>> *pPlaylists { nullptr };
    Itdb_iTunesDB *piTunesDB { nullptr };

    gboolean bSuccess { setup(strMountPoint, &piTunesDB, &pPlaylists, &pTracks) };

    if (bSuccess) {
        while (true) {
            std::cout << "Testing iteration\n";
            GError *pError { nullptr };

            std::unique_ptr<Track> newTrack { std::make_unique<Track>(
                "Hesitating", "Malcom Todd", "Prom before", "Pop", "", 0, TRASH_TRACK_ID, FALSE
            )};

            Itdb_Track *pTrack { add_new_track(
                piTunesDB,
                *(pPlaylists->front()),
                *newTrack,
                strPathToSong, 
                pError
            )};

            if (pTrack) {
                pTracks->push_back(std::move(newTrack));
            }

            break;
        }
    }

    shutdown(piTunesDB, pPlaylists, pTracks);

    return 0;
}