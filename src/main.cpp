// #include "include/httplib.h"
#include "itdb.h"
#include "include/ipod_management.h"
#include "include/yt2ipod.h"
#include <iostream>
#include <cassert>
#include <array>

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

    std::array<std::string, 6> songPaths {
        "/home/philip-o/Desktop/temp for ipod/Music/F01/BFHC.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F01/YTUC.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F00/libgpod826620.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F00/WEUS.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F02/GAGF.mp3",
        "/home/philip-o/Desktop/temp for ipod/Music/F02/NMBO.mp3"
    };

    if (bSuccess) {
        while (true) {
            std::cout << "Testing iteration\n";
            GError *pError { nullptr };

            int indexToName { 40 };

            for (const auto& track : *pTracks) {
                std::cout << "track title: " << track->m_strTitle << '\n';
                std::cout << "track id: " << track->m_dID << '\n';
            }

            for (const auto& trackID : pPlaylists->front()->m_TrackIDs) {
                std::cout << "TrackID: " << trackID << '\n';
            }


            for (const auto& path : songPaths) {
                std::string name { path.substr(indexToName) };
                Track test_track {
                    name.c_str(),
                    "Testing Artist",
                    "Testing Album",
                    "Testing Genre",
                    "",
                    0,
                    TRASH_TRACK_ID,
                    TRUE
                };


                add_new_track(
                    piTunesDB,
                    *(pPlaylists->front()),
                    test_track,
                    pTracks,
                    path,
                    pError
                );
            }

            for (const auto& trackID : pPlaylists->front()->m_TrackIDs) {
                std::cout << "TrackID: " << trackID << '\n';
            }

            for (const auto& track : *pTracks) {
                std::cout << "track title: " << track->m_strTitle << '\n';
                std::cout << "track id: " << track->m_dID << '\n';
            }

            for (int i { 0 }; i < 6; i++) {
                Track& test_track { *(pTracks->back()) };
                remove_track(
                    piTunesDB,
                    *(pPlaylists->front()),
                    pPlaylists,
                    test_track,
                    pTracks,
                    pError
                );
            }

            for (const auto& track : *pTracks) {
                std::cout << "track title: " << track->m_strTitle << '\n';
                std::cout << "track id: " << track->m_dID << '\n';
            }

            for (const auto& trackID : pPlaylists->front()->m_TrackIDs) {
                std::cout << "TrackID: " << trackID << '\n';
            }

            

            break;
        }
    }

    shutdown(piTunesDB, pPlaylists, pTracks);

    return 0;
}