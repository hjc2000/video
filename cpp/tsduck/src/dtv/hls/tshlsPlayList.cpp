//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tshlsPlayList.h"
#include "tshlsTagAttributes.h"
#include "tsFileUtils.h"


//----------------------------------------------------------------------------
// Clear the content of the playlist.
//----------------------------------------------------------------------------

void ts::hls::PlayList::clear()
{
    _valid = false;
    _version = 1;
    _type = PlayListType::UNKNOWN;
    _original.clear();
    _fileBase.clear();
    _isURL = false;
    _url.clear();
    _targetDuration = 0;
    _mediaSequence = 0;
    _endList = false;
    _utcDownload = Time::Epoch;
    _utcTermination = Time::Epoch;
    _segments.clear();
    _playlists.clear();
    _altPlaylists.clear();
    _loadedContent.clear();
    _extraTags.clear();
    // Preserve _autoSaveDir
}


//----------------------------------------------------------------------------
// Reset the content of a playlist.
//----------------------------------------------------------------------------

void ts::hls::PlayList::reset(ts::hls::PlayListType type, const ts::UString &filename, int version)
{
    clear();
    _valid = true;
    _version = version;
    _type = type;
    _original = AbsoluteFilePath(filename);
    _fileBase = DirectoryName(_original) + fs::path::preferred_separator;
    _isURL = false;
    _url.clear();
    _extraTags.clear();
}


//----------------------------------------------------------------------------
// Build an URL for a media segment or sub playlist.
//----------------------------------------------------------------------------

void ts::hls::PlayList::buildURL(MediaElement& media, const UString& uri) const
{
    media.relativeURI = uri;
    media.url.clear();

    if (_isURL) {
        // Build a full URL, based on original URL.
        media.url.setURL(uri, _url);
        media.filePath = media.url.getPath();
    }
    else if (uri.startWith(u"/")) {
        // The original URI was a file and the segment is an absolute file name.
        media.filePath = uri;
    }
    else {
        // The original URI was a file and the segment is a relative file name.
        media.filePath = _fileBase + uri;
    }
}


//----------------------------------------------------------------------------
// Set the playlist type.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::setType(PlayListType type, Report& report, bool forced)
{
    if (forced ||                           // forced => no check
        _type == type ||                    // same type was already known => no change
        _type == PlayListType::UNKNOWN ||   // type was unknown => now we know it.
        (_type == PlayListType::LIVE && (type == PlayListType::VOD || type == PlayListType::EVENT)))
                                            // media playlist without EXT-X-PLAYLIST-TYPE tag ("live") => now we get a tag
    {
        _type = type;
        return true;
    }
    else {
        report.error(u"incompatible tags or URI in HLS playlist, cannot be both master, VoD and event playlist");
        _valid = false;
        return false;
    }
}


//----------------------------------------------------------------------------
// Set the playlist type as media playlist.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::setTypeMedia(Report& report)
{
    switch (_type) {
        case PlayListType::UNKNOWN:
            // Force the playlist to be a media playlist without EXT-X-PLAYLIST-TYPE tag so far.
            _type = PlayListType::LIVE;
            return true;
        case PlayListType::VOD:
        case PlayListType::EVENT:
        case PlayListType::LIVE:
            // Already a media playlist.
            return true;
        case PlayListType::MASTER:
        default:
            report.error(u"incompatible tags or URI in HLS playlist, cannot be both master and media playlist");
            _valid = false;
            return false;
    }
}


//----------------------------------------------------------------------------
// Set various properties in the playlist.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::setTargetDuration(ts::Second duration, Report& report)
{
    if (setTypeMedia(report)) {
        _targetDuration = duration;
        return true;
    }
    else {
        return false;
    }
}

bool ts::hls::PlayList::setMediaSequence(size_t seq, Report& report)
{
    if (setTypeMedia(report)) {
        _mediaSequence = seq;
        return true;
    }
    else {
        return false;
    }
}

bool ts::hls::PlayList::setEndList(bool end, Report& report)
{
    if (setTypeMedia(report)) {
        _endList = end;
        return true;
    }
    else {
        return false;
    }
}


