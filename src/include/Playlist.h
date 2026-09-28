#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <string>
#include <memory>
#include "Track.h"
#include "iPod.h"

class Track;
class iPod;

// You'll note that in the codebase, Itdb_Playlist * are preferred to Playlist when actually
// manipulating iTunesDB data. We store these things simply so its easier to JSONify them
class Playlist {

public:

    Playlist(Itdb_Playlist *_playlist);

    std::string name() const;
    gboolean is_mpl() const;
    gboolean is_smart_pl() const;
    guint64 id() const;
    std::vector<guint32> track_ids() const;

    gboolean set_name(const gchar *name);
    gboolean set_is_smart_pl(gboolean is_smart_pl);
    gboolean set_id(gint64 id);
    gboolean add_track(Track& track);
    gboolean remove_track(Track& track);
    gboolean contains_track(Track& track) const;

    friend class iPod;

private:
    Itdb_Playlist *__playlist; // Internal playlist
    Itdb_Playlist *internal_playlist(); // return internal playlist
};

#endif