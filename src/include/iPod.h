#ifndef IPOD_H
#define IPOD_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include "Track.h"
#include "Playlist.h"
#include "dr_mp3.h"
#include <utility>
#include <filesystem>

class iPod {
public:

    // No copy/move semantics for now
    iPod(iPod&) = delete;
    iPod &operator=(iPod&) = delete;
    iPod(iPod&&) = delete;
    iPod &operator-(iPod&&) = delete;

    iPod(const gchar *mount_point);
    ~iPod();

    gboolean create_track(std::string& track_name, std::string& track_artist, std::string& track_album,
                          std::string& track_genre, std::string& song_path, guint64 playlist_id);
    gboolean add_track(guint32 track_id, guint64 playlist_id);
    gboolean remove_track(guint32 track_id, guint64 playlist_id);
    gboolean update_track(guint32 track_id, Track& updated_track);
    gboolean track_name_exists(std::string_view track_name);
    
    gboolean create_playlist(std::string& playlist_name, gboolean is_spl);
    gboolean remove_playlist(Playlist& target_playlist);
    gboolean update_playlist(Playlist& target_playlist);
    gboolean playlist_name_exists(std::string_view playlist_name);
    guint64 mpl_playlist_id() const;

    gboolean write_to_itunesdb();

    std::string_view gpod_error_msg() const;

    gboolean reset_gpod_error();

private:
    Itdb_iTunesDB *__iTunesDB;
    GError *__gpod_error;
    std::vector<std::unique_ptr<Playlist>> __playlists;
    std::vector<std::unique_ptr<Track>> __tracks;
    Playlist __error_playlist;
    Track __error_track;
    Track& track_by_id(guint32 track_id);
    Playlist& playlist_by_id(guint64 playlist_id);

    
};

#endif