//----------------------------------------------------------------------------
// Get a constant reference to a component.
//----------------------------------------------------------------------------

const ts::hls::MediaSegment ts::hls::PlayList::EmptySegment;
const ts::hls::MediaPlayList ts::hls::PlayList::EmptyPlayList;
const ts::hls::AltPlayList ts::hls::PlayList::EmptyAltPlayList;

const ts::hls::MediaSegment& ts::hls::PlayList::segment(size_t index) const
{
    return index < _segments.size() ? _segments[index] : EmptySegment;
}

bool ts::hls::PlayList::popFirstSegment()
{
    if (_segments.empty()) {
        return false;
    }
    else {
        _segments.pop_front();
        _mediaSequence++;
        return true;
    }
}

bool ts::hls::PlayList::popFirstSegment(MediaSegment& seg)
{
    if (_segments.empty()) {
        seg = EmptySegment;
        return false;
    }
    else {
        seg = _segments.front();
        _segments.pop_front();
        _mediaSequence++;
        return true;
    }
}

const ts::hls::MediaPlayList& ts::hls::PlayList::playList(size_t index) const
{
    return index < _playlists.size() ? _playlists[index] : EmptyPlayList;
}

const ts::hls::AltPlayList& ts::hls::PlayList::altPlayList(size_t index) const
{
    return index < _altPlaylists.size() ? _altPlaylists[index] : EmptyAltPlayList;
}


//----------------------------------------------------------------------------
// Delete a media playlist description from a master playlist.
//----------------------------------------------------------------------------

void ts::hls::PlayList::deletePlayList(size_t index)
{
    if (index < _playlists.size()) {
        _playlists.erase(_playlists.begin() + index);
    }
}

void ts::hls::PlayList::deleteAltPlayList(size_t index)
{
    if (index < _altPlaylists.size()) {
        _altPlaylists.erase(_altPlaylists.begin() + index);
    }
}


//----------------------------------------------------------------------------
// Add a segment or sub-playlist in a playlist.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::addSegment(const MediaSegment& seg, Report& report)
{
    if (seg.relativeURI.empty()) {
        report.error(u"empty media segment URI");
        return false;
    }
    else if (setTypeMedia(report)) {
        // Add the segment.
        _segments.push_back(seg);
        // Build a relative URI.
        if (!_isURL && !_original.empty()) {
            // The playlist's URI is a file name, update the segment's URI.
            _segments.back().relativeURI = RelativeFilePath(seg.relativeURI, _fileBase, FILE_SYSTEM_CASE_SENSITVITY, true);
        }
        return true;
    }
    else {
        return false;
    }
}


bool ts::hls::PlayList::addPlayList(const MediaPlayList& pl, Report& report)
{
    if (pl.relativeURI.empty()) {
        report.error(u"empty media playlist URI");
        return false;
    }
    else if (setType(PlayListType::MASTER, report)) {
        // Add the media playlist.
        _playlists.push_back(pl);
        // Build a relative URI.
        if (!_isURL && !_original.empty()) {
            // The master playlist's URI is a file name, update the media playlist's URI.
            _playlists.back().relativeURI = RelativeFilePath(pl.relativeURI, _fileBase, FILE_SYSTEM_CASE_SENSITVITY, true);
        }
        return true;
    }
    else {
        return false;
    }
}


bool ts::hls::PlayList::addAltPlayList(const AltPlayList& pl, Report& report)
{
    if (setType(PlayListType::MASTER, report)) {
        // Add the media playlist.
        _altPlaylists.push_back(pl);
        // Build a relative URI if there is one (the URI field is optional in an alternative rendition playlist).
        if (!pl.relativeURI.empty() && !_isURL && !_original.empty()) {
            // The master playlist's URI is a file name, update the media playlist's URI.
            _altPlaylists.back().relativeURI = RelativeFilePath(pl.relativeURI, _fileBase, FILE_SYSTEM_CASE_SENSITVITY, true);
        }
        return true;
    }
    else {
        return false;
    }
}


//----------------------------------------------------------------------------
// Select a media playlist with specific constraints.
//----------------------------------------------------------------------------

