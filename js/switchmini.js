if (typeof window.SwitchMiniPlayer === 'undefined') {
    window.SwitchMiniPlayer = true;

    // Obtain the area on the top-right, containing the Google account information.
    var rightArea = document.querySelector('#oneGoogleWrapper > div:first-child > div:first-child > div:first-child');

    // Add a button that, when clicked, opens the Last.fm popover.
    var frame = document.createElement('div');
    var compactButton = document.createElement('a');
    compactButton.id = 'compactButton';
    compactButton.addEventListener('click', function(e) {
        window.GoogleMusicApp.on_compactButton_clicked();
        e.preventDefault();
    });
    var miniButton = document.createElement('a');
    miniButton.id = 'miniButton';
    miniButton.addEventListener('click', function(e) {
        window.GoogleMusicApp.on_miniButton_clicked();
        e.preventDefault();
    });
    frame.appendChild(miniButton);
    frame.appendChild(compactButton);
    rightArea.appendChild(frame);
}
