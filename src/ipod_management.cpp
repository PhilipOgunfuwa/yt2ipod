#include "itdb.h"
#include "include/ipod_management.h"
#include "include/dr_mp3.h"
#include <utility>
#include <filesystem>
#include <cassert> // or #include <assert.h> fine
#include <string>
#include <string_view>
#include <iostream>
#include <array>
#include <algorithm>

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
             gint32 dTrackLen_ms,
             guint32 dID,
             gboolean bTransferred)
    : m_strTitle { "" }
    , m_strArtist { "" }
    , m_strAlbum { "" }
    , m_strGenre { "" }
    , m_strIpodPath { "" }
    , m_dTrackLen_ms { dTrackLen_ms }
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
                   bool bIsMPL, 
                   bool bIsSmartPL, 
                   guint64 dID) 
    : m_TrackIDs { TrackIDs }
    , m_strName { "" }
    , m_bIsMPL { bIsMPL }
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
std::vector<std::unique_ptr<Playlist>> *get_playlists(Itdb_iTunesDB *pDB) {
    std::vector<std::unique_ptr<Playlist>> *playlists { new std::vector<std::unique_ptr<Playlist>>{} };

    if (!pDB)
        return playlists;

    GList *pCurrentNode { pDB->playlists };

    // Populate playlists from iTunesDB
    while (pCurrentNode) {
        Itdb_Playlist *pCurPlaylist { static_cast<Itdb_Playlist *>(pCurrentNode->data) };

        if (!pCurPlaylist)
            break;

        std::vector<guint32> trackIDs = get_track_ids(pCurPlaylist);
        gboolean bIsMPL { itdb_playlist_is_mpl(pCurPlaylist) };

        // Make playlist (that front end will use)
        std::unique_ptr<Playlist> playlist { std::make_unique<Playlist>(
            std::move(trackIDs),
            pCurPlaylist->name,
            bIsMPL,
            pCurPlaylist->is_spl,
            pCurPlaylist->id
        )};

        playlists->push_back(std::move(playlist));
        
        pCurrentNode = pCurrentNode->next;
    }

    return playlists;
}

/// @brief Get tracks from iTunesDB
/// @param pDB 
/// @return tracks from iTunesDB 
std::vector<std::unique_ptr<Track>> *get_tracks(Itdb_iTunesDB *pDB) {
    std::vector<std::unique_ptr<Track>> *tracks { new std::vector<std::unique_ptr<Track>>{} };

    if (pDB == nullptr)
        return tracks;

    GList *pCurrentNode { pDB->tracks };

    // Populate tracks
    while (pCurrentNode) {
        Itdb_Track *pCurTrack { static_cast<Itdb_Track *>(pCurrentNode->data) };

        if (!pCurTrack)
            break;

        // Make track (that the front end will use)

        std::unique_ptr<Track> track { std::make_unique<Track>(
            pCurTrack->title,
            pCurTrack->artist,
            pCurTrack->album,
            pCurTrack->genre,
            pCurTrack->ipod_path,
            pCurTrack->tracklen,
            pCurTrack->id,
            FALSE // Track doesn't need added to iTunesDB
        )};

        tracks->push_back(std::move(track));

        pCurrentNode = pCurrentNode->next;
    }

    return tracks;
}

