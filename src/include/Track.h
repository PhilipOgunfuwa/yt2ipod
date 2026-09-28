#ifndef TRACK_H
#define TRACK_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include "Playlist.h"

// You'll note that in the codebase, Itdb_Track * are preferred to Track when actually
// manipulating iTunesDB data. We store these things simply so its easier to JSONify them
class Track {

public:
    // No defined copy/move semantics for now
    Track(Track&) = delete;
    Track &operator=(Track&) = delete;
    Track(Track&&) = delete;
    Track &operator=(Track&&) = delete;

    Track(Itdb_Track *_track);

    std::string title() const; // get title of track
    std::string artist() const; // get artist of track
    std::string album() const; // get album track is in
    std::string genre() const; // get genre of track
    std::string iPod_path() const; // get path of track in ipod (: seperated)
    gint32 track_length_ms() const; // get track length in milliseconds
    guint32 id() const; // get unique id for track

    gboolean set_title(const gchar *title);
    gboolean set_artist(const gchar *artist);
    gboolean set_album(const gchar *ablum);
    gboolean set_genre(const gchar *genre);
    gboolean set_iPod_path(const gchar *iPodPath);
    gboolean set_track_length_ms(gint32 track_length_ms);
    gboolean set_id(guint32 id);
    
    friend class Playlist; // Playlists can access internals of a track

private:
        Itdb_Track *__track; // Internal track
        Itdb_Track *internal_track(); // get internal track
};

#endif