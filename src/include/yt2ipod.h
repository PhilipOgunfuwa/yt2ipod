#ifndef YT2IPOD_H
#define YT2IPOD_H

#include "ipod_stuff.h"
#include <glib.h>

gboolean setup(
    const std::string& strMountPount, 
    Itdb_iTunesDB **ppDB, 
    std::vector<std::unique_ptr<Playlist>> **ppPlaylists, 
    std::vector<std::unique_ptr<Track>> **ppPtracks
);

void shutdown(
    Itdb_iTunesDB *ppDB,
    std::vector<std::unique_ptr<Playlist>> *playlists,
    std::vector<std::unique_ptr<Track>> *tracks
);


#endif