size_t ts::hls::PlayList::selectPlayList(const BitRate& minBitrate, const BitRate& maxBitrate, size_t minWidth, size_t maxWidth, size_t minHeight, size_t maxHeight) const
{
    for (size_t i = 0; i < _playlists.size(); ++i) {
        const MediaPlayList& pl(_playlists[i]);
        if ((minBitrate == 0 || pl.bandwidth >= minBitrate) &&
            (maxBitrate == 0 || (pl.bandwidth > 0 && pl.bandwidth <= maxBitrate)) &&
            (minWidth == 0 || pl.width >= minWidth) &&
            (maxWidth == 0 || (pl.width > 0 && pl.width <= maxWidth)) &&
            (minHeight == 0 || pl.height >= minHeight) &&
            (maxHeight == 0 || (pl.height > 0 && pl.height <= maxHeight)))
        {
            // Match all criteria.
            return i;
        }
    }

    // None found.
    return NPOS;
}

size_t ts::hls::PlayList::selectPlayListLowestBitRate() const
{
    size_t result = NPOS;
    BitRate ref = BitRate::MAX;
    BitRate val = 0;
    for (size_t i = 0; i < _playlists.size(); ++i) {
        if ((val = _playlists[i].bandwidth) < ref) {
            result = i;
            ref = val;
        }
    }
    return result;
}

size_t ts::hls::PlayList::selectPlayListHighestBitRate() const
{
    size_t result = NPOS;
    BitRate ref = 0;
    BitRate val = 0;
    for (size_t i = 0; i < _playlists.size(); ++i) {
        if ((val = _playlists[i].bandwidth) > ref) {
            result = i;
            ref = val;
        }
    }
    return result;
}

size_t ts::hls::PlayList::selectPlayListLowestResolution() const
{
    size_t result = NPOS;
    size_t ref = std::numeric_limits<size_t>::max();
    size_t val = 0;
    for (size_t i = 0; i < _playlists.size(); ++i) {
        if ((val = _playlists[i].width * _playlists[i].height) < ref) {
            result = i;
            ref = val;
        }
    }
    return result;
}

size_t ts::hls::PlayList::selectPlayListHighestResolution() const
{
    size_t result = NPOS;
    size_t ref = 0;
    size_t val = 0;
    for (size_t i = 0; i < _playlists.size(); ++i) {
        if ((val = _playlists[i].width * _playlists[i].height) > ref) {
            result = i;
            ref = val;
        }
    }
    return result;
}


//----------------------------------------------------------------------------
// Select the first alternative rendition playlist with specific criteria.
//----------------------------------------------------------------------------

size_t ts::hls::PlayList::selectAltPlayList(const UString& type, const UString& name, const UString& groupId, const UString& language) const
{
    for (size_t i = 0; i < _altPlaylists.size(); ++i) {
        const AltPlayList& pl(_altPlaylists[i]);
        if ((type.empty() || pl.type.similar(type)) &&
            (name.empty() || pl.name.similar(name)) &&
            (groupId.empty() || pl.groupId.similar(groupId)) &&
            (language.empty() || pl.language.similar(language)))
        {
            // Match all criteria.
            return i;
        }
    }

    // None found.
    return NPOS;
}


//----------------------------------------------------------------------------
// Load the playlist from a text file.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::loadFile(const UString& filename, bool strict, PlayListType type, Report& report)
{
    clear();
    _type = type;

    // Keep file name.
    _original = filename;
    _fileBase = DirectoryName(filename) + fs::path::preferred_separator;
    _isURL = false;

    // Check strict conformance: according to RFC 8216, a playlist must either ends in .m3u8 or .m3u.
    if (strict && !filename.endWith(u".m3u8", CASE_INSENSITIVE) && !filename.endWith(u".m3u", CASE_INSENSITIVE)) {
        report.error(u"Invalid file name extension for HLS playlist in %s", {filename});
        return false;
    }

    // Load the file.
    if (UString::Load(_loadedContent, filename)) {
        // Autosave if necessary, ignore errors.
        autoSave(report);
        // Load from the text.
        return parse(strict, report);
    }
    else {
        report.error(u"error loading %s", {filename});
        return false;
    }
}


