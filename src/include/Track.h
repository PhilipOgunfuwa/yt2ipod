#ifndef TRACK_H
#define TRACK_H

#include "itdb.h"
#include <glib.h>
#include <vector>
#include <string>
#include <string_view>
#include <memory>

// You'll note that in the codebase, Itdb_Track * are preferred to Track when actually
// manipulating iTunesDB data. We store these things simply so its easier to JSONify them
class Track {

    Track(Track&) = delete;
    Track& operator=(Track&) = delete;
    Track(Track&&) = delete;
    Track& operator=(Track&&) = delete;

    Track(std::string_view strTitle,
          std::string_view strArtist,
          std::string_view strAlbum,
          std::string_view strGenre,
          std::string_view strIpodPath,
          gint32 dTrackLen_ms,
          guint32 dID,
          gboolean bTransferred);

    std::string title; // title of track
    std::string artist; // artist of track
    std::string album; // album track is in
    std::string genre; // genre of track
    std::string iPodPath; // path of track in ipod
    gint32 trackLen_ms; // length of track in ms
    guint32 id; // unique id for track
    gboolean transferred; // true if track needs to be added to iTunesDB
};

#endif