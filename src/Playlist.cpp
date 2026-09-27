#include "include/Playlist.h"

Playlist::Playlist(Itdb_Playlist *_playlist)
    : __playlist { _playlist }
{}

std::string Playlist::name() const {
    if (__playlist && __playlist->name) return __playlist->name;
    return "";
}

gboolean Playlist::is_mpl() const { 
    if (__playlist) return itdb_playlist_is_mpl(__playlist);
    return FALSE;
}

gboolean Playlist::is_smart_pl() const {
    if (__playlist) return __playlist->is_spl;
    return FALSE;
}

guint64 Playlist::id() const {
    if (__playlist) return __playlist->id;
    return -1; // Overflows but it doesn't really matter
}

std::vector<guint32> Playlist::track_ids() const {

    std::vector<guint32> _track_ids {};

    if (__playlist) {
        // Get track ids in playlist
        GList *pCurrentNode = __playlist->members;

        // Populate buffer of track ids for each track in playlist
        while (pCurrentNode) {
            Itdb_Track *pCurrentTrack = static_cast<Itdb_Track *>(pCurrentNode->data);

            // Playlist has track(s)
            if (pCurrentTrack) _track_ids.push_back(pCurrentTrack->id);

            pCurrentNode = pCurrentNode->next;
        }
    }

    return _track_ids;
}

gboolean Playlist::set_name(const gchar *name) {
    if (__playlist && name) {
        __playlist->name = g_strdup(name);
        return __playlist->name != NULL;
    }

    return FALSE;
}

gboolean Playlist::set_is_smart_pl(gboolean is_smart_pl) {
    if (__playlist) {
        __playlist->is_spl = is_smart_pl;
        return TRUE;
    }

    return FALSE;
}

gboolean Playlist::set_id(gint64 id) {
    if (__playlist) {
        __playlist->id = id;
        return TRUE;
    }

    return FALSE;
}


/*
    gboolean set_name(const gchar *name);
    gboolean set_is_mpl(gboolean is_mpl);
    gboolean set_is_smart_pl(gboolean is_smart_pl);
    gboolean set_id(gint64 id);
    gboolean add_track(const Track& track);
    gboolean remove_track(const Track& track);
*/