//----------------------------------------------------------------------------
// Load the playlist from its text content.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::loadText(const UString& text, bool strict, PlayListType type, Report& report)
{
    clear();
    _type = type;
    return parse(text, strict, report);
}

//----------------------------------------------------------------------------
// Load from the text content with embedded line separators.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::parse(const UString& text, bool strict, Report& report)
{
    text.toRemoved(CARRIAGE_RETURN).split(_loadedContent, LINE_FEED, false, false);
    return parse(strict, report);
}


//----------------------------------------------------------------------------
// Load from the text content.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::parse(bool strict, Report& report)
{
    // Global media segment or playlist information.
    // Contains properties which are valid until next occurence of same property.
    MediaPlayList plGlobal;
    MediaSegment segGlobal;

    // Next media segment or playlist information.
    // Contains properties which are valid for next URI only.
    MediaPlayList plNext;
    MediaSegment segNext;

    // Current tag and parameters.
    Tag tag = Tag::EXTM3U;
    UString tagParams;

    // The playlist must always start with #EXTM3U.
    if (_loadedContent.empty() || !getTag(_loadedContent.front(), tag, tagParams, strict, report) || tag != Tag::EXTM3U) {
        report.error(u"invalid HLS playlist, does not start with #EXTM3U");
        return false;
    }

    // Assume valid playlist, invalidate when necessary.
    _valid = true;

    // Initial download time.
    _utcDownload = _utcTermination = Time::CurrentUTC();

    // Loop on all lines in file.
    uint32_t lineNumber = 0;
    for (const auto& it : _loadedContent) {

        // In non-strict mode, ignore leading and trailing spaces.
        UString line(it);
        if (!strict) {
            line.trim();
        }
        lineNumber++;
        report.log(2, u"playlist: %s", {line});

        // A line is one of blank, comment, tag, URI.
        if (isURI(line, strict, report)) {
            // URI line, add media segment or media playlist description, depending on current playlist type.
            if (isMaster()) {
                // Enqueue a new playlist description.
                buildURL(plNext, line);
                _playlists.push_back(plNext);
                if (!plNext.filePath.endWith(u".m3u8", CASE_INSENSITIVE)) {
                    report.debug(u"unexpected playlist file extension in reference URI: %s", {line});
                }
                // Reset description of next playlist.
                plNext = plGlobal;
            }
            else if (isMedia()) {
                // Enqueue a new media segment.
                buildURL(segNext, line);
                _utcTermination += segNext.duration;
                _segments.push_back(segNext);
                if (!segNext.filePath.endWith(u".ts", CASE_INSENSITIVE)) {
                    report.debug(u"unexpected segment file extension in reference URI: %s", {line});
                }
                // Reset description of next segment.
                segNext = segGlobal;
            }
            else {
                report.debug(u"unknown URI: %s", {line});
                _valid = false;
            }
        }
        else if (getTag(line, tag, tagParams, strict, report)) {
            // The line contains a tag.
            switch (tag) {
                case Tag::EXTM3U: {
                    if (strict && lineNumber > 1) {
                        report.error(u"misplaced: %s", {line});
                        _valid = false;
                    }
                    break;
                }
                case Tag::VERSION: {
                    if (!tagParams.toInteger(_version) && strict) {
                        report.error(u"invalid HLS playlist version: %s", {line});
                        _valid = false;
                    }
                    break;
                }
                case Tag::EXTINF: {
                    // #EXTINF:duration,[title]
                    // Apply to next segment only.
                    const size_t comma = tagParams.find(u",");  // can be NPOS
                    if (!TagAttributes::ToMilliValue(segNext.duration, tagParams.substr(0, comma))) {
                        report.error(u"invalid segment duration in %s", {line});
                        _valid = false;
                    }
                    if (comma != NPOS) {
                        segNext.title.assign(tagParams, comma + 1);
                        segNext.title.trim();
                    }
                    break;
                }
                case Tag::BITRATE: {
                    // #EXT-X-BITRATE:<rate>
                    BitRate kilobits = 0;
                    if (kilobits.fromString(tagParams)) {
                        // Apply to one or more segments.
                        segGlobal.bitrate = segNext.bitrate = 1024 * kilobits;
                    }
                    else if (strict) {
                        report.error(u"invalid segment bitrate in %s", {line});
                        _valid = false;
                    }
                    break;
                }
                case Tag::GAP: {
                    // #EXT-X-GAP
                    // Apply to next segment only.
                    segNext.gap = true;
                    break;
                }
                case Tag::TARGETDURATION: {
                    // #EXT-X-TARGETDURATION:s
                    if (!tagParams.toInteger(_targetDuration) && strict) {
                        report.error(u"invalid target duration in %s", {line});
                        _valid = false;
                    }
                    break;
                }
                case Tag::MEDIA_SEQUENCE: {
                    // #EXT-X-MEDIA-SEQUENCE:number
                    if (!tagParams.toInteger(_mediaSequence) && strict) {
                        report.error(u"invalid media sequence in %s", {line});
                        _valid = false;
                    }
                    break;
                }
                case Tag::ENDLIST: {
                    _endList = true;
                    break;
                }
                case Tag::PLAYLIST_TYPE: {
                    if (tagParams.similar(u"VOD")) {
                        setType(PlayListType::VOD, report);
                    }
                    else if (tagParams.similar(u"EVENT")) {
                        setType(PlayListType::EVENT, report);
                    }
                    else {
                        report.error(u"invalid playlist type '%s' in %s", {tagParams, line});
                        _valid = false;
                    }
                    break;
                }
                case Tag::STREAM_INF: {
                    // #EXT-X-STREAM-INF:<attribute-list>
                    // Apply to next playlist only.
                    const TagAttributes attr(tagParams);
                    attr.getValue(plNext.bandwidth, u"BANDWIDTH");
                    attr.getValue(plNext.averageBandwidth, u"AVERAGE-BANDWIDTH");
                    attr.value(u"RESOLUTION").scan(u"%dx%d", {&plNext.width, &plNext.height});
                    attr.getMilliValue(plNext.frameRate, u"FRAME-RATE");
                    plNext.codecs = attr.value(u"CODECS");
                    plNext.hdcp = attr.value(u"HDCP-LEVEL");
                    plNext.videoRange = attr.value(u"VIDEO-RANGE");
                    plNext.video = attr.value(u"VIDEO");
                    plNext.audio = attr.value(u"AUDIO");
                    plNext.subtitles = attr.value(u"SUBTITLES");
                    plNext.closedCaptions = attr.value(u"CLOSED-CAPTIONS");
                    break;
                }
                case Tag::MEDIA: {
                    // #EXT-X-MEDIA:<attribute-list>
                    const TagAttributes attr(tagParams);
                    AltPlayList pl;
                    pl.name = attr.value(u"NAME");
                    pl.type = attr.value(u"TYPE");
                    pl.groupId = attr.value(u"GROUP-ID");
                    pl.stableRenditionId = attr.value(u"STABLE-RENDITION-ID");
                    pl.language = attr.value(u"LANGUAGE");
                    pl.assocLanguage = attr.value(u"ASSOC-LANGUAGE");
                    pl.inStreamId = attr.value(u"INSTREAM-ID");
                    pl.characteristics = attr.value(u"CHARACTERISTICS");
                    pl.channels = attr.value(u"CHANNELS");
                    pl.isDefault = attr.value(u"DEFAULT").similar(u"YES");
                    pl.autoselect = attr.value(u"AUTOSELECT").similar(u"YES");
                    pl.forced = attr.value(u"FORCED").similar(u"YES");
                    const UString uri(attr.value(u"URI"));
                    if (!uri.empty()) {
                        buildURL(pl, uri);
                        if (!pl.filePath.endWith(u".m3u8", CASE_INSENSITIVE)) {
                            report.debug(u"unexpected playlist file extension in reference URI: %s", {uri});
                        }
                    }
                    _altPlaylists.push_back(pl);
                    break;
                }
                case Tag::BYTERANGE:
                case Tag::DISCONTINUITY:
                case Tag::KEY:
                case Tag::MAP:
                case Tag::PROGRAM_DATE_TIME:
                case Tag::DATERANGE:
                case Tag::SKIP:
                case Tag::PRELOAD_HINT:
                case Tag::RENDITION_REPORT:
                case Tag::DISCONTINUITY_SEQUENCE:
                case Tag::I_FRAMES_ONLY:
                case Tag::PART_INF:
                case Tag::SERVER_CONTROL:
                case Tag::I_FRAME_STREAM_INF:
                case Tag::SESSION_DATA:
                case Tag::SESSION_KEY:
                case Tag::CONTENT_STEERING:
                case Tag::INDEPENDENT_SEGMENTS:
                case Tag::START:
                case Tag::DEFINE:
                case Tag::PART:
                    // Currently ignored tags.
                    break;
                default:
                    // Should not get there.
                    assert(false);
                    break;
            }
        }
    }

    return _valid;
}