/// @brief Adds track to iTunesDB and uses path to a string
/// @param pDB 
/// @param strMountPoint
/// @param targetPlaylist
/// @param newTrack 
/// @param strPathToSong
/// @param pError
/// @return Itdb_Track if successfully added to itdb else NULL we also set pError
Itdb_Track *add_new_track(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    Track& newTrack,
    const std::string& strSrcSongPath,
    GError *pError
) 
{
    std::cout << "Adding track to iPod\n";

    assert(pDB && "Passed nullptr to for the iTunesDB");
    assert(!pError && "Passed non nullptr for the GError");

    Itdb_Track *pTrack { nullptr };

    if (!pDB || pError)
        return pTrack;

    pTrack = itdb_track_new();

    if (!pTrack)
        return pTrack; // will be a nullptr

    // Set up pTrack with data from newTrack
    pTrack->title = g_strdup(newTrack.m_strTitle.c_str());
    pTrack->artist = g_strdup(newTrack.m_strAlbum.c_str());
    pTrack->album = g_strdup(newTrack.m_strAlbum.c_str());
    pTrack->genre = g_strdup(newTrack.m_strGenre.c_str());


    // Add track to iTunesDB, and then update track to be linked to song and copy to iPod
    itdb_track_add(pDB, pTrack, END_OF_ITUNESDB);

    // Add track+song file to iPod
    {
        gboolean bSuccess { itdb_cp_track_to_ipod(pTrack, strSrcSongPath.c_str(), &pError) }; 

        
        if (bSuccess) {
            std::cout << "Successfully copied track to iPod\n";
            // Update id
            newTrack.m_dID = pTrack->id;

            // Update length of track (in ms)
            drmp3 song;
            drmp3_init_file(&song, strSrcSongPath.c_str(), nullptr);
            drmp3_uint64 dSongFrameCount { drmp3_get_pcm_frame_count(&song) }; // frame
            drmp3_uint32 dSongBitRate { song.sampleRate }; // in seconds

            // Size of song is frame count / bit rate
            gint32 dTrackLen_ms = static_cast<gint32>(
                (dSongFrameCount / dSongBitRate) * 1000
            );

            pTrack->tracklen = dTrackLen_ms;
            newTrack.m_dTrackLen_ms = dTrackLen_ms;

            drmp3_uninit(&song);
        }

        else {
            std::cout << "Failed to copy track to iPod\n";
        }
    }

    std::cout << "Adding track to playlist (" <<  targetPlaylist.m_strName << ")\n";
    // Add track to target playlist
    Itdb_Playlist *pTargetPlaylist { itdb_playlist_by_id(pDB, targetPlaylist.m_dID) };
    itdb_playlist_add_track(pTargetPlaylist, pTrack, END_OF_PL);
    targetPlaylist.m_TrackIDs.push_back(pTrack->id); // Add id to playlist
    std::cout << "Added track to playlist\n";

    return pTrack;
}

