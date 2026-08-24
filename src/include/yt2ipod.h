#ifndef YT2IPOD_H
#define YT2IPOD_H

#include "ipod_management.h"
#include <glib.h>

gboolean setup(
    const std::string& strMountPount, 
    Itdb_iTunesDB **ppDB, 
    std::vector<std::unique_ptr<Playlist>> **ppPlaylists, 
    std::vector<std::unique_ptr<Track>> **ppPtracks
);

void shutdown(
    Itdb_iTunesDB *pDB,
    std::vector<std::unique_ptr<Playlist>> *pPlaylists,
    std::vector<std::unique_ptr<Track>> *pTracks
);


#endif