//----------------------------------------------------------------------------
// Check if the line contains a valid tag.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::getTag(const UString& line, Tag& tag, UString& params, bool strict, Report& report)
{
    // Check if this is a tag line.
    if (!line.startWith(u"#EXT", strict ? CASE_SENSITIVE : CASE_INSENSITIVE)) {
        return false;
    }

    // This is a tag line. Locate the tag name (letters, digits and dash).
    size_t pos = 1;
    while (pos < line.size() && (IsAlpha(line[pos]) || IsDigit(line[pos]) || line[pos] == u'-')) {
        ++pos;
    }

    // Identify the tag. Report unknown tag but do not invalidate the playlist.
    if (!TagNames.getValue(tag, line.substr(1, pos - 1), strict)) {
        report.log(strict ? Severity::Error : Severity::Debug, u"unsupported HLS tag: %s", {line.substr(1, pos - 1)});
        return false;
    }

    // Set playlist type based on tags which are unique to a playlist type.
    const TagFlags flags = TagProperties(tag);
    if ((flags & (TagFlags::MASTER | TagFlags::MEDIA)) == TagFlags::MASTER) {
        // This is a master-only tag.
        setType(PlayListType::MASTER, report);
    }
    else if ((flags & (TagFlags::MASTER | TagFlags::MEDIA)) == TagFlags::MEDIA) {
        // This is a media-only tag.
        setTypeMedia(report);
    }

    // The tag must be alone of followed by ':'.
    while (!strict && pos < line.size() && IsSpace(line[pos])) {
        ++pos;
    }
    if (pos < line.size()) {
        if (line[pos] == u':') {
            ++pos; // skip ':'
        }
        else {
            report.error(u"invalid HLS playlist line: %s", {line});
            _valid = false;
            return false;
        }
    }
    while (!strict && pos < line.size() && IsSpace(line[pos])) {
        ++pos;
    }

    // Rest of the line is the tag parameters.
    params.assign(line, pos);
    return true;
}


