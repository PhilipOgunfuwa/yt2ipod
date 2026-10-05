#include "include/iPod.h"

iPod::iPod(const gchar *mount_point) 
    : __iTunesDB { NULL }
    , __gpod_error { NULL }
    , __playlists {}
    , __tracks {}
    , __master_playlist { NULL }
    , __error_playlist { NULL }
    , __error_track { NULL }
{
    if (mount_point) __iTunesDB = itdb_parse(mount_point, &__gpod_error);

    if (__iTunesDB) {
        GList *current_node { __iTunesDB->playlists };

        // Add playlists
        while (current_node) {
            Itdb_Playlist *current_playlist { static_cast<Itdb_Playlist *>(current_node->data) };
            __playlists.push_back( std::make_unique<Playlist>(current_playlist) );
            // Last playlist we just added is iPod's master playlist
            if (!__master_playlist && __playlists.back()->is_mpl()) 
                __master_playlist = __playlists.back().get();
            current_node = current_node->next;
        }

        current_node = __iTunesDB->tracks;
        // Add tracks
        while (current_node) {
            Itdb_Track *current_track { static_cast<Itdb_Track *>(current_node->data) };
            __tracks.push_back( std::make_unique<Track>(current_track) );
            current_node = current_node->next;
        }
    }
}

iPod::~iPod() {
    if (__gpod_error) g_error_free(__gpod_error);
    itdb_free(__iTunesDB); // This frees all internal itdb objects so we don't have to :)
    // let std::unique_ptr deallocate memory itself
}

gboolean iPod::create_track(std::string& track_name, std::string& track_artist, std::string& track_album,
                            std::string& track_genre, std::string& song_path) {
    if (!__iTunesDB || !__master_playlist) return FALSE;

    Itdb_Track *_new_track { itdb_track_new() };
    std::unique_ptr<Track> new_track { std::make_unique<Track>(_new_track) };

    new_track->set_title(track_name.c_str());
    new_track->set_artist(track_artist.c_str());
    new_track->set_album(track_album.c_str());
    new_track->set_genre(track_album.c_str());

    // Add track to iTunesDB
    itdb_track_add(__iTunesDB, new_track->internal_track(), -1);

    // Copy song at path to iPod
    // Add track+song file to iPod
    {
        gboolean success { itdb_cp_track_to_ipod(new_track->internal_track(), song_path.c_str(), &__gpod_error) }; 

        if (success) {
            std::cout << "Successfully copied track to iPod\n";

            // Update length of track (in ms)
            drmp3 song;
            drmp3_init_file(&song, song_path.c_str(), nullptr);
            drmp3_uint64 song_frame_count { drmp3_get_pcm_frame_count(&song) }; // frame
            drmp3_uint32 song_bit_rate { song.sampleRate }; // in seconds

            // Size of song is frame count / bit rate
            gint32 track_len_ms = static_cast<gint32>(
                (song_frame_count / song_bit_rate) * 1000 // to turn into ms
            );

            new_track->set_track_length_ms(track_len_ms);

            drmp3_uninit(&song);
        }

        else {
            std::cout << "Failed to copy track to iPod\n";
            return FALSE;
        }
    }

    __master_playlist->add_track(*new_track);
    __tracks.push_back(std::move(new_track));
    return TRUE;
}

gboolean iPod::add_track_to_pl(guint32 track_id, guint64 playlist_id) {

    Playlist& target_playlist { playlist_by_id(playlist_id) };
    if (&target_playlist == &__error_playlist) return FALSE;

    Track& target_track { track_by_id(track_id) };
    if (&target_track == &__error_track) return FALSE;

    target_playlist.add_track(target_track);
    return TRUE;
}

gboolean iPod::remove_track_from_pl(guint32 track_id, guint64 playlist_id) {

    Track& track { track_by_id(track_id) };
    if (&track == &__error_track) return FALSE;

    gboolean success { FALSE };

    // Remove track from ipod
    if (mpl_id() == playlist_id) {
        if (!__iTunesDB) return FALSE;

        // Remove track from any other playlists
        for (auto& playlist : __playlists) {
            if (playlist->contains_track(track)) playlist->remove_track(track);
        }

        const gchar *iPod_mount_path { itdb_get_mountpoint(__iTunesDB) };
        gchar *rel_song_path { g_strdup(track.iPod_path().c_str()) }; // We need to free this


        // We have all the data we need
        if (iPod_mount_path && rel_song_path) {
            itdb_filename_ipod2fs(rel_song_path); // iPod uses ':' as file dir deliminator. This swaps back to '/'
            gchar *abs_song_path { g_strconcat(iPod_mount_path, rel_song_path, NULL) }; // we need to free this
            std::filesystem::path song_path { abs_song_path };
            success = std::filesystem::remove(song_path);
            g_free(abs_song_path); // Free string
        }

        g_free(rel_song_path); // Free string

        if (success) { // Only do if we actually removed song file
            itdb_track_remove(track.internal_track()); // Free track & remove from iTunesDB
            
            // Remove track from saved internal tracks
            for (int i { 0 }; i < __tracks.size(); i++) {
                if (__tracks.at(i)->id() == track.id()) {
                    __tracks.erase(__tracks.begin()+i);
                    break;
                }
            }

            write_to_itunesdb(); // We write because this is very final (removing song file)
            // if we didn't write then if they rebooted after this, technically the track would still exist on
            // next time they pulled it up (but with song file gone!)
            // be sure to ask user if they are 100% sure to remove from mpl
        }
    }

    // Remove track from playlist
    else {
        Playlist& playlist { playlist_by_id(playlist_id) };
        if (&playlist == &__error_playlist) return FALSE;
        if (!playlist.contains_track(track)) return FALSE;
        success = playlist.remove_track(track);
    }

    return TRUE;
}

