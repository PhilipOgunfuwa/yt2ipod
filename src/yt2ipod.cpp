#include "include/yt2ipod.h"
#include "include/ipod_stuff.h"
#include <memory>
#include <iostream>
#include <string>
#include <cassert>

/// @brief Sets up iTunesDB from mount point, and puts playlists and tracks into passed in vecs
/// @param strMountPoint 
/// @param pDB // Out parameter
/// @param playlists // Out parameter
/// @param tracks // Out parameter
/// @return True if setup was successful, and false in any other case
gboolean setup(
    const std::string& strMountPoint, 
    Itdb_iTunesDB **pDB, 
    std::vector<std::unique_ptr<Playlist>> **playlists,
    std::vector<std::unique_ptr<Track>> **tracks
)
{
    std::cout << "Entering yt2ipod setup\n";
    assert(pDB && "Passed a non nullptr for iTunesDB");
    assert(playlists && "Passed a non nullptr for playlists");
    assert(tracks && "Passed a non nullptr for tracks");

    if (!pDB || !playlists || !tracks) {
        std::cout << "Exiting yt2ipod setup\n";
        return FALSE;
    }

    GError *pError { nullptr };
    std::cout << "Opening iTunesDB from iPod mount point @ " << strMountPoint << '\n';
    *pDB = itdb_parse(strMountPoint.c_str(), &pError);

    if (!*pDB || pError) {
        std::cout << "Failed to open iTunesDB\n";
        std::cout << "error: " << pError->message << '\n'; // might want to switch to using fprintf(stderr, char*fmt, ...) here
        std::cout << "Exiting yt2ipod setup\n";
        return FALSE;
    }

    // Copy the iTuneDB to passed in iTunesDB

    std::cout << "Successfully opened iTunesDB\n";
    std::cout << "Getting track(s) from iTunesDB\n";
    *tracks = get_tracks(*pDB);
    std::cout << "total of " << (*tracks)->size() << " track(s)\n";

    std::cout << "Getting playlist(s) from iTunesDB\n";
    *playlists = get_playlists(*pDB);
    std::cout << "total of " << (*playlists)->size() << " playlist(s)\n";

    /*
    I imagine server stuff will be here aswell
    */

    std::cout << "Successfully setup yt2ipod\n";
    return TRUE;
}


/// @brief Frees anything from iTunesDB
/// @param pDB 
/// @param pPlaylists
/// @param pTracks
void shutdown(
    Itdb_iTunesDB *pDB,
    std::vector<std::unique_ptr<Playlist>> *pPlaylists,
    std::vector<std::unique_ptr<Track>> *pTracks
) 
{

    std::cout << "Entering yt2ipod shutdown\n";
    // assert(pDB && "Passed a nullptr for iTunesDB");

    // Possibly handle adding tracks/playlists that are not already in itdb (Go thru and check)

    if (pDB) {
        const std::string& strMountPoint { itdb_get_mountpoint(pDB) };
        std::cout << "Closing iTunesDB from iPod mount point @ " << strMountPoint << '\n';

        // do final writes to iTunesDB
        std::cout << "Writing to iTunesDB...\n";
        GError *pError { nullptr };
        gboolean bSuccess { itdb_write(pDB, &pError) };

        if (bSuccess) {
            std::cout << "Successfully wrote to iTunesDB\n";
        }

        else {
            std::cout << "Failed to write to iTunesDB\n";
            std::cout << "error: " << pError->message << '\n';
            /*
            We should probably add a way to ask if someone wants to write to iTunesDB again??
            */
           std::cout << "Changes made since last iTunesDB write won't be applied\n";
        }

        std::cout << "Freeing memory for iTunesDB\n";
        itdb_free(pDB);
    }
    
    std::cout << "Exiting yt2ipod shutdown\n";
}