/// @brief Get ids of tracks in Itdb_Playlist
/// @param pPlaylist 
/// @return ids of tracks 
std::vector<guint32> get_track_ids(Itdb_Playlist *pPlaylist) {

    assert(pPlaylist && "Passed nullptr for Itdb Playlist");

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

/// @brief Remove track from target playlist (And all other playlists if it is the master playlist)
/// @param pDB 
/// @param targetPlaylist 
/// @param pPlaylists 
/// @param track 
/// @return True on success and False in any other case
gboolean remove_track(
    Itdb_iTunesDB *pDB,
    Playlist& targetPlaylist,
    std::vector<std::unique_ptr<Playlist>> *pPlaylists,
    Track& targetTrack,
    GError *pError
)
{
    std::cout << "Removing track from iPod\n";

    assert(pDB && "Passed nullptr for iTunesDB");
    assert(pPlaylists && "Passed nullptr for Playlists");

    if (!pDB || !pPlaylists)
        return FALSE;

    Itdb_Playlist *pTargetItdbPl { itdb_playlist_by_id(pDB, targetPlaylist.m_dID) };

    if (!pTargetItdbPl) {
        std::cout << "Target playlist \"" << targetPlaylist.m_strName << "\" is not in iTunesDB\n";
        return FALSE;
    }

    Itdb_Track *pTargetItdbTrack { itdb_track_by_id(pDB, targetTrack.m_dID) };

    if (!pTargetItdbTrack) {
        std::cout << "Target track \"" << targetTrack.m_dID << "\" is not in iTunesDB\n";
        return FALSE;
    }

    // Get all playlists that contain track and need to be removed    
    gboolean bIsMPL { itdb_playlist_is_mpl(pTargetItdbPl) };

    if (bIsMPL) { // If removing from MPL we are removing from EVERYTHING
        for (const auto& playlist : *pPlaylists) {
            Itdb_Playlist *pCurItdbPl { itdb_playlist_by_id(pDB, playlist->m_dID) };
            
            // Remove track if playlist has it
            if (itdb_playlist_contains_track(pCurItdbPl, pTargetItdbTrack)) {
                std::cout << "Removing track \"" << targetTrack.m_strTitle << "\" from \"" << playlist->m_strName << "\"\n";
                itdb_playlist_remove_track(pCurItdbPl, pTargetItdbTrack);
                std::cout << "Removed track \"" << targetTrack.m_strTitle << "\" from \"" << playlist->m_strName << "\"\n";

                // Remove track from regular playlists object
                // Note that dTrackID is an iterator for the trackID in playlist
                auto trackID { std::find(playlist->m_TrackIDs.begin(), playlist->m_TrackIDs.end(), targetTrack.m_dID) };

                // Track ID (this should ALWAYS execute...) is in our verison of playlist
                if (trackID != playlist->m_TrackIDs.end());
                    playlist->m_TrackIDs.erase(trackID);
            }
        }

        std::cout << "Removing \"" << targetTrack.m_strTitle << "\" from iPod iTunesDB\n";
        itdb_track_remove(pTargetItdbTrack);
        std::cout << "Removed \"" << targetTrack.m_strTitle << "\" from iPod iTunesDB\n";
        
        std::cout << "Removing \"" << targetTrack.m_strIpodPath << "\" from iPod Music Directory\n";
        {
            gboolean bSuccess { FALSE };

            const gchar *striPodMountPath { itdb_get_mountpoint(pDB) };
            gchar *strRelSongPath { g_strdup(targetTrack.m_strIpodPath.c_str()) }; // We need to free this

            // We have all the data we need
            if (striPodMountPath && strRelSongPath) {
                itdb_filename_ipod2fs(strRelSongPath); // iPod uses ':' as file dir deliminator. This swaps back to '/'
                gchar *strAbsSongPath { g_strconcat(striPodMountPath, strRelSongPath, NULL) }; // we need to free this
                std::cout << "Absolute path to song is \"" << strAbsSongPath << "\"\n";
                std::filesystem::path AbsSongPath { strAbsSongPath };

                // Successfully removed song
                if (std::filesystem::remove(AbsSongPath)) 
                    std::cout << "Removed \"" << targetTrack.m_strIpodPath << "\" from iPod Music Directory\n";

                // Didn't remove song
                else 
                    std::cout << "Failed to remove \"" << targetTrack.m_strIpodPath << "\" from iPod Music Directory\n";

                g_free(strAbsSongPath);
            }
            else {
                std::cout << "Failed to remove \"" << targetTrack.m_strIpodPath << "\" from iPod Music Directory\n";
                if (!striPodMountPath)
                    std::cout << "Failed to get iPods mount point from iTunesDB\n";

                if (!strRelSongPath)
                    std::cout << "Failed to get iPods relative song path from target track\n";
            }

            g_free(strRelSongPath);
        }

    }

    // Remove from single target playlist
    else {
        if (itdb_playlist_contains_track(pTargetItdbPl, pTargetItdbTrack)) {
            std::cout << "Removing track \"" << targetTrack.m_strTitle << "\" from \"" << targetPlaylist.m_strName << "\"\n";
            itdb_playlist_remove_track(pTargetItdbPl, pTargetItdbTrack);
            std::cout << "Removed track \"" << targetTrack.m_strTitle << "\" from \"" << targetPlaylist.m_strName << "\"\n";

            // Remove track from regular playlists object
            // Note that dTrackID is an iterator for the trackID in playlist
            auto trackID { std::find(targetPlaylist.m_TrackIDs.begin(), targetPlaylist.m_TrackIDs.end(), targetTrack.m_dID) };

                // Track ID (this should ALWAYS execute...) is in our verison of playlist
                if (trackID !=targetPlaylist.m_TrackIDs.end());
                    targetPlaylist.m_TrackIDs.erase(trackID);
        }
    }

    std::cout << "Writing to iTunesDB...\n";
    gboolean bSuccess { itdb_write(pDB, &pError) };

    if (bSuccess) {
        std::cout << "Successfully wrote to iTunesDB\n";
        return TRUE;
    }

    // Failure
    std::cout << "Failed to write to iTunesDB\n";
    std::cout << "error: " << pError->message << '\n';
    return FALSE;
}