//----------------------------------------------------------------------------
// Check if the line contains a valid URI.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::isURI(const UString& line, bool strict, Report& report)
{
    if (line.empty() || line.startWith(u"#")) {
        // Not an URI line.
        return false;
    }

    // Build a full path of the URI and extract the path name (without trailing query or fragment).
    MediaElement me;
    buildURL(me, line);
    const UString name(me.url.isValid() ? me.url.getPath() : me.filePath);

    // If the URI extension is known, set playlist type.
    if (name.endWith(u".m3u8", CASE_INSENSITIVE) || name.endWith(u".m3u", CASE_INSENSITIVE)) {
        // Reference to another playlist, this is a master playlist.
        setType(PlayListType::MASTER, report);
    }
    else if (name.endWith(u".ts", CASE_INSENSITIVE)) {
        // Reference to a TS file, this is a media playlist.
        setTypeMedia(report);
    }

    return true;
}


//----------------------------------------------------------------------------
// Perform automatic save of the loaded playlist.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::autoSave(Report& report)
{
    if (_autoSaveDir.empty() || _original.empty()) {
        // No need to save
        return true;
    }
    else {
        const UString name(_autoSaveDir + fs::path::preferred_separator + BaseName(_original));
        report.verbose(u"saving playlist to %s", {name});
        const bool ok = UString::Save(_loadedContent, name);
        if (!ok) {
            report.warning(u"error saving playlist to %s", {name});
        }
        return ok;
    }
}


