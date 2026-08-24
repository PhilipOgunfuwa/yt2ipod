#ifndef IPOD_STUFF_H
#define IPOD_STUFF_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <string>
#include <memory>


#define END_OF_ITUNESDB -1 // Add track to end of itunesdb
#define END_OF_PL -1
#define TRASH_TRACK_ID 0xFFFFFFFF
#define TRASH_PL_ID 0xFFFFFFFFFFFFFFFF

struct Playlist;
struct Track;

struct Playlist {

    Playlist(std::vector<guint32>&& TrackIDs, 
             const gchar *strName, 
             bool bIsSmartPL, 
             guint64 dID);

    std::vector<guint32> m_TrackIDs; // unique ids for tracks in playlist
    std::string m_strName; // name of playlist
    bool m_bIsSmartPL; // true if playlist is smart playlist
    guint64 m_dID; // unique id for playlist
};

struct Track {

    Track(const gchar *strTitle,
          const gchar *strArtist,
          const gchar *strAlbum,
          const gchar *strGenre,
          const gchar *strIpodPath,
          gint32 dTrackLen_ms,
          guint32 dID,
          gboolean bTransferred);

    std::string m_strTitle; // title of track
    std::string m_strArtist; // artist of track
    std::string m_strAlbum; // album track is in
    std::string m_strGenre; // genre of track
    std::string m_strIpodPath; // path of track in ipod
    gint32 m_dTrackLen_ms; // length of track in ms
    guint32 m_dID; // unique id for track
    gboolean m_bTransferred; // true if track needs to be added to iTunesDB
};

// functions that front end will call
/*
        Playlist Functions
*/

std::vector<std::unique_ptr<Playlist>> *get_playlists(Itdb_iTunesDB *pDB);

/*
        Track Functions
*/

std::vector<std::unique_ptr<Track>> *get_tracks(Itdb_iTunesDB *pDB);

std::vector<guint32> get_track_ids(Itdb_Playlist *pPlaylist);


// May want to consolidate this into one func (combine w/ add_new_track)
Itdb_Track *add_track(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    Track& track,
    GError *pError
);

Itdb_Track *add_new_track(
    Itdb_iTunesDB *pDB, 
    Playlist& targetPlaylist, 
    Track& newTrack,
    const std::string& strSrcSongPath,
    GError *pError
);

gboolean remove_track(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    Track& track,
    GError *pError
);

#endif