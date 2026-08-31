#ifndef IPOD_MANAGEMENT_H
#define IPOD_MANAGEMENT_H
#include "itdb.h"
#include <glib.h>
#include <vector>
#include <string>
#include <memory>


#define END_OF_ITUNESDB -1 // Add track to end of itunesdb
#define END_OF_PL -1
#define TRASH_TRACK_ID 0xFFFFFFFF
#define TRASH_PL_ID 0xFFFFFFFFFFFFFFFF

struct Playlist; // Basically serves as a way to move between JSON Playlist that frontend uses to backend of itdb
struct Track; // Basically serves as a way to move between JSON Track that frontend uses to backend of itdb

// You'll note that in the codebase, Itdb_Playlist * are preferred to Playlist when actually
// manipulating iTunesDB data. We store these things simply so its easier to JSONify them
struct Playlist {

    Playlist(std::vector<guint32>&& TrackIDs, 
             const gchar *strName, 
             bool bIsMPL,
             bool bIsSmartPL, 
             guint64 dID);

    Playlist(const gchar *strName, 
             bool bIsMPL,
             bool bIsSmartPL, 
             guint64 dID);

    std::vector<guint32> m_TrackIDs; // unique ids for tracks in playlist
    std::string m_strName; // name of playlist
    bool m_bIsMPL; // true if playlist is master playlist
    bool m_bIsSmartPL; // true if playlist is smart playlist
    guint64 m_dID; // unique id for playlist
};

// You'll note that in the codebase, Itdb_Track * are preferred to Track when actually
// manipulating iTunesDB data. We store these things simply so its easier to JSONify them
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

Itdb_Playlist *add_playlist(
    Itdb_iTunesDB *pDB,
    std::vector<std::unique_ptr<Playlist>> *pPlaylists,
    Playlist& newPlaylist,
    GError *pError
);

gboolean update_playlist(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    GError *pError  
);

gboolean remove_playlist(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    std::vector<std::unique_ptr<Playlist>> *pPlaylists,
    GError *pError
);

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
    std::vector<std::unique_ptr<Track>> *pTracks,
    const std::string& strSrcSongPath,
    GError *pError
);

gboolean update_track(
    Itdb_iTunesDB *pDB,
    Track& targetTrack,
    GError *pError
);

gboolean remove_track(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    std::vector<std::unique_ptr<Playlist>> *pPlaylists,
    Track& track,
    std::vector<std::unique_ptr<Track>> *pTracks,
    GError *pError
);



#endif