#include "include/Track.h"

Track::Track(Itdb_Track *_track)
    : __track { _track }
{}

std::string Track::title() const {
    if (__track && __track->title) return __track->title;
    return "";
}

std::string Track::artist() const {
    if (__track && __track->artist) return __track->artist;
    return "";
}

std::string Track::album() const {
    if (__track && __track->album) return __track->album;
    return "";
}

std::string Track::genre() const {
    if (__track && __track->genre) return __track->genre;
    return "";
}

std::string Track::iPod_path() const {
    if (__track && __track->ipod_path) return __track->ipod_path;
    return "";
}

gint32 Track::track_length_ms() const {
    if (__track) return __track->tracklen;
    return -1;
}

guint32 Track::id() const {
    if (__track) return __track->id;
    return -1; // Will over flow but it doesn't really matter
}

gboolean Track::set_title(const gchar *title) {
    if (__track && title) {
        __track->title = g_strdup(title);
        return __track->title != NULL;
    }

    return FALSE;
}

gboolean Track::set_title(const gchar *title) {
    if (__track && title) {
        __track->title = g_strdup(title);
        return __track->title != NULL;
    }

    return FALSE;
}

gboolean Track::set_artist(const gchar *artist) {
    if (__track && artist) {
        __track->artist = g_strdup(artist);
        return __track->artist != NULL;
    }

    return FALSE;
}

gboolean Track::set_album(const gchar *album) {
    if (__track && album) {
        __track->album = g_strdup(album);
        return __track->album != NULL;
    }

    return FALSE;
}

gboolean Track::set_genre(const gchar *genre) {
    if (__track && genre) {
        __track->genre = g_strdup(genre);
        return __track->genre != NULL;
    }

    return FALSE;
}

gboolean Track::set_iPod_path(const gchar *iPod_path) {
    if (__track && iPod_path) {
        __track->ipod_path = g_strdup(iPod_path);
        return __track->ipod_path != NULL;
    }

    return FALSE;
}

gboolean Track::set_track_length_ms(gint32 track_length_ms) {
    if (__track) {
        __track->tracklen = track_length_ms;
        return TRUE;
    }

    return FALSE;
}
gboolean Track::set_id(guint32 id) {
    if (__track) {
        __track->id = id;
        return TRUE;
    }

    return FALSE;
}