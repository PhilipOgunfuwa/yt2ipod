#include "include/iPod.h"

iPod::iPod(const gchar *mount_point) 
    : __iTunesDB { NULL }
    , __gpod_error { NULL }
    , __playlists {}
    , __tracks {}
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
    itdb_free(__iTunesDB);
}

gboolean iPod::create_track(std::string& track_name, std::string& track_artist, std::string& track_album,
                            std::string& track_genre, std::string& song_path, guint64 playlist_id) {
    if (!__iTunesDB || !itdb_playlist_by_id(__iTunesDB, playlist_id)) return FALSE;

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

    playlist_by_id(playlist_id).add_track(*new_track);
    __tracks.push_back(std::move(new_track));
    return TRUE;
}

gboolean iPod::add_track(guint32 track_id, guint64 playlist_id) {
    
    Playlist& target_playlist { playlist_by_id(playlist_id) };
    if (&target_playlist == &__error_playlist) return FALSE;

    Track& target_track { track_by_id(track_id) };
    if (&target_track == &__error_track) return FALSE;

    target_playlist.add_track(target_track);
    return TRUE;
}

gboolean iPod::remove_track(guint32 track_id, guint64 playlist_id) {
    return FALSE;
}

gboolean iPod::update_track(guint32 track_id, Track& updated_track) {
    return FALSE;
}

gboolean iPod::track_name_exists(std::string_view track_name) {
    for (int i { 0 }; i < __tracks.size(); i++)
        if (__tracks.at(i)->title() == track_name) return TRUE;


    return FALSE;
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

gboolean iPod::remove_playlist(Playlist& target_playlist) {
    return FALSE;
}

gboolean iPod::update_playlist(Playlist& target_playlist) {
    return FALSE;
}

gboolean iPod::playlist_name_exists(std::string_view playlist_name) {
    return FALSE;
}

Playlist& iPod::playlist_by_id(guint64 playlist_id) {
    for (int i { 0 }; i < __playlists.size(); i++) {
        if (__playlists.at(i)->id() == playlist_id)
            return *__playlists.at(i);
    }

    return __error_playlist;
}

guint64 iPod::mpl_playlist_id() const {
    for (int i { 0 }; i < __playlists.size(); i++) {
        if (__playlists.at(i)->is_mpl()) return __playlists.at(i)->id();
    }

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