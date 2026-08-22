#include "itdb.h"
#include "include/yt2ipod.h"

/// @brief Initialize a track
/// @param strTitle 
/// @param strArtist 
/// @param strAlbum 
/// @param strGenre 
/// @param strIpodPath 
/// @param dID 
Track::Track(const gchar *strTitle,
             const gchar *strArtist,
             const gchar *strAlbum,
             const gchar *strGenre,
             const gchar *strIpodPath,
             guint32 dID,
             gboolean bTransferred)
    : m_strTitle { "" }
    , m_strArtist { "" }
    , m_strAlbum { "" }
    , m_strGenre { "" }
    , m_strIpodPath { "" }
    , m_dID { dID }
    , m_bTransferred { bTransferred }
{

    // Convert C-style string to std::strings if non null

    if (strTitle)
        m_strTitle = strTitle;

    if (strArtist)
        m_strArtist = strArtist;

    if (strAlbum)
        m_strAlbum = strAlbum;

    if (strGenre)
        m_strGenre = strGenre;

    if (strIpodPath)
        m_strIpodPath = strIpodPath;
}

/// @brief Initialize a playlist
/// @param TrackIDs 
/// @param strName 
/// @param bIsSmartPL 
/// @param dID 
Playlist::Playlist(std::vector<guint32>&& TrackIDs, 
                   const gchar *strName, 
                   bool bIsSmartPL, 
                   guint64 dID) 
    : m_TrackIDs { TrackIDs }
    , m_strName { "" }
    , m_bIsSmartPL { bIsSmartPL }
    , m_dID { dID }
{
    // Turn C-style strings to std::strings if non null
    if (strName)
        m_strName = (strName);
}

/// @brief Get playlists from iTunesDB
/// @param pDB 
/// @return array of playlists
std::vector<Playlist> get_playlists(Itdb_iTunesDB *pDB) {
    std::vector<Playlist> playlists {};

    if (!pDB)
        return playlists;

    GList *pCurrentNode { pDB->playlists };

    // Populate playlists from iTunesDB
    while (pCurrentNode) {
        Itdb_Playlist *pCurPlaylist { static_cast<Itdb_Playlist *>(pCurrentNode->data) };

        if (!pCurPlaylist)
            break;

        std::vector<guint32> trackIDs = get_track_ids(pCurPlaylist);

        // Make playlist (that front end will use)
        Playlist playlist {
            std::move(trackIDs),
            pCurPlaylist->name,
            pCurPlaylist->is_spl,
            pCurPlaylist->id,
        };

        playlists.push_back(std::move(playlist));
        
        pCurrentNode = pCurrentNode->next;
    }

    return playlists;
}

/// @brief Get tracks from iTunesDB
/// @param pDB 
/// @return tracks from iTunesDB
std::vector<Track> get_tracks(Itdb_iTunesDB *pDB) {
    std::vector<Track> tracks {};

    if (pDB == nullptr)
        return tracks;

    GList *pCurrentNode { pDB->tracks };

    // Populate tracks
    while (pCurrentNode) {
        Itdb_Track *pCurTrack { static_cast<Itdb_Track *>(pCurrentNode->data) };

        if (!pCurTrack)
            break;

        // Make track (that the front end will use)
        Track track {
            pCurTrack->title,
            pCurTrack->artist,
            pCurTrack->album,
            pCurTrack->genre,
            pCurTrack->ipod_path,
            pCurTrack->id,
            FALSE, // Track doesn't need added to iTunesDB
        };

        tracks.push_back(std::move(track));

        pCurrentNode = pCurrentNode->next;
    }

    return tracks;
}

/// @brief Adds track to iTunesDB and uses path to a string
/// @param pDB 
/// @param strMountPoint
/// @param track 
/// @param strPathToSong
/// @param pError
/// @return Itdb_Track if successfully added to itdb else NULL we also set pError
Itdb_Track *add_new_track(
    Itdb_iTunesDB *pDB,
    const std::string& strMountpoint,
    Playlist& targetPlaylist,
    Track& newTrack,
    const std::string& strSrcSongPath,
    GError **pError
) 
{
    Itdb_Track *pTrack { nullptr };

    if (!pDB || *pError)
        return pTrack;

    pTrack = itdb_track_new();

    if (!pTrack)
        return pTrack; // will be a nullptr

    // Set up pTrack with data from newTrack
    pTrack->title = g_strdup(newTrack.m_strTitle.c_str());
    pTrack->artist = g_strdup(newTrack.m_strAlbum.c_str());
    pTrack->album = g_strdup(newTrack.m_strAlbum.c_str());
    pTrack->genre = g_strdup(newTrack.m_strGenre.c_str());


    // Copy song to ipod and then add track to ipod 
    itdb_track_add(pDB, pTrack, END_OF_ITUNESDB);
    itdb_cp_track_to_ipod(pTrack, strSrcSongPath.c_str(), pError); 


    // Add track to target playlist
    Itdb_Playlist *pTargetPlaylist { itdb_playlist_by_id(pDB, targetPlaylist.m_dID) };
    itdb_playlist_add_track(pTargetPlaylist, pTrack, END_OF_PL);

    return pTrack;
}

/// @brief Get ids of tracks in Itdb_Playlist
/// @param pPlaylist 
/// @return ids of tracks 
std::vector<guint32> get_track_ids(Itdb_Playlist *pPlaylist) {
    std::vector<guint32> trackIDs {};

    if (pPlaylist == nullptr)
        return trackIDs;

    GList *pCurrentNode = pPlaylist->members;

    // Populate buffer of track ids for each track in playlist
    while (pCurrentNode) {
        Itdb_Track *pCurrentTrack = static_cast<Itdb_Track *>(pCurrentNode->data);

        // Playlist has track(s)
        if (pCurrentTrack) {
            trackIDs.push_back(pCurrentTrack->id);
        }

        pCurrentNode = pCurrentNode->next;
    }

    return trackIDs;
}