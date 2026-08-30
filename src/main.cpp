// #include "include/httplib.h"
#include "itdb.h"
#include "include/ipod_management.h"
#include "include/yt2ipod.h"
#include <iostream>
#include <cassert>

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

    assert(pTracks && "pTracks in nullptr");
    assert(pPlaylists && "pPlaylists is nullptr");
    assert(piTunesDB && "iTunesDB is nullptr");

    if (bSuccess) {
        while (true) {
            std::cout << "Testing iteration\n";
            GError *pError { nullptr };


            for (const auto& playlist : *pPlaylists) {
                std::cout << "Freist debug write\n";
                std::cout << "playlist name: " << playlist->m_strName << '\n';
            }

            for (int i { 0 }; i < 5; i++) {
                Playlist new_playlist {
                    "Testing playlist",
                    FALSE,
                    FALSE,
                    TRASH_PL_ID
                };
                std::cout << new_playlist.m_strName << '\n';
                add_playlist(piTunesDB, pPlaylists, new_playlist, pError);
            }

            for (const auto& playlist : *pPlaylists) {
                std::cout << "second debnug writerew\n";
                std::cout << "playlist name: " << playlist->m_strName << '\n';
            }

            for(int i { 0 }; i < 4; i++) {
                Playlist& playlist { *(pPlaylists->back().get()) };
                gboolean bEntered { remove_playlist(
                    piTunesDB,
                    playlist,
                    pPlaylists,
                    pError
                ) };

                if (bEntered)
                    std::cout << "we remove stuff ig\n";

                if (pError) 
                    std::cout << "sum went run ig\n";
            }

            for (const auto& playlist : *pPlaylists) {\
                std::cout << "LLast dsebug wirtes\n";
                std::cout << "playlist name: " << playlist->m_strName << '\n';
            }

            break;
        }
    }

    shutdown(piTunesDB, pPlaylists, pTracks);

    return 0;
}