//----------------------------------------------------------------------------
// Implementation of StringifyInterface
//----------------------------------------------------------------------------

ts::UString ts::hls::PlayList::toString() const
{
    UString str;

    if (_isURL) {
        const size_t slash = _original.rfind(u'/');
        str = slash == NPOS ? _original : _original.substr(slash + 1);
    }
    else {
        str = BaseName(_original);
    }
    if (!str.empty()) {
        str.append(u", ");
    }
    if (!_valid) {
        str.append(u"invalid playlist");
    }
    else if (isMedia()) {
        str.append(u"media playlist");
    }
    else if (isMaster()) {
        str.append(u"master playlist");
    }
    else {
        str.append(u"unknown playlist");
    }
    str.append(isUpdatable() ? u", updatable (live)" : u", static");
    if (isMedia()) {
        str.format(u", %d segments", {_segments.size()});
    }
    else if (_type == PlayListType::MASTER) {
        str.format(u", %d media playlists", {_playlists.size()});
        if (!_altPlaylists.empty()) {
            str.format(u", %d alternative rendition playlists", {_altPlaylists.size()});
        }
    }
    if (_targetDuration > 0) {
        str.format(u", %d seconds/segment", {_targetDuration});
    }
    return str;
}


//----------------------------------------------------------------------------
// Save the playlist to a text file.
//----------------------------------------------------------------------------

