// #include "include/httplib.h"
#include "itdb.h"
#include "include/yt2ipod.h"
#include <iostream>

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "usage: prog /ipod/mnt/point\n";
        return -1;
    }


    std::string strMountPoint { argv[1] };
    GError *pError = nullptr;
    Itdb_iTunesDB *pIpodMusicDB = itdb_parse(strMountPoint.c_str(), &pError);

    std::vector<Track> tracks { get_tracks(pIpodMusicDB) };
    std::vector<Playlist> playlists { get_playlists(pIpodMusicDB) };

    for (const auto& playlist : playlists) {
        std::cout << playlist.m_strName << '\n';
    }

    for (const auto& track : tracks) {
        std::cout << track.m_strTitle << '\n';
        std::cout << "path: " << track.m_strIpodPath << '\n';
    }



    Track track {
        "E85 [Official Visualizer].mp3",
        "Don Toliver",
        "Octane",
        "Rap",
        "",
        TRASH_TRACK_ID,
        TRUE,
    };

    const gchar *filepath { "resources/Don Toliver - E85 [Official Visualizer].mp3" };

    Itdb_Track *pTrack { add_new_track(pIpodMusicDB, strMountPoint, playlists[0], track, filepath, &pError) };

    if (pTrack) {
        std::cout << "added to ipod :)\n";
        std::cout << pTrack->ipod_path << '\n';
        std::cout << pTrack->id << '\n';
        itdb_write(pIpodMusicDB, &pError);
    }

    itdb_free(pIpodMusicDB);


    return 0;
}