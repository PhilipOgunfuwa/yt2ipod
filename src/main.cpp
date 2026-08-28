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
                std::cout << "Playlist ame: " << playlist->m_strName << '\n';
                std::cout << "id: " << playlist->m_dID << '\n';
            }

            Playlist &testPlaylist { *(pPlaylists->at(2)) };
            std::cout << testPlaylist.m_strName << " not so new name\n";
            testPlaylist.m_strName = "why no change :(";

            gboolean bSuccess { update_playlist(piTunesDB, testPlaylist, pError) };

            for (const auto& playlist : *pPlaylists) {
                std::cout << "Playlist ame: " << playlist->m_strName << '\n';
                std::cout << "id: " << playlist->m_dID << '\n';
            }

            break;
        }
    }

    shutdown(piTunesDB, pPlaylists, pTracks);

    return 0;
}