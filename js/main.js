/*
 * js/main.js
 *
 * This script is part of the JavaScript interface used to interact with
 * the Google Play Music page, in order to provide notifications functionality.
 *
 * Created by Sajid Anwar.
 *
 * Subject to terms and conditions in LICENSE.md.
 *
 */

// This check ensures that, even though this script is run multiple times, our code is only attached once.
if (typeof window.MusicAPI === 'undefined') {
    window.MusicAPI = {};

    /* Set up for Safari versions less than 7. */
    if (typeof window.MutationObserver === 'undefined') {
        window.MutationObserver = window.WebKitMutationObserver;
    }

    /* Create a volume API. */
    window.MusicAPI.Volume = {

        // A reference to the volume slider element.
        slider: document.querySelector('#vslider'),

        // Get the current volume level.
        getVolume: function() {
            return parseInt(window.MusicAPI.Volume.slider.getAttribute('aria-valuenow'));
        },

        // Set the volume level (0 - 100).
        setVolume: function(vol) {
            var current = window.MusicAPI.Volume.getVolume();

            if (vol > current) {
                window.MusicAPI.Volume.increaseVolume(vol - current);
            }
            else if (vol < current) {
                window.MusicAPI.Volume.decreaseVolume(current - vol);
            }
        },

        // Increase the volume by an amount (default of 1).
        increaseVolume: function(amount) {
            if (typeof amount === 'undefined')
                amount = 1;

            for (var i = 0; i < amount; i++) {
                window.Keyboard.sendKey(window.MusicAPI.Volume.slider, window.Keyboard.KEY_UP);
            }
        },

        // Decrease the volume by an amount (default of 1).
        decreaseVolume: function(amount) {
            if (typeof amount === 'undefined')
                amount = 1;

            for (var i = 0; i < amount; i++) {
                window.Keyboard.sendKey(window.MusicAPI.Volume.slider, window.Keyboard.KEY_DOWN);
            }
        }
    };

    /* Create a playback API. */
    window.MusicAPI.Playback = {

        // References to the media playback elements.
        _eplayPause:  document.querySelector('button[data-id="play-pause"]'),
        _eforward:    document.querySelector('button[data-id="forward"]'),
        _erewind:     document.querySelector('button[data-id="rewind"]'),
        _eshuffle:    document.querySelector('button[data-id="shuffle"]'),
        _erepeat:     document.querySelector('button[data-id="repeat"]'),

        // Playback modes.
        STOPPED:    0,
        PAUSED:     1,
        PLAYING:    2,

        // Repeat modes.
        LIST_REPEAT:    'LIST_REPEAT',
        SINGLE_REPEAT:  'SINGLE_REPEAT',
        NO_REPEAT:      'NO_REPEAT',

        // Shuffle modes.
        ALL_SHUFFLE:    'ALL_SHUFFLE',
        NO_SHUFFLE:     'NO_SHUFFLE',

        // Time functions.
        getPlaybackTime: function() {
            return parseInt(slider.getAttribute('aria-valuenow'));
        },

        setPlaybackTime: function(milliseconds) {
            var percent = milliseconds / parseFloat(slider.getAttribute('aria-valuemax'));
            var lower = slider.offsetLeft + 6;
            var upper = slider.offsetLeft + slider.clientWidth - 6;
            var x = lower + percent*(upper - lower);

            window.Mouse.clickAtLocation(slider, x, 0)
        },

        // Playback functions.
        playPause:      function() { MusicAPI.Playback._eplayPause.click(); },
        forward:        function() { MusicAPI.Playback._eforward.click(); },
        rewind:         function() { MusicAPI.Playback._erewind.click(); },

        getShuffle:     function() { return MusicAPI.Playback._eshuffle.value; },
        toggleShuffle:  function() { MusicAPI.Playback._eshuffle.click(); },

        getRepeat:      function() {
            return MusicAPI.Playback._erepeat.value;
        },

        changeRepeat:   function(mode) {
            if (!mode) {
                // Toggle between repeat modes once.
                MusicAPI.Playback._erepeat.click();
            }
            else {
                // Toggle between repeat modes until the desired mode is activated.
                while (MusicAPI.Playback.getRepeat() !== mode) {
                    MusicAPI.Playback._erepeat.click();
                }
            }
        },

        changeShuffle:   function(mode) {
            if (!mode) {
                // Toggle between repeat modes once.
                MusicAPI.Playback._eshuffle.click();
            }
            else {
                // Toggle between repeat modes until the desired mode is activated.
                while (MusicAPI.Playback.getShuffle() !== mode) {
                    MusicAPI.Playback._eshuffle.click();
                }
            }
        },

        // Taken from the Google Play Music page.
        toggleVisualization: function() {
            SJBpost('toggleVisualization');
        }

    };

    /* Create a rating API. */
    window.MusicAPI.Rating = {

        // Determine whether the rating system is thumbs or stars.
        isStarsRatingSystem: function() {
            return document.querySelector('.rating-container.stars') !== null;
        },

        // Get current rating.
        getRating: function() {
            var el = document.querySelector('.player-rating-container li.selected');

            if (el) {
                return el.value;
            }
            else {
                return 0;
            }
        },

        // Thumbs up.
        toggleThumbsUp: function() {
            var el = document.querySelector('.player-rating-container li[data-rating="5"]');

            if (el)
                el.click();
        },

        // Thumbs down.
        toggleThumbsDown: function() {
            var el = document.querySelector('.player-rating-container li[data-rating="1"]');

            if (el)
                el.click();
        },

        // Set a star rating.
        setStarRating: function(rating) {
            var el = document.querySelector('.player-rating-container li[data-rating="' + rating + '"]');

            if (el)
                el.click();
        }
    };

    /* Miscellaneous functions. */
    window.MusicAPI.Extras = {

        // Get a shareable URL of the song on Google Play Music.
        getSongURL: function() {
            var albumEl = document.querySelector('.player-album');
            var artistEl = document.querySelector('.player-artist');

            var urlTemplate = 'https://play.google.com/music/m/';
            var url = null;

            var parseID = function(id) {
                return id.substring(0, id.indexOf('/'));
            };

            if (albumEl === null && aristEl === null)
                return null;

            var albumId = parseID(albumEl.dataset.id);
            var artistId = parseID(artistEl.dataset.id);

            if (albumId) {
                url = urlTemplate + albumId;
            }
            else if (artistId) {
                url = urlTemplate + artistId;
            }

            return url;
        }
    };

    var lastTitle = "";
    var lastArtist = "";
    var lastAlbum = "";

    var addObserver = new MutationObserver(function(mutations) {
        mutations.forEach(function(m) {
            for (var i = 0; i < m.addedNodes.length; i++) {
                var target = m.addedNodes[i];
                var name = target.id || target.className;

                if (name == 'text-wrapper')  {
                    var now = new Date();

                    var title = document.querySelector('#playerSongTitle');
                    var artist = document.querySelector('#player-artist');
                    var album = document.querySelector('.player-album');
                    var art = document.querySelector('#playingAlbumArt');
                    var duration = parseInt(document.querySelector('#player #slider').getAttribute('aria-valuemax')) / 1000;

                    title = (title) ? title.innerText : 'Unknown';
                    artist = (artist) ? artist.innerText : 'Unknown';
                    album = (album) ? album.innerText : 'Unknown';
                    art = (art) ? art.src : null;

                    // The art may be a protocol-relative URL, so normalize it to HTTPS.
                    if (art && art.slice(0, 2) === '//') {
                        art = 'https:' + art;
                    }

                    // Make sure that this is the first of the notifications for the
                    // insertion of the song information elements.
                    if (lastTitle != title || lastArtist != artist || lastAlbum != album) {
                        window.GoogleMusicApp.notifySong(title, artist, album, art, duration);

                        lastTitle = title;
                        lastArtist = artist;
                        lastAlbum = album;
                    }
                }
            }
        });
    });

    var shuffleObserver = new MutationObserver(function(mutations) {
        mutations.forEach(function(m) {
            var target = m.target;
            var id = target.dataset.id;

            if (id == 'shuffle') {
                window.GoogleMusicApp.shuffleChanged(target.value);
            }
        });
    });

    var repeatObserver = new MutationObserver(function(mutations) {
        mutations.forEach(function(m) {
            var target = m.target;
            var id = target.dataset.id;

            if (id == 'repeat') {
                window.GoogleMusicApp.repeatChanged(target.value);
            }
        });
    });

    var playbackObserver = new MutationObserver(function(mutations) {
        mutations.forEach(function(m) {
            var target = m.target;
            var id = target.dataset.id;

            if (id == 'play-pause') {
                var mode;
                var playing = target.classList.contains('playing');

                if (playing) {
                    mode = MusicAPI.Playback.PLAYING;
                }
                else {
                    // If there is a current song, then the player is paused.
                    if (document.querySelector('#playerSongInfo').childNodes.length) {
                        mode = MusicAPI.Playback.PAUSED;
                    }
                    else {
                        mode = MusicAPI.Playback.STOPPED;
                    }
                }

                window.GoogleMusicApp.playbackChanged(mode);
            }
        });
    });

    var playbackTimeObserver = new MutationObserver(function(mutations) {
        mutations.forEach(function(m) {
            var target = m.target;
            var id = target.id;

            if (id == 'slider') {
                var currentTime = parseInt(target.getAttribute('aria-valuenow'));
                var totalTime = parseInt(target.getAttribute('aria-valuemax'));
                window.GoogleMusicApp.playbackTimeChanged(currentTime, totalTime);
            }
        });
    });

    var ratingObserver = new MutationObserver(function(mutations) {
        mutations.forEach(function(m) {
            var target = m.target;

            if (target.classList.contains('selected'))
            {
                window.GoogleMusicApp.ratingChanged(target.dataset.rating);
            }
        });
    });


    addObserver.observe(document.querySelector('#playerSongInfo'), { childList: true, subtree: true });
    shuffleObserver.observe(document.querySelector('#player button[data-id="shuffle"]'), { attributes: true });
    repeatObserver.observe(document.querySelector('#player button[data-id="repeat"]'), { attributes: true });
    playbackObserver.observe(document.querySelector('#player button[data-id="play-pause"]'), { attributes: true });
    playbackTimeObserver.observe(document.querySelector('#player #slider'), { attributes: true });
    ratingObserver.observe(document.querySelector('#player .player-rating-container'), { attributes: true, subtree: true });
}