bool ts::hls::PlayList::saveFile(const ts::UString &filename, ts::Report &report) const
{
    // Check that we have a valid file name to store the file.
    if (filename.empty() && (_isURL || _original.empty())) {
        report.error(u"no file name specified to store the HLS playlist");
        return false;
    }

    // Generate the text content.
    const UString text(textContent(report));
    if (text.empty()) {
        return false;
    }

    // Save the file.
    const UString& name(filename.empty() ? _original : filename);
    if (!text.save(name, false, true)) {
        report.error(u"error saving HLS playlist in %s", {name});
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Build the text content of the playlist.
//----------------------------------------------------------------------------

ts::UString ts::hls::PlayList::textContent(ts::Report &report) const
{
    // Filter out invalid content.
    if (!_valid) {
        report.error(u"invalid HLS playlist content");
        return UString();
    }

    // Start building the content.
    UString text;
    text.format(u"#%s\n#%s:%d\n", {TagNames.name(Tag::EXTM3U), TagNames.name(Tag::VERSION), _version});

    // Insert application-specific tags before standard tags.
    for (const auto& tag : _extraTags) {
        text.format(u"%s%s\n", {tag.startWith(u"#") ? u"" : u"#", tag});
    }

    if (isMaster()) {
        // Loop on all alternative rendition playlists.
        for (const auto& pl : _altPlaylists) {
            // The initial fields are required.
            text.format(u"#%s:TYPE=%s,GROUP-ID=\"%s\",NAME=\"%s\"", {TagNames.name(Tag::MEDIA), pl.type, pl.groupId, pl.name});
            if (pl.isDefault) {
                text.append(u",DEFAULT=YES");
            }
            if (pl.autoselect) {
                text.append(u",AUTOSELECT=YES");
            }
            if (pl.forced) {
                text.append(u",FORCED=YES");
            }
            if (!pl.language.empty()) {
                text.format(u",LANGUAGE=\"%s\"", {pl.language});
            }
            if (!pl.assocLanguage.empty()) {
                text.format(u",ASSOC-LANGUAGE=\"%s\"", {pl.assocLanguage});
            }
            if (!pl.stableRenditionId.empty()) {
                text.format(u",STABLE-RENDITION-ID=\"%s\"", {pl.stableRenditionId});
            }
            if (!pl.inStreamId.empty()) {
                text.format(u",INSTREAM-ID=\"%s\"", {pl.inStreamId});
            }
            if (!pl.characteristics.empty()) {
                text.format(u",CHARACTERISTICS=\"%s\"", {pl.characteristics});
            }
            if (!pl.channels.empty()) {
                text.format(u",CHANNELS=\"%s\"", {pl.channels});
            }
            if (!pl.relativeURI.empty()) {
                text.format(u",URI=\"%s\"", {pl.relativeURI});
            }
            // Close the #EXT-X-MEDIA line.
            text.append(u'\n');
        }
        // Loop on all media playlists.
        for (const auto& pl : _playlists) {
            if (!pl.relativeURI.empty()) {
                // The #EXT-X-STREAM-INF line must exactly preceed the URI line.
                // Take care about string parameters: some are documented as quoted-string and
                // some as enumerated-string. The former shall be quoted, the latter shall not.
                text.format(u"#%s:BANDWIDTH=%d", {TagNames.name(Tag::STREAM_INF), pl.bandwidth.toInt()});
                if (pl.averageBandwidth > 0) {
                    text.format(u",AVERAGE-BANDWIDTH=%d", {pl.averageBandwidth.toInt()});
                }
                if (pl.frameRate > 0) {
                    text.format(u",FRAME-RATE=%d.%03d", {pl.frameRate / 1000, pl.frameRate % 1000});
                }
                if (pl.width > 0 && pl.height > 0) {
                    text.format(u",RESOLUTION=%dx%d", {pl.width, pl.height});
                }
                if (!pl.codecs.empty()) {
                    text.format(u",CODECS=\"%s\"", {pl.codecs});
                }
                if (!pl.hdcp.empty()) {
                    text.format(u",HDCP-LEVEL=%s", {pl.hdcp});
                }
                if (!pl.videoRange.empty()) {
                    text.format(u",VIDEO-RANGE=%s", {pl.videoRange});
                }
                if (!pl.video.empty()) {
                    text.format(u",VIDEO=\"%s\"", {pl.video});
                }
                if (!pl.audio.empty()) {
                    text.format(u",AUDIO=\"%s\"", {pl.audio});
                }
                if (!pl.subtitles.empty()) {
                    text.format(u",SUBTITLES=\"%s\"", {pl.subtitles});
                }
                if (!pl.closedCaptions.empty()) {
                    if (pl.closedCaptions.similar(u"NONE")) {
                        // enumerated-string
                        text.append(u",CLOSED-CAPTIONS=NONE");
                    }
                    else {
                        // quoted-string
                        text.format(u",CLOSED-CAPTIONS=\"%s\"", {pl.closedCaptions});
                    }
                }
                // Close the #EXT-X-STREAM-INF line.
                text.append(u'\n');
                // The URI line must come right after #EXT-X-STREAM-INF.
                text.format(u"%s\n", {pl.relativeURI});
            }
        }
    }
    else if (isMedia()) {
        // Global tags.
        text.format(u"#%s:%d\n", {TagNames.name(Tag::TARGETDURATION), _targetDuration});
        text.format(u"#%s:%d\n", {TagNames.name(Tag::MEDIA_SEQUENCE), _mediaSequence});
        if (_type == PlayListType::VOD) {
            text.format(u"#%s:VOD\n", {TagNames.name(Tag::PLAYLIST_TYPE)});
        }
        else if (_type == PlayListType::EVENT) {
            text.format(u"#%s:EVENT\n", {TagNames.name(Tag::PLAYLIST_TYPE)});
        }

        // Loop on all media segments.
        for (const auto& seg : _segments) {
            if (!seg.relativeURI.empty()) {
                text.format(u"#%s:%d.%03d,%s\n", {TagNames.name(Tag::EXTINF), seg.duration / MilliSecPerSec, seg.duration % MilliSecPerSec, seg.title});
                if (seg.bitrate > 1024) {
                    text.format(u"#%s:%d\n", {TagNames.name(Tag::BITRATE), (seg.bitrate / 1024).toInt()});
                }
                if (seg.gap) {
                    text.format(u"#%s\n", {TagNames.name(Tag::GAP)});
                }
                text.format(u"%s\n", {seg.relativeURI});
            }
        }

        // Mark end of list when necessary.
        if (_endList) {
            text.format(u"#%s\n", {TagNames.name(Tag::ENDLIST)});
        }
    }
    else {
        report.error(u"unknown HLS playlist type (master or media playlist)");
        text.clear();
    }

    return text;
}
