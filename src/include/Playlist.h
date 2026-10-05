#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <string>
#include <memory>
#include "Track.h"

// You'll note that in the codebase, Itdb_Playlist * are preferred to Playlist when actually
// manipulating iTunesDB data. We store these things simply so its easier to JSONify them
class Playlist {

public:

    Playlist(Itdb_Playlist *_playlist);

    std::string name() const; // Name of playlist
    gboolean is_mpl() const; // Boolean of whether playlist is iPod master playlist
    gboolean is_smart_pl() const; // Boolean of whether playlist smart playlist
    guint64 id() const; // Unique ID of playlist
    std::vector<guint32> track_ids() const; // Get copy of all track ids in playlist
    gboolean operator==(const Playlist& other) const;
 
    gboolean set_name(const gchar *name); // Set name of playlist
    gboolean set_is_smart_pl(gboolean is_smart_pl); // Set wether playlist is smart pl
    gboolean set_id(gint64 id); // Set playlist id
    gboolean add_track(Track& track); // Add track from playlist
    gboolean remove_track(Track& track); // Remove track from playlist
    gboolean contains_track(Track& track) const; // Returns boolean if track is in playlist

    friend class iPod;

private:
    Itdb_Playlist *__playlist; // Internal playlist
    Itdb_Playlist *internal_playlist(); // return internal playlist
};

#endif