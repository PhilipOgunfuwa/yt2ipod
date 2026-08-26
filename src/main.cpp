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

            for (const auto& track : *pTracks) {
                std::cout << "Cur track: " << track->m_strTitle << '\n';
            }

            Track& TargetTrack { *(pTracks->at(3)) };
            std::cout << "Track before \"" << TargetTrack.m_strTitle << "\"\n";

            TargetTrack.m_strArtist = "Test";
            TargetTrack.m_strTitle = "Testing Title";

            update_track(piTunesDB, TargetTrack, pError);

            std::cout << "Track after \"" << TargetTrack.m_strTitle << "\"\n";

            for (const auto& track : *pTracks) {
                std::cout << "Cur track: " << track->m_strTitle << '\n';
            }

            break;
        }
    }

    shutdown(piTunesDB, pPlaylists, pTracks);

    return 0;
}