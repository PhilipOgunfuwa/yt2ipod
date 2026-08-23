#ifndef YT2IPOD_H
#define YT2IPOD_H

#include "ipod_stuff.h"
#include <glib.h>

gboolean setup(
    const std::string& strMountPount, 
    Itdb_iTunesDB *pDB, 
    std::vector<std::unique_ptr<Playlist>> *playlists, 
    std::vector<std::unique_ptr<Track>> *tracks
);

void shutdown(
    Itdb_iTunesDB *pDB,
    std::vector<std::unique_ptr<Playlist>> *playlists,
    std::vector<std::unique_ptr<Track>> *tracks
);


#endif