gboolean iPod::update_track(guint32 track_id, Track& updated_track) {
        //TODO
    return FALSE;
}

gboolean iPod::track_name_exists(std::string_view track_name) {
    for (int i { 0 }; i < __tracks.size(); i++)
        if (__tracks.at(i)->title() == track_name) return TRUE;


    return FALSE;
}

std::vector<Track> iPod::tracks() const {
    std::vector<Track> tracks {};

    for (int i { 0 }; i < __tracks.size(); i++) tracks.push_back(*__tracks.at(i));

    return tracks;
}

Track& iPod::track_by_id(guint32 track_id) {
    for (int i { 0 }; i < __tracks.size(); i++) {
        if (__tracks.at(i)->id() == track_id)
            return *__tracks.at(i);
    }

    return __error_track;
}

gboolean iPod::create_playlist(std::string& playlist_name, gboolean is_spl) {
    if (!__iTunesDB) return FALSE;

    Itdb_Playlist *_new_playlist { itdb_playlist_new(playlist_name.c_str(), is_spl) };
    std::unique_ptr<Playlist> new_playlist { std::make_unique<Playlist>(_new_playlist) };
    
    itdb_playlist_add(__iTunesDB, new_playlist->internal_playlist(), -1);
    __playlists.push_back(std::move(new_playlist));

    return TRUE;
}

gboolean iPod::remove_playlist(guint64 playlist_id) {
    if (!__iTunesDB || playlist_id == mpl_id()) return FALSE;

    Playlist& playlist { playlist_by_id(playlist_id) };
    if (&playlist == &__error_playlist) return FALSE;

    itdb_playlist_remove(playlist.internal_playlist());

    // Remove playlist from out internal playlists
    for (int i { 0 }; i < __playlists.size(); i++) {
        if (__playlists.at(i)->id() == playlist.id()) {
            __playlists.erase(__playlists.begin()+i);
            return TRUE;
        }
    }

    return FALSE;
}

gboolean iPod::update_playlist(guint64 playlist_id, Playlist& target_playlist) {
    Playlist& playlist { playlist_by_id(playlist_id) };

    if (&playlist == &__error_playlist) return FALSE;


    return FALSE;
}

gboolean iPod::playlist_name_exists(std::string_view playlist_name) {
    for (int i { 0 }; i < __playlists.size(); i++) {
        if (__playlists.at(i)->name() == playlist_name) return TRUE;
    }

    return FALSE;
}

std::vector<Playlist> iPod::playlists() const {
    std::vector<Playlist> playlists {};

    for (int i { 0 }; i < __playlists.size(); i++) playlists.push_back(*__playlists.at(i));

    return playlists;
}

Playlist& iPod::playlist_by_id(guint64 playlist_id) {
    for (int i { 0 }; i < __playlists.size(); i++) {
        if (__playlists.at(i)->id() == playlist_id)
            return *__playlists.at(i);
    }

    return __error_playlist;
}

guint64 iPod::mpl_id() const {
    if (__master_playlist) return __master_playlist->id();

    return -1; // Overflows but this should never really fail
}

gboolean iPod::write_to_itunesdb() {

    gboolean success { FALSE };

    if (!__iTunesDB) return success;

    std::cout << "\n\n";

    std::cout << "Writing to iTunesDB\n";

    success = itdb_write(__iTunesDB, &__gpod_error);

    if (success) {
        std::cout << "Successfully wrote to iTunesDB\n";
    }

    else {
        std::cout << "Failed to write to iTunesDB\n";
        std::cout << "error: " << __gpod_error->message << '\n';
    }

    std::cout << "\n\n";

    return success;
}

std::string_view iPod::gpod_error_msg() const {
    if (!__gpod_error) return "";
    return __gpod_error->message;
}

gboolean iPod::reset_gpod_error() {
    if (!__gpod_error) return FALSE;
    g_error_free(__gpod_error);
    return !__gpod_error;
}


/*

    iPod(std::string_view mount_point);
    ~iPod();

    gboolean add_track(Track& new_track, Playlist& target_playlist);
    gboolean remove_track(Track& new_track, Playlist& target_playlist);
    gboolean update_track(Track& target_track);
    gboolean track_name_exists(std::string_view track_name);

    gboolean add_playlist(Playlist& new_playlist);
    gboolean remove_playlist(Playlist& target_playlist);
    gboolean update_playlist(Playlist& target_playlist);
    gboolean playlist_name_exists(std::string_view playlist_name);


private:
    Itdb_iTunesDB *__iTunesDB;
    std::vector<std::unique_ptr<Playlist>> __playlists;
    std::vector<std::unique_ptr<Track>> __tracks;
    */