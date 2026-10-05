#ifndef IPOD_H
#define IPOD_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
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

    // Simply const char * b/c itdb_parse uses that
    iPod(const gchar *mount_point);
    ~iPod();

    gboolean create_track(std::string& track_name, std::string& track_artist, std::string& track_album,
                          std::string& track_genre, std::string& song_path); // Create track, and add song to iPod filesystem
    gboolean add_track_to_pl(guint32 track_id, guint64 playlist_id); // Add already created track to a playlist
    gboolean remove_track_from_pl(guint32 track_id, guint64 playlist_id); // Remove a track from a playlist (if mpl remove from iPod)
    gboolean update_track(guint32 track_id, Track& updated_track); // Update data in track
    gboolean track_name_exists(std::string_view track_name); // See if track with same name exists
    std::vector<Track> tracks() const; // Get copy of all tracks
    
    gboolean create_playlist(std::string& playlist_name, gboolean is_spl); // Create playlist
    gboolean remove_playlist(guint64 playlist_id); // Remove playlist
    gboolean update_playlist(guint64 playlist_id, const std::string& new_name, ); // Update playlist data
    gboolean playlist_name_exists(std::string_view playlist_name); // See if playlist with same name exists
    guint64 mpl_id() const; // Get id of master playlist 
    std::vector<Playlist> playlists() const; // Get copy of all playlists

    gboolean write_to_itunesdb(); // Save changes to iTunesDB (iPod)

    std::string_view gpod_error_msg() const; // Get message from gpod error if any

    gboolean reset_gpod_error(); // Reset gpod error

private:
    Itdb_iTunesDB *__iTunesDB; // Internal iTunesDB on iPod
    GError *__gpod_error; // error that libgpod uses for error msgs
    std::vector<std::unique_ptr<Playlist>> __playlists; // Internal playlists on iPod
    std::vector<std::unique_ptr<Track>> __tracks; // Internal tracks on iPod
    Playlist *__master_playlist; // Master playlist that ALL iPods must have
    Playlist __error_playlist; // Playlist returned when error occurs finding one (DNE)
    Track __error_track; // Track returned when error occurs finding one (DNE)
    Track& track_by_id(guint32 track_id); // Get reference to iPod track (instead of copy)
    Playlist& playlist_by_id(guint64 playlist_id); // Get reference to iPod playlist (instead of copy